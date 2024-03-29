/* Copyright (c) 2012 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/*
 * Remote Stream - An audio steam from/to a client.
 */
#ifndef CRAS_RSTREAM_H_
#define CRAS_RSTREAM_H_

#include "cras_shm.h"
#include "cras_types.h"

struct cras_iodev;
struct cras_rclient;

/* cras_rstream is used to manage an active audio stream from
 * a client.  Each client can have any number of open streams for
 * playing or recording.
 */
struct cras_rstream {
	cras_stream_id_t stream_id;
	enum CRAS_STREAM_TYPE stream_type;
	enum CRAS_STREAM_DIRECTION direction;
	int fd; /* Socket for requesting and sending audio buffer events. */
	int shm_key;
	int shm_id;
	size_t shm_size;
	size_t buffer_frames; /* Buffer size in frames. */
	size_t cb_threshold; /* Callback client when this much is left. */
	size_t min_cb_level; /* Don't callback unless this much is avail. */
	uint32_t flags;
	struct cras_rclient *client;
	struct cras_audio_shm_area *shm;
	struct cras_iodev *iodev;
	struct cras_rstream *prev, *next;
	struct cras_audio_format format;
};

/* Creates an rstream.
 * Args:
 *    stream_type - CRAS_STREAM_TYPE.
 *    direction - CRAS_STREAM_OUTPUT or CRAS_STREAM_INPUT.
 *    format - The audio format the stream wishes to use.
 *    buffer_frames - Total number of audio frames to buffer.
 *    cb_threshold - # of frames when to request more from the client.
 *    min_cb_level - Minimum # of frames to request from the client.
 *    flags - Unused.
 *    client - The client that owns this stream.
 *    stream_out - Filled with the newly created stream pointer.
 * Returns:
 *    0 on success, EINVAL if an invalid argument is passed, or ENOMEM if out of
 *    memory.
 */
int cras_rstream_create(cras_stream_id_t stream_id,
			enum CRAS_STREAM_TYPE stream_type,
			enum CRAS_STREAM_DIRECTION direction,
			const struct cras_audio_format *format,
			size_t buffer_frames,
			size_t cb_threshold,
			size_t min_cb_level,
			uint32_t flags,
			struct cras_rclient *client,
			struct cras_rstream **stream_out);
/* Destroys an rstream. */
void cras_rstream_destroy(struct cras_rstream *stream);

/* Gets the total buffer size in frames for the given client stream. */
static inline size_t cras_rstream_get_buffer_size(
		const struct cras_rstream *stream)
{
	return stream->buffer_frames;
}

/* Gets the callback threshold in frames for the given client stream. */
static inline size_t cras_rstream_get_cb_threshold(
		const struct cras_rstream *stream)
{
	return stream->cb_threshold;
}

/* Gets the min cb threshold in frames for the given client stream. */
static inline size_t cras_rstream_get_min_cb_level(
		const struct cras_rstream *stream)
{
	return stream->min_cb_level;
}

/* Gets the stream type of this stream. */
static inline enum CRAS_STREAM_TYPE cras_rstream_get_type(
		const struct cras_rstream *stream)
{
	return stream->stream_type;
}

/* Gets the direction (input/output) of the stream. */
static inline enum CRAS_STREAM_DIRECTION cras_rstream_get_direction(
		const struct cras_rstream *stream)
{
	return stream->direction;
}

/* Gets the format for the stream. */
static inline void cras_rstream_set_format(struct cras_rstream *stream,
					   const struct cras_audio_format *fmt)
{
	stream->format = *fmt;
}

/* Sets the format for the stream. */
static inline int cras_rstream_get_format(const struct cras_rstream *stream,
					  struct cras_audio_format *fmt)
{
	*fmt = stream->format;
	return 0;
}

/* Gets fd to be used to poll this client for audio. */
static inline void cras_rstream_set_audio_fd(struct cras_rstream *stream,
					     int fd)
{
	stream->fd = fd;
}

/* Sets fd to be used to poll this client for audio. */
static inline int cras_rstream_get_audio_fd(const struct cras_rstream *stream)
{
	return stream->fd;
}

/* Gets the shm key used to find the shm region. */
static inline int cras_rstream_get_shm_key(const struct cras_rstream *stream)
{
	return stream->shm_key;
}

/* Gets the total size of shm memory allocated. */
static inline size_t cras_rstream_get_total_shm_size(
		const struct cras_rstream *stream)
{
	return stream->shm->size + sizeof(struct cras_audio_shm_area);
}

/* Gets shared memory region for this stream. */
static inline struct cras_audio_shm_area *cras_rstream_get_shm(
		const struct cras_rstream *stream)
{
	return stream->shm;
}

/* Gets the size of the shared memory region for this stream. */
static inline size_t cras_rstream_get_shm_size(
		const struct cras_rstream *stream)
{
	return stream->shm->size;
}

/* Gets the input/output device for a stream. */
static inline struct cras_iodev *cras_rstream_get_iodev(
		const struct cras_rstream *s)
{
	return s->iodev;
}

/* Sets the input/output device for a stream. */
static inline void cras_rstream_set_iodev(struct cras_rstream *s,
					  struct cras_iodev *o)
{
	s->iodev = o;
}

/* Requests data from the stream will request at least min_req frames. */
int cras_rstream_request_audio(const struct cras_rstream *stream, size_t count);
/* Requests a complete buffer worth of audio. */
int cras_rstream_request_audio_buffer(const struct cras_rstream *stream);
/* Tells a capture client that count frames are ready. */
int cras_rstream_audio_ready(const struct cras_rstream *stream, size_t count);
/* Waits for the response to a request for audio. */
int cras_rstream_get_audio_request_reply(const struct cras_rstream *stream);
/* Sends a message to the client telling him to re-attach the stream. Used when
 * moving a stream between io devices. */
void cras_rstream_send_client_reattach(const struct cras_rstream *stream);

#endif /* CRAS_RSTREAM_H_ */
