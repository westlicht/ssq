
#include <stdlib.h>

#include "log.h"
#include "param.h"
#include "mout.h"
#include "line.h"

static void do_step(line_t *line, mio_timestamp_t timestamp);
static void start_step(line_t *line, mio_timestamp_t timestamp);
static void stop_step(line_t *line, mio_timestamp_t timestamp);
static int get_line_output(line_t *line, int step);
static int get_param(line_t *line, param_t *param);
static void line_mode_changed(param_t *param);
static void first_step_changed(param_t *param);
static void last_step_changed(param_t *param);
static void set_line_mode(line_t *line, int mode);
static void set_steps_class(line_t *line, param_class_t class);

/*
 * Initializes a line.
 */
void line_init(line_t *line, int index, sequence_t *sequence)
{
	line->sequence = sequence;
	line->index = index;

	param_init(&line->line_mode, PARAM_CLASS_LINE_MODE, line, 0);
	param_set_changed(&line->line_mode, line_mode_changed);
	param_init(&line->play_mode, PARAM_CLASS_PLAY_MODE, line, PARAM_FLAG_CAN_CONNECT);
	param_init(&line->first_step, PARAM_CLASS_FIRST_STEP, line, 0);
	param_set_changed(&line->first_step, first_step_changed);
	param_init(&line->last_step, PARAM_CLASS_LAST_STEP, line, 0);
	param_set_changed(&line->last_step, last_step_changed);
	param_set(&line->last_step, NUM_STEPS - 1);
	param_init(&line->sync_mode, PARAM_CLASS_SYNC_MODE, line, 0);
	param_init(&line->sync_base, PARAM_CLASS_SYNC_BASE, line, 0);
	param_init(&line->note, PARAM_CLASS_NOTE, line, 0);
	param_init(&line->gate, PARAM_CLASS_GATE, line, PARAM_FLAG_CAN_CONNECT);
	param_init(&line->length, PARAM_CLASS_LENGTH, line, PARAM_FLAG_CAN_CONNECT);
	param_init(&line->midi_port, PARAM_CLASS_MIDI_PORT, line, PARAM_FLAG_CAN_CONNECT);
	param_init(&line->midi_cc, PARAM_CLASS_MIDI_CC, line, 0);
	param_init(&line->velocity, PARAM_CLASS_VELOCITY, line, PARAM_FLAG_CAN_CONNECT);
	param_init(&line->add, PARAM_CLASS_ADD, line, PARAM_FLAG_CAN_CONNECT);
	
	line->line_mode_changed = NULL;
	line->first_last_changed = NULL;
	
	set_line_mode(line, LINE_MODE_OFF);
	
	line->played_note = NULL;
	line_reset(line);	
}

/*
 * Resets a line.
 */
void line_reset(line_t *line)
{
	mout_stop_note(line->played_note, mio_get_timestamp());
	line->played_note = NULL;
	
	line->pulses = 0;
	line->cur_step = -1;
	line->prev_step = -1;
	line->direction = 1;
};

/*
 * Process a single pulse.
 */
void line_pulse(line_t *line, int pulse, mio_timestamp_t timestamp)
{
	int gate = get_param(line, &line->gate);
	int length = get_param(line, &line->length);
	
	if ((line->pulses % gate) == 0) {
		do_step(line, timestamp);
		line->pulses = 1;
	} else {
		line->pulses++;
		if (line->pulses >= length)
			stop_step(line, timestamp);
	}
}

/*
 * Loads a line from a file.
 */
int line_load(line_t *line, FILE *file, int version)
{
	int i;

	/* load line parameters */
	param_load(&line->line_mode, file);
	param_load(&line->play_mode, file);
	param_load(&line->first_step, file);
	param_load(&line->last_step, file);
	param_load(&line->sync_mode, file);
	param_load(&line->sync_base, file);
	param_load(&line->gate, file);
	param_load(&line->length, file);
	param_load(&line->midi_port, file);
	param_load(&line->midi_cc, file);
	param_load(&line->velocity, file);
	param_load(&line->add, file);
	
	/* load step parameters */
	for (i = 0; i < NUM_STEPS; i++)
		param_load(&line->step_values[i], file);
	
	for (i = 0; i < NUM_STEPS; i++)
		param_load(&line->step_modes[i], file);
		
	return 0;
}

/*
 * Saves a line to a file.
 */
