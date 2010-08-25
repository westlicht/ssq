#ifndef __MMI_H__
#define __MMI_H__

#include "mio.h"
#include "pattern.h"
#include "line.h"

/** mmi state */
typedef struct {
	sequence_t *sequence;  /**< selected sequence */
	int sequence_index;    /**< selected sequence index */
	line_t *line;          /**< selected line */
	int line_index;        /**< selected line index */
	int last_edited_step;  /**< last edited step number */
	int beat_blink;        /**< beat blinker */
} mmi_state_t;

/**
 * Initializes the mmi.
 * @return Returns 0 if successful.
 */
int mmi_init(void);

/**
 * Shuts the mmi down.
 */
void mmi_shutdown(void);

/**
 * Updates the mmi.
 */
void mmi_update(void);

/**
 * Called from the sequencer thread.
 * @param pulse Pulse
 * @param timestamp Timestamp
 */
void mmi_pulse(int pulse, mio_timestamp_t timestamp);

/**
 * Returns the mmi state.
 * @return Returns the mmi state.
 */
mmi_state_t *mmi_get_state(void);

#endif /*__MMI_H__*/
