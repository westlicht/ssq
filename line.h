#ifndef __LINE_H__
#define __LINE_H__

#include "defines.h"
#include "mio.h"
#include "mout.h"
#include "param.h"
#include "objects.h"

/**
 * Initializes a line.
 * @param line Line
 * @param index Line index in pattern
 * @param sequence Parent sequence
 */
void line_init(line_t *line, int index, sequence_t *sequence);

/**
 * Resets a line.
 * @param line Line
 */
void line_reset(line_t *line);

/**
 * Process a single pulse.
 * @param line Line
 * @param pulse Pulse
 * @param timestamp Timestamp
 */
void line_pulse(line_t *line, int pulse, mio_timestamp_t timestamp);

/**
 * Loads a line from a file.
 * @param line Line
 * @param file File
 * @param version Version
 * @return Returns 0 if successful.
 */
int line_load(line_t *line, FILE *file, int version);

/**
 * Saves a line to a file.
 * @param line Line
 * @param file File
 * @param version Version
 * @return Returns 0 if successful.
 */
int line_save(line_t *line, FILE *file, int version);

#endif /*__LINE_H__*/
