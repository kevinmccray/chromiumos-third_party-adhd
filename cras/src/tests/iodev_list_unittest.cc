// Copyright (c) 2012 The Chromium OS Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stdio.h>
#include <gtest/gtest.h>

extern "C" {
#include "cras_iodev.h"
#include "cras_iodev_list.h"
#include "cras_rstream.h"
#include "utlist.h"
}

namespace {

class IoDevTestSuite : public testing::Test {
  protected:
    virtual void SetUp() {
      sample_rates_[0] = 44100;
      sample_rates_[1] = 48000;
      sample_rates_[2] = 0;

      channel_counts_[0] = 2;
      channel_counts_[1] = 0;

      d1_.add_stream = add_stream_1;
      d1_.rm_stream = rm_stream_1;
      d1_.format = NULL;
      d1_.info.idx = -999;
      d1_.supported_rates = sample_rates_;
      d1_.supported_channel_counts = channel_counts_;
      d2_.add_stream = add_stream_2;
      d2_.rm_stream = rm_stream_2;
      d2_.format = NULL;
      d2_.info.idx = -999;
      d2_.supported_rates = sample_rates_;
      d2_.supported_channel_counts = channel_counts_;
    }

    static int add_stream_1(struct cras_iodev *iodev,
        struct cras_rstream *stream)
    {
      add_stream_1_called_++;
      return cras_iodev_append_stream(iodev, stream);
    }

    static int rm_stream_1(struct cras_iodev *iodev,
        struct cras_rstream *stream)
    {
      rm_stream_1_called_++;
      return cras_iodev_delete_stream(iodev, stream);
    }

    static int add_stream_2(struct cras_iodev *iodev,
        struct cras_rstream *stream)
    {
      add_stream_2_called_++;
      return cras_iodev_append_stream(iodev, stream);
    }

    static int rm_stream_2(struct cras_iodev *iodev,
        struct cras_rstream *stream)
    {
      rm_stream_2_called_++;
      return cras_iodev_delete_stream(iodev, stream);
    }

