/* Copyright (c) 2011 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */
#include "codec.h"
#include "linkerset.h"


static char const *initialize[] = {
    "-q cset name='DL1 Mixer Multimedia' 1",
    "-q cset name='Sidetone Mixer Playback' 1",
    "-q cset name='DL1 PDM Switch' 1",
    "-q cset name='DL1 Media Playback Volume' 90",
    "-q cset name='SDT DL Volume' 120",
    "-q cset name='DL2 Mixer Multimedia' 1",
    "-q cset name='DL2 Media Playback Volume' 90",
    "-q cset name='Headset Left Playback' 1",
    "-q cset name='Headset Right Playback' 1",
    "-q cset name='Headset Playback Volume' 13",
    "-q cset name='AUXL Playback Switch' 1",
    "-q cset name='AUXR Playback Switch' 1",
    "-q cset name='Handsfree Left Playback' 1",
    "-q cset name='Handsfree Right Playback' 1",
    "-q cset name='Handsfree Playback Volume' 27",
    "-q cset name='MUX_UL00' 11",
    "-q cset name='MUX_UL01' 12",
    "-q cset name='Capture Preamplifier Volume' 2",
    "-q cset name='Capture Volume' 4",
    "-q cset name='Analog Left Capture Route' 2",
    "-q cset name='Analog Right Capture Route' 2",
    NULL
};

#define DECLARE_BOARD(_board)                                   \
    static codec_desc_t _board##_codec_desc = {                 \
        .codec             = "twl6040",                         \
        .board             = "omap5-panda",                     \
        .initialize        = initialize,                        \
        .headphone_insert  = NULL,                  		\
        .headphone_remove  = NULL,                  		\
        .microphone_insert = NULL,              		\
        .microphone_remove = NULL		                \
    };                                                          \
    LINKERSET_ADD_ITEM(codec_desc, _board##_codec_desc);

DECLARE_BOARD(omap5_panda)
