
#include <assert.h>
#include <stdlib.h>

#include "log.h"
#include "config.h"
#include "core.h"
#include "mcontrol.h"
#include "pattern.h"
#include "line.h"
#include "seq.h"
#include "screen.h"
#include "mmi.h"

#define NUM_GLOBAL_PARAMS     8

#define CC_STEP_VALUE_FIRST   1
#define CC_STEP_VALUE_COUNT   32

#define CC_STEP_MODE_FIRST    33
#define CC_STEP_MODE_COUNT    32

#define CC_LINE_FIRST         65
#define CC_LINE_COUNT         8

#define CC_SEQUENCE_FIRST     73
#define CC_SEQUENCE_COUNT     4

#define CC_LINE_PARAM_FIRST   81
#define CC_LINE_PARAM_COUNT   NUM_LINE_PARAMS

#define CC_GLOBAL_PARAM_FIRST 97
#define CC_GLOBAL_PARAM_COUNT NUM_GLOBAL_PARAMS

#define CC_BUTTON_F1          77
#define CC_BUTTON_F2          78
#define CC_BUTTON_F3          79
#define CC_BUTTON_F4          80
#define CC_BUTTON_PLAY        105
#define CC_BUTTON_STOP        106
#define CC_BUTTON_PREV        107
#define CC_BUTTON_NEXT        108


/** cc buttons */
typedef enum {
	BUTTON_CC_F1,
	BUTTON_CC_F2,
	BUTTON_CC_F3,
	BUTTON_CC_F4,
	BUTTON_CC_PLAY,
	BUTTON_CC_STOP,
	BUTTON_CC_PREV,
	BUTTON_CC_NEXT,
	BUTTON_CC_LAST,
} button_cc_t;

/** cc button table entry */
typedef struct {
	button_cc_t id;
	int cc;
} button_cc_entry_t;

/** cc button table */
static button_cc_entry_t s_button_cc_table[] = {
	{ BUTTON_CC_F1, CC_BUTTON_F1 },
	{ BUTTON_CC_F2, CC_BUTTON_F2 },
	{ BUTTON_CC_F3, CC_BUTTON_F3 },
	{ BUTTON_CC_F4, CC_BUTTON_F4 },
	{ BUTTON_CC_PLAY, CC_BUTTON_PLAY },
	{ BUTTON_CC_STOP, CC_BUTTON_STOP },
	{ BUTTON_CC_PREV, CC_BUTTON_PREV },
	{ BUTTON_CC_NEXT, CC_BUTTON_NEXT },
};

/** global parameters */
static param_t *s_global_params[NUM_GLOBAL_PARAMS];


static config_t *s_config;
static mctrl_t s_mctrl;
static mmi_state_t s_mmi_state;
static pattern_t *s_pattern;

static void cc_changed(mctrl_t *mctrl, int cc, int value);
static void step_value_changed(int step, int value);
static void step_mode_changed(int step);
static void line_changed(int line);
static void sequence_changed(int sequence);
static void line_param_changed(int index, int value);
static void global_param_changed(int index, int value);
static void button_cc_changed(button_cc_entry_t *entry, int value);
static void button_cc_pressed(button_cc_t button);
static void show_selected_line(int index);
static void show_selected_sequence(int index);
static void show_line_steps(line_t *line);
static void show_line_params(line_t *line);
static void show_global_params(void);
static void handle_beat_blink(void);
static void line_mode_changed(line_t *line);
static void first_last_changed(line_t *line);

/*
 * Initializes the mmi.
 */
int mmi_init(void)
{
	s_config = core_get_config();
	s_pattern = seq_get_pattern();
	
	/* init screen */
	if (scr_init() != 0)
		return -1;
	
	/* init controller */
	if (mctrl_init(&s_mctrl, mio_get_input_device_by_name(s_config->control_input), 
		mio_get_output_device_by_name(s_config->control_output)) != 0)
		return -1;
		
	/* set callbacks */
	mctrl_get_callbacks(&s_mctrl)->cc_changed = cc_changed;
	
	/* set global params */
	s_global_params[7] = &s_pattern->tempo;
	
	/* show initial state */
	sequence_changed(0);
	show_global_params();
	
	return 0;
}

/*
 * Shuts the mmi down.
 */
