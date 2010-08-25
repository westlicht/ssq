
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "defines.h"
#include "param_class.h"

/* ENUM TABLE DEFINITIONS -------------------------------------------------- */ 

/* line mode table */
static enum_entry_t s_enum_table_line_mode[] = {
	{ "Off",       LINE_MODE_OFF },
	{ "Note",      LINE_MODE_NOTE },
	{ "Velocity",  LINE_MODE_VEL },
	{ "Gate",      LINE_MODE_GATE },
	{ "Length",    LINE_MODE_LEN },
	{ "MIDI",      LINE_MODE_MIDI },
	{ "Add",       LINE_MODE_ADD },
	{ "Control",   LINE_MODE_CTRL },
	{ "Play Mode", LINE_MODE_MODE },
};

/* gate table */
static enum_entry_t s_enum_table_gate[] = {
	{ "1/32", 3 },
	{ "1/16", 6 },
	{ "1/8",  12 },
	{ "1/4",  24 },
	{ "1/2",  48 },
	{ "1/1",  96 },
	{ "2/1",  192 },
	{ "4/1",  384 },
	{ "8/1",  768 },
	{ "16/1", 1536 },
};

/* play mode table */
static enum_entry_t s_enum_table_play_mode[] = {
	{ ">>>",      PLAY_MODE_FWD },
	{ "<<<",      PLAY_MODE_BWD },
	{ "<->", PLAY_MODE_PINGPONG },
	{ "<=>",  PLAY_MODE_FWD_BWD },
	{ "???",   PLAY_MODE_RANDOM },
};

/* step mode table */
static enum_entry_t s_enum_table_step_mode[] = {
	{ "On",   STEP_MODE_ON },
	{ "Off",  STEP_MODE_OFF },
	{ "Skip", STEP_MODE_SKIP },
};

/* sync mode table */
static enum_entry_t s_enum_table_sync_mode[] = {
	{ "Auto",   SYNC_MODE_AUTO },
	{ "Manual", SYNC_MODE_MANUAL },
};

/* PARAMATER print_value FUNCTIONS ----------------------------------------- */

static void print_value_none(param_class_def_t *class_def, int value, char *str, int len);
static void print_value_int(param_class_def_t *class_def, int value, char *str, int len);
static void print_value_int_plus_one(param_class_def_t *class_def, int value, char *str, int len);
static void print_value_enum(param_class_def_t *class_def, int value, char *str, int len);
static void print_value_note(param_class_def_t *class_def, int value, char *str, int len);
static void print_value_note_ofs(param_class_def_t *class_def, int value, char *str, int len);
static void print_value_midi_port(param_class_def_t *class_def, int value, char *str, int len);

static char *s_note_table[] = { "C-", "C#", "D-", "D#", "E-", "F-", "F#", "G-", "G#", "A-", "A#", "B-" };

/* PARAMETER CLASS DEFINITIONS --------------------------------------------- */

