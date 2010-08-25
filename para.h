/** @file para.h
 *
 * This file defines the interface for the libpara library for reading XML
 * parameter files.
 *
 * Note: libpara depends on libxml2
 *
 * The XML parameter files look like this:
 *
 * <?xml version="1.0" encoding="iso-8859-1"?>
 * <test>
 *     <char name="testchar" value="123"/>
 *     <int name="testint" value="12345"/>
 *     <long name="testlong" value="12345678"/>
 *     <float name="testfloat" value="1.234"/>
 *     <double name="testdouble" value="1.234567"/>
 *     <string name="teststring" value="just a little test"/>
 * </test>
 *
 * The following code would read the item "teststring" in the section "test":
 *
 * para_handle_t handle;
 * char buffer[256];
 * handle = para_new();
 * if (handle) {
 *   if (para_load(handle, "./config") == 0) {
 *     para_set_section(handle, "test");
 *     if (para_read_string(handle, "teststring", &buffer[0], sizeof(buffer)) == 0) {
 *       // teststring successfully read!
 *     }
 *   }
 *   para_free(handle);
 * }
 *
 * @author  Simon Kallweit, intefo AG
 * @version $Id: $
 */

#ifndef __PARA_H__
#define __PARA_H__

/** Handle to a parameter file */
typedef struct para_t * para_handle_t;

/**
 * This function alloactes a new parameter file.
 * @return Returns a handle to the parameter file, or 0 on failure.
 */
para_handle_t para_new(void);

/**
 * This function frees a parameter file.
 * @param handle Parameter file handle
 */
void para_free(para_handle_t handle);

/**
 * This function clears the internal data of the parameter file.
 * @param handle Parameter file handle
 */
void para_clear(para_handle_t handle);

/**
 * This function loads a parameter file from a file.
 * @param handle Parameter file handle
 * @param file_name Filename
 * @return Returns 0 on success, otherwise an error code is returned.
 */
int para_load_from_file(para_handle_t handle, const char *filename);

/**
 * This function loads a parameter file from memory.
 * @param handle Parameter file handle
 * @param data Parameter file data
 * @param size Parameter file size
 * @return Returns 0 on success, otherwise an error code is returned.
 */
int para_load_from_mem(para_handle_t handle, char *data, int size);

/**
 * This function saves a parameter file a file.
 * @param handle Parameter file handle
 * @param file_name Filename
 * @return Returns 0 on success, otherwise an error code is returned.
 */
int para_save_to_file(para_handle_t handle, const char *filename);

/**
 * This function checks if a top level section exists in the parameter file.
 * @param handle Parameter file handle
 * @param section Section to select
 * @return Returns 0 on success, otherwise an error code is returned.
 */
int para_section_exists(para_handle_t handle, const char *section);

/**
 * This function checks if a child section relative to the current section
 * exists in the parameter file.
 * @param handle Parameter file handle
 * @param section Section to select
 * @return Returns 0 on success, otherwise an error code is returned.
 */
int para_child_section_exists(para_handle_t handle, const char *section);

/**
 * This function sets the active top level section in the parameter file.
 * @param handle Parameter file handle
 * @param section Section to select
 * @return Returns 0 on success, otherwise an error code is returned.
 */
int para_set_section(para_handle_t handle, const char *section);

/**
 * This function moves to a child section in the parameter file.
 * @param handle Parameter file handle
 * @param section Section to select
 * @return Returns 0 on success, otherwise an error code is returned.
 */
int para_set_child_section(para_handle_t handle, const char *section);

/**
 * This function moves to the parent section in the parameter file.
 * @param handle Parameter file handle
 * @return Returns 0 on success, otherwise an error code is returned.
 */
int para_set_parent_section(para_handle_t handle);

/**
 * This function creates a new top level section in the parameter file and makes
 * it the active section. If the section already exists, it's selected.
 * @param handle Parameter file handle
 * @param section Section to create
 * @return Returns 0 on success, otherwise an error code is returned.
 */
int para_create_section(para_handle_t handle, const char *section);

/**
 * This function creates a new child section in the parameter file and makes
 * it the active section. If the section already exists, it's selected.
 * @param handle Parameter file handle
 * @param section Section to create
 * @return Returns 0 on success, otherwise an error code is returned.
 */
int para_create_child_section(para_handle_t handle, const char *section);

/**
 * This function retrieves the number of child sections in the current section.
 * Attention: This does currently only work if the current section does only
 * contain child sections.
 * @param handle Parameter file handle
 * @param count Number of child sections
 * @return Returns 0 on success, otherwise an error code is returned.
 */
int para_get_child_section_count(para_handle_t handle, int *count);

