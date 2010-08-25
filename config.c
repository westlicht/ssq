
#include <stdio.h>

#include "log.h"
#include "para.h"
#include "config.h"

/*
 * Loads default configuration.
 */
void config_default(config_t *config)
{
	config->control_input[0] = 0;
	config->control_output[0] = 0;
	config->seq_input[0] = 0;
	config->seq_output[0] = 0;
}

/*
 * Loads configuration from an xml file.
 */
int config_load(config_t *config, const char *filename)
{
	para_handle_t para;
	int result = -1;
	
	/* create parameter object */
	para = para_new();
	if (para == 0) {
		LOG(LOG_ERROR, "cannot allocate para");
		return -1;
	}
	
	/* load parameters from file */
	if (para_load_from_file(para, filename) != 0) {
		LOG(LOG_ERR, "cannot load configuration file from '%s'", filename);
		goto out;
	}
	
	/* go to sequencer section */
	if (para_set_section(para, "ssq") != 0) {
		LOG(LOG_ERR, "invalid configuration file '%s'", filename);
		goto out;
	}
	
	para_read_string(para, "control_input", config->control_input, sizeof(config->control_input));
	para_read_string(para, "control_output", config->control_output, sizeof(config->control_output));
	para_read_string(para, "seq_input", config->seq_input, sizeof(config->seq_input));
	para_read_string(para, "seq_output", config->seq_output, sizeof(config->seq_output));

	result = 0;
	
out:
	/* free parameter object */
	para_free(para);
		
	return result;
}
