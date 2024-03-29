// Copyright (c) 2012 The Chromium OS Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stdio.h>
#include <sys/select.h>
#include <gtest/gtest.h>

extern "C" {
//  Override select so it can be stubbed.
static int select_return_value;
static struct timeval select_timeval;
static int select_max_fd;
static fd_set select_in_fds;
static fd_set select_out_fds;
int ut_select(int nfds,
              fd_set *readfds,
              fd_set *writefds,
              fd_set *exceptfds,
              struct timeval *timeout) {
  select_max_fd = nfds;
  select_timeval.tv_sec = timeout->tv_sec;
  select_timeval.tv_usec = timeout->tv_usec;
  select_in_fds = *readfds;
  *readfds = select_out_fds;
  return select_return_value;
}
#define select ut_select

#include "cras_shm.h"
#include "cras_system_state.h"
#include "cras_types.h"
#include "cras_alsa_mixer.h"

//  Include C file to test static functions.
#include "cras_alsa_io.c"
}

//  Data for simulating functions stubbed below.
static int cras_alsa_open_called;
static int cras_iodev_append_stream_ret;
static int cras_alsa_get_avail_frames_ret;
static int cras_alsa_get_avail_frames_avail;
static int cras_alsa_start_called;
static int cras_rstream_audio_ready_count;
static uint8_t *cras_alsa_mmap_begin_buffer;
static size_t cras_alsa_mmap_begin_frames;
static size_t cras_mix_add_stream_count;
static int cras_mix_add_stream_dont_fill_next;
static size_t cras_rstream_request_audio_called;
static size_t cras_alsa_fill_properties_called;
static size_t alsa_mixer_set_dBFS_called;
static int alsa_mixer_set_dBFS_value;
static const struct cras_alsa_mixer_output *alsa_mixer_set_dBFS_output;
static size_t alsa_mixer_set_capture_dBFS_called;
static int alsa_mixer_set_capture_dBFS_value;
static size_t cras_alsa_mixer_list_outputs_called;
static size_t cras_alsa_mixer_list_outputs_device_value;
static cras_system_volume_changed_cb sys_register_volume_cb_value;
static void * sys_register_volume_cb_arg;
static size_t sys_register_volume_cb_called;
static size_t sys_remove_volume_cb_called;
static size_t sys_get_volume_called;
static size_t sys_get_volume_return_value;
static cras_system_volume_changed_cb sys_register_capture_gain_cb_value;
static void * sys_register_capture_gain_cb_arg;
static size_t sys_register_capture_gain_cb_called;
static size_t sys_remove_capture_gain_cb_called;
static size_t sys_get_capture_gain_called;
static long sys_get_capture_gain_return_value;
static size_t alsa_mixer_set_mute_called;
static int alsa_mixer_set_mute_value;
static const struct cras_alsa_mixer_output *alsa_mixer_set_mute_output;
static cras_system_volume_changed_cb sys_register_mute_cb_value;
static size_t alsa_mixer_set_capture_mute_called;
static int alsa_mixer_set_capture_mute_value;
static cras_system_volume_changed_cb sys_register_capture_mute_cb_value;
static void * sys_register_mute_cb_arg;
static size_t sys_register_mute_cb_called;
static size_t sys_remove_mute_cb_called;
static size_t sys_get_mute_called;
static int sys_get_mute_return_value;
static void * sys_register_capture_mute_cb_arg;
static size_t sys_register_capture_mute_cb_called;
static size_t sys_remove_capture_mute_cb_called;
static size_t sys_get_capture_mute_called;
static int sys_get_capture_mute_return_value;
static struct cras_alsa_mixer *fake_mixer = (struct cras_alsa_mixer *)1;
static struct cras_alsa_mixer_output **cras_alsa_mixer_list_outputs_outputs;
static size_t cras_alsa_mixer_list_outputs_outputs_length;
static size_t cras_alsa_mixer_set_output_active_state_called;
static std::vector<struct cras_alsa_mixer_output *>
    cras_alsa_mixer_set_output_active_state_outputs;
static std::vector<int> cras_alsa_mixer_set_output_active_state_values;
static size_t cras_alsa_mixer_default_volume_curve_called;
static cras_volume_curve *fake_curve;
static size_t cras_iodev_post_message_to_playback_thread_called;
static size_t cras_iodev_init_called;
static size_t cras_iodev_deinit_called;
static size_t sys_set_volume_limits_called;
static size_t sys_set_capture_gain_limits_called;
static size_t cras_alsa_mixer_get_minimum_capture_gain_called;
static size_t cras_alsa_mixer_get_maximum_capture_gain_called;
static size_t cras_alsa_jack_list_create_called;
static size_t cras_alsa_jack_list_destroy_called;
static jack_state_change_callback *cras_alsa_jack_list_create_cb;
static void *cras_alsa_jack_list_create_cb_data;
static size_t cras_iodev_move_stream_type_called;
static size_t cras_iodev_move_stream_type_default_called;

void ResetStubData() {
  cras_alsa_open_called = 0;
  cras_iodev_append_stream_ret = 0;
  cras_alsa_get_avail_frames_ret = 0;
  cras_alsa_get_avail_frames_avail = 0;
  cras_alsa_start_called = 0;
  select_return_value = 0;
  cras_rstream_request_audio_called = 0;
  select_max_fd = -1;
  cras_mix_add_stream_dont_fill_next = 0;
  cras_alsa_fill_properties_called = 0;
  sys_register_volume_cb_called = 0;
  sys_remove_volume_cb_called = 0;
  sys_get_volume_called = 0;
  sys_register_capture_gain_cb_called = 0;
  sys_remove_capture_gain_cb_called = 0;
  sys_get_capture_gain_called = 0;
  alsa_mixer_set_dBFS_called = 0;
  alsa_mixer_set_capture_dBFS_called = 0;
  sys_register_mute_cb_called = 0;
  sys_remove_mute_cb_called = 0;
  sys_get_mute_called = 0;
  sys_register_capture_mute_cb_called = 0;
  sys_remove_capture_mute_cb_called = 0;
  sys_get_capture_mute_called = 0;
  alsa_mixer_set_mute_called = 0;
  alsa_mixer_set_capture_mute_called = 0;
  cras_alsa_mixer_list_outputs_called = 0;
  cras_alsa_mixer_list_outputs_outputs_length = 0;
  cras_alsa_mixer_set_output_active_state_called = 0;
  cras_alsa_mixer_set_output_active_state_outputs.clear();
  cras_alsa_mixer_set_output_active_state_values.clear();
  cras_alsa_mixer_default_volume_curve_called = 0;
  cras_iodev_post_message_to_playback_thread_called = 0;
  cras_iodev_init_called = 0;
  cras_iodev_deinit_called = 0;
  sys_set_volume_limits_called = 0;
  sys_set_capture_gain_limits_called = 0;
  cras_alsa_mixer_get_minimum_capture_gain_called = 0;
  cras_alsa_mixer_get_maximum_capture_gain_called = 0;
  cras_alsa_jack_list_create_called = 0;
  cras_alsa_jack_list_destroy_called = 0;
  cras_iodev_move_stream_type_called = 0;
  cras_iodev_move_stream_type_default_called = 0;
}

static long fake_get_dBFS(const cras_volume_curve *curve, size_t volume)
{
  return (volume - 100) * 100;
}

