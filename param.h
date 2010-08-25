#ifndef __PARAM_H__
#define __PARAM_H__

#include <stdio.h>

#include "param_class.h"

/* parameter flags */
#define PARAM_FLAG_CAN_CONNECT (1 << 0)

typedef struct param param_t;

typedef void (* param_changed_t) (param_t *param);

/** parameter */
struct param {
	param_class_def_t *class_def; /**< parameter class definition */
	int value;                    /**< current value */
	void *owner;                  /**< parameter owner */
	int flags;                    /**< flags */
	int cc_acc;                   /**< cc accumulator */
	/* callbacks */
	param_changed_t changed;      /**< value changed callback */
};

/**
 * Inits a parameter.
 * @param param Parameter
 * @param class Parameter class identifier
 * @param owner Parameter owner
 * @param flags Parameter flags
 */
void param_init(param_t *param, param_class_t class, void *owner, int flags);

/**
 * Sets the 'changed' callback function.
 * @param Parameter
 * @param changed Changed callback
 */
void param_set_changed(param_t *param, param_changed_t changed);

/**
 * Sets a parameter.
 * @param param Parameter
 * @param value Value
 */
void param_set(param_t *param, int value);

/**
 * Sets a parameter from a cc value (0-127).
 * @param param Parameter
 * @param value Value
 */ 
void param_set_cc(param_t *param, int value);

/**
 * Sets a parameter from a relative value.
 * @param param Parameter
 * @param value Relative value
 */
void param_set_rel_cc(param_t *param, int value);

/**
 * Increments the parameter by 1.
 * @param param Parameter
 */
void param_inc(param_t *param);

/**
 * Decrements the parameter by 1.
 * @param param Parameter
 */
void param_dec(param_t *param);

/**
 * Gets a parameter.
 * @param param Parameter
 * @return Returns parameters value.
 */
int param_get(param_t *param);

/**
 * Gets a parameter in cc value form (0-127).
 * @param param Parameter
 * @return Returns the parameters cc value.
 */
int param_get_cc(param_t *param);

/**
 * Gets a parameters enum value (from lookup table).
 * @param param Parameter
 * @return Returns the enum value.
 */
int param_get_enum(param_t *param);

/**
 * Gets the parameters value. If parameter is an enum the value is
 * looked up in the table.
 * @param param Parameter
 * @return Returns the parameters value.
 */
int param_get_value(param_t *param);

/**
 * Gets the parameters default value. If parameter is an enum the value is
 * looked up in the table.
 * @param param Parameter
 * @return Returns the parameters default value.
 */
int param_get_default_value(param_t *param);

/**
 * Gets a parameters value as a formatted string.
 * @param param Parameter
 * @param str String buffer
 * @param len Length of string buffer
 */
void param_get_str(param_t *param, char *str, int len);

/**
 * Returns the parameter class name.
 * @return Returns the parameter class name.
 */
char *param_get_name(param_t *param);

/**
 * Returns 1 if parameter is connected.
 * @param param Parameter
 * @return Returns 1 if parameter is connected, 0 otherwise.
 */
int param_is_connected(param_t *param);

/**
 * Returns the connected index if parameter is connected, -1 otherwise.
 * @param param Parameter
 * @return Returns the connected index.
 */
int param_get_connected_index(param_t *param);

/**
 * Loads a parameter from a file.
 * @param param Parameter
 * @param file File
 * @return Returns 0 if successful.
 */
int param_load(param_t *param, FILE *file);

/**
 * Saves a parameter to a file.
 * @param param Parameter
 * @param file File
 * @return Returns 0 if successful.
 */
int param_save(param_t *param, FILE *file);

#endif /*__PARAM_H__*/
