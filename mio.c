
#include <stdlib.h>
#include <string.h>

#include "portmidi.h"
#include "porttime.h"

#include "log.h"
#include "mio.h"

static int s_device_count;
static mio_device_t *s_devices;

static void init_io_devices(void);

/*
 * Initializes the midi io subsystem.
 */
int mio_init(void)
{
	if (Pm_Initialize() != pmNoError) {
		LOG(LOG_ERROR, "cannot initialize portmidi");
		return -1;
	}
	
	Pt_Start(1, NULL, NULL);
	
	init_io_devices();
	
	return 0;
}

/*
 * Shuts the midi io subsystem down.
 */
void mio_shutdown(void)
{
	Pm_Terminate();
}

/*
 * Returns the current timestamp.
 */
mio_timestamp_t mio_get_timestamp(void)
{
	return Pt_Time();
}

/*
 * Returns the number of midi io devices.
 */
int mio_get_device_count(void)
{
	return s_device_count;
}

/*
 * Returns a midi io device.
 */
mio_device_t *mio_get_device(int id)
{
	return &s_devices[id];
}

/*
 * Returns a midi io device by name.
 */
mio_device_t *mio_get_input_device_by_name(const char *name)
{
	int id;
	
	for (id = 0; id < s_device_count; id++)
		if ((strcmp(s_devices[id].name, name) == 0) && (s_devices[id].input > 0))
			return &s_devices[id];
	
	return NULL;
}

/*
 * Returns a midi io device by name.
 */
mio_device_t *mio_get_output_device_by_name(const char *name)
{
	int id;
	
	for (id = 0; id < s_device_count; id++)
		if ((strcmp(s_devices[id].name, name) == 0) && (s_devices[id].output > 0))
			return &s_devices[id];
	
	return NULL;
}

/*
 * Opens a midi input stream.
 */
int mio_open_input(mio_stream_t *stream, mio_device_t *dev)
{
	stream->dev = dev;
		
	if (Pm_OpenInput(&stream->stream, stream->dev->id, NULL, MIO_BUF_LEN, NULL, NULL) != pmNoError) {
		LOG(LOG_ERROR, "cannot open input stream");
		return -1;
	}
	
	return 0;
}

/*
 * Opens a midi output stream.
 */
int mio_open_output(mio_stream_t *stream, mio_device_t *dev, int latency)
{
	stream->dev = dev;
		
	if (Pm_OpenOutput(&stream->stream, stream->dev->id, NULL, MIO_BUF_LEN, NULL, NULL, latency) != pmNoError) {
		LOG(LOG_ERROR, "cannot open output stream");
		return -1;
	}
	
	return 0;
}

/*
 * Closes a midi stream.
 */
void mio_close(mio_stream_t *stream)
{
	Pm_Close(stream->stream);
}

/*
 * Reads from an input stream.
 */
int mio_read(mio_stream_t *stream, mio_event_t *buf, int len)
{
	return Pm_Read(stream->stream, (PmEvent *) buf, len);
}

/*
 * Writes to an output stream.
 */
int mio_write(mio_stream_t *stream, mio_event_t *buf, int len)
{
	if (Pm_Write(stream->stream, (PmEvent *) buf, len) == pmNoError)
		return 0;
	LOG(LOG_ERROR, "cannot write to midi output");
	return -1;
}

/**
 * Inits the internal table of midi io devices.
 */
static void init_io_devices(void)
{
	int id;
	const PmDeviceInfo *dev;
	mio_device_t *mio_dev;
	
	s_device_count = Pm_CountDevices();
	
	s_devices = malloc(sizeof(mio_device_t) * s_device_count);
	
	for (id = 0; id < s_device_count; id++) {
		dev = Pm_GetDeviceInfo(id);
		mio_dev = &s_devices[id];
		mio_dev->id = id;
		mio_dev->name = dev->name;
		mio_dev->interface = dev->interf;
		mio_dev->input = dev->input;
		mio_dev->output = dev->output;
	}
}
