
#include <pthread.h>
#include <unistd.h>

#include "log.h"
#include "mio.h"
#include "mmi.h"
#include "clock.h"
#include "pattern.h"
#include "seq.h"

static seq_run_state_t s_run_state;
static clk_t s_clock;
static pattern_t s_pattern;

static pthread_t s_thread;
static int s_thread_stop = 0;

static void *seq_thread(void *data);
static void clock_cb(clk_t *clk, int beat, mio_timestamp_t timestamp);

/*
 * Intializes the sequencer.
 */
int seq_init(void)
{
	s_run_state = SEQ_STOPPED;
	
	if (pthread_create(&s_thread, NULL, seq_thread, NULL))
		s_thread = 0;
		
	if (!s_thread) {
		LOG(LOG_ERROR, "cannot create sequencer thread");
		return -1;
	}
	
	clk_set_bpm(&s_clock, 130, 24);
	
	pattern_init(&s_pattern);
		
	return 0;
}

/*
 * Shuts the sequencer down.
 */
void seq_shutdown(void)
{
	seq_stop();
	
	s_thread_stop = 1;
	pthread_join(s_thread, NULL);
}

/*
 * Returns the pattern.
 */
pattern_t *seq_get_pattern(void)
{
	return &s_pattern;
}

/*
 * Sets the tempo.
 */
void seq_set_tempo(float tempo)
{
	clk_set_bpm(&s_clock, tempo, 24);
}

/*
 * Starts the sequencer.
 */
void seq_start(void)
{
	if (s_run_state == SEQ_RUNNING)
		seq_stop();

	pattern_reset(&s_pattern);
	clk_start(&s_clock);
	s_run_state = SEQ_RUNNING;
}

/*
 * Stops the sequencer.
 */
void seq_stop(void)
{
	if (s_run_state == SEQ_STOPPED)
		return;
		
	s_run_state = SEQ_STOPPED;
}

/*
 * Continues the sequencer.
 */
void seq_continue(void)
{
	
}

/**
 * Returns the current run state.
 * @return Returns the current run state.
 */
seq_run_state_t seq_get_run_state(void)
{
	return s_run_state;
}

/*
 * Returns the current pulse.
 */
int seq_get_pulse(void)
{
	return clk_get_pulse(&s_clock);
}

/*
 * Returns the elapsed time in milliseconds since last start.
 */
int seq_get_elapsed_time(void)
{
	return clk_get_elapsed_time(&s_clock);
}


/**
 * Sequencer thread.
 * @param data User data
 * @return Return code.
 */
static void *seq_thread(void *data)
{
	while (!s_thread_stop) {
		if (s_run_state == SEQ_RUNNING)
			clk_update(&s_clock, clock_cb);
		usleep(1);
	}
	
	pthread_exit(NULL);
}

/**
 * Callback from clock.
 */
static void clock_cb(clk_t *clk, int pulse, mio_timestamp_t timestamp)
{
	//LOG(LOG_INFO, "pulse: %d timestamp: %ld", pulse, timestamp);
	pattern_pulse(&s_pattern, pulse, timestamp);
	mmi_pulse(pulse, timestamp);
}