    struct cras_iodev d1_;
    struct cras_iodev d2_;
    size_t sample_rates_[3];
    size_t channel_counts_[2];
    static int add_stream_1_called_;
    static int rm_stream_1_called_;
    static int add_stream_2_called_;
    static int rm_stream_2_called_;
};

int IoDevTestSuite::add_stream_1_called_;
int IoDevTestSuite::rm_stream_1_called_;
int IoDevTestSuite::add_stream_2_called_;
int IoDevTestSuite::rm_stream_2_called_;
size_t cras_server_send_to_all_clients_called;
size_t cras_server_send_to_all_clients_num_outputs;
size_t cras_server_send_to_all_clients_num_inputs;

// Test adding/removing an iodev to the list.
TEST_F(IoDevTestSuite, AddRemoveOutput) {
  struct cras_iodev_info *dev_info;
  int rc;
  uint32_t found_mask;

  rc = cras_iodev_list_add_output(&d1_, 1);
  EXPECT_EQ(0, rc);
  EXPECT_EQ(0, d1_.info.idx);
  // Test can't insert same iodev twice.
  rc = cras_iodev_list_add_output(&d1_, 1);
  EXPECT_NE(0, rc);
  // Test insert a second output.
  rc = cras_iodev_list_add_output(&d2_, 1);
  EXPECT_EQ(0, rc);
  EXPECT_EQ(1, d2_.info.idx);

  // List the outputs.
  rc = cras_iodev_list_get_outputs(&dev_info);
  EXPECT_EQ(2, rc);
  if (rc == 2) {
    found_mask = 0;
    for (int i = 0; i < rc; i++) {
      size_t idx = dev_info[i].idx;
      EXPECT_EQ(0, (found_mask & (1 << idx)));
      found_mask |= (1 << idx);
    }
    EXPECT_EQ(0x03, found_mask);
  }
  if (rc > 0)
    free(dev_info);

  // Test that it is removed if no attached streams.
  d1_.streams = (struct cras_io_stream *)NULL;
  d2_.streams = (struct cras_io_stream *)NULL;
  rc = cras_iodev_list_rm_output(&d1_);
  EXPECT_EQ(0, rc);
  // Test that we can't remove a dev twice.
  rc = cras_iodev_list_rm_output(&d1_);
  EXPECT_NE(0, rc);
  // Should be 1 dev now.
  rc = cras_iodev_list_get_outputs(&dev_info);
  EXPECT_EQ(1, rc);
  // Remove other dev.
  rc = cras_iodev_list_rm_output(&d2_);
  EXPECT_EQ(0, rc);
  // Should be 0 devs now.
  rc = cras_iodev_list_get_outputs(&dev_info);
  EXPECT_EQ(0, rc);
}

// Test auto routing for outputs.
TEST_F(IoDevTestSuite, AutoRouteOutputs) {
  int rc;
  struct cras_iodev *ret_dev;

  rc = cras_iodev_list_add_output(&d1_, 1);
  EXPECT_EQ(0, rc);
  // Test can't insert same iodev twice.
  rc = cras_iodev_list_add_output(&d1_, 1);
  EXPECT_NE(0, rc);
  ret_dev = cras_get_iodev_for_stream_type(CRAS_STREAM_TYPE_DEFAULT,
      CRAS_STREAM_OUTPUT);
  EXPECT_EQ(&d1_, ret_dev);
  // Test insert a second output.
  rc = cras_iodev_list_add_output(&d2_, 1);
  EXPECT_EQ(0, rc);
  ret_dev = cras_get_iodev_for_stream_type(CRAS_STREAM_TYPE_DEFAULT,
      CRAS_STREAM_OUTPUT);
  EXPECT_EQ(&d2_, ret_dev);

  // Test that it is removed if no attached streams.
  d1_.streams = (struct cras_io_stream *)NULL;
  d2_.streams = (struct cras_io_stream *)NULL;
  rc = cras_iodev_list_rm_output(&d2_);
  EXPECT_EQ(0, rc);
  // Test that we can't remove a dev twice.
  rc = cras_iodev_list_rm_output(&d2_);
  EXPECT_NE(0, rc);
  // Default should fall back to d1.
  ret_dev = cras_get_iodev_for_stream_type(CRAS_STREAM_TYPE_DEFAULT,
      CRAS_STREAM_OUTPUT);
  EXPECT_EQ(&d1_, ret_dev);
  // Remove other dev.
  rc = cras_iodev_list_rm_output(&d1_);
  EXPECT_EQ(0, rc);
}

// Test adding/removing an input dev to the list.
TEST_F(IoDevTestSuite, AddRemoveInput) {
  struct cras_iodev_info *dev_info;
  int rc, i;
  uint32_t found_mask;

  cras_server_send_to_all_clients_called = 0;
  rc = cras_iodev_list_add_input(&d1_, 1);
  EXPECT_EQ(0, rc);
  EXPECT_GE(d1_.info.idx, 0);
  EXPECT_EQ(1, cras_server_send_to_all_clients_called);
  // Test can't insert same iodev twice.
  rc = cras_iodev_list_add_input(&d1_, 1);
  EXPECT_NE(0, rc);
  EXPECT_EQ(1, cras_server_send_to_all_clients_called);
  // Test insert a second input.
  rc = cras_iodev_list_add_input(&d2_, 1);
  EXPECT_EQ(0, rc);
  EXPECT_GE(d2_.info.idx, 1);
  EXPECT_EQ(2, cras_server_send_to_all_clients_called);

  // List the outputs.
  rc = cras_iodev_list_get_inputs(&dev_info);
  EXPECT_EQ(2, rc);
  if (rc == 2) {
    found_mask = 0;
    for (i = 0; i < rc; i++) {
      size_t idx = dev_info[i].idx;
      EXPECT_EQ(0, (found_mask & (1 << idx)));
      found_mask |= (1 << idx);
    }
  }
  if (rc > 0)
    free(dev_info);

  // Test that it is removed if no attached streams.
  d1_.streams = (struct cras_io_stream *)NULL;
  d2_.streams = (struct cras_io_stream *)NULL;
  rc = cras_iodev_list_rm_input(&d1_);
  EXPECT_EQ(0, rc);
  // Test that we can't remove a dev twice.
  rc = cras_iodev_list_rm_input(&d1_);
  EXPECT_NE(0, rc);
  // Should be 1 dev now.
  rc = cras_iodev_list_get_inputs(&dev_info);
  EXPECT_EQ(1, rc);
  // Remove other dev.
  rc = cras_iodev_list_rm_input(&d2_);
  EXPECT_EQ(0, rc);
  // Should be 0 devs now.
  rc = cras_iodev_list_get_inputs(&dev_info);
  EXPECT_EQ(0, rc);
}

// Test stream attach/detach.
TEST_F(IoDevTestSuite, AttachDetachStream) {
  struct cras_iodev *ret_dev;
  struct cras_rstream s1, s2;
  int rc;

  rc = cras_iodev_list_add_output(&d1_, 1);
  EXPECT_EQ(0, rc);
  rc = cras_iodev_list_add_output(&d2_, 0);
  EXPECT_EQ(0, rc);

  // Default should 1, because 2 specified not to auto-route.
  ret_dev = cras_get_iodev_for_stream_type(CRAS_STREAM_TYPE_DEFAULT,
      CRAS_STREAM_OUTPUT);
  EXPECT_EQ(&d1_, ret_dev);

  s1.stream_id = 555;
  s1.stream_type = CRAS_STREAM_TYPE_DEFAULT;
  s1.direction = CRAS_STREAM_OUTPUT;
  s1.flags = 0;
  s1.format.format = SND_PCM_FORMAT_S16_LE;
  s1.format.frame_rate = 48000;
  s1.format.num_channels = 2;

  // Attaching a stream.
  add_stream_1_called_ = rm_stream_1_called_ = 0;
  rc = cras_iodev_attach_stream(&d1_, &s1);
  EXPECT_EQ(0, rc);
  EXPECT_EQ(1, add_stream_1_called_);
  EXPECT_EQ(&d1_, s1.iodev);
  EXPECT_NE((void *)NULL, d1_.streams);
  if (d1_.streams != NULL)
    EXPECT_EQ(&s1, d1_.streams->stream);

  // Can't add same stream twice.
  rc = cras_iodev_attach_stream(&d1_, &s1);
  EXPECT_NE(0, rc);

  // Test moving to invalid device.
  rc = cras_iodev_move_stream_type(CRAS_STREAM_TYPE_DEFAULT, 949);
  EXPECT_NE(0, rc);

  // Test that routing to the already default device is a nop.
  rc = cras_iodev_move_stream_type(CRAS_STREAM_TYPE_DEFAULT, d1_.info.idx);
  EXPECT_EQ(0, rc);
  EXPECT_EQ(0, rm_stream_1_called_);

  // Move the stream. Should just remove and wait for add from client.
  rc = cras_iodev_move_stream_type(CRAS_STREAM_TYPE_DEFAULT, d2_.info.idx);
  EXPECT_EQ(0, rc);
  EXPECT_EQ(1, rm_stream_1_called_);
  EXPECT_EQ(NULL, d1_.streams);

  // Test that new streams of the same type will get assigned to the same
  // output device.
  ret_dev = cras_get_iodev_for_stream_type(s1.stream_type, s1.direction);
  EXPECT_EQ(&d2_, ret_dev);

  // Attaching a stream.
  add_stream_2_called_ = rm_stream_2_called_ = 0;
  rc = cras_iodev_attach_stream(&d2_, &s1);
  EXPECT_EQ(0, rc);
  EXPECT_EQ(1, add_stream_2_called_);
  EXPECT_EQ(&d2_, s1.iodev);
  EXPECT_NE((void *)NULL, d2_.streams);
  if (d2_.streams != NULL)
    EXPECT_EQ(&s1, d2_.streams->stream);

  // Test switching back to the original default stream.
  rc = cras_iodev_move_stream_type_default(CRAS_STREAM_TYPE_DEFAULT,
                                           s1.direction);
  EXPECT_EQ(0, rc);
  EXPECT_EQ(1, rm_stream_2_called_);
  EXPECT_EQ(NULL, d1_.streams);

  // Test that streams now go back tot default.
  ret_dev = cras_get_iodev_for_stream_type(s1.stream_type, s1.direction);
  EXPECT_EQ(&d1_, ret_dev);

  // Test detaching non-existent stream.
  add_stream_2_called_ = rm_stream_2_called_ = 0;
  rc = cras_iodev_detach_stream(&d2_, &s2);
  EXPECT_EQ(1, rm_stream_2_called_);
  EXPECT_NE(0, rc);

  // Detaching a stream.
  rc = cras_iodev_attach_stream(&d2_, &s1);
  rm_stream_2_called_  = 0;
  rc = cras_iodev_detach_stream(&d2_, &s1);
  EXPECT_EQ(0, rc);
  EXPECT_EQ(1, rm_stream_2_called_);
  EXPECT_EQ(NULL, s1.iodev);

  rc = cras_iodev_list_rm_output(&d1_);
  EXPECT_EQ(0, rc);
  rc = cras_iodev_list_rm_output(&d2_);
  EXPECT_EQ(0, rc);
}

TEST_F(IoDevTestSuite, SupportedFormatSecondary) {
  struct cras_audio_format fmt;
  int rc;

  fmt.format = SND_PCM_FORMAT_S16_LE;
  fmt.frame_rate = 48000;
  fmt.num_channels = 2;
  rc = cras_iodev_set_format(&d1_, &fmt);
  EXPECT_EQ(0, rc);
  EXPECT_EQ(SND_PCM_FORMAT_S16_LE, fmt.format);
  EXPECT_EQ(48000, fmt.frame_rate);
  EXPECT_EQ(2, fmt.num_channels);
}

TEST_F(IoDevTestSuite, SupportedFormatPrimary) {
  struct cras_audio_format fmt;
  int rc;

  fmt.format = SND_PCM_FORMAT_S16_LE;
  fmt.frame_rate = 44100;
  fmt.num_channels = 2;
  rc = cras_iodev_set_format(&d1_, &fmt);
  EXPECT_EQ(0, rc);
  EXPECT_EQ(SND_PCM_FORMAT_S16_LE, fmt.format);
  EXPECT_EQ(44100, fmt.frame_rate);
  EXPECT_EQ(2, fmt.num_channels);
}

TEST_F(IoDevTestSuite, SupportedFormatDivisor) {
  struct cras_audio_format fmt;
  int rc;

  fmt.format = SND_PCM_FORMAT_S16_LE;
  fmt.frame_rate = 96000;
  fmt.num_channels = 2;
  d1_.format = NULL;
  rc = cras_iodev_set_format(&d1_, &fmt);
  EXPECT_EQ(0, rc);
  EXPECT_EQ(SND_PCM_FORMAT_S16_LE, fmt.format);
  EXPECT_EQ(48000, fmt.frame_rate);
  EXPECT_EQ(2, fmt.num_channels);
}

TEST_F(IoDevTestSuite, UnsupportedChannelCount) {
  struct cras_audio_format fmt;
  int rc;

  fmt.format = SND_PCM_FORMAT_S16_LE;
  fmt.frame_rate = 96000;
  fmt.num_channels = 1;
  d1_.format = NULL;
  rc = cras_iodev_set_format(&d1_, &fmt);
  EXPECT_EQ(0, rc);
  EXPECT_EQ(SND_PCM_FORMAT_S16_LE, fmt.format);
  EXPECT_EQ(48000, fmt.frame_rate);
  EXPECT_EQ(2, fmt.num_channels);
}

TEST_F(IoDevTestSuite, SupportedFormatFallbackDefault) {
  struct cras_audio_format fmt;
  int rc;

  fmt.format = SND_PCM_FORMAT_S16_LE;
  fmt.frame_rate = 96008;
  fmt.num_channels = 2;
  d1_.format = NULL;
  rc = cras_iodev_set_format(&d1_, &fmt);
  EXPECT_EQ(0, rc);
  EXPECT_EQ(SND_PCM_FORMAT_S16_LE, fmt.format);
  EXPECT_EQ(44100, fmt.frame_rate);
  EXPECT_EQ(2, fmt.num_channels);
}

}  //  namespace

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