static param_class_def_t s_param_classes[] = {
	{
		.class = PARAM_CLASS_NONE,
		.name = "",
		.typ = PARAM_INT,
		.def = 0,
		.min = 0,
		.max = 1,
		.cc_sens = 5,
		.enum_table = NULL,
		.print_value = print_value_none,
	}, {
		.class = PARAM_CLASS_LINE_MODE,
		.name = "Mode",
		.typ = PARAM_ENUM,
		.def = 0,
		.min = 0,
		.max = ENUM_TABLE_MAX(s_enum_table_line_mode),
		.cc_sens = 5,
		.enum_table = s_enum_table_line_mode,
		.print_value = print_value_enum,
	}, {
		.class = PARAM_CLASS_PLAY_MODE,
		.name = "Play Mode",
		.typ = PARAM_ENUM,
		.def = 0,
		.min = 0,
		.max = ENUM_TABLE_MAX(s_enum_table_play_mode),
		.cc_sens = 5,
		.enum_table = s_enum_table_play_mode,
		.print_value = print_value_enum,
	}, {
		.class = PARAM_CLASS_FIRST_STEP,
		.name = "First Step",
		.typ = PARAM_INT,
		.def = 0,
		.min = 0,
		.max = NUM_STEPS - 1,
		.cc_sens = 5,
		.enum_table = NULL,
		.print_value = print_value_int_plus_one,
	}, {
		.class = PARAM_CLASS_LAST_STEP,
		.name = "Last Step",
		.typ = PARAM_INT,
		.def = NUM_STEPS - 1,
		.min = 0,
		.max = NUM_STEPS - 1,
		.cc_sens = 5,
		.enum_table = NULL,
		.print_value = print_value_int_plus_one,
	}, {
		.class = PARAM_CLASS_GATE,
		.name = "Gate",
		.typ = PARAM_ENUM,
		.def = 0,
		.min = 0,
		.max = ENUM_TABLE_MAX(s_enum_table_gate),
		.cc_sens = 5,
		.enum_table = s_enum_table_gate,
		.print_value = print_value_enum,
	}, {
		.class = PARAM_CLASS_LENGTH,
		.name = "Length",
		.typ = PARAM_ENUM,
		.def = 0,
		.min = 0,
		.max = ENUM_TABLE_MAX(s_enum_table_gate),
		.cc_sens = 5,
		.enum_table = s_enum_table_gate,
		.print_value = print_value_enum,
	}, {
		.class = PARAM_CLASS_NOTE,
		.name = "Note",
		.typ = PARAM_INT,
		.def = 60,
		.min = 0,
		.max = 127,
		.cc_sens = 5,
		.enum_table = NULL,
		.print_value = print_value_note,
	}, {
		.class = PARAM_CLASS_NOTE_OFS,
		.name = "Note Offset",
		.typ = PARAM_INT,
		.def = 0,
		.min = -12,
		.max = 12,
		.cc_sens = 5,
		.enum_table = NULL,
		.print_value = print_value_note_ofs,
	}, {
		.class = PARAM_CLASS_STEP_MODE,
		.name = "Step Mode",
		.typ = PARAM_ENUM,
		.def = 0,
		.min = 0,
		.max = ENUM_TABLE_MAX(s_enum_table_step_mode),
		.cc_sens = 1,
		.enum_table = s_enum_table_step_mode,
		.print_value = print_value_enum,
	}, {
		.class = PARAM_CLASS_SYNC_MODE,
		.name = "Sync Mode",
		.typ = PARAM_INT,
		.def = 0,
		.min = 0,
		.max = ENUM_TABLE_MAX(s_enum_table_sync_mode),
		.cc_sens = 5,
		.enum_table = s_enum_table_sync_mode,
		.print_value = print_value_enum,
	}, {
		.class = PARAM_CLASS_SYNC_BASE,
		.name = "Sync Base",
		.typ = PARAM_ENUM,
		.def = 0,
		.min = 0,
		.max = ENUM_TABLE_MAX(s_enum_table_gate),
		.cc_sens = 5,
		.enum_table = s_enum_table_gate,
		.print_value = print_value_enum,
	}, {
		.class = PARAM_CLASS_MIDI_PORT,
		.name = "MIDI Port",
		.typ = PARAM_INT,
		.def = 0,
		.min = 0,
		.max = 31,
		.cc_sens = 5,
		.enum_table = NULL,
		.print_value = print_value_midi_port,
	}, {
		.class = PARAM_CLASS_MIDI_CC,
		.name = "Controller",
		.typ = PARAM_INT,
		.def = 0,
		.min = 0,
		.max = 127,
		.cc_sens = 5,
		.enum_table = NULL,
		.print_value = print_value_int,
	}, {
		.class = PARAM_CLASS_VELOCITY,
		.name = "Velocity",
		.typ = PARAM_INT,
		.def = 0,
		.min = 0,
		.max = 127,
		.cc_sens = 1,
		.enum_table = NULL,
		.print_value = print_value_int,
	}, {
		.class = PARAM_CLASS_ADD,
		.name = "Add",
		.typ = PARAM_INT,
		.def = 0,
		.min = -64,
		.max = 63,
		.cc_sens = 1,
		.enum_table = NULL,
		.print_value = print_value_int,
	}, {
		.class = PARAM_CLASS_BPM,
		.name = "BPM",
		.typ = PARAM_INT,
		.def = 130,
		.min = 30,
		.max = 250,
		.cc_sens = 1,
		.enum_table = NULL,
		.print_value = print_value_int,
	}
};

