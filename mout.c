
#include <assert.h>

#include "lightlist.h"
#include "mio.h"
#include "mout.h"

/** notes in note buffer */
#define NUM_NOTES 1024

/** maximum number of output streams */
#define MAX_STREAMS 2

static mio_stream_t *s_streams[MAX_STREAMS];
static mout_note_t s_note_buffer[NUM_NOTES];
static struct list_head s_notes;

/*
 * Initializes the midi output subsystem.
 */
int mout_init(void)
{
	int i;
	
	INIT_LIST_HEAD(&s_notes);
	
	for (i = 0; i < NUM_NOTES; i++)
		list_add_tail(&s_note_buffer[i].item, &s_notes);
	
	return 0;
}

/*
 * Shuts the midi output subsystem down.
 */
void mout_shutdown(void)
{
	mout_stop_all();
}

/*
 * Registers a midi output stream.
 */
void mout_register_output(int id, mio_stream_t *stream)
{
	assert(id >= 0 && id < MAX_STREAMS);
	s_streams[id] = stream;
}

/*
 * Returns an output stream.
 */
mio_stream_t *mout_get_output(int id)
{
	assert(id >= 0 && id < MAX_STREAMS);
	return s_streams[id];
}

/*
 * Play a note and return a reference onto it (for later stopping of the note).
 */
mout_note_t *mout_play_note(int id, unsigned char channel, unsigned char note, unsigned char vel, mio_timestamp_t timestamp)
{
	mio_stream_t *stream = s_streams[id];
	mio_event_t event;
	mout_note_t *notebuf;

	if (!stream)
		return NULL;
	
	/* get first note from buffer */
	notebuf = list_entry(s_notes.next, mout_note_t, item);
	
	/* exit if there are no notes left */
	if (notebuf->active)
		return NULL;
		
	/* play the note */
	event.message = mio_message(MIO_CMD_NOTE_ON, channel, note, vel);
	event.timestamp = timestamp;
	mio_write(stream, &event, 1);
	
	/* store the note */
	notebuf->stream = stream;
	notebuf->channel = channel;
	notebuf->note = note;
	notebuf->active = 1;
	
	/* move note to tail of list */
	list_move_tail(&notebuf->item, &s_notes);
	
	return notebuf;
}

/*
 * Stops a previously played note.
 */
void mout_stop_note(mout_note_t *note, mio_timestamp_t timestamp)
{
	mio_event_t event;
	
	if (!note)
		return;
	
	if (!note->active)
		return;
		
	/* stop the note */
	event.message = mio_message(MIO_CMD_NOTE_OFF, note->channel, note->note, 0);
	event.timestamp = timestamp;
	mio_write(note->stream, &event, 1);
	
	/* disable note and move to head of the list */
	note->active = 0;
	list_move(&note->item, &s_notes);
}

/*
 * Sets a cc value.
 */
void mout_set_cc(int id, unsigned char channel, unsigned char cc, unsigned char value, mio_timestamp_t timestamp)
{
	mio_stream_t *stream = s_streams[id];
	mio_event_t event;
	
	/* send cc */
	event.message = mio_message(MIO_CMD_CONTROL_CHANGE, channel, cc, value);
	event.timestamp = timestamp;
	mio_write(stream, &event, 1);
}

/*
 * Stops all previously played notes.
 */
void mout_stop_all(void)
{
	mout_note_t *note;
	
	list_for_each_entry(note, &s_notes, item)
		if (note->active)
			mout_stop_note(note, mio_get_timestamp());
}
