/** @file para.h
 *
 * This file implements the libpara functions.
 *
 * @author  Simon Kallweit, intefo AG
 * @version $Id: $
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <expat.h>

#include "para.h"

/* Internal structure for an attribute */
struct para_attr {
	struct para_attr *next; /**< next attribute */
	char *name;             /**< attribute naame */
	char *value;            /**< attribute value */
};

/* Internal structure for a node */
struct para_node {
	struct para_node *parent;   /**< parent node */
	struct para_node *next;     /**< next node */
	struct para_node *children; /**< first child node */
	struct para_attr *attr;     /**< first attribute */
	char *name;
};

/** Internal structure for a parameter file */
typedef struct {
	struct para_node *root;    /**< root node */
	struct para_node *section; /**< current section node */	
} para_t;

/** Autodeclaration of a pointer to the para structure casted from the handle */
#define DECLARE_PARA_FROM_HANDLE(handle) para_t *para = (para_t *) (handle)

/* Local functions */
static void parser_start_element(void *user_data, const XML_Char *name, const XML_Char **atts);
static void parser_end_element(void *user_data, const XML_Char *name);

static struct para_node *create_node(struct para_node *parent, const char *name);
static void destroy_node(struct para_node *node);

static struct para_attr *create_attr(struct para_node *node, const char *name, const char *value);
//static void destroy_attr(struct para_node *node, struct para_attr *attr);
static void clear_attr(struct para_node *node);

static struct para_node* find_node(struct para_node *section, const char *typ, const char *name);
static struct para_attr *find_attr(struct para_node *node, const char *name);
static char *get_value(struct para_node *section, const char *typ, const char *name, const char *attr);
static int set_value(struct para_node *section, const char *typ, const char *name, const char *attr, const char *value); 

static int save_node_to_file(FILE *f, struct para_node *node, int ident);


/*
 * This function alloactes a new parameter file.
 */
para_handle_t para_new(void)
{
	/* allocate para structure */
	para_t *para = malloc(sizeof(para_t));	
	
	if (!para)
		return 0;
		
	para->root = create_node(NULL, "root");
	para->section = para->root;

	/* return address as handle */
	return (para_handle_t) para;
}

/*
 * This function frees a parameter file.
 */
void para_free(para_handle_t handle)
{
	DECLARE_PARA_FROM_HANDLE(handle);
	
	if (!para)
		return;

	destroy_node(para->root);
	free(para);
}

/*
 * This function clears the internal data of the parameter file.
 */
void para_clear(para_handle_t handle)
{
	DECLARE_PARA_FROM_HANDLE(handle);
	
	if (!para)
		return;

	destroy_node(para->root);
	para->root = create_node(NULL, "root");
	para->section = para->root;
}

/*
 * This function loads a parameter file from a file.
 */
int para_load_from_file(para_handle_t handle, const char *filename)
{
	DECLARE_PARA_FROM_HANDLE(handle);
	XML_Parser parser;
	FILE *f;
	int result = -1;
	char buf[4096];
	int len;
	
	para_clear(handle);

	if (!para)
		goto out;
		
	f = fopen(filename, "r");
	if (!f)
		goto out;
		
	parser = XML_ParserCreate("UTF-8");
	if (!parser)
		goto out_close_file;
		
	XML_SetElementHandler(parser, parser_start_element, parser_end_element);
	XML_SetUserData(parser, para);
	
	while (1) {
		len = fread(buf, 1, sizeof(buf), f);
		if (!XML_Parse(parser, buf, len, len == 0))
			goto out_free_parser;
		if (len <= 0)
			break;
	}
	
	result = 0;
	
out_free_parser:	
	XML_ParserFree(parser);
	
out_close_file:
	fclose(f);
	
out:
	return result;
}

/*
 * This function loads a parameter file from memory.
 */
int para_load_from_mem(para_handle_t handle, char *data, int size)
{
	DECLARE_PARA_FROM_HANDLE(handle);
	XML_Parser parser;
	int result = -1;
   	
	para_clear(handle);

	if (!para)
		goto out;
		
	parser = XML_ParserCreate("UTF-8");
	if (!parser)
		goto out;
		
	XML_SetElementHandler(parser, parser_start_element, parser_end_element);
	XML_SetUserData(parser, para);
	
	if (!XML_Parse(parser, data, size, 1))
		goto out_free_parser;
	
	result = 0;
	
out_free_parser:	
	XML_ParserFree(parser);
	
out:
	return result;
}