static int s_param_connect_table[PARAM_CLASS_LAST * PARAM_CLASS_LAST];




/*
 * Returns the class definition.
 */ 
param_class_def_t *param_class_get_def(param_class_t class)
{
	return &s_param_classes[class];
}

/*
 * Inits the valid connection table.
 */
void param_init_param_connections(void)
{
	param_set_valid_connection(PARAM_CLASS_GATE, PARAM_CLASS_GATE);
	param_set_valid_connection(PARAM_CLASS_LENGTH, PARAM_CLASS_GATE);
	param_set_valid_connection(PARAM_CLASS_PLAY_MODE, PARAM_CLASS_PLAY_MODE);
	param_set_valid_connection(PARAM_CLASS_MIDI_PORT, PARAM_CLASS_MIDI_PORT);
	param_set_valid_connection(PARAM_CLASS_MIDI_CC, PARAM_CLASS_MIDI_CC);
	param_set_valid_connection(PARAM_CLASS_VELOCITY, PARAM_CLASS_VELOCITY);
	param_set_valid_connection(PARAM_CLASS_ADD, PARAM_CLASS_ADD);
}

/*
 * Defines class2 as valid connection target of class1.
 */
void param_set_valid_connection(param_class_t class1, param_class_t class2)
{
	s_param_connect_table[class1 * PARAM_CLASS_LAST + class2] = 1;
}

/*
 * Returns 1 if class2 is a valid connection target of class1.
 * Otherwise 0 is returned.
 */
int param_is_valid_connection(param_class_t class1, param_class_t class2)
{
	return s_param_connect_table[class1 * PARAM_CLASS_LAST + class2];
}



/**
 * Prints a none value.
 */
static void print_value_none(param_class_def_t *class_def, int value, char *str, int len)
{
	strncpy(str, "-", len);
}

/**
 * Prints an integer value.
 */
static void print_value_int(param_class_def_t *class_def, int value, char *str, int len)
{
	snprintf(str, len, "%d", value);
}

/**
 * Prints an integer value plus one.
 */
static void print_value_int_plus_one(param_class_def_t *class_def, int value, char *str, int len)
{
	snprintf(str, len, "%d", value + 1);
}

/**
 * Prints an enum value.
 */
static void print_value_enum(param_class_def_t *class_def, int value, char *str, int len)
{
	snprintf(str, len, "%s", class_def->enum_table[value].name);
}

/**
 * Prints a midi note value.
 */
static void print_value_note(param_class_def_t *class_def, int value, char *str, int len)
{
	if (value >= 0 && value <= 119) {
		snprintf(str, len, "%s%d", s_note_table[value % 12], value / 12);
	} else {
		snprintf(str, len, "%s", "-");
	}
}

/**
 * Prints a note offset value.
 */
static void print_value_note_ofs(param_class_def_t *class_def, int value, char *str, int len)
{
	if (value == 0)
		strncpy(str, " 0", len);
	else
		value > 0 ? snprintf(str, len, "+%d", value) : snprintf(str, len, "%d", value);
}

static void print_value_midi_port(param_class_def_t *class_def, int value, char *str, int len)
{
	snprintf(str, len, "%s-%d", (value / 16) < 1 ? "A" : "B", (value % 16) + 1);
}
