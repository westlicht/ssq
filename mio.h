#ifndef __MIO_H__
#define __MIO_H__

#include "portmidi.h"

#define MIO_BUF_LEN 1024

/** midi io device information */
typedef struct {
	int id;
	const char *name;
	const char *interface;
	int input;
	int output;
} mio_device_t;

/** midi input or output stream */
typedef struct {
	mio_device_t *dev;
	PmStream *stream;
} mio_stream_t;

/** timestamp */
typedef long mio_timestamp_t;

/* some macros to read/write midi messages */
#define mio_message(cmd, channel, data1, data2) \
	((((data2) << 16) & 0xFF0000) | \
	(((data1) << 8) & 0xFF00) | \
	((cmd | channel) & 0xFF))
#define mio_message_status(msg)  ((msg) & 0xff)
#define mio_message_cmd(msg)     ((msg) & 0xf0)
#define mio_message_channel(msg) ((msg) & 0x0f)
#define mio_message_data1(msg)   (((msg) >> 8) & 0xff)
#define mio_message_data2(msg)   (((msg) >> 16) & 0xff)

/* midi commands */
#define MIO_CMD_NOTE_OFF           0x80
#define MIO_CMD_NOTE_ON            0x90
#define MIO_CMD_AFTERTOUCH         0xa0
#define MIO_CMD_CONTROL_CHANGE     0xb0
#define MIO_CMD_PROGRAM_CHANGE     0xc0
#define MIO_CMD_CHANNEL_PRESSURE   0xd0
#define MIO_CMD_PITCH_WHEEL        0xe0

/** message */
typedef long mio_message_t;

/** event */
typedef struct {
	mio_message_t message;
	mio_timestamp_t timestamp;
} mio_event_t;

/**
 * Initializes the midi io subsystem.
 * @return Returns 0 if succecssful.
 */
int mio_init(void);

/**
 * Shuts the midi io subsystem down.
 */
void mio_shutdown(void);

/**
 * Returns the current timestamp.
 * @return Returns the current timestamp.
 */
mio_timestamp_t mio_get_timestamp(void);

/**
 * Returns the number of midi io devices.
 * @return Returns the number of midi io devices.
 */
int mio_get_device_count(void);

/**
 * Returns a midi io device.
 * @param id Device id
 * @return Returns the midi io device.
 */
mio_device_t *mio_get_device(int id);

/**
 * Returns a midi io device by name.
 * @param name Name
 * @return Returns the midi io with the given name or NULL if not found.
 */
mio_device_t *mio_get_input_device_by_name(const char *name);

/**
 * Returns a midi io device by name.
 * @param name Name
 * @return Returns the midi io with the given name or NULL if not found.
 */
mio_device_t *mio_get_output_device_by_name(const char *name);

/**
 * Opens a midi input stream.
 * @param stream Stream
 * @param dev Device
 * @return Returns 0 if successful.
 */
int mio_open_input(mio_stream_t *stream, mio_device_t *dev);

/**
 * Opens a midi output stream.
 * @param stream Stream
 * @param dev device
 * @param latency Outpu latency in ms
 * @return Returns 0 if successful.
 */
int mio_open_output(mio_stream_t *stream, mio_device_t *dev, int latency);

/**
 * Closes a midi stream.
 * @param stream Stream
 */
void mio_close(mio_stream_t *stream);

/**
 * Reads from an input stream.
 * @param stream Stream
 * @param buf Event buffer
 * @param len Length of event buffer
 */
int mio_read(mio_stream_t *stream, mio_event_t *buf, int len);

/**
 * Writes to an output stream.
 * @param stream Stream
 * @param buf Event buffer
 * @param len Length of event buffer
 * @return Returns 0 if successful.
 */
int mio_write(mio_stream_t *stream, mio_event_t *buf, int len);

#endif /*__MIO_H__*/