/*
 * This function saves a parameter file to a file.
 */
int para_save_to_file(para_handle_t handle, const char *filename)
{
	DECLARE_PARA_FROM_HANDLE(handle);
	FILE *f;
	struct para_node *n;
	
	if (!para)
		return -1;
		
	f = fopen(filename, "w+");
	if (!f)
		return -1;
		
	fputs("<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n", f);
	
	for (n = para->root->children; n; n = n->next)
		save_node_to_file(f, n, 0);
	
	fclose(f);

	return 0;
}

/*
 * This function checks if a top level section exists in the parameter file.
 */
int para_section_exists(para_handle_t handle, const char *section)
{
	DECLARE_PARA_FROM_HANDLE(handle);
	struct para_node *n;
	
	if (!para)
		return -1;
	
	/* check the first level of nodes */
	for (n = para->root; n; n = n->next) {
		if (strcmp(n->name, section) == 0) {
			/* section found */
			return 0;
		}
	}
	
	return -1;
}

/*
 * This function checks if a child section relative to the current section
 * exists in the parameter file.
 */
int para_child_section_exists(para_handle_t handle, const char *section)
{
	DECLARE_PARA_FROM_HANDLE(handle);
	struct para_node *n;
	
	if (!para)
		return -1;
	
	if (!para->section)
		return -1;
	
	for (n = para->section->children; n; n = n->next) {
		if (strcmp(n->name, section) == 0) {
			/* section found */
			return 0;
		}
	}
	
	return -1;
}

/*
 * This function sets the active top level section in the parameter file.
 */
int para_set_section(para_handle_t handle, const char *section)
{
	DECLARE_PARA_FROM_HANDLE(handle);
	struct para_node *n;
	
	if (!para)
		return -1;
	
	/* check the first level of nodes */
	for (n = para->root->children; n; n = n->next) {
		if (strcmp(n->name, section) == 0) {
			/* section found */
			para->section = n;
			return 0;
		}
	}
	
	/* no section was found */
	para->section = NULL;
	
	return -1;
}

/*
 * This function moves to a child section in the parameter file.
 */
int para_set_child_section(para_handle_t handle, const char *section)
{
	DECLARE_PARA_FROM_HANDLE(handle);
	struct para_node *n;
	
	if (!para)
		return -1;

	if (!para->section)
		return -1;

	/* check the first level of nodes */
	for (n = para->section->children; n; n = n->next) {
		if (strcmp(n->name, section) == 0) {
			/* section found */
			para->section = n;
			return 0;
		}
	}
	
	return -1;
}

/*
 * This function moves to the parent section in the parameter file.
 */
int para_set_parent_section(para_handle_t handle)
{
	DECLARE_PARA_FROM_HANDLE(handle);
	
	if (!para)
		return -1;
	
	if (!para->section)
		return -1;

	if (para->section->parent) {
		para->section = para->section->parent;
		return 0;
	}
	
	return -1;
}

/*
 * This function creates a new top level section in the parameter file and makes
 * it the active section. If the section already exists, it's selected.
 */
int para_create_section(para_handle_t handle, const char *section)
{
	DECLARE_PARA_FROM_HANDLE(handle);
	struct para_node *n;
	
	if (!para)
		return -1;

	/* check if section already exists, and select if true */
	if (para_set_section(handle, section) == 0)
		return 0;
	
	/* add and select new section */
	n = create_node(para->root, section);
	if (n) {
		para->section = n;
		return 0;
	}
	
	return -1;
}

/*
 * This function creates a new child section in the parameter file and makes
 * it the active section. If the section already exists, it's selected.
 */
int para_create_child_section(para_handle_t handle, const char *section)
{
	DECLARE_PARA_FROM_HANDLE(handle);
	struct para_node *n;
	
	if (!para)
		return -1;

	/* check if section already exists, and select if true */
	if (para_set_child_section(handle, section) == 0)
		return 0;
	
	/* add and select new section */
	n = create_node(para->section, section);
	if (n) {
		para->section = n;
		return 0;
	}
	
	return -1;
}

/*
 * This function retrieves the number of child sections in the current section.
 */
int para_get_child_section_count(para_handle_t handle, int *count)
{
	DECLARE_PARA_FROM_HANDLE(handle);
	struct para_node *n;
	struct para_attr *a;
	
	if (!para)
		return -1;

	if (!para->section)
		return -1;

	/* check the first level of nodes */
	*count = 0;
	for (n = para->section->children; n; n = n->next) {
		a = find_attr(n, "name");
		if (!a)
			(*count)++;
	}
	
	return 0;
}

