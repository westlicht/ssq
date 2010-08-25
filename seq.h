#ifndef __SEQ_H__
#define __SEQ_H__

#include "pattern.h"

/** sequencer run state */
typedef enum {
	SEQ_STOPPED,
	SEQ_RUNNING
} seq_run_state_t;

/**
 * Intializes the sequencer.
 * @return Returns 0 if successful.
 */
int seq_init(void);

/**
 * Shuts the sequencer down.
 */
void seq_shutdown(void);

/**
 * Returns the pattern.
 * @return Returns the pattern.
 */
pattern_t *seq_get_pattern(void);

/**
 * Sets the tempo.
 * @param tempo Tempo in BPM
 */
void seq_set_tempo(float tempo);

/**
 * Starts the sequencer.
 */
void seq_start(void);

/**
 * Stops the sequencer.
 */
void seq_stop(void);

/**
 * Continues the sequencer.
 */
void seq_continue(void);

/**
 * Returns the current run state.
 * @return Returns the current run state.
 */
seq_run_state_t seq_get_run_state(void);

/**
 * Returns the current pulse.
 * @preturn Returns the current pulse.
 */
int seq_get_pulse(void);

/**
 * Returns the elapsed time in milliseconds since last start.
 * @return Returns the elapsed time.
 */
int seq_get_elapsed_time(void);




#endif /*__SEQ_H__*/
