#ifndef __MOUT_H__
#define __MOUT_H__

#include "lightlist.h"
#include "mio.h"

/** note object */
typedef struct {
	struct list_head item;
	mio_stream_t *stream;
	unsigned char channel;
	unsigned char note;
	unsigned char active;
} mout_note_t;

/**
 * Initializes the midi output subsystem.
 * @return Returns 0 if successful.
 */
int mout_init(void);

/**
 * Shuts the midi output subsystem down.
 */
void mout_shutdown(void);

/**
 * Registers a midi output stream.
 * @param id Stream id
 * @param stream Output stream
 */
void mout_register_output(int id, mio_stream_t *stream);

/**
 * Returns an output stream.
 * @param id Stream id
 * @return Returns the output stream.
 */
mio_stream_t *mout_get_output(int id);

/**
 * Play a note and return a reference onto it (for later stopping of the note).
 * @param id Stream id
 * @param channel Midi channel
 * @param note Note
 * @param vel Velocity
 * @param timestamp Timestamp
 * @return Returns a reference to the played note.
 */
mout_note_t *mout_play_note(int id, unsigned char channel, unsigned char note, unsigned char vel, mio_timestamp_t timestamp);

/**
 * Stops a previously played note.
 * @param note Note
 * @param timestamp Timestamp
 */
void mout_stop_note(mout_note_t *note, mio_timestamp_t timestamp);

/**
 * Sets a cc value.
 * @param id Stream id
 * @param channel Midi channel
 * @param cc CC number
 * @param value CC value
 * @param timestamp Timestamp
 */
void mout_set_cc(int id, unsigned char channel, unsigned char cc, unsigned char value, mio_timestamp_t timestamp);

/**
 * Stops all previously played notes.
 */
void mout_stop_all(void);

#endif /*__MOUT_H__*/