/**
 * This function selects a child section by index (0..n-1).
 * Note: After using this function one should call the para_set_parent_section()
 * function to get back to the main section.
 * Attention: This does currently only work if the current section does only
 * contain child sections.
 * @param handle Parameter file handle
 * @param index Child section index
 * @return Returns 0 on success, otherwise an error code is returned.
 */
int para_set_child_section_by_index(para_handle_t handle, int index);

/**
 * This function reads a char from the parameter file.
 * @param handle Parameter file handle
 * @param name Field name
 * @param value Value buffer
 * @return Returns 0 on success, otherwise an error code is returned.
 */
int para_read_char(para_handle_t handle, const char *name, char *value);

/**
 * This function reads an int from the parameter file.
 * @param handle Parameter file handle
 * @param name Field name
 * @param value Value buffer
 * @return Returns 0 on success, otherwise an error code is returned.
 */
int para_read_int(para_handle_t handle, const char *name, int *value);

/**
 * This function reads a long from the parameter file.
 * @param handle Parameter file handle
 * @param name Field name
 * @param value Value buffer
 * @return Returns 0 on success, otherwise an error code is returned.
 */
int para_read_long(para_handle_t handle, const char *name, long *value);

/**
 * This function reads a float from the parameter file.
 * @param handle Parameter file handle
 * @param name Field name
 * @param value Value buffer
 * @return Returns 0 on success, otherwise an error code is returned.
 */
int para_read_float(para_handle_t handle, const char *name, float *value);

/**
 * This function reads a double from the parameter file.
 * @param handle Parameter file handle
 * @param name Field name
 * @param value Value buffer
 * @return Returns 0 on success, otherwise an error code is returned.
 */
int para_read_double(para_handle_t handle, const char *name, double *value);

/**
 * This function reads a string from the parameter file.
 * @param handle Parameter file handle
 * @param name Field name
 * @param value Value buffer
 * @param len Length of buffer
 * @return Returns 0 on success, otherwise an error code is returned.
 */
int para_read_string(para_handle_t handle, const char *name, char *value, int len);

/**
 * This function reads an additional property of an item from the parameter file.
 * @param handle Parameter file handle
 * @param name Field name
 * @param property Property name
 * @param value Value buffer
 * @param len Length of buffer
 * @return Returns 0 on success, otherwise an error code is returned.
 */
int para_read_property(para_handle_t handle, const char *name, const char *property, char *value, int len);

/**
 * This function reads an additional property of the current section.
 * @param handle Parameter file handle
 * @param property Property name
 * @param value Value buffer
 * @param len Length of buffer
 * @return Returns 0 on success, otherwise an error code is returned.
 */
int para_read_section_property(para_handle_t handle, const char *property, char *value, int len);

/**
 * This function writes a char to the parameter file.
 * @param handle Parameter file handle
 * @param name Field name
 * @param value Value
 * @return Returns 0 on success, otherwise an error code is returned.
 */
int para_write_char(para_handle_t handle, const char *name, char value);

/**
 * This function writes an int to the parameter file.
 * @param handle Parameter file handle
 * @param name Field name
 * @param value Value
 * @return Returns 0 on success, otherwise an error code is returned.
 */
int para_write_int(para_handle_t handle, const char *name, int value);

/**
 * This function writes a long to the parameter file.
 * @param handle Parameter file handle
 * @param name Field name
 * @param value Value
 * @return Returns 0 on success, otherwise an error code is returned.
 */
int para_write_long(para_handle_t handle, const char *name, long value);

/**
 * This function writes a float to the parameter file.
 * @param handle Parameter file handle
 * @param name Field name
 * @param value Value
 * @return Returns 0 on success, otherwise an error code is returned.
 */
int para_write_float(para_handle_t handle, const char *name, float value);

/**
 * This function writes a double to the parameter file.
 * @param handle Parameter file handle
 * @param name Field name
 * @param value Value
 * @return Returns 0 on success, otherwise an error code is returned.
 */
int para_write_double(para_handle_t handle, const char *name, double value);

/**
 * This function writes a string to the parameter file.
 * @param handle Parameter file handle
 * @param name Field name
 * @param value Value
 * @return Returns 0 on success, otherwise an error code is returned.
 */
int para_write_string(para_handle_t handle, const char *name, const char *value);

/**
 * This function writes an additional property to an item in the parameter file.
 * @param handle Parameter file handle
 * @param name Field name
 * @param property Property name
 * @param value Value buffer
 * @return Returns 0 on success, otherwise an error code is returned.
 */
int para_write_property(para_handle_t handle, const char *name, const char *property, const char *value);

/**
 * This function writes an additional property of the current section.
 * @param handle Parameter file handle
 * @param property Property name
 * @param value Value buffer
 * @return Returns 0 on success, otherwise an error code is returned.
 */
int para_write_section_property(para_handle_t handle, const char *property, char *value);

#endif /*__PARA_H__*/
