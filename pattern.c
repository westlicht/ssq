
#include <stdio.h>
#include <string.h>

#include "log.h"
#include "filedefs.h"
#include "param.h"
#include "sequence.h"
#include "seq.h"
#include "pattern.h"

static void tempo_changed(param_t *param);

/*
 * Initializes a pattern.
 */
void pattern_init(pattern_t *pattern)
{
	int i;
	
	param_init(&pattern->tempo, PARAM_CLASS_BPM, pattern, 0);
	param_set_changed(&pattern->tempo, tempo_changed);
	
	for (i = 0; i < NUM_SEQUENCES; i++)
		sequence_init(&pattern->sequences[i], i, pattern);
		
	pattern_clear(pattern);
}

/*
 * Clears a pattern.
 */
void pattern_clear(pattern_t *pattern)
{
	int i;
	
	for (i = 0; i < NUM_SEQUENCES; i++)
		sequence_clear(&pattern->sequences[i]);
}

/*
 * Resets a pattern.
 */
void pattern_reset(pattern_t *pattern)
{
	int i;
	
	for (i = 0; i < NUM_SEQUENCES; i++)
		sequence_reset(&pattern->sequences[i]);
}

/*
 * Process a single pulse.
 */
void pattern_pulse(pattern_t *pattern, int pulse, mio_timestamp_t timestamp)
{
	int i;
	
	for (i = 0; i < NUM_SEQUENCES; i++)
		sequence_pulse(&pattern->sequences[i], pulse, timestamp);
}

/*
 * Loads a pattern from a file.
 */
int pattern_load(pattern_t *pattern, const char *filename)
{
	int result = -1;
	FILE *file;
	int i;
	file_header_t header;
	
	/* open file */
	file = fopen(filename, "r");
	if (!file) {
		LOG(LOG_INFO, "cannot open file '%s' for reading", filename);
		goto out;
	}
		
	/* load header */
	if (fread(&header, sizeof(header), 1, file) != 1) {
		LOG(LOG_INFO, "cannot read from file '%s'", filename);
		goto out_close_file;
	}
	
	/* check magic */
	if (strncmp(header.magic, FILE_MAGIC, sizeof(header.magic)) != 0) {
		LOG(LOG_INFO, "wrong magic number in file '%s'", filename);
		goto out_close_file;
	}
	
	/* check version */
	if (header.version > FILE_VERSION) {
		LOG(LOG_INFO, "file '%s' has newer version than this release", filename);
		goto out_close_file;
	}	

	/* load pattern parameters */
	param_load(&pattern->tempo, file);
	
	/* load sequences */
	for (i = 0; i < NUM_SEQUENCES; i++)
		if (sequence_load(&pattern->sequences[i], file, header.version) != 0)
			goto out_close_file;
		
	result = 0;
	
out_close_file:
	fclose(file);
out:	
	return result;
}

/*
 * Saves a pattern to a file.
 */
int pattern_save(pattern_t *pattern, const char *filename)
{
	int result = -1;
	FILE *file;
	int i;
	file_header_t header;
	
	/* open file */
	file = fopen(filename, "w");
	if (!file) {
		LOG(LOG_INFO, "cannot open file '%s' for writing", filename);
		goto out;
	}

	/* write header */
	strncpy(header.magic, FILE_MAGIC, sizeof(header.magic));
	header.version = FILE_VERSION;
	if (fwrite(&header, sizeof(header), 1, file) != 1) {
		LOG(LOG_INFO, "cannot write to file '%s'", filename);
		goto out_close_file;
	}
	
	/* write pattern parameters */
	param_save(&pattern->tempo, file);
	
	/* write sequences */	
	for (i = 0; i < NUM_SEQUENCES; i++)
		if (sequence_save(&pattern->sequences[i], file, header.version) != 0)
			goto out_close_file;
		
	result = 0;
	
out_close_file:
	fclose(file);
out:	
	return result;
}

/**
 * Called when tempo is changed.
 */
static void tempo_changed(param_t *param)
{
	seq_set_tempo(param_get(param));
}
