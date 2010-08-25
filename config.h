#ifndef __CONFIG_H__
#define __CONFIG_H__

/** application configuration */
typedef struct {
	char control_input[128];
	char control_output[128];
	char seq_input[128];
	char seq_output[128];
} config_t;

/**
 * Loads default configuration.
 * @param config Configuration
 */
void config_default(config_t *config);

/**
 * Loads configuration from an xml file.
 * @param config Configuration
 * @param filename Filename
 * @return Returns 0 if successful.
 */
int config_load(config_t *config, const char *filename);

#endif /*__CONFIG_H__*/
