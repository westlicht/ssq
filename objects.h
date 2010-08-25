#ifndef __OBJECTS_H__
#define __OBJECTS_H__

#include "defines.h"
#include "param.h"
#include "mout.h"

typedef struct sequence sequence_t;
typedef struct pattern pattern_t;
typedef struct line line_t;

/** max number of params per line */
#define NUM_LINE_PARAMS 16

typedef void (* line_mode_changed_t) (line_t *line);
typedef void (* first_last_changed_t) (line_t *line);

/* data used for a single line */
struct line {
	sequence_t *sequence;
	int index;
	param_t line_mode;
	param_t play_mode;
	param_t first_step;
	param_t last_step;
	param_t sync_mode;
	param_t sync_base;
	param_t note;
	param_t gate;
	param_t length;
	param_t midi_port;
	param_t midi_cc;
	param_t velocity;
	param_t add;

	param_t *params[NUM_LINE_PARAMS];
		
	param_t step_values[NUM_STEPS];
	param_t step_modes[NUM_STEPS];
	
	param_t output;
	
	int pulses;
	int cur_step;
	int prev_step;
	int direction;
	
	mout_note_t *played_note;
	
	line_mode_changed_t line_mode_changed;
	first_last_changed_t first_last_changed;
};

/** sequence */
struct sequence {
	pattern_t *pattern;
	int index;
	line_t lines[NUM_LINES];
	param_t *outputs[NUM_LINES];
};

/** pattern */
struct pattern {
	struct sequence sequences[NUM_SEQUENCES];
	param_t tempo;
};

#endif /*__OBJECTS_H__*/