namespace {

TEST(AlsaIoInit, InitializePlayback) {
  struct alsa_io *aio;
  struct cras_alsa_mixer * const fake_mixer = (struct cras_alsa_mixer*)2;

  ResetStubData();
  aio = (struct alsa_io *)alsa_iodev_create(0, 0,
                                            fake_mixer, 0,
                                            CRAS_STREAM_OUTPUT);
  ASSERT_NE(aio, (void *)NULL);
  EXPECT_EQ(SND_PCM_STREAM_PLAYBACK, aio->alsa_stream);
  EXPECT_EQ((void *)possibly_fill_audio, (void *)aio->alsa_cb);
  EXPECT_EQ(1, cras_alsa_fill_properties_called);
  EXPECT_EQ(1, cras_alsa_mixer_list_outputs_called);
  EXPECT_EQ(0, cras_alsa_mixer_list_outputs_device_value);

  alsa_iodev_destroy((struct cras_iodev *)aio);
}

TEST(AlsaIoInit, RouteBasedOnJackCallback) {
  struct alsa_io *aio;
  struct cras_alsa_mixer * const fake_mixer = (struct cras_alsa_mixer*)2;

  ResetStubData();
  aio = (struct alsa_io *)alsa_iodev_create(0, 0,
                                            fake_mixer, 0,
                                            CRAS_STREAM_OUTPUT);
  ASSERT_NE(aio, (void *)NULL);
  EXPECT_EQ(SND_PCM_STREAM_PLAYBACK, aio->alsa_stream);
  EXPECT_EQ((void *)possibly_fill_audio, (void *)aio->alsa_cb);
  EXPECT_EQ(1, cras_alsa_fill_properties_called);
  EXPECT_EQ(1, cras_alsa_mixer_list_outputs_called);
  EXPECT_EQ(0, cras_alsa_mixer_list_outputs_device_value);
  EXPECT_EQ(1, cras_alsa_jack_list_create_called);

  fake_curve =
    static_cast<struct cras_volume_curve *>(calloc(1, sizeof(*fake_curve)));
  fake_curve->get_dBFS = fake_get_dBFS;

  cras_alsa_jack_list_create_cb(NULL, 1, cras_alsa_jack_list_create_cb_data);
  EXPECT_EQ(1, cras_iodev_move_stream_type_called);
  cras_alsa_jack_list_create_cb(NULL, 0, cras_alsa_jack_list_create_cb_data);
  EXPECT_EQ(1, cras_iodev_move_stream_type_default_called);

  alsa_iodev_destroy((struct cras_iodev *)aio);
  EXPECT_EQ(1, cras_alsa_jack_list_destroy_called);
  free(fake_curve);
}

TEST(AlsaIoInit, InitializeCapture) {
  struct alsa_io *aio;

  ResetStubData();
  aio = (struct alsa_io *)alsa_iodev_create(0, 0,
                                            fake_mixer, 0,
                                            CRAS_STREAM_INPUT);
  ASSERT_NE(aio, (void *)NULL);
  EXPECT_EQ(SND_PCM_STREAM_CAPTURE, aio->alsa_stream);
  EXPECT_EQ((void *)possibly_read_audio, (void *)aio->alsa_cb);
  EXPECT_EQ(1, cras_alsa_fill_properties_called);

  alsa_iodev_destroy((struct cras_iodev *)aio);
}

//  Test handling of different amounts of outputs.
TEST(AlsaOutputNode, TwoOutputs) {
  int rc;
  struct alsa_io *aio;
  struct cras_alsa_mixer * const fake_mixer = (struct cras_alsa_mixer*)2;
  struct cras_alsa_mixer_output *outputs[2];
  struct cras_alsa_mixer_output *fake_output =
      reinterpret_cast<struct cras_alsa_mixer_output *>(7);

  ResetStubData();
  outputs[0] =
    static_cast<struct cras_alsa_mixer_output *>(calloc(1, sizeof(**outputs)));
  outputs[1] =
    static_cast<struct cras_alsa_mixer_output *>(calloc(1, sizeof(**outputs)));
  fake_curve =
    static_cast<struct cras_volume_curve *>(calloc(1, sizeof(*fake_curve)));
  fake_curve->get_dBFS = fake_get_dBFS;
  outputs[0]->volume_curve = fake_curve;
  outputs[1]->volume_curve = fake_curve;
  cras_alsa_mixer_list_outputs_outputs = outputs;
  cras_alsa_mixer_list_outputs_outputs_length = ARRAY_SIZE(outputs);
  aio = (struct alsa_io *)alsa_iodev_create(0, 0,
                                            fake_mixer, 0,
                                            CRAS_STREAM_OUTPUT);
  ASSERT_NE(aio, (void *)NULL);
  EXPECT_EQ(SND_PCM_STREAM_PLAYBACK, aio->alsa_stream);
  EXPECT_EQ(1, cras_alsa_mixer_list_outputs_called);
  EXPECT_EQ(0, cras_alsa_mixer_list_outputs_device_value);

  rc = alsa_iodev_set_active_output((struct cras_iodev *)aio, fake_output);
  EXPECT_EQ(-EINVAL, rc);
  ResetStubData();
  rc = alsa_iodev_set_active_output((struct cras_iodev *)aio, outputs[0]);
  EXPECT_EQ(0, rc);
  EXPECT_EQ(2, alsa_mixer_set_mute_called);
  EXPECT_EQ(outputs[0], alsa_mixer_set_mute_output);
  EXPECT_EQ(1, alsa_mixer_set_dBFS_called);
  EXPECT_EQ(outputs[0], alsa_mixer_set_dBFS_output);
  ASSERT_EQ(2, cras_alsa_mixer_set_output_active_state_called);
  EXPECT_EQ(outputs[0], cras_alsa_mixer_set_output_active_state_outputs[0]);
  EXPECT_EQ(1, cras_alsa_mixer_set_output_active_state_values[0]);
  EXPECT_EQ(outputs[1], cras_alsa_mixer_set_output_active_state_outputs[1]);
  EXPECT_EQ(0, cras_alsa_mixer_set_output_active_state_values[1]);

  alsa_iodev_destroy((struct cras_iodev *)aio);
  free(outputs[0]);
  free(outputs[1]);
  free(fake_curve);
}

//  Test thread add/rm stream, open_alsa, and iodev config.
class AlsaAddStreamSuite : public testing::Test {
  protected:
    virtual void SetUp() {
      aio_output_ = (struct alsa_io *)alsa_iodev_create(0, 0, fake_mixer, 0,
          CRAS_STREAM_OUTPUT);
      aio_output_->base.direction = CRAS_STREAM_OUTPUT;
      aio_input_ = (struct alsa_io *)alsa_iodev_create(0, 0, fake_mixer, 0,
          CRAS_STREAM_INPUT);
      aio_input_->base.direction = CRAS_STREAM_INPUT;
      fmt_.frame_rate = 44100;
      fmt_.num_channels = 2;
      fmt_.format = SND_PCM_FORMAT_S16_LE;
      aio_input_->base.format = &fmt_;
      aio_output_->base.format = &fmt_;
      ResetStubData();
      cras_alsa_get_avail_frames_ret = -1;
      fake_curve =
        static_cast<struct cras_volume_curve *>(calloc(1, sizeof(*fake_curve)));
      fake_curve->get_dBFS = fake_get_dBFS;
    }

    virtual void TearDown() {
      alsa_iodev_destroy((struct cras_iodev *)aio_output_);
      alsa_iodev_destroy((struct cras_iodev *)aio_input_);
      cras_alsa_get_avail_frames_ret = 0;
      free(fake_curve);
    }