int line_save(line_t *line, FILE *file, int version)
{
	int i;

	/* save line parameters */
	param_save(&line->line_mode, file);
	param_save(&line->play_mode, file);
	param_save(&line->first_step, file);
	param_save(&line->last_step, file);
	param_save(&line->sync_mode, file);
	param_save(&line->sync_base, file);
	param_save(&line->gate, file);
	param_save(&line->length, file);
	param_save(&line->midi_port, file);
	param_save(&line->midi_cc, file);
	param_save(&line->velocity, file);
	param_save(&line->add, file);
	
	/* save step parameters */
	for (i = 0; i < NUM_STEPS; i++)
		param_save(&line->step_values[i], file);
	
	for (i = 0; i < NUM_STEPS; i++)
		param_save(&line->step_modes[i], file);
		
	return 0;
}

/**
 * Advance a single step.
 * @param line Line
 * @param timestamp Timestamp
 */
static void do_step(line_t *line, mio_timestamp_t timestamp)
{
	int play_mode = get_param(line, &line->play_mode);
	int first = get_param(line, &line->first_step);
	int last = get_param(line, &line->last_step);
	
	switch (play_mode) {
	case PLAY_MODE_FWD:
		line->direction = 1;
		line->cur_step++;
		if (line->cur_step > last)
			line->cur_step = first;
		break;
	case PLAY_MODE_BWD:
		line->direction = -1;
		line->cur_step--;
		if (line->cur_step < first)
			line->cur_step = last;
		break;
	case PLAY_MODE_PINGPONG:
		line->cur_step += line->direction;
		if (line->direction > 0) {
			if (line->cur_step > last) {
				line->direction = -1;
				line->cur_step = last - 1;
			}
		} else {
			if (line->cur_step < first) {
				line->direction = 1;
				line->cur_step = first + 1;
			}
		}
		break;
	case PLAY_MODE_FWD_BWD:
		line->cur_step += line->direction;
		if (line->direction > 0) {
			if (line->cur_step > last) {
				line->direction = -1;
				line->cur_step = last;
			}
		} else {
			if (line->cur_step < first) {
				line->direction = 1;
				line->cur_step = first;
			}
		}
		break;
	case PLAY_MODE_RANDOM:
		line->cur_step = first + random() % (last - first + 1);
		break;
	}
	
	/* make sure step is in the first-last interval */
	line->cur_step = line->cur_step < first ? first : line->cur_step;
	line->cur_step = line->cur_step > last ? last : line->cur_step;
	
	/* check if we should skip that step */
	/* FIXME this could be blocking if all steps are skipped */
	if (param_get_enum(&line->step_modes[line->cur_step]) == STEP_MODE_SKIP)
		do_step(line, timestamp);
		
	/* get current output */
	param_set(&line->output, get_line_output(line, line->cur_step));

	/* trigger a step */		
	start_step(line, timestamp);
}

/**
 * Starts the current step. E.g. plays a note, outputs cc etc.
 * @param line Line
 * @param timestamp Timestamp
 */
static void start_step(line_t *line, mio_timestamp_t timestamp)
{
	int line_mode = get_param(line, &line->line_mode);
	
	int midi_port = get_param(line, &line->midi_port);
	int id = midi_port / 16;
	int channel = midi_port % 16;
	int note, vel, cc, value;
	mout_note_t *new_note;
	
	switch (line_mode) {
	case LINE_MODE_NOTE:
		note = get_param(line, &line->output);
		vel = get_param(line, &line->velocity);
		new_note = mout_play_note(id, channel, note, vel, timestamp);
		mout_stop_note(line->played_note, timestamp + 1);
		line->played_note = new_note;
		break;
	case LINE_MODE_CTRL:
		cc = get_param(line, &line->midi_cc);
		value = get_param(line, &line->output);
		LOG(LOG_INFO, "set cc %d to %d", cc, value);
		mout_set_cc(id, channel, cc, value, timestamp);
		break;
	}
}

/**
 * Stops the previous step. E.g. stops a note etc.
 * @param line Line
 * @param timestamp Timestamp
 */
static void stop_step(line_t *line, mio_timestamp_t timestamp)
{
	int line_mode = get_param(line, &line->line_mode);
	
	switch (line_mode) {
	case LINE_MODE_NOTE:
		mout_stop_note(line->played_note, timestamp + 1);
		line->played_note = NULL;
		break;
	case LINE_MODE_CTRL:
		break;
	}
}

