#ifndef __SEQUENCE_H__
#define __SEQUENCE_H__

#include <stdio.h>

#include "defines.h"
#include "mio.h"
#include "objects.h"

/**
 * Initializes a sequence.
 * @param sequence Sequence
 * @param index Sequence index in pattern
 * @param pattern Parent pattern
 */
void sequence_init(sequence_t *sequence, int index, pattern_t *pattern);

/**
 * Clears a sequence.
 * @param sequence Sequence
 */
void sequence_clear(sequence_t *sequence);

/**
 * Resets a sequence.
 * @param sequence Sequence
 * @param sequence Sequence
 */
void sequence_reset(sequence_t *sequence);

/**
 * Process a single pulse.
 * @param sequence Sequence
 * @param pulse Pulse
 * @param timestamp Timestamp
 */
void sequence_pulse(sequence_t *sequence, int pulse, mio_timestamp_t timestamp);

/**
 * Loads a sequence from a file.
 * @param sequence Sequence
 * @param file File
 * @param version Version
 * @return Returns 0 if successful.
 */
int sequence_load(sequence_t *sequence, FILE *file, int version);

/**
 * Saves a sequence to a file.
 * @param sequence Sequence
 * @param file File
 * @param version Version
 * @return Returns 0 if successful.
 */
int sequence_save(sequence_t *sequence, FILE *file, int version);

#endif /*__SEQUENCE_H__*/