  struct alsa_io *aio_output_;
  struct alsa_io *aio_input_;
  struct cras_audio_format fmt_;
};

TEST_F(AlsaAddStreamSuite, SimpleAddOutputStream) {
  int rc;
  struct cras_rstream *new_stream;
  struct cras_audio_format *fmt;
  const size_t fake_system_volume = 55;
  const size_t fake_system_volume_dB = (fake_system_volume - 100) * 100;

  fmt = (struct cras_audio_format *)malloc(sizeof(*fmt));
  memcpy(fmt, &fmt_, sizeof(fmt_));
  aio_output_->base.format = fmt;
  new_stream = (struct cras_rstream *)calloc(1, sizeof(*new_stream));
  new_stream->fd = 55;
  new_stream->buffer_frames = 65;
  new_stream->cb_threshold = 80;
  memcpy(&new_stream->format, fmt, sizeof(*fmt));
  aio_output_->num_underruns = 3; //  Something non-zero.
  sys_get_volume_return_value = fake_system_volume;
  rc = thread_add_stream(aio_output_, new_stream);
  ASSERT_EQ(0, rc);
  EXPECT_EQ(55, aio_output_->base.streams->stream->fd);
  EXPECT_EQ(1, cras_alsa_open_called);
  EXPECT_EQ(SND_PCM_FORMAT_S16_LE, aio_output_->base.format->format);
  //  open_alsa should configure the following.
  EXPECT_EQ(0, aio_output_->num_underruns);
  EXPECT_EQ(0, cras_alsa_start_called); //  Shouldn't start playback.
  EXPECT_NE((void *)NULL, aio_output_->handle);
  EXPECT_EQ(1, alsa_mixer_set_dBFS_called);
  EXPECT_EQ(fake_system_volume_dB, alsa_mixer_set_dBFS_value);
  EXPECT_EQ(1, sys_set_volume_limits_called);
  EXPECT_EQ(1, sys_register_volume_cb_called);
  EXPECT_EQ(1, sys_register_mute_cb_called);
  EXPECT_EQ(1, alsa_mixer_set_mute_called);
  EXPECT_EQ(0, alsa_mixer_set_mute_value);

  //  remove the stream.
  rc = thread_remove_stream(aio_output_, new_stream);
  EXPECT_EQ(0, rc);
  EXPECT_EQ((void *)NULL, aio_output_->handle);
  EXPECT_EQ(1, sys_remove_volume_cb_called);
  EXPECT_EQ(1, sys_remove_mute_cb_called);

  free(new_stream);
}

TEST_F(AlsaAddStreamSuite, AddRmTwoOutputStreams) {
  int rc;
  struct cras_rstream *new_stream, *second_stream;
  struct cras_audio_format *fmt;

  fmt = (struct cras_audio_format *)malloc(sizeof(*fmt));
  memcpy(fmt, &fmt_, sizeof(fmt_));
  aio_output_->base.format = fmt;
  new_stream = (struct cras_rstream *)calloc(1, sizeof(*new_stream));
  new_stream->fd = 55;
  new_stream->buffer_frames = 65;
  new_stream->cb_threshold = 80;
  memcpy(&new_stream->format, fmt, sizeof(*fmt));
  rc = thread_add_stream(aio_output_, new_stream);
  ASSERT_EQ(0, rc);

  //  Second stream has lower latency(config_alsa_iodev_params should re-config)
  second_stream = (struct cras_rstream *)calloc(1, sizeof(*second_stream));
  second_stream->fd = 56;
  second_stream->buffer_frames = 25;
  second_stream->cb_threshold = 12;
  memcpy(&second_stream->format, fmt, sizeof(*fmt));
  rc = thread_add_stream(aio_output_, second_stream);
  ASSERT_EQ(0, rc);
  EXPECT_EQ(SND_PCM_FORMAT_S16_LE, aio_output_->base.format->format);

  //  remove the stream.
  rc = thread_remove_stream(aio_output_, second_stream);
  EXPECT_EQ(0, rc);
  EXPECT_NE((void *)NULL, aio_output_->handle);
  rc = thread_remove_stream(aio_output_, new_stream);
  EXPECT_EQ(0, rc);
  EXPECT_EQ((void *)NULL, aio_output_->handle);

  free(new_stream);
  free(second_stream);
}

TEST_F(AlsaAddStreamSuite, SetVolumeAndMute) {
  int rc;
  struct cras_rstream *new_stream;
  struct cras_audio_format *fmt;
  const size_t fake_system_volume = 55;
  const size_t fake_system_volume_dB = (fake_system_volume - 100) * 100;

  fmt = (struct cras_audio_format *)malloc(sizeof(*fmt));
  memcpy(fmt, &fmt_, sizeof(fmt_));
  aio_output_->base.format = fmt;
  new_stream = (struct cras_rstream *)calloc(1, sizeof(*new_stream));
  new_stream->fd = 55;
  new_stream->buffer_frames = 65;
  new_stream->cb_threshold = 80;
  memcpy(&new_stream->format, fmt, sizeof(*fmt));
  aio_output_->num_underruns = 3; //  Something non-zero.
  sys_get_volume_return_value = fake_system_volume;
  rc = thread_add_stream(aio_output_, new_stream);
  ASSERT_EQ(0, rc);
  EXPECT_EQ(1, alsa_mixer_set_dBFS_called);
  EXPECT_EQ(fake_system_volume_dB, alsa_mixer_set_dBFS_value);
  EXPECT_EQ(1, sys_register_volume_cb_called);
  EXPECT_EQ(1, sys_register_mute_cb_called);
  EXPECT_EQ(1, alsa_mixer_set_mute_called);
  EXPECT_EQ(0, alsa_mixer_set_mute_value);

  alsa_mixer_set_mute_called = 0;
  alsa_mixer_set_mute_value = 0;
  alsa_mixer_set_dBFS_called = 0;
  alsa_mixer_set_dBFS_value = 0;
  sys_get_volume_return_value = 50;
  sys_register_volume_cb_value(sys_register_volume_cb_arg);
  EXPECT_EQ(1, alsa_mixer_set_mute_called);
  EXPECT_EQ(0, alsa_mixer_set_mute_value);
  EXPECT_EQ(1, alsa_mixer_set_dBFS_called);
  EXPECT_EQ(-5000, alsa_mixer_set_dBFS_value);
  EXPECT_EQ(NULL, alsa_mixer_set_dBFS_output);

  alsa_mixer_set_mute_called = 0;
  alsa_mixer_set_mute_value = 0;
  alsa_mixer_set_dBFS_called = 0;
  alsa_mixer_set_dBFS_value = 0;
  sys_get_volume_return_value = 0;
  sys_register_volume_cb_value(sys_register_volume_cb_arg);
  EXPECT_EQ(1, alsa_mixer_set_mute_called);
  EXPECT_EQ(1, alsa_mixer_set_mute_value);
  EXPECT_EQ(1, alsa_mixer_set_dBFS_called);
  EXPECT_EQ(-10000, alsa_mixer_set_dBFS_value);

  //  remove the stream.
  rc = thread_remove_stream(aio_output_, new_stream);
  EXPECT_EQ(0, rc);
  EXPECT_EQ((void *)NULL, aio_output_->handle);
  EXPECT_EQ(1, sys_remove_volume_cb_called);
  EXPECT_EQ(1, sys_remove_mute_cb_called);

  free(new_stream);
}

TEST_F(AlsaAddStreamSuite, AppendStreamErrorPropogated) {
  int rc;
  struct cras_rstream *new_stream;
  cras_iodev_append_stream_ret = -10;
  new_stream = (struct cras_rstream *)calloc(1, sizeof(*new_stream));
  rc = thread_add_stream(aio_output_, new_stream);
  EXPECT_EQ(-10, rc);
  free(new_stream);
}

TEST_F(AlsaAddStreamSuite, SimpleAddInputStream) {
  int rc;
  struct cras_rstream *new_stream;
  struct cras_audio_format *fmt;

  cras_alsa_open_called = 0;
  fmt = (struct cras_audio_format *)malloc(sizeof(*fmt));
  memcpy(fmt, &fmt_, sizeof(fmt_));
  aio_input_->base.format = fmt;
  new_stream = (struct cras_rstream *)calloc(1, sizeof(*new_stream));
  new_stream->fd = 55;
  memcpy(&new_stream->format, fmt, sizeof(*fmt));
  rc = thread_add_stream(aio_input_, new_stream);
  ASSERT_EQ(0, rc);
  EXPECT_EQ(55, aio_input_->base.streams->stream->fd);
  EXPECT_EQ(1, cras_alsa_open_called);
  EXPECT_EQ(1, cras_alsa_start_called); //  Shouldn start capture.
  EXPECT_EQ(1, sys_set_capture_gain_limits_called);
  EXPECT_EQ(1, sys_register_capture_gain_cb_called);
  EXPECT_EQ(1, sys_register_capture_mute_cb_called);
  rc = thread_remove_stream(aio_input_, new_stream);
  EXPECT_EQ(0, rc);
  EXPECT_EQ(1, sys_remove_capture_gain_cb_called);
  EXPECT_EQ(1, sys_remove_capture_mute_cb_called);
  free(new_stream);
}

TEST_F(AlsaAddStreamSuite, OneInputStreamPerDevice) {
  int rc;
  struct cras_rstream *new_stream;

  cras_alsa_open_called = 0;
  new_stream = (struct cras_rstream *)calloc(1, sizeof(*new_stream));
  aio_input_->base.streams = reinterpret_cast<struct cras_io_stream*>(0x01);
  rc = thread_add_stream(aio_input_, new_stream);
  EXPECT_NE(0, rc);
  EXPECT_EQ(0, cras_alsa_open_called);
  free(new_stream);
}

//  Test the audio capture path, this involves a lot of setup before calling the
//  funcitons we want to test.  Will need to setup the device, a fake stream,
//  and a fake shm area to put samples in.
class AlsaCaptureStreamSuite : public testing::Test {
  protected:
    virtual void SetUp() {
      aio_ = (struct alsa_io *)alsa_iodev_create(0, 0, fake_mixer, 0,
          CRAS_STREAM_INPUT);
      fmt_.frame_rate = 44100;
      fmt_.num_channels = 2;
      fmt_.format = SND_PCM_FORMAT_S16_LE;
      aio_->base.format = &fmt_;
      aio_->base.buffer_size = 16384;
      aio_->base.cb_threshold = 480;

      shm_ = (struct cras_audio_shm_area *)calloc(1,
          sizeof(*shm_) + aio_->base.cb_threshold * 8);
      shm_->frame_bytes = 4;
      shm_->used_size = aio_->base.cb_threshold * 4; // channels * bytes/sample
      shm_->size = shm_->used_size * 2;

      rstream_ = (struct cras_rstream *)calloc(1, sizeof(*rstream_));
      rstream_->shm = shm_;
      memcpy(&rstream_->format, &fmt_, sizeof(fmt_));

      cras_iodev_append_stream(&aio_->base, rstream_);

      cras_alsa_mmap_begin_buffer = (uint8_t *)malloc(shm_->used_size);
      cras_alsa_mmap_begin_frames = aio_->base.cb_threshold;

      ResetStubData();
    }