void mmi_shutdown(void)
{
	scr_shutdown();
	
	mctrl_shutdown(&s_mctrl);
}

/*
 * Updates the mmi.
 */
void mmi_update(void)
{
	s_mmi_state.last_edited_step = -1;
	
	mctrl_update(&s_mctrl);
	
	scr_update();
	
	handle_beat_blink();
}

/*
 * Called from the sequencer thread.
 */
void mmi_pulse(int pulse, mio_timestamp_t timestamp)
{
	if ((pulse % 24) == 0)
		s_mmi_state.beat_blink = 1;
		
	scr_dirty();
}

/*
 * Returns the mmi state.
 */
mmi_state_t *mmi_get_state(void)
{
	return &s_mmi_state;
}

/**
 * Callback called when a registered cc control changed.
 * @param mctrl Midi controller
 * @param cc Midi cc
 */
static void cc_changed(mctrl_t *mctrl, int cc, int value)
{
	button_cc_entry_t *button_cc;
	int i;
	
	if (cc >= CC_STEP_VALUE_FIRST && cc < CC_STEP_VALUE_FIRST + CC_STEP_VALUE_COUNT)
		step_value_changed(cc - CC_STEP_VALUE_FIRST, value);
	else if (cc >= CC_STEP_MODE_FIRST && cc < CC_STEP_MODE_FIRST + CC_STEP_MODE_COUNT)
		step_mode_changed(cc - CC_STEP_MODE_FIRST);
	else if (cc >= CC_LINE_FIRST && cc < CC_LINE_FIRST + CC_LINE_COUNT)
		line_changed(cc - CC_LINE_FIRST);
	else if (cc >= CC_SEQUENCE_FIRST && cc < CC_SEQUENCE_FIRST + CC_SEQUENCE_COUNT)
		sequence_changed(cc - CC_SEQUENCE_FIRST);
	else if (cc >= CC_LINE_PARAM_FIRST && cc < CC_LINE_PARAM_FIRST + CC_LINE_PARAM_COUNT)
		line_param_changed(cc - CC_LINE_PARAM_FIRST, value);
	else if (cc >= CC_GLOBAL_PARAM_FIRST && cc < CC_GLOBAL_PARAM_FIRST + CC_GLOBAL_PARAM_COUNT)
		global_param_changed(cc - CC_GLOBAL_PARAM_FIRST, value);
	else {
		/* check buttons */
		for (i = 0; i < BUTTON_CC_LAST; i++) {
			button_cc = &s_button_cc_table[i];
			if (button_cc->cc == cc) {
				button_cc_changed(button_cc, value);
				return;
			}
		}
#if 0		
		/* check params */
		for (i = 0; i < PARAM_CC_LAST; i++) {
			param_cc = &s_param_cc_table[i];
			if (param_cc->cc == cc) {
				param_cc_changed(param_cc, value);
				return;
			}
		}
#endif
	}	
}

/**
 * Called when a step value has changed.
 */
static void step_value_changed(int step, int value)
{
	param_t *param = &s_mmi_state.line->step_values[step];
	param_set_rel_cc(param, value);
	s_mmi_state.last_edited_step = step;
	scr_dirty();
}

/**
 * Called when a step enabled value has changed.
 */
static void step_mode_changed(int step)
{
	param_inc(&s_mmi_state.line->step_modes[step]);
	s_mmi_state.last_edited_step = step;
	scr_dirty();
}

/**
 * Called when a line button has been pressed.
 */
static void line_changed(int line)
{
	s_mmi_state.line_index = line;
	s_mmi_state.line = &s_mmi_state.sequence->lines[line];
	s_mmi_state.line->line_mode_changed = line_mode_changed;
	s_mmi_state.line->first_last_changed = first_last_changed;
	show_selected_line(line);
	show_line_steps(s_mmi_state.line);
	show_line_params(s_mmi_state.line);
	scr_dirty();
}

static void sequence_changed(int sequence)
{
	s_mmi_state.sequence_index = sequence;
	s_mmi_state.sequence = &s_pattern->sequences[sequence];
	show_selected_sequence(sequence);
	line_changed(0);
}

static void line_param_changed(int index, int value)
{
	param_t *param = s_mmi_state.line->params[index];
	
	if (!param)
		return;
		
	param_set_rel_cc(param, value);
	scr_dirty();
}

