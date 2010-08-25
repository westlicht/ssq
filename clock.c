
#include "mio.h"
#include "clock.h"

// FIXME divider should be protected by a semaphore

/*
 * Sets the clock's tempo.
 */
void clk_set_bpm(clk_t *clk, float bpm, int ppq)
{
	clk->bpm = bpm;
	clk->divider = (int) ((double) 60000000 / (bpm * ppq));
}

/*
 * Starts the clock.
 */
void clk_start(clk_t *clk)
{
	clk->pulse = -1;
	clk->start_time = mio_get_timestamp();
	clk->last_time = mio_get_timestamp();
	clk->us = 0;
	clk->total_us = 0;
}

/*
 * Updates the clock.
 */
void clk_update(clk_t *clk, clk_cb_t cb)
{
	mio_timestamp_t current_time;
	
	current_time = mio_get_timestamp();
	clk->us += (current_time - clk->last_time) * 1000;
	clk->last_time = current_time;
	
	while (clk->us >= clk->divider) {
		clk->us -= clk->divider;
		clk->total_us += clk->divider;
		clk->pulse++;
		cb(clk, clk->pulse, clk->total_us / 1000); 
	}  
}

/*
 * Returns the current pulse.
 */
int clk_get_pulse(clk_t *clk)
{
	return clk->pulse;
}

/*
 * Returns the elapsed time in milliseconds.
 */
mio_timestamp_t clk_get_elapsed_time(clk_t *clk)
{
	return clk->last_time - clk->start_time;
}