    virtual void TearDown() {
      free(cras_alsa_mmap_begin_buffer);
      cras_iodev_delete_stream(&aio_->base, rstream_);
      alsa_iodev_destroy((struct cras_iodev *)aio_);
      free(rstream_);
      free(shm_);
    }

  struct alsa_io *aio_;
  struct cras_rstream *rstream_;
  struct cras_audio_format fmt_;
  struct cras_audio_shm_area *shm_;
};

TEST_F(AlsaCaptureStreamSuite, PossiblyReadGetAvailError) {
  struct timespec ts;
  int rc;

  cras_alsa_get_avail_frames_ret = -4;
  rc = possibly_read_audio(aio_, &ts);
  EXPECT_EQ(-4, rc);
  EXPECT_EQ(0, ts.tv_sec);
  EXPECT_EQ(0, ts.tv_nsec);
}

TEST_F(AlsaCaptureStreamSuite, PossiblyReadEmpty) {
  struct timespec ts;
  int rc;
  uint64_t nsec_expected;

  //  If no samples are present, it should sleep for cb_threshold frames.
  cras_alsa_get_avail_frames_ret = 0;
  cras_alsa_get_avail_frames_avail = 0;
  nsec_expected = aio_->base.cb_threshold * 1000000000 / fmt_.frame_rate;
  rc = possibly_read_audio(aio_, &ts);
  EXPECT_EQ(0, rc);
  EXPECT_EQ(0, ts.tv_sec);
  EXPECT_GE(ts.tv_nsec, nsec_expected - 1000);
  EXPECT_LE(ts.tv_nsec, nsec_expected + 1000);
}

TEST_F(AlsaCaptureStreamSuite, PossiblyReadHasDataDrop) {
  struct timespec ts;
  int rc;
  uint64_t nsec_expected;

  //  A full block plus 4 frames.  No streams attached so samples are dropped.
  aio_->base.streams = NULL;
  cras_alsa_get_avail_frames_ret = 0;
  cras_alsa_get_avail_frames_avail = aio_->base.cb_threshold + 4;
  nsec_expected = (aio_->base.cb_threshold - 4) * 1000000000 / fmt_.frame_rate;
  rc = possibly_read_audio(aio_, &ts);
  EXPECT_EQ(0, rc);
  EXPECT_EQ(0, ts.tv_sec);
  EXPECT_GE(ts.tv_nsec, nsec_expected - 1000);
  EXPECT_LE(ts.tv_nsec, nsec_expected + 1000);
}

TEST_F(AlsaCaptureStreamSuite, PossiblyReadHasDataWriteStream) {
  struct timespec ts;
  int rc;
  uint64_t nsec_expected;

  //  A full block plus 4 frames.
  cras_alsa_get_avail_frames_ret = 0;
  cras_alsa_get_avail_frames_avail = aio_->base.cb_threshold + 4;
  nsec_expected = (aio_->base.cb_threshold - 4) * 1000000000 / fmt_.frame_rate;
  cras_rstream_audio_ready_count = 999;
  //  Give it some samples to copy.
  rc = possibly_read_audio(aio_, &ts);
  EXPECT_EQ(0, rc);
  EXPECT_EQ(0, ts.tv_sec);
  EXPECT_GE(ts.tv_nsec, nsec_expected - 1000);
  EXPECT_LE(ts.tv_nsec, nsec_expected + 1000);
  EXPECT_EQ(aio_->base.cb_threshold, cras_rstream_audio_ready_count);
  for (size_t i = 0; i < aio_->base.cb_threshold; i++)
    EXPECT_EQ(cras_alsa_mmap_begin_buffer[i], shm_->samples[i]);
}

TEST_F(AlsaCaptureStreamSuite, PossiblyReadWriteTwoBuffers) {
  struct timespec ts;
  int rc;

  //  A full block plus 4 frames.
  cras_alsa_get_avail_frames_ret = 0;
  cras_alsa_get_avail_frames_avail = aio_->base.cb_threshold + 4;
  cras_rstream_audio_ready_count = 999;
  //  Give it some samples to copy.
  rc = possibly_read_audio(aio_, &ts);
  EXPECT_EQ(0, rc);
  EXPECT_EQ(0, shm_->num_overruns);
  EXPECT_EQ(aio_->base.cb_threshold, cras_rstream_audio_ready_count);
  for (size_t i = 0; i < aio_->base.cb_threshold; i++)
    EXPECT_EQ(cras_alsa_mmap_begin_buffer[i], shm_->samples[i]);

  cras_rstream_audio_ready_count = 999;
  rc = possibly_read_audio(aio_, &ts);
  EXPECT_EQ(0, rc);
  EXPECT_EQ(0, shm_->num_overruns);
  EXPECT_EQ(aio_->base.cb_threshold, cras_rstream_audio_ready_count);
  for (size_t i = 0; i < aio_->base.cb_threshold; i++)
    EXPECT_EQ(cras_alsa_mmap_begin_buffer[i],
        shm_->samples[i + shm_->used_size]);
}

TEST_F(AlsaCaptureStreamSuite, PossiblyReadWriteThreeBuffers) {
  struct timespec ts;
  int rc;

  //  A full block plus 4 frames.
  cras_alsa_get_avail_frames_ret = 0;
  cras_alsa_get_avail_frames_avail = aio_->base.cb_threshold + 4;
  //  Give it some samples to copy.
  rc = possibly_read_audio(aio_, &ts);
  EXPECT_EQ(0, rc);
  EXPECT_EQ(0, shm_->num_overruns);
  EXPECT_EQ(aio_->base.cb_threshold, cras_rstream_audio_ready_count);
  for (size_t i = 0; i < aio_->base.cb_threshold; i++)
    EXPECT_EQ(cras_alsa_mmap_begin_buffer[i], shm_->samples[i]);

  cras_rstream_audio_ready_count = 999;
  rc = possibly_read_audio(aio_, &ts);
  EXPECT_EQ(0, rc);
  EXPECT_EQ(0, shm_->num_overruns);
  EXPECT_EQ(aio_->base.cb_threshold, cras_rstream_audio_ready_count);
  for (size_t i = 0; i < aio_->base.cb_threshold; i++)
    EXPECT_EQ(cras_alsa_mmap_begin_buffer[i],
        shm_->samples[i + shm_->used_size]);

  cras_rstream_audio_ready_count = 999;
  rc = possibly_read_audio(aio_, &ts);
  EXPECT_EQ(0, rc);
  EXPECT_EQ(1, shm_->num_overruns);  //  Should have overrun.
  EXPECT_EQ(aio_->base.cb_threshold, cras_rstream_audio_ready_count);
  for (size_t i = 0; i < aio_->base.cb_threshold; i++)
    EXPECT_EQ(cras_alsa_mmap_begin_buffer[i], shm_->samples[i]);
}

//  Test the audio playback path.
class AlsaPlaybackStreamSuite : public testing::Test {
  protected:
    virtual void SetUp() {
      aio_ = (struct alsa_io *)alsa_iodev_create(0, 0, fake_mixer, 0,
          CRAS_STREAM_OUTPUT);
      fmt_.frame_rate = 44100;
      fmt_.num_channels = 2;
      fmt_.format = SND_PCM_FORMAT_S16_LE;
      aio_->base.format = &fmt_;
      aio_->base.buffer_size = 16384;
      aio_->base.used_size = 480;
      aio_->base.cb_threshold = 96;

      SetupShm(&shm_);
      SetupShm(&shm2_);
      SetupRstream(&rstream_, shm_, 1);
      SetupRstream(&rstream2_, shm2_, 2);

      cras_iodev_append_stream(&aio_->base, rstream_);

      cras_alsa_mmap_begin_buffer = (uint8_t *)malloc(shm_->used_size);
      cras_alsa_mmap_begin_frames =
          aio_->base.used_size - aio_->base.cb_threshold;

      ResetStubData();
    }

