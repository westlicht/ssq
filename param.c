
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "defines.h"
#include "param.h"

/*
 * Inits a parameter.
 */
void param_init(param_t *param, param_class_t class, void *owner, int flags)
{
	param->class_def = param_class_get_def(class);
	param->value = param->class_def->def;
	param->owner = owner;
	param->flags = flags;
	param->changed = NULL;
}

/*
 * Sets the 'changed' callback function.
 */
void param_set_changed(param_t *param, param_changed_t changed)
{
	param->changed = changed;
}

/*
 * Sets a parameter.
 */
void param_set(param_t *param, int value)
{
	int min = param->class_def->min;
	int max = param->class_def->max;
	
	if (param->flags & PARAM_FLAG_CAN_CONNECT)
		max += NUM_SOURCES;
		
	/* make sure value stays in min-max interval */
	value = value < min ? min : value;
	value = value > max ? max : value;
	
	/* change value */
	if (value != param->value) {
		param->value = value;
		if (param->changed)
			param->changed(param);
	}
}

/*
 * Sets a parameter from a cc value (0-127).
 */ 
void param_set_cc(param_t *param, int value)
{
	int min = param->class_def->min;
	int max = param->class_def->max;
	
	if (param->flags & PARAM_FLAG_CAN_CONNECT)
		max += NUM_SOURCES;

	param_set(param, min + ((float) value / 127.0) * (float) (max - min));
}

/*
 * Sets a parameter from a relative value.
 */
void param_set_rel_cc(param_t *param, int value)
{
	int steps;
	int sens;
	
	if (value & 0x40)
		value -= 0x80;
		
	sens = param->value > param->class_def->max ? 5 : param->class_def->cc_sens;
	
	param->cc_acc += value;
	steps = param->cc_acc / sens;
	if (steps) {
		param->cc_acc -= steps * sens;
		param_set(param, param->value + steps);
	}
}

/*
 * Increments the parameter by 1.
 */
void param_inc(param_t *param)
{
	int min = param->class_def->min;
	int max = param->class_def->max;
	
	if (param->flags & PARAM_FLAG_CAN_CONNECT)
		max += NUM_SOURCES;

	int new_value = param->value + 1;
	if (new_value > max)
		new_value = min;
	param_set(param, new_value);
}

/*
 * Decrements the parameter by 1.
 */
void param_dec(param_t *param)
{
	int min = param->class_def->min;
	int max = param->class_def->max;
	
	if (param->flags & PARAM_FLAG_CAN_CONNECT)
		max += NUM_SOURCES;

	int new_value = param->value - 1;
	if (new_value < min)
		new_value = max;
	param_set(param, new_value);
}

/*
 * Gets a parameter.
 */
int param_get(param_t *param)
{
	return param->value;
}

/*
 * Gets a parameter in cc value form (0-127).
 */
int param_get_cc(param_t *param)
{
	int min = param->class_def->min;
	int max = param->class_def->max;
	
	if (param->flags & PARAM_FLAG_CAN_CONNECT)
		max += NUM_SOURCES;

	return ((float) (param->value - min) / (float) (max - min)) * 127;
}

/*
 * Gets a parameters enum value (from lookup table).
 */
int param_get_enum(param_t *param)
{
	assert(param->class_def->typ == PARAM_ENUM);

	if (param->flags & PARAM_FLAG_CAN_CONNECT) {
		return param->value > param->class_def->max ? 0 : param->class_def->enum_table[param->value].value;
	} else {
		return param->class_def->enum_table[param->value].value;
	}
}

/**
 * Gets the parameters value. If parameter is an enum the value is
 * looked up in the table.
 * @param param Parameter
 * @return Returns the parameters value.
 */
int param_get_value(param_t *param)
{
	switch (param->class_def->typ) {
	case PARAM_INT:
		return param->value;
	case PARAM_ENUM:
		return param->class_def->enum_table[param->value].value;
	default:
		return 0;
	}
}

/*
 * Gets the parameters default value. If parameter is an enum the value is
 * looked up in the table.
 */
int param_get_default_value(param_t *param)
{
	switch (param->class_def->typ) {
	case PARAM_INT:
		return param->class_def->def;
	case PARAM_ENUM:
		return param->class_def->enum_table[param->class_def->def].value;
	default:
		return 0;
	}
}

/*
 * Gets a parameters value as a formatted string.
 */
void param_get_str(param_t *param, char *str, int len)
{
	int source;
	
	assert(param->class_def->print_value);

	if ((param->flags & PARAM_FLAG_CAN_CONNECT) && (param->value > param->class_def->max)) {
		source = param->value - param->class_def->max - 1;
		snprintf(str, len, "L%d%s", source % NUM_LINES + 1, source >= NUM_LINES ? "s" : ""); 
	} else {
		param->class_def->print_value(param->class_def, param->value, str, len);
	}
}

/*
 * Returns the parameter class name.
 */
char *param_get_name(param_t *param)
{
	return param->class_def->name;
}

/*
 * Returns 1 if parameter is connected.
 */
int param_is_connected(param_t *param)
{
	return param->value > param->class_def->max ? 1 : 0;
}

/*
 * Returns the connected index if parameter is connected, -1 otherwise.
 */
int param_get_connected_index(param_t *param)
{
	return param->value > param->class_def->max ? param->value - param->class_def->max - 1 : -1;
}

/*
 * Loads a parameter from a file.
 */
int param_load(param_t *param, FILE *file)
{
	int result;

	result = fread(&param->value, sizeof(param->value), 1, file) != 1;
	if (result == 0)
		if (param->changed)
			param->changed(param);

	return result;		
}

/*
 * Saves a parameter to a file.
 */
int param_save(param_t *param, FILE *file)
{
	return fwrite(&param->value, sizeof(param->value), 1, file) != 1;
}
