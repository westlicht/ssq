#ifndef __CORE_H__
#define __CORE_H__

#include "mio.h"
#include "config.h"

/**
 * Initializes the core.
 * @return Returns 0 if successful.
 */
int core_init(void);

/**
 * Shuts the core down.
 */
void core_shutdown(void);

/**
 * Application's main loop.
 */
void core_run(void);

/**
 * Exit application.
 */
void core_exit(void);

/**
 * Returns the configuration.
 * @return Returns the configuration.
 */
config_t *core_get_config(void);

/**
 * Returns the default input stream.
 * @return Returns the default input stream.
 */
mio_stream_t *core_get_input(void);

/**
 * Returns the default output stream.
 * @return Returns the default output stream.
 */
mio_stream_t *core_get_output(void);

#endif /*__CORE_H__*/