/*
 * This function selects a child section by index (0..n-1).
 */
int para_set_child_section_by_index(para_handle_t handle, int index)
{
	DECLARE_PARA_FROM_HANDLE(handle);
	struct para_node *n;
	struct para_attr *a;
	
	if (!para)
		return -1;

	if (!para->section)
		return -1;

	/* check the first level of nodes */
	for (n = para->section->children; n; n = n->next) {
		a = find_attr(n, "name");
		if (a)
			continue;
		if (index == 0) {
			para->section = n;
			return 0;
		}
		index--;
	}
	
	return -1;
}

/*
 * This function reads a char from the parameter file.
 */
int para_read_char(para_handle_t handle, const char *name, char *value)
{
	DECLARE_PARA_FROM_HANDLE(handle);
	char *v;
	
	if (!para)
		return -1;
		
	v = get_value(para->section, "char", name, "value");
	if (!v)
		return -1;
		
	*value = atoi(v);
	
	return 0;
}

/**
 * This function reads an int from the parameter file.
 */
int para_read_int(para_handle_t handle, const char *name, int *value)
{
	DECLARE_PARA_FROM_HANDLE(handle);
	char *v;
	
	if (!para)
		return -1;
		
	v = get_value(para->section, "int", name, "value");
	if (!v)
		return -1;
		
	*value = atoi(v);
	
	return 0;
}

/**
 * This function reads a long from the parameter file.
 */
int para_read_long(para_handle_t handle, const char *name, long *value)
{
	DECLARE_PARA_FROM_HANDLE(handle);
	char *v;
	
	if (!para)
		return -1;
		
	v = get_value(para->section, "long", name, "value");
	if (!v)
		return -1;
		
	*value = atol(v);
	
	return 0;
}

/**
 * This function reads a float from the parameter file.
 */
int para_read_float(para_handle_t handle, const char *name, float *value)
{
	DECLARE_PARA_FROM_HANDLE(handle);
	char *v;
	
	if (!para)
		return -1;
		
	v = get_value(para->section, "float", name, "value");
	if (!v)
		return -1;
		
	*value = atof(v);
	
	return 0;
}

/**
 * This function reads a double from the parameter file.
 */
int para_read_double(para_handle_t handle, const char *name, double *value)
{
	DECLARE_PARA_FROM_HANDLE(handle);
	char *v;
	
	if (!para)
		return -1;
		
	v = get_value(para->section, "float", name, "value");
	if (!v)
		return -1;
		
	*value = atof(v);
	
	return 0;
}

/**
 * This function reads a string from the parameter file.
 */
int para_read_string(para_handle_t handle, const char *name, char *value, int len)
{
	DECLARE_PARA_FROM_HANDLE(handle);
	char *v;
	
	if (!para)
		return -1;
		
	v = get_value(para->section, "string", name, "value");
	if (!v)
		return -1;
		
	strncpy(value, v, len);
	value[len - 1] = 0;
	
	return 0;
}

/*
 * This function reads an additional property of an item from the parameter file.
 */
int para_read_property(para_handle_t handle, const char *name, const char *property, char *value, int len)
{
	DECLARE_PARA_FROM_HANDLE(handle);
	char *v;
	
	if (!para)
		return -1;
		
	v = get_value(para->section, "string", name, property);
	if (!v)
		return -1;
		
	strncpy(value, v, len);
	value[len - 1] = 0;
	
	return 0;
}

/*
 * This function reads an additional property of the current section.
 */
int para_read_section_property(para_handle_t handle, const char *property, char *value, int len)
{
	DECLARE_PARA_FROM_HANDLE(handle);
	struct para_attr *a;
	
	if (!para)
		return -1;
		
	a = find_attr(para->section, property);
	if (!a)
		return -1;
		
	strncpy(value, a->value, len);
	value[len - 1] = 0;
	
	return 0;
}

/*
 * This function writes a char to the parameter file.
 */
int para_write_char(para_handle_t handle, const char *name, char value)
{
	DECLARE_PARA_FROM_HANDLE(handle);
	char buffer[8];
	
	if (!para)
		return -1;
	
	snprintf(buffer, sizeof(buffer), "%d", value);
	
	return set_value(para->section, "char", name, "value", buffer);
}

/*
 * This function writes an int to the parameter file.
 */
