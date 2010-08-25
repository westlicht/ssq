#ifndef __CLOCK_H__
#define __CLOCK_H__

#include "mio.h"

/** beat clock */
typedef struct {
	float bpm;                   /**< beats per minute of the clock */
	int divider;                 /**< beat divider in microseconds */
	int pulse;                   /**< current pulse */
	mio_timestamp_t start_time;  /**< timestamp when clock is started */
	mio_timestamp_t last_time;   /**< timestamp of last processing */
	unsigned long long us;       /**< new time in microseconds */
	unsigned long long total_us; /**< absolute time in microseconds */
} clk_t;

typedef void (* clk_cb_t) (clk_t *clk, int pulse, mio_timestamp_t timestamp);

/**
 * Sets the clock's tempo.
 * @param clk Clock
 * @param bpm Beats (quarter notes) per minute
 * @param ppq Pulses per quarter
 */
void clk_set_bpm(clk_t *clk, float bpm, int ppq);

/**
 * Starts the clock.
 * @param clk Clock
 */
void clk_start(clk_t *clk);

/**
 * Updates the clock.
 * @param clk Clock
 * @param cb Callback for beats
 */
void clk_update(clk_t *clk, clk_cb_t cb);

/**
 * Returns the current pulse.
 * @param clk Clock
 * @return Returns the current pulse.
 */
int clk_get_pulse(clk_t *clk);

/**
 * Returns the elapsed time in milliseconds.
 * @param clk Clock
 * @return Returns the elapsed time.
 */
mio_timestamp_t clk_get_elapsed_time(clk_t *clk);

#endif /*__CLOCK_H__*/
