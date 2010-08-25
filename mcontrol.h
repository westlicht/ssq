#ifndef __MCONTROL_H__
#define __MCONTROL_H__

#include "mio.h"

typedef struct mctrl mctrl_t;
typedef struct mctrl_cc mctrl_cc_t;
typedef struct mctrl_callbacks mctrl_callbacks_t;

/** midi controller callbacks */
struct mctrl_callbacks {
	void (* cc_changed) (mctrl_t *mctrl, int cc, int value);
};

/** midi controller */
struct mctrl {
	mio_stream_t input;
	mio_stream_t output;
	mctrl_callbacks_t callbacks;
};

/**
 * Initializes a midi controller.
 * @param mctrl Midi controller
 * @param input Input device
 * @param output Output device
 * @return Returns 0 if successful.
 */
int mctrl_init(mctrl_t *mctrl, mio_device_t *input, mio_device_t *output);

/**
 * Shuts a midi controller down.
 * @param mctrl Midi controller
 */
void mctrl_shutdown(mctrl_t *mctrl);

/**
 * Returns callback functions.
 * @param mctrl Midi controller
 * @return Returns callback functions.
 */
mctrl_callbacks_t *mctrl_get_callbacks(mctrl_t *mctrl);

/**
 * Updates the midi controller by parsing incoming messages.
 * @param mctrl Midi Controller
 */
void mctrl_update(mctrl_t *mctrl);

/**
 * Sets a cc controller value.
 * @param mctrl Midi Controller
 * @param cc Midi cc
 * @param value Value to set
 */
void mctrl_cc_set(mctrl_t *mctrl, int cc, int value);



#endif /*__MCONTROL_H__*/