static void global_param_changed(int index, int value)
{
	param_t *param = s_global_params[index];
	
	if (!param)
		return;
		
	param_set_rel_cc(param, value);
	scr_dirty();
}

static void button_cc_changed(button_cc_entry_t *entry, int value)
{
	mctrl_cc_set(&s_mctrl, entry->cc, 0);
	if (value == 127)
		button_cc_pressed(entry->id);
}

static void button_cc_pressed(button_cc_t button)
{
	switch (button) {
	case BUTTON_CC_F1:
		LOG(LOG_INFO, "F1");
		LOG(LOG_INFO, "saving ...");
		pattern_save(s_pattern, "test.pat");
		break;
	case BUTTON_CC_F2:
		LOG(LOG_INFO, "F2");
		LOG(LOG_INFO, "loading ...");
		pattern_load(s_pattern, "test.pat");
		break;
	case BUTTON_CC_F3:
		LOG(LOG_INFO, "F3");
		break;
	case BUTTON_CC_F4:
		LOG(LOG_INFO, "F4");
		break;
	case BUTTON_CC_PLAY:
		LOG(LOG_INFO, "PLAY");
		seq_start();
		break;
	case BUTTON_CC_STOP:
		LOG(LOG_INFO, "STOP");
		seq_stop();
		break;
	case BUTTON_CC_PREV:
		LOG(LOG_INFO, "PREV");
		break;
	case BUTTON_CC_NEXT:
		LOG(LOG_INFO, "NEXT");
		break;
	default:
		break;
	}
	
	scr_dirty();
}

/**
 * Show the selected line by setting all line buttons.
 */
static void show_selected_line(int index)
{
	int i;

	for (i = 0; i < CC_LINE_COUNT; i++)
		mctrl_cc_set(&s_mctrl, CC_LINE_FIRST + i, i == index ? 127 : 0);
}

/**
 * Show the selected sequence by setting all sequence buttons.
 */
static void show_selected_sequence(int index)
{
	int i;

	for (i = 0; i < CC_SEQUENCE_COUNT; i++)
		mctrl_cc_set(&s_mctrl, CC_SEQUENCE_FIRST + i, i == index ? 127 : 0);
}


static void show_line_steps(line_t *line)
{
	int i;
	
	for (i = 0; i < NUM_STEPS; i++)
		mctrl_cc_set(&s_mctrl, CC_STEP_VALUE_FIRST + i, param_get_cc(&line->step_values[i]));
	
}

static void show_line_params(line_t *line)
{
	int i;
	param_t *param;
	
	for (i = 0; i < NUM_LINE_PARAMS; i++) {
		param = line->params[i];
		if (!param)
			continue;
		mctrl_cc_set(&s_mctrl, CC_LINE_PARAM_FIRST + i, param_get_cc(param));
	}
}

static void show_global_params(void)
{
	int i;
	param_t *param;
	
	for (i = 0; i < NUM_GLOBAL_PARAMS; i++) {
		param = s_global_params[i];
		if (!param)
			continue;
		mctrl_cc_set(&s_mctrl, CC_GLOBAL_PARAM_FIRST + i, param_get_cc(param));
	}
}

static void handle_beat_blink(void)
{
	if (s_mmi_state.beat_blink > 0) {
		if (s_mmi_state.beat_blink == 1) {
			mctrl_cc_set(&s_mctrl, CC_BUTTON_PLAY, 127);
		} else if (s_mmi_state.beat_blink > 10) {
			mctrl_cc_set(&s_mctrl, CC_BUTTON_PLAY, 0);
			s_mmi_state.beat_blink = 0;
			return;
		}
		s_mmi_state.beat_blink++;
	}
}

/**
 * Gets called when a line has changed it's mode.
 */
static void line_mode_changed(line_t *line)
{
	// FIXME assert(line == s_mmi_state.line);

	show_line_steps(s_mmi_state.line);
	show_line_params(s_mmi_state.line);
}

/**
 * Gets called when a line has changed it's first or last step.
 */
static void first_last_changed(line_t *line)
{
	assert(line == s_mmi_state.line);

	show_line_params(s_mmi_state.line);
}