int para_write_int(para_handle_t handle, const char *name, int value)
{
	DECLARE_PARA_FROM_HANDLE(handle);
	char buffer[32];
	
	if (!para)
		return -1;
	
	snprintf(buffer, sizeof(buffer), "%d", value);
	
	return set_value(para->section, "int", name, "value", buffer);
}

/*
 * This function writes a long to the parameter file.
 */
int para_write_long(para_handle_t handle, const char *name, long value)
{
	DECLARE_PARA_FROM_HANDLE(handle);
	char buffer[32];
	
	if (!para)
		return -1;
	
	snprintf(buffer, sizeof(buffer), "%ld", value);
	
	return set_value(para->section, "long", name, "value", buffer);
}

/*
 * This function writes a float to the parameter file.
 */
int para_write_float(para_handle_t handle, const char *name, float value)
{
	DECLARE_PARA_FROM_HANDLE(handle);
	char buffer[32];
	
	if (!para)
		return -1;
	
	snprintf(buffer, sizeof(buffer), "%f", value);
	
	return set_value(para->section, "float", name, "value", buffer);
}

/*
 * This function writes a double to the parameter file.
 */
int para_write_double(para_handle_t handle, const char *name, double value)
{
	DECLARE_PARA_FROM_HANDLE(handle);
	char buffer[32];
	
	if (!para)
		return -1;
	
	snprintf(buffer, sizeof(buffer), "%f", value);
	
	return set_value(para->section, "double", name, "value", buffer);
}

/*
 * This function writes a string to the parameter file.
 */
int para_write_string(para_handle_t handle, const char *name, const char* value)
{
	DECLARE_PARA_FROM_HANDLE(handle);
	
	if (!para)
		return -1;
	
	return set_value(para->section, "string", name, "value", value);
}

/*
 * This function writes an additional property to an item in the parameter file.
 */
int para_write_property(para_handle_t handle, const char *name, const char *property, const char *value)
{
	DECLARE_PARA_FROM_HANDLE(handle);
	struct para_node *n;
	struct para_attr *a;
	
	if (!para)
		return -1;
		
	n = find_node(para->section, 0, name);
	if (!n)
		return -1;
	a = find_attr(n, property);
	if (a) {
		free(a->name);
		a->name = strdup(value);
	} else {
		a = create_attr(n, property, value);
		if (!a)
			return -1;
	}
	
	return 0;	
}

/*
 * This function writes an additional property of the current section.
 */
int para_write_section_property(para_handle_t handle, const char *property, char *value)
{
	DECLARE_PARA_FROM_HANDLE(handle);
	struct para_attr *a;
	
	if (!para)
		return -1;

	if (!para->section)
		return -1;
		
	a = find_attr(para->section, property);
	if (a) {
		free(a->name);
		a->name = strdup(value);
	} else {
		a = create_attr(para->section, property, value);
		if (!a)
			return -1;
	}
	
	return 0;	
}





static void parser_start_element(void *user_data, const XML_Char *name, const XML_Char **atts)
{
	DECLARE_PARA_FROM_HANDLE(user_data);
	const XML_Char **attr;
	const char *attr_name, *attr_value;
	
	para->section = create_node(para->section, name);
	
	attr = atts;
	while (*attr) {
		attr_name = *attr;
		attr++;
		attr_value = *attr;
		attr++;
		create_attr(para->section, attr_name, attr_value);
	}
}

static void parser_end_element(void *user_data, const XML_Char *name)
{
	DECLARE_PARA_FROM_HANDLE(user_data);
	
	para->section = para->section->parent;
}

static struct para_node *create_node(struct para_node *parent, const char *name)
{
	struct para_node *new, *n;
	
	new = malloc(sizeof(struct para_node));
	if (!new)
		return NULL;
		
	new->parent = parent;
	new->next = NULL;
	new->children = NULL;
	new->attr = NULL;
	new->name = strdup(name);
	
	/* append to children list of parent */
	if (parent) {
		if (parent->children) {
			for (n = parent->children; n->next; n = n->next);
			n->next = new;
		} else {
			parent->children = new;
		}
	}
	
	return new;	
}

static void destroy_node(struct para_node *node)
{
	struct para_node *n;
	
	/* destroy child nodes */
	while (node->children)
		destroy_node(node->children);
	
	/* clear attributes */
	clear_attr(node);
	
	/* remove from list */
	if (node->parent) {
		if (node->parent->children == node) {
			node->parent->children = node->next;
		} else {
			for (n = node->parent->children; n->next != node; n = n->next);
			n->next = node->next;
		}
	}
	
	/* destroy node */
	free(node->name);
	free(node);
}

