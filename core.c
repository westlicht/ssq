
#include <unistd.h>

#include "log.h"
#include "config.h"
#include "mio.h"
#include "mout.h"
#include "seq.h"
#include "mmi.h"
#include "param.h"
#include "core.h"

static int s_terminate = 0;
static config_t s_config;
static mio_stream_t s_input, s_output;

/*
 * Initializes the core.
 */
int core_init(void)
{
	int i, count;
	mio_device_t *dev;
	
	/* init parameter connection table */
	param_init_param_connections();
	
	/* load configuration */
	if (config_load(&s_config, "config.xml") != 0)
		return -1;
	
	/* init midi io */
	if (mio_init() != 0)
		return -1;
	
	/* enumerate devices */
	count = mio_get_device_count();
	LOG(LOG_INFO, "found %d midi devices:", count);
	for (i = 0; i < count; i++) {
		dev = mio_get_device(i);
		LOG(LOG_INFO, "  #%d - %s [%s] (in: %d out: %d)", i, dev->name, dev->interface, dev->input, dev->output);
	}
		
	/* open sequencer input/output */
	if (mio_open_input(&s_input, mio_get_input_device_by_name(s_config.seq_input)) != 0)
		return -1;
	if (mio_open_output(&s_output, mio_get_output_device_by_name(s_config.seq_output), OUTPUT_LATENCY) != 0)
		return -1;
		
	/* init midi output */
	if (mout_init() != 0)
		return -1;
		
	mout_register_output(0, &s_output);
		
	/* init sequencer */
	if (seq_init() != 0)
		return -1;
		
	/* init mmi */
	if (mmi_init() != 0)
		return -1;
		
	return 0;
}

/*
 * Shuts the core down.
 */
void core_shutdown(void)
{
	mmi_shutdown();
	
	seq_shutdown();
	
	mout_shutdown();
	
	mio_close(&s_input);
	mio_close(&s_output);
		
	mio_shutdown();
}

/*
 * Application's main loop.
 */
void core_run(void)
{
	while (!s_terminate) {
		mmi_update();
		usleep(1000);
	}
	
	seq_stop();
}

/*
 * Exit application.
 */
void core_exit(void)
{
	s_terminate = 1;
}

/*
 * Returns the configuration.
 */
config_t *core_get_config(void)
{
	return &s_config;
}

/*
 * Returns the default input stream.
 */
mio_stream_t *core_get_input(void)
{
	return &s_input;
}

/*
 * Returns the default output stream.
 */
mio_stream_t *core_get_output(void)
{
	return &s_output;
}