extern "C" {

// Stubs

int cras_iodev_append_stream(struct cras_iodev *iodev,
			     struct cras_rstream *stream) {
  struct cras_io_stream *out;

  /* Check that we don't already have this stream */
  DL_SEARCH_SCALAR(iodev->streams, out, stream, stream);
  if (out != NULL)
    return -EEXIST;

  /* New stream, allocate a container and add it to the list. */
  out = static_cast<struct cras_io_stream*>(calloc(1, sizeof(*out)));
  if (out == NULL)
    return -ENOMEM;
  out->stream = stream;
  out->shm = cras_rstream_get_shm(stream);
  out->fd = cras_rstream_get_audio_fd(stream);
  DL_APPEND(iodev->streams, out);

  return 0;
}

int cras_iodev_delete_stream(struct cras_iodev *iodev,
			     struct cras_rstream *stream) {
  struct cras_io_stream *out;

  /* Find stream, and if found, delete it. */
  DL_SEARCH_SCALAR(iodev->streams, out, stream, stream);
  if (out == NULL)
    return -EINVAL;
  DL_DELETE(iodev->streams, out);
  free(out);

  return 0;
}

void cras_rstream_send_client_reattach(const struct cras_rstream *stream) {
}

void cras_server_send_to_all_clients(struct cras_client_message *msg) {
  cras_client_iodev_list* cmsg = reinterpret_cast<cras_client_iodev_list*>(msg);
  cras_server_send_to_all_clients_called++;
  cras_server_send_to_all_clients_num_outputs = cmsg->num_outputs;
  cras_server_send_to_all_clients_num_inputs = cmsg->num_inputs;
}

}  // extern "C"