static struct para_attr *create_attr(struct para_node *node, const char *name, const char *value)
{
	struct para_attr *new, *a;
	
	new = malloc(sizeof(struct para_attr));
	if (!new)
		return NULL;

	new->next = NULL;
	new->name = strdup(name);
	new->value = strdup(value);
	
	/* append attribute to list */
	if (node->attr) {
		for (a = node->attr; a->next; a = a->next);
		a->next = new;
	} else {
		node->attr = new;
	}

	return new;
}

#if 0 /* this is not yet needed */
static void destroy_attr(struct para_node *node, struct para_attr *attr)
{
	struct para_attr *a;
	
	if (node->attr) {
		for (a = node->attr; a->next != attr; a = a->next);
		a->next = attr->next;
	}
	
	/* destroy attribute */
	free(attr->name);
	free(attr->value);
	free(attr);
}
#endif

static void clear_attr(struct para_node *node)
{
	struct para_attr *a, *n;
	
	a = node->attr;
	while (a) {
		n = a->next;
		/* destroy attribute */
		free(a->name);
		free(a->value);
		free(a);
		a = n;
	}
	
	node->attr = NULL;
}

static struct para_node* find_node(struct para_node *section, const char *typ, const char *name)
{
	struct para_node *n;
	struct para_attr *a;
	
	if (typ) {
		for (n = section->children; n; n = n->next) {
			if (strcmp(n->name, typ) == 0) {
				a = find_attr(n, "name");
				if (!a)
					continue;
				if (strcmp(a->value, name) == 0)
					return n;
			}
		}
	} else {
		for (n = section->children; n; n = n->next) {
			a = find_attr(n, "name");
			if (!a)
				continue;
			if (strcmp(a->value, name) == 0)
				return n;
		}
	}
	
	return NULL;
}

static struct para_attr *find_attr(struct para_node *node, const char *name)
{
	struct para_attr *a;
	
	for (a = node->attr; a; a = a->next) {
		if (strcmp(a->name, name) == 0) {
			return a;
		}
	}
	
	return NULL;
}

static char *get_value(struct para_node *section, const char *typ, const char *name, const char *attr)
{
	struct para_node *n;
	struct para_attr *a;
	
	n = find_node(section, typ, name);
	if (n) {
		a = find_attr(n, attr);
		if (a)
			return a->value;
	}
	
	return NULL;
}

static int set_value(struct para_node *section, const char *typ, const char *name, const char *attr, const char *value)
{
	struct para_node *n;
	struct para_attr *a;
	
	n = find_node(section, typ, name);
	
	/* create node if not found */
	if (!n) {
		n = create_node(section, typ);
		if (!n)
			return -1;
		a = create_attr(n, "name", name);
		if (!a)
			return -1;
	}
	
	a = find_attr(n, attr);
	if (!a) {
		a = create_attr(n, attr, value);
		if (!a)
			return -1;
	} else {
		free(a->value);
		a->value = strdup(value);
	}
	
	return 0;	
} 

static int save_node_to_file(FILE *f, struct para_node *node, int ident)
{
	char line[4096];
	char *p = line;
	struct para_node *n;
	struct para_attr *a;
	int i;

	/* add tabs for identation */
	for (i = 0; i < ident; i++) {
		*p = '\t'; p++;
	}
	
	/* add node open tag */
	*p = '<'; p++;
	
	/* add node name */
	strcpy(p, node->name); p += strlen(node->name);
	
	/* add attributes */
	for (a = node->attr; a; a = a->next) {
		*p = ' '; p++;
		strcpy(p, a->name); p += strlen(a->name);
		*p = '='; p++;
		*p = '"'; p++;
		strcpy(p, a->value); p += strlen(a->value);
		*p = '"'; p++;
	}
	
	if (node->children) {
		/* add node end tag */
		strcpy(p, ">\n\0");
		fputs(line, f);
		
		for (n = node->children; n; n = n->next)
			save_node_to_file(f, n, ident + 1);
		
		p = strchr(line, '<');
		p++; *p = '/'; p++;
		strcpy(p, node->name); p += strlen(node->name);
		strcpy(p, ">\n\0");
		fputs(line, f);
	} else {
		/* add node end tag */
		strcpy(p, "/>\n\0");
		fputs(line, f);
	}
	
	return 0;
}
