
#include "line.h"
#include "sequence.h"

/*
 * Initializes a sequence.
 */
void sequence_init(sequence_t *sequence, int index, pattern_t *pattern)
{
	int i;
	
	sequence->pattern = pattern;
	sequence->index = index;
	
	for (i = 0; i < NUM_LINES; i++) {
		line_init(&sequence->lines[i], i, sequence);
		sequence->outputs[i] = &sequence->lines[i].output;
	}
}

/*
 * Clears a sequence.
 */
void sequence_clear(sequence_t *sequence)
{
}

/*
 * Resets a sequence.
 */
void sequence_reset(sequence_t *sequence)
{
	int i;
	
	for (i = 0; i < NUM_LINES; i++)
		line_reset(&sequence->lines[i]);
}

/*
 * Process a single pulse.
 */
void sequence_pulse(sequence_t *sequence, int pulse, mio_timestamp_t timestamp)
{
	int i;
	
	for (i = 0; i < NUM_LINES; i++)
		line_pulse(&sequence->lines[i], pulse, timestamp);
}

/*
 * Loads a sequence from a file.
 */
int sequence_load(sequence_t *sequence, FILE *file, int version)
{
	int i;
	
	for (i = 0; i < NUM_LINES; i++)
		if (line_load(&sequence->lines[i], file, version) != 0)
			return -1;
			
	return 0;
}

/*
 * Saves a sequence to a file.
 */
int sequence_save(sequence_t *sequence, FILE *file, int version)
{
	int i;
	
	for (i = 0; i < NUM_LINES; i++)
		if (line_save(&sequence->lines[i], file, version) != 0)
			return -1;
			
	return 0;
}