static int get_line_output(line_t *line, int step)
{
	int line_mode = get_param(line, &line->line_mode);
	
	switch (line_mode) {
	case LINE_MODE_NOTE:
		return get_param(line, &line->note) + get_param(line, &line->add) + get_param(line, &line->step_values[step]); 
	case LINE_MODE_VEL:
		return get_param(line, &line->add) + get_param(line, &line->step_values[step]); 
	case LINE_MODE_GATE:
		return get_param(line, &line->step_values[step]); 
	case LINE_MODE_LEN:
		return get_param(line, &line->step_values[step]); 
	case LINE_MODE_MIDI:
		return get_param(line, &line->step_values[step]); 
	case LINE_MODE_ADD:
		return get_param(line, &line->add) + get_param(line, &line->step_values[step]); 
	case LINE_MODE_CTRL:
		return get_param(line, &line->add) + get_param(line, &line->step_values[step]); 
	case LINE_MODE_MODE:
		return get_param(line, &line->step_values[step]); 
	default:
		return 0;
	}
}

static int get_param(line_t *line, param_t *param)
{
	int index;
	int step;
	int sync;
	param_t *target;

	if (param_is_connected(param)) {
		index  = param_get_connected_index(param);
		sync = index / NUM_LINES;
		index %= NUM_LINES;
		step = (line->cur_step + NUM_STEPS) % NUM_STEPS;
		target = &line->sequence->lines[index].output;
			
		if (param_is_valid_connection(param->class_def->class, target->class_def->class)) {
			return sync ? get_line_output(&line->sequence->lines[index], step) : param_get(target);
		} else {
			return param_get_default_value(param);
		}
	}
	
	return param_get_value(param);
}

static void line_mode_changed(param_t *param)
{
	set_line_mode(param->owner, param_get_enum(param));
}

static void first_step_changed(param_t *param)
{
	line_t *line = param->owner;

	if (param_get(param) > param_get(&line->last_step)) {
		param_set(&line->last_step, param_get(param));
		if (line->first_last_changed)
			line->first_last_changed(line);
	}
	
}

static void last_step_changed(param_t *param)
{
	line_t *line = param->owner;

	if (param_get(param) < param_get(&line->first_step)) {
		param_set(&line->first_step, param_get(param));
		if (line->first_last_changed)
			line->first_last_changed(line);
	}
}

static void set_line_mode(line_t *line, int mode)
{
	int i;
	
	for (i = 0; i < NUM_LINE_PARAMS; i++)
		line->params[i] = NULL;
		
	/* add line mode parameter */
	line->params[0] = &line->line_mode;

	/* exit if line mod is off */		
	if (mode == LINE_MODE_OFF) {
		set_steps_class(line, PARAM_CLASS_NONE);
		goto out;
	}
		
	/* add default parameters */
	line->params[1] = &line->play_mode;
	line->params[2] = &line->first_step;
	line->params[3] = &line->last_step;
	line->params[4] = &line->sync_mode;
	line->params[5] = &line->sync_base;
	line->params[6] = &line->gate;

	switch (mode) {
	case LINE_MODE_NOTE:
		line->params[7] = &line->length;
		line->params[8] = &line->note;
		line->params[9] = &line->velocity;
		line->params[10] = &line->midi_port;
		line->params[15] = &line->add;
		set_steps_class(line, PARAM_CLASS_NOTE_OFS);
		param_init(&line->output, PARAM_CLASS_NOTE, line, 0);
		break;
	case LINE_MODE_VEL:
		line->params[15] = &line->add;
		set_steps_class(line, PARAM_CLASS_VELOCITY);
		break;
	case LINE_MODE_GATE:
		set_steps_class(line, PARAM_CLASS_GATE);
		break;
	case LINE_MODE_LEN:
		set_steps_class(line, PARAM_CLASS_LENGTH);
		break;
	case LINE_MODE_MIDI:
		set_steps_class(line, PARAM_CLASS_MIDI_PORT);
		break;
	case LINE_MODE_ADD:
		line->params[15] = &line->add;
		set_steps_class(line, PARAM_CLASS_ADD);
		break;
	case LINE_MODE_CTRL:
		line->params[7] = &line->midi_cc;
		line->params[8] = &line->midi_port;
		line->params[15] = &line->add;
		set_steps_class(line, PARAM_CLASS_ADD);
		break;
	case LINE_MODE_MODE:
		set_steps_class(line, PARAM_CLASS_PLAY_MODE);
		
		break;
	}
	
out:
	if (line->line_mode_changed)
		line->line_mode_changed(line);
}

static void set_steps_class(line_t *line, param_class_t class)
{
	int i;
	
	for (i = 0; i < NUM_STEPS; i++)
		param_init(&line->step_values[i], class, line, 0);
	
	for (i = 0; i < NUM_STEPS; i++)
		param_init(&line->step_modes[i], PARAM_CLASS_STEP_MODE, line, 0);
		
	param_init(&line->output, class, line, 0);
}
