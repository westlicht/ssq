#ifndef __PATTERN_H__
#define __PATTERN_H__

#include "defines.h"
#include "mio.h"
#include "param.h"
#include "objects.h"

/**
 * Initializes a pattern.
 * @param pattern Pattern
 */
void pattern_init(pattern_t *pattern);

/**
 * Clears a pattern.
 * @param pattern Pattern
 */
void pattern_clear(pattern_t *pattern);

/**
 * Resets a pattern.
 * @param pattern Pattern
 */
void pattern_reset(pattern_t *pattern);

/**
 * Process a single pulse.
 * @param pattern Pattern
 * @param pulse Pulse
 * @param timestamp Timestamp
 */
void pattern_pulse(pattern_t *pattern, int pulse, mio_timestamp_t timestamp);

/**
 * Loads a pattern from a file.
 * @param pattern Pattern
 * @param filename Filename
 * @return Returns 0 if successful.
 */
int pattern_load(pattern_t *pattern, const char *filename);

/**
 * Saves a pattern to a file.
 * @param pattern Pattern
 * @param filename Filename
 * @return Returns 0 if successful.
 */
int pattern_save(pattern_t *pattern, const char *filename);

#endif /*__PATTERN_H__*/
