#ifndef __PARAM_CLASS_H__
#define __PARAM_CLASS_H__

#include "defines.h"

/** parameter class identifiers */
typedef enum {
	PARAM_CLASS_NONE,
	PARAM_CLASS_LINE_MODE,
	PARAM_CLASS_PLAY_MODE,
	PARAM_CLASS_FIRST_STEP,
	PARAM_CLASS_LAST_STEP,
	PARAM_CLASS_GATE,
	PARAM_CLASS_LENGTH,
	PARAM_CLASS_NOTE,
	PARAM_CLASS_NOTE_OFS,
	PARAM_CLASS_STEP_MODE,
	PARAM_CLASS_SYNC_MODE,
	PARAM_CLASS_SYNC_BASE,
	PARAM_CLASS_MIDI_PORT,
	PARAM_CLASS_MIDI_CC,
	PARAM_CLASS_VELOCITY,
	PARAM_CLASS_ADD,
	PARAM_CLASS_BPM,
	PARAM_CLASS_LAST,
} param_class_t;

/** parameter types */
typedef enum {
	PARAM_INT,
	PARAM_ENUM,
} param_typ_t;

/** enum lookup table entry */
typedef struct {
	char *name;
	int value;
} enum_entry_t;

/** macro to compute enum table size */
#define ENUM_TABLE_SIZE(x) (sizeof(x) / sizeof(enum_entry_t))
#define ENUM_TABLE_MAX(x) (ENUM_TABLE_SIZE(x) - 1)

typedef struct param_class_def param_class_def_t;

/** parameter class definition */
struct param_class_def {
	param_class_t class;
	char *name;
	param_typ_t typ;
	int def;
	int min;
	int max;
	int cc_sens;
	enum_entry_t *enum_table;
	void (* print_value) (param_class_def_t *class_def, int value, char *str, int len);
};

/**
 * Returns the class definition.
 * @param class Class identifier
 * @return Returns a pointer to the class definition.
 */ 
param_class_def_t *param_class_get_def(param_class_t class);

/**
 * Inits the valid connection table.
 */
void param_init_param_connections(void);

/**
 * Defines class2 as valid connection target of class1.
 * @param class1 Class 1
 * @param class2 Class 2
 */
void param_set_valid_connection(param_class_t class1, param_class_t class2);

/**
 * Returns 1 if class2 is a valid connection target of class1.
 * Otherwise 0 is returned.
 * @param class1 Class 1
 * @param class2 Class 2
 * @return Returns 1 for a valid connection.
 */
int param_is_valid_connection(param_class_t class1, param_class_t class2);

#endif /*__PARAM_CLASS_H__*/