    virtual void TearDown() {
      free(cras_alsa_mmap_begin_buffer);
      cras_iodev_delete_stream(&aio_->base, rstream_);
      alsa_iodev_destroy((struct cras_iodev *)aio_);
      free(rstream_);
      free(shm_);
    }

    void SetupShm(struct cras_audio_shm_area **shm) {
      *shm = (struct cras_audio_shm_area *)calloc(1,
          sizeof(**shm) + aio_->base.used_size * 8);
      (*shm)->frame_bytes = 4;
      (*shm)->used_size = aio_->base.used_size * 4; //  channels * bytes/sample
      (*shm)->size = (*shm)->used_size * 2;
    }

    void SetupRstream(struct cras_rstream **rstream,
                      struct cras_audio_shm_area *shm,
                      int fd) {
      *rstream = (struct cras_rstream *)calloc(1, sizeof(**rstream));
      (*rstream)->shm = shm;
      memcpy(&(*rstream)->format, &fmt_, sizeof(fmt_));
      (*rstream)->fd = fd;
    }

  struct alsa_io *aio_;
  struct cras_rstream *rstream_;
  struct cras_rstream *rstream2_;
  struct cras_audio_format fmt_;
  struct cras_audio_shm_area *shm_;
  struct cras_audio_shm_area *shm2_;
};

TEST_F(AlsaPlaybackStreamSuite, PossiblyFillGetAvailError) {
  struct timespec ts;
  int rc;

  cras_alsa_get_avail_frames_ret = -4;
  rc = possibly_fill_audio(aio_, &ts);
  EXPECT_EQ(-4, rc);
  EXPECT_EQ(0, ts.tv_sec);
  EXPECT_EQ(0, ts.tv_nsec);
}

TEST_F(AlsaPlaybackStreamSuite, PossiblyFillEarlyWake) {
  struct timespec ts;
  int rc;
  uint64_t nsec_expected;

  //  If woken and still have tons of data to play, go back to sleep.
  cras_alsa_get_avail_frames_ret = 0;
  cras_alsa_get_avail_frames_avail =
      aio_->base.buffer_size - aio_->base.cb_threshold * 2;
  nsec_expected = (aio_->base.cb_threshold) * 1000000000 / fmt_.frame_rate;
  rc = possibly_fill_audio(aio_, &ts);
  EXPECT_EQ(0, rc);
  EXPECT_EQ(0, ts.tv_sec);
  EXPECT_GE(ts.tv_nsec, nsec_expected - 1000);
  EXPECT_LE(ts.tv_nsec, nsec_expected + 1000);
}

TEST_F(AlsaPlaybackStreamSuite, PossiblyFillGetFromStreamFull) {
  struct timespec ts;
  int rc;
  uint64_t nsec_expected;

  //  Have cb_threshold samples left.
  cras_alsa_get_avail_frames_ret = 0;
  cras_alsa_get_avail_frames_avail =
      aio_->base.buffer_size - aio_->base.cb_threshold;
  nsec_expected = (aio_->base.used_size - aio_->base.cb_threshold) *
      1000000000 / fmt_.frame_rate;

  //  shm has plenty of data in it.
  shm_->write_offset[0] = shm_->used_size;

  FD_ZERO(&select_out_fds);
  FD_SET(rstream_->fd, &select_out_fds);
  select_return_value = 1;

  rc = possibly_fill_audio(aio_, &ts);
  EXPECT_EQ(0, rc);
  EXPECT_EQ(0, ts.tv_sec);
  EXPECT_GE(ts.tv_nsec, nsec_expected - 1000);
  EXPECT_LE(ts.tv_nsec, nsec_expected + 1000);
  EXPECT_EQ(aio_->base.used_size - aio_->base.cb_threshold,
            cras_mix_add_stream_count);
  EXPECT_EQ(0, cras_rstream_request_audio_called);
  EXPECT_EQ(-1, select_max_fd);
}

TEST_F(AlsaPlaybackStreamSuite, PossiblyFillGetFromStreamFullDoesntMix) {
  struct timespec ts;
  int rc;
  uint64_t nsec_expected;

  //  Have cb_threshold samples left.
  cras_alsa_get_avail_frames_ret = 0;
  cras_alsa_get_avail_frames_avail =
      aio_->base.buffer_size - aio_->base.cb_threshold;
  nsec_expected = (aio_->base.used_size - aio_->base.cb_threshold) *
      1000000000 / fmt_.frame_rate;

  //  shm has plenty of data in it.
  shm_->write_offset[0] = shm_->used_size;

  //  Test that nothing breaks if there is an empty stream.
  cras_mix_add_stream_dont_fill_next = 1;

  FD_ZERO(&select_out_fds);
  FD_SET(rstream_->fd, &select_out_fds);
  select_return_value = 1;

  rc = possibly_fill_audio(aio_, &ts);
  EXPECT_EQ(0, rc);
  EXPECT_EQ(0, cras_rstream_request_audio_called);
  EXPECT_EQ(-1, select_max_fd);
  EXPECT_EQ(0, shm_->read_offset[0]);
  EXPECT_EQ(0, shm_->read_offset[1]);
  EXPECT_EQ(shm_->used_size, shm_->write_offset[0]);
  EXPECT_EQ(0, shm_->write_offset[1]);
}

TEST_F(AlsaPlaybackStreamSuite, PossiblyFillGetFromStreamNeedFill) {
  struct timespec ts;
  int rc;
  uint64_t nsec_expected;

  //  Have cb_threshold samples left.
  cras_alsa_get_avail_frames_ret = 0;
  cras_alsa_get_avail_frames_avail =
      aio_->base.buffer_size - aio_->base.cb_threshold;
  nsec_expected = (aio_->base.used_size - aio_->base.cb_threshold) *
      1000000000 / fmt_.frame_rate;

  //  shm is out of data.
  shm_->write_offset[0] = 0;

  FD_ZERO(&select_out_fds);
  FD_SET(rstream_->fd, &select_out_fds);
  select_return_value = 1;

  rc = possibly_fill_audio(aio_, &ts);
  EXPECT_EQ(0, rc);
  EXPECT_EQ(0, ts.tv_sec);
  EXPECT_GE(ts.tv_nsec, nsec_expected - 1000);
  EXPECT_LE(ts.tv_nsec, nsec_expected + 1000);
  EXPECT_EQ(aio_->base.used_size - aio_->base.cb_threshold,
            cras_mix_add_stream_count);
  EXPECT_EQ(1, cras_rstream_request_audio_called);
  EXPECT_NE(-1, select_max_fd);
  EXPECT_EQ(0, memcmp(&select_out_fds, &select_in_fds, sizeof(select_in_fds)));
  EXPECT_EQ(0, shm_->read_offset[0]);
  EXPECT_EQ(0, shm_->write_offset[0]);
}

TEST_F(AlsaPlaybackStreamSuite, PossiblyFillGetFromTwoStreamsFull) {
  struct timespec ts;
  int rc;
  uint64_t nsec_expected;

  //  Have cb_threshold samples left.
  cras_alsa_get_avail_frames_ret = 0;
  cras_alsa_get_avail_frames_avail =
      aio_->base.buffer_size - aio_->base.cb_threshold;
  nsec_expected = (aio_->base.used_size - aio_->base.cb_threshold) *
      1000000000 / fmt_.frame_rate;

  //  shm has plenty of data in it.
  shm_->write_offset[0] = shm_->used_size;
  shm2_->write_offset[0] = shm2_->used_size;

  cras_iodev_append_stream(&aio_->base, rstream2_);

  rc = possibly_fill_audio(aio_, &ts);
  EXPECT_EQ(0, rc);
  EXPECT_EQ(0, ts.tv_sec);
  EXPECT_GE(ts.tv_nsec, nsec_expected - 1000);
  EXPECT_LE(ts.tv_nsec, nsec_expected + 1000);
  EXPECT_EQ(aio_->base.used_size - aio_->base.cb_threshold,
            cras_mix_add_stream_count);
  EXPECT_EQ(0, cras_rstream_request_audio_called);
  EXPECT_EQ(-1, select_max_fd);
}

TEST_F(AlsaPlaybackStreamSuite, PossiblyFillGetFromTwoStreamsFullOneMixes) {
  struct timespec ts;
  int rc;
  uint64_t nsec_expected;
  size_t written_expected;

  //  Have cb_threshold samples left.
  cras_alsa_get_avail_frames_ret = 0;
  cras_alsa_get_avail_frames_avail =
      aio_->base.buffer_size - aio_->base.cb_threshold;
  written_expected = (aio_->base.used_size - aio_->base.cb_threshold);
  nsec_expected = written_expected *
      1000000000 / fmt_.frame_rate;

  //  shm has plenty of data in it.
  shm_->write_offset[0] = shm_->used_size;
  shm2_->write_offset[0] = shm2_->used_size;

  cras_iodev_append_stream(&aio_->base, rstream2_);

  //  Test that nothing breaks if one stream doesn't fill.
  cras_mix_add_stream_dont_fill_next = 1;

  rc = possibly_fill_audio(aio_, &ts);
  EXPECT_EQ(0, rc);
  EXPECT_EQ(0, cras_rstream_request_audio_called);
  EXPECT_EQ(0, shm_->read_offset[0]);  //  No write from first stream.
  EXPECT_EQ(written_expected * 4, shm2_->read_offset[0]);
}

TEST_F(AlsaPlaybackStreamSuite, PossiblyFillGetFromTwoStreamsNeedFill) {
  struct timespec ts;
  int rc;
  uint64_t nsec_expected;

  //  Have cb_threshold samples left.
  cras_alsa_get_avail_frames_ret = 0;
  cras_alsa_get_avail_frames_avail =
      aio_->base.buffer_size - aio_->base.cb_threshold;
  nsec_expected = (aio_->base.used_size - aio_->base.cb_threshold) *
      1000000000 / fmt_.frame_rate;

  //  shm has nothing left.
  shm_->write_offset[0] = 0;
  shm2_->write_offset[0] = 0;

  cras_iodev_append_stream(&aio_->base, rstream2_);

  FD_ZERO(&select_out_fds);
  FD_SET(rstream_->fd, &select_out_fds);
  FD_SET(rstream2_->fd, &select_out_fds);
  select_return_value = 2;

  rc = possibly_fill_audio(aio_, &ts);
  EXPECT_EQ(0, rc);
  EXPECT_EQ(0, ts.tv_sec);
  EXPECT_GE(ts.tv_nsec, nsec_expected - 1000);
  EXPECT_LE(ts.tv_nsec, nsec_expected + 1000);
  EXPECT_EQ(aio_->base.used_size - aio_->base.cb_threshold,
            cras_mix_add_stream_count);
  EXPECT_EQ(2, cras_rstream_request_audio_called);
  EXPECT_NE(-1, select_max_fd);
}

TEST_F(AlsaPlaybackStreamSuite, PossiblyFillGetFromTwoStreamsFillOne) {
  struct timespec ts;
  int rc;
  uint64_t nsec_expected;

  //  Have cb_threshold samples left.
  cras_alsa_get_avail_frames_ret = 0;
  cras_alsa_get_avail_frames_avail =
      aio_->base.buffer_size - aio_->base.cb_threshold;
  nsec_expected = (aio_->base.used_size - aio_->base.cb_threshold) *
      1000000000 / fmt_.frame_rate;

  //  One has too little the other is full.
  shm_->write_offset[0] = 40;
  shm_->write_buf_idx = 1;
  shm2_->write_offset[0] = shm2_->used_size;
  shm2_->write_buf_idx = 1;

  cras_iodev_append_stream(&aio_->base, rstream2_);

  FD_ZERO(&select_out_fds);
  FD_SET(rstream_->fd, &select_out_fds);
  select_return_value = 1;

  rc = possibly_fill_audio(aio_, &ts);
  EXPECT_EQ(0, rc);
  EXPECT_EQ(0, ts.tv_sec);
  EXPECT_GE(ts.tv_nsec, nsec_expected - 1000);
  EXPECT_LE(ts.tv_nsec, nsec_expected + 1000);
  EXPECT_EQ(aio_->base.used_size - aio_->base.cb_threshold,
            cras_mix_add_stream_count);
  EXPECT_EQ(1, cras_rstream_request_audio_called);
  EXPECT_NE(-1, select_max_fd);
}

}  //  namespace

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

