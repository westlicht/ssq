
#include <stdlib.h>

#include "log.h"
#include "mcontrol.h"

static void process_midi_event(mctrl_t *mctrl, mio_event_t *event);
static void process_midi_cc(mctrl_t *mctrl, mio_event_t *event);

/*
 * Initializes a midi controller.
 */
int mctrl_init(mctrl_t *mctrl, mio_device_t *input, mio_device_t *output)
{
	if (mio_open_input(&mctrl->input, input) != 0) {
		LOG(LOG_ERROR, "cannot open input stream for midi controller");
		return -1;
	}
	
	if (mio_open_output(&mctrl->output, output, 0) != 0) {
		LOG(LOG_ERROR, "cannot open output stream for midi controller");
		return -1;
	}
	
	mctrl->callbacks.cc_changed = NULL;
	
	return 0;
}

/*
 * Shuts a midi controller down.
 */
void mctrl_shutdown(mctrl_t *mctrl)
{
	mio_close(&mctrl->input);
	mio_close(&mctrl->output);
}

/*
 * Returns callback functions.
 */
mctrl_callbacks_t *mctrl_get_callbacks(mctrl_t *mctrl)
{
	return &mctrl->callbacks;
}

/*
 * Updates the midi controller by parsing incoming messages.
 */
void mctrl_update(mctrl_t *mctrl)
{
	int i, count;
	mio_event_t buf[MIO_BUF_LEN];

	count = mio_read(&mctrl->input, buf, MIO_BUF_LEN);
	if (count < 1)
		return;

	for (i = 0; i < count; i++)
		process_midi_event(mctrl, &buf[i]);
}

/*
 * Sets a cc controller value.
 */
void mctrl_cc_set(mctrl_t *mctrl, int cc, int value)
{
	mio_event_t event;
	
	event.message = mio_message(MIO_CMD_CONTROL_CHANGE, 0, cc, value);
	event.timestamp = mio_get_timestamp();

	mio_write(&mctrl->output, &event, 1);
}


/**
 * Process a single midi event.
 * @param mctrl Midi controller
 * @param event Midi event
 */
static void process_midi_event(mctrl_t *mctrl, mio_event_t *event)
{
	if (mio_message_cmd(event->message) == MIO_CMD_CONTROL_CHANGE)
		process_midi_cc(mctrl, event);
}

/**
 * Process a single midi CC event.
 * @param mctrl Midi controller
 * @param event Midi event
 */
static void process_midi_cc(mctrl_t *mctrl, mio_event_t *event)
{
	int channel;
	int cc;
	int value;
	
	channel = mio_message_channel(event->message);
	cc = mio_message_data1(event->message);
	value = mio_message_data2(event->message);
	
	if (mctrl->callbacks.cc_changed)
		mctrl->callbacks.cc_changed(mctrl, cc, value);
	
//	LOG(LOG_INFO, "cc received (cc: %d value: %d)", cc, value);
}