//  Stubs

extern "C" {

//  From iodev.
int cras_iodev_list_add_output(struct cras_iodev *output, int auto_route)
{
  return 0;
}
int cras_iodev_list_rm_output(struct cras_iodev *dev)
{
  return 0;
}

int cras_iodev_list_add_input(struct cras_iodev *input, int auto_route)
{
  return 0;
}
int cras_iodev_list_rm_input(struct cras_iodev *dev)
{
  return 0;
}
int cras_iodev_move_stream_type(enum CRAS_STREAM_TYPE type, size_t index)
{
  cras_iodev_move_stream_type_called++;
  return 0;
}
int cras_iodev_move_stream_type_default(enum CRAS_STREAM_TYPE type,
					enum CRAS_STREAM_DIRECTION direction)
{
  cras_iodev_move_stream_type_default_called++;
  return 0;
}

int cras_iodev_init(struct cras_iodev *iodev,
		    enum CRAS_STREAM_DIRECTION direction,
		    void *(*thread_function)(void *arg),
		    void *thread_data)
{
  cras_iodev_init_called++;
  return 0;
}
void cras_iodev_deinit(struct cras_iodev *dev)
{
  cras_iodev_deinit_called++;
}
int cras_iodev_get_thread_poll_fd(const struct cras_iodev *iodev)
{
  return 0;
}
int cras_iodev_read_thread_command(struct cras_iodev *iodev,
				   uint8_t *buf,
				   size_t max_len)
{
  return 0;
}
int cras_iodev_send_command_response(struct cras_iodev *iodev, int rc)
{
  return 0;
}
int cras_iodev_append_stream(struct cras_iodev *dev,
			     struct cras_rstream *stream)
{
  struct cras_io_stream *out;

  if (cras_iodev_append_stream_ret) {
    int rc = cras_iodev_append_stream_ret;
    cras_iodev_append_stream_ret = 0;
    return rc;
  }

  /* New stream, allocate a container and add it to the list. */
  out = (struct cras_io_stream *)calloc(1, sizeof(*out));
  if (out == NULL)
    return -ENOMEM;
  out->stream = stream;
  out->shm = cras_rstream_get_shm(stream);
  out->fd = cras_rstream_get_audio_fd(stream);
  DL_APPEND(dev->streams, out);

  return 0;
}
int cras_iodev_post_message_to_playback_thread(struct cras_iodev *iodev,
					       struct cras_iodev_msg *msg)
{
  cras_iodev_post_message_to_playback_thread_called++;
  return 0;
}
int cras_iodev_delete_stream(struct cras_iodev *dev,
			     struct cras_rstream *stream)
{
  struct cras_io_stream *out;

  /* Find stream, and if found, delete it. */
  DL_SEARCH_SCALAR(dev->streams, out, stream, stream);
  if (out == NULL)
    return -EINVAL;
  DL_DELETE(dev->streams, out);
  free(out);

  return 0;
}
void cras_iodev_fill_time_from_frames(size_t frames,
				      size_t cb_threshold,
				      size_t frame_rate,
				      struct timespec *ts)
{
	size_t to_play_usec;

        ts->tv_sec = 0;
	/* adjust sleep time to target our callback threshold */
	if (frames > cb_threshold)
		to_play_usec = (frames - cb_threshold) *
			1000000 / frame_rate;
	else
		to_play_usec = 0;
	ts->tv_nsec = to_play_usec * 1000;

	while (ts->tv_nsec > 1000000000L) {
		ts->tv_sec++;
		ts->tv_nsec -= 1000000000L;
	}
}
void cras_iodev_set_playback_timestamp(size_t frame_rate,
				       size_t frames,
				       struct timespec *ts)
{
}
void cras_iodev_set_capture_timestamp(size_t frame_rate,
				      size_t frames,
				      struct timespec *ts)
{
}
void cras_iodev_config_params_for_streams(struct cras_iodev *iodev)
{
}

//  From alsa helper.
int cras_alsa_pcm_open(snd_pcm_t **handle, const char *dev,
		       snd_pcm_stream_t stream)
{
  *handle = (snd_pcm_t *)0x24;
  cras_alsa_open_called++;
  return 0;
}
int cras_alsa_pcm_close(snd_pcm_t *handle)
{
  return 0;
}
int cras_alsa_pcm_start(snd_pcm_t *handle)
{
  cras_alsa_start_called++;
  return 0;
}
int cras_alsa_pcm_drain(snd_pcm_t *handle)
{
  return 0;
}
int cras_alsa_fill_properties(const char *dev,
			      snd_pcm_stream_t stream,
			      size_t **rates,
			      size_t **channel_counts)
{
  *rates = (size_t *)malloc(sizeof(**rates) * 3);
  (*rates)[0] = 44100;
  (*rates)[1] = 48000;
  (*rates)[2] = 0;
  *channel_counts = (size_t *)malloc(sizeof(**channel_counts) * 2);
  (*channel_counts)[0] = 2;
  (*channel_counts)[1] = 0;

  cras_alsa_fill_properties_called++;
  return 0;
}
int cras_alsa_set_hwparams(snd_pcm_t *handle, struct cras_audio_format *format,
			   snd_pcm_uframes_t *buffer_size)
{
  return 0;
}
int cras_alsa_set_swparams(snd_pcm_t *handle)
{
  return 0;
}
int cras_alsa_get_avail_frames(snd_pcm_t *handle, snd_pcm_uframes_t buf_size,
			       snd_pcm_uframes_t *used)
{
  *used = cras_alsa_get_avail_frames_avail;
  return cras_alsa_get_avail_frames_ret;
}
int cras_alsa_get_delay_frames(snd_pcm_t *handle, snd_pcm_uframes_t buf_size,
			       snd_pcm_sframes_t *delay)
{
  *delay = 0;
  return 0;
}
int cras_alsa_mmap_begin(snd_pcm_t *handle, size_t format_bytes,
			 uint8_t **dst, snd_pcm_uframes_t *offset,
			 snd_pcm_uframes_t *frames, size_t *underruns)
{
  *dst = cras_alsa_mmap_begin_buffer;
  *frames = cras_alsa_mmap_begin_frames;
  return 0;
}
int cras_alsa_mmap_commit(snd_pcm_t *handle, snd_pcm_uframes_t offset,
			  snd_pcm_uframes_t frames, size_t *underruns)
{
  return 0;
}
int cras_alsa_attempt_resume(snd_pcm_t *handle)
{
  return 0;
}

//  From util.
int cras_set_rt_scheduling(int rt_lim)
{
  return 0;
}
int cras_set_thread_priority(int priority)
{
  return 0;
}

//  From rstream.
int cras_rstream_request_audio(const struct cras_rstream *stream, size_t count)
{
  cras_rstream_request_audio_called++;
  return 0;
}
int cras_rstream_request_audio_buffer(const struct cras_rstream *stream)
{
  return 0;
}
int cras_rstream_get_audio_request_reply(const struct cras_rstream *stream)
{
  return 0;
}
int cras_rstream_audio_ready(const struct cras_rstream *stream, size_t count)
{
  cras_rstream_audio_ready_count = count;
  return 0;
}

//  ALSA stubs.
int snd_pcm_format_physical_width(snd_pcm_format_t format)
{
  return 16;
}

snd_pcm_state_t snd_pcm_state(snd_pcm_t *handle)
{
  return SND_PCM_STATE_RUNNING;
}

const char *snd_strerror(int errnum)
{
  return "Alsa Error in UT";
}

//  From mixer.
size_t cras_mix_add_stream(struct cras_audio_shm_area *shm,
			   size_t num_channels,
			   uint8_t *dst,
			   size_t *count,
			   size_t *index)
{
  if (cras_mix_add_stream_dont_fill_next) {
    cras_mix_add_stream_dont_fill_next = 0;
    return 0;
  }
  cras_mix_add_stream_count = *count;
  *index = *index + 1;
  return *count;
}

const char *cras_alsa_mixer_get_output_name(
		const struct cras_alsa_mixer_output *output)
{
  return "";
}

//  From system_state.
size_t cras_system_get_volume()
{
  sys_get_volume_called++;
  return sys_get_volume_return_value;
}

int cras_system_register_volume_changed_cb(cras_system_volume_changed_cb cb,
					   void *arg)
{
  sys_register_volume_cb_called++;
  sys_register_volume_cb_value = cb;
  sys_register_volume_cb_arg = arg;
  return 0;
}

int cras_system_remove_volume_changed_cb(cras_system_volume_changed_cb cb,
					 void *arg)
{
  sys_remove_volume_cb_called++;
  return 0;
}

long cras_system_get_capture_gain()
{
  sys_get_capture_gain_called++;
  return sys_get_capture_gain_return_value;
}

int cras_system_register_capture_gain_changed_cb(
    cras_system_volume_changed_cb cb, void *arg)
{
  sys_register_capture_gain_cb_called++;
  sys_register_capture_gain_cb_value = cb;
  sys_register_capture_gain_cb_arg = arg;
  return 0;
}

int cras_system_remove_capture_gain_changed_cb(
    cras_system_volume_changed_cb cb, void *arg)
{
  sys_remove_capture_gain_cb_called++;
  return 0;
}

int cras_system_get_mute()
{
  sys_get_mute_called++;
  return sys_get_mute_return_value;
}

int cras_system_register_mute_changed_cb(cras_system_volume_changed_cb cb,
					 void *arg)
{
  sys_register_mute_cb_called++;
  sys_register_mute_cb_value = cb;
  sys_register_mute_cb_arg = arg;
  return 0;
}

int cras_system_remove_mute_changed_cb(cras_system_volume_changed_cb cb,
				       void *arg)
{
  sys_remove_mute_cb_called++;
  return 0;
}

int cras_system_get_capture_mute()
{
  sys_get_capture_mute_called++;
  return sys_get_capture_mute_return_value;
}

int cras_system_register_capture_mute_changed_cb(
    cras_system_volume_changed_cb cb, void *arg)
{
  sys_register_capture_mute_cb_called++;
  sys_register_capture_mute_cb_value = cb;
  sys_register_capture_mute_cb_arg = arg;
  return 0;
}

int cras_system_remove_capture_mute_changed_cb(cras_system_volume_changed_cb cb,
                                               void *arg)
{
  sys_remove_capture_mute_cb_called++;
  return 0;
}

void cras_system_set_volume_limits(long min, long max)
{
  sys_set_volume_limits_called++;
}

void cras_system_set_capture_gain_limits(long min, long max)
{
  sys_set_capture_gain_limits_called++;
}

//  From cras_alsa_mixer.
void cras_alsa_mixer_set_dBFS(struct cras_alsa_mixer *m,
			      long dB_level,
			      struct cras_alsa_mixer_output *output)
{
  alsa_mixer_set_dBFS_called++;
  alsa_mixer_set_dBFS_value = dB_level;
  alsa_mixer_set_dBFS_output = output;
}

void cras_alsa_mixer_set_mute(struct cras_alsa_mixer *cras_mixer,
			      int muted,
			      struct cras_alsa_mixer_output *mixer_output)
{
  alsa_mixer_set_mute_called++;
  alsa_mixer_set_mute_value = muted;
  alsa_mixer_set_mute_output = mixer_output;
}

void cras_alsa_mixer_set_capture_dBFS(struct cras_alsa_mixer *m, long dB_level)
{
  alsa_mixer_set_capture_dBFS_called++;
  alsa_mixer_set_capture_dBFS_value = dB_level;
}

void cras_alsa_mixer_set_capture_mute(struct cras_alsa_mixer *m, int mute)
{
  alsa_mixer_set_capture_mute_called++;
  alsa_mixer_set_capture_mute_value = mute;
}

void cras_alsa_mixer_list_outputs(struct cras_alsa_mixer *cras_mixer,
				  size_t device_index,
				  cras_alsa_mixer_output_callback cb,
				  void *callback_arg)
{
  cras_alsa_mixer_list_outputs_called++;
  cras_alsa_mixer_list_outputs_device_value = device_index;
  for (size_t i = 0; i < cras_alsa_mixer_list_outputs_outputs_length; i++) {
    cb(cras_alsa_mixer_list_outputs_outputs[i], callback_arg);
  }
}

struct cras_volume_curve *cras_alsa_mixer_create_volume_curve_for_name(
		const struct cras_alsa_mixer *cmix,
		const char *name)
{
	return NULL;
}

int cras_alsa_mixer_set_output_active_state(
		struct cras_alsa_mixer_output *output,
		int active)
{
  cras_alsa_mixer_set_output_active_state_called++;
  cras_alsa_mixer_set_output_active_state_outputs.push_back(output);
  cras_alsa_mixer_set_output_active_state_values.push_back(active);
  return 0;
}

const struct cras_volume_curve *cras_alsa_mixer_default_volume_curve(
		const struct cras_alsa_mixer *cras_mixer)
{
  cras_alsa_mixer_default_volume_curve_called++;
  return fake_curve;
}

void cras_volume_curve_destroy(struct cras_volume_curve *curve)
{
}

long cras_alsa_mixer_get_minimum_capture_gain(struct cras_alsa_mixer *cmix)
{
	cras_alsa_mixer_get_minimum_capture_gain_called++;
	return 0;
}

long cras_alsa_mixer_get_maximum_capture_gain(struct cras_alsa_mixer *cmix)
{
	cras_alsa_mixer_get_maximum_capture_gain_called++;
	return 0;
}

// From cras_alsa_jack
struct cras_alsa_jack_list *cras_alsa_jack_list_create(
		unsigned int card_index,
		unsigned int device_index,
		struct cras_alsa_mixer *mixer,
		enum CRAS_STREAM_DIRECTION direction,
		jack_state_change_callback *cb,
		void *cb_data)
{
  cras_alsa_jack_list_create_called++;
  cras_alsa_jack_list_create_cb = cb;
  cras_alsa_jack_list_create_cb_data = cb_data;
  return (struct cras_alsa_jack_list *)0xfee;
}

void cras_alsa_jack_list_destroy(struct cras_alsa_jack_list *jack_list)
{
  cras_alsa_jack_list_destroy_called++;
}

void cras_alsa_jack_list_report(const struct cras_alsa_jack_list *jack_list)
{
}

const char *cras_alsa_jack_get_name(const struct cras_alsa_jack *jack)
{
	return NULL;
}

struct cras_alsa_mixer_output *cras_alsa_jack_get_mixer_output(
    const struct cras_alsa_jack *jack)
{
  return NULL;
}

}
