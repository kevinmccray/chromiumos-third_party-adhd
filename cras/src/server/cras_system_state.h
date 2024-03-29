/* Copyright (c) 2012 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 * Handles various system-level settings.
 *
 * Volume:  The system volume is represented as a value from 0 to 100.  This
 * number will be interpreted by the output device and applied to the hardware.
 * The value will be mapped to dB by the active device as it will know its curve
 * the best.
 */

#ifndef CRAS_SYSTEM_STATE_H_
#define CRAS_SYSTEM_STATE_H_

#include <stddef.h>

#define CRAS_MAX_SYSTEM_VOLUME 100
#define DEFAULT_CAPTURE_GAIN 2000 /* 20dB of gain. */
/* Default to 1--dB of range for palyback and capture. */
#define DEFAULT_MIN_VOLUME_DBFS -10000
#define DEFAULT_MAX_VOLUME_DBFS 0
#define DEFAULT_MIN_CAPTURE_GAIN -5000
#define DEFAULT_MAX_CAPTURE_GAIN 5000


/* Callback functions to be notified when settings change. data is a user
 * provided argument that will be passed back. */
typedef void (*cras_system_volume_changed_cb)(void *data);

/* Initialize system settings. */
void cras_system_state_init();

/* Sets the system volume.  Will be applied by the active device. */
void cras_system_set_volume(size_t volume);
/* Gets the current system volume. */
size_t cras_system_get_volume();

/* Adds a callback to call when the volume changes.
 * Args:
 *    cb - Function to call when volume changes.
 *    arg - Value to pass back to callback.
 */
int cras_system_register_volume_changed_cb(cras_system_volume_changed_cb cb,
					   void *arg);

/* Removes a callback to call when the volume changes.  Only removes the entry
 * if both cb and arg match the values passed to the register function.
 * Args:
 *    cb - Function to call when volume changes.
 *    arg - Value to passed back to callback.
 */
int cras_system_remove_volume_changed_cb(cras_system_volume_changed_cb cb,
					 void *arg);

/* Sets the system capture volume.  Will be applied by the active device. */
void cras_system_set_capture_gain(long gain);
/* Gets the current system capture volume. */
long cras_system_get_capture_gain();

/* Adds a callback to call when the capture volume changes.
 * Args:
 *    cb - Function to call when capture volume changes.
 *    arg - Value to pass back to callback.
 */
int cras_system_register_capture_gain_changed_cb(
		cras_system_volume_changed_cb cb, void *arg);

/* Removes a callback to call when the capture volume changes.  Only removes the
 * entry if both cb and arg match the values passed to the register function.
 * Args:
 *    cb - Function to call when capture volume changes.
 *    arg - Value to passed back to callback.
 */
int cras_system_remove_capture_gain_changed_cb(cras_system_volume_changed_cb cb,
					       void *arg);

/* Sets if the system is muted or not. */
void cras_system_set_mute(int muted);
/* Gets the current mute state of the system. */
int cras_system_get_mute();

/* Adds a callback to call when the mute state changes.
 * Args:
 *    cb - Function to call when mute state changes.
 *    arg - Value to pass back to callback.
 */
int cras_system_register_mute_changed_cb(cras_system_volume_changed_cb cb,
					 void *arg);

/* Removes a callback to call when the mute state changes.  Only removes the
 * entry if both cb and arg match the values passed to the register function.
 * Args:
 *    cb - Function to call when volume changes.
 *    arg - Value to passed back to callback.
 */
int cras_system_remove_mute_changed_cb(cras_system_volume_changed_cb cb,
				       void *arg);

/* Sets if the system capture path is muted or not. */
void cras_system_set_capture_mute(int muted);
/* Gets the current mute state of the system capture path. */
int cras_system_get_capture_mute();

/* Adds a callback to call when the capture mute state changes.
 * Args:
 *    cb - Function to call when the capture mute state changes.
 *    arg - Value to pass back to callback.
 */
int cras_system_register_capture_mute_changed_cb(
		cras_system_volume_changed_cb cb, void *arg);

/* Removes a callback to call when the capture mute state changes.  Only removes
 * the entry if both cb and arg match the values passed to the register
 * function.
 * Args:
 *    cb - Function to call when volume changes.
 *    arg - Value to passed back to callback.
 */
int cras_system_remove_capture_mute_changed_cb(cras_system_volume_changed_cb cb,
					       void *arg);

/* Sets the value in dB of the MAX and MIN volume settings.  This will allow
 * clients to query what range of control is available.  Both arguments are
 * specified as dB * 100.
 * Args:
 *     min - dB value when volume = 1 (0 mutes).
 *     max - dB value when volume = CRAS_MAX_SYSTEM_VOLUME
 */
void cras_system_set_volume_limits(long min, long max);
/* Returns the dB value when volume = 1, in dB * 100. */
long cras_system_get_min_volume();
/* Returns the dB value when volume = CRAS_MAX_SYSTEM_VOLUME, in dB * 100. */
long cras_system_get_max_volume();

/* Adds a callback to call when the volume limits change.
 * Args:
 *    cb - Function to call when there is a change.
 *    arg - Value to pass back to callback.
 */
int cras_system_register_volume_limits_changed_cb(
		cras_system_volume_changed_cb cb, void *arg);

/* Removes a callback to call when the volume limits change.  Only removes
 * the entry if both cb and arg match the values passed to the register
 * function.
 * Args:
 *    cb - Function to call when there is a change.
 *    arg - Value to passed back to callback.
 */
int cras_system_remove_volume_limits_changed_cb(
		cras_system_volume_changed_cb cb, void *arg);

/* Sets the limits in dB * 100 of the MAX and MIN capture gain.  This will allow
 * clients to query what range of control is available.  Both arguments are
 * specified as dB * 100.
 * Args:
 *     min - minimum allowed capture gain.
 *     max - maximum allowed capture gaax.
 */
void cras_system_set_capture_gain_limits(long min, long max);
/* Returns the max value allowed for capture gain in dB * 100. */
long cras_system_get_min_capture_gain();
/* Returns the min value allowed for capture gain in dB * 100. */
long cras_system_get_max_capture_gain();

/* Adds a card at the given index to the system.  When a new card is found
 * (through a udev event notification) this will add the card to the system,
 * causing its devices to become available for playback/capture.
 * Args:
 *    alsa_card_index - Index ALSA uses to refer to the card.  The X in "hw:X".
 * Returns:
 *    0 on success, negative error on failure (Can't create or card already
 *    exists).
 */
int cras_system_add_alsa_card(size_t alsa_card_index);

/* Removes a card.  When a device is removed this will do the cleanup.  Device
 * at index must have been added using cras_system_add_alsa_card().
 * Args:
 *    alsa_card_index - Index ALSA uses to refer to the card.  The X in "hw:X".
 * Returns:
 *    0 on success, negative error on failure (Can't destroy or card doesn't
 *    exist).
 */
int cras_system_remove_alsa_card(size_t alsa_card_index);

/* Poll interface provides a way of getting a callback when 'select'
 * returns for a given file descriptor.
 */

/* Register the function to use to inform the select loop about new
 * file descriptors and callbacks.
 * Args:
 *    add - The function to call when a new fd is added.
 *    rm - The function to call when a new fd is removed.
 *    select_data - Additional value passed back to add and remove.
 * Returns:
 *    0 on success, or -EBUSY if there is already a registered handler.
 */
int cras_system_set_select_handler(int (*add)(int fd,
					      void (*callback)(void *data),
					      void *callback_data,
					      void *select_data),
				  void (*rm)(int fd, void *select_data),
				  void *select_data);

/* Adds the fd and callback pair.  When select indicates that fd is readable,
 * the callback will be called.
 * Args:
 *    fd - The file descriptor to pass to select(2).
 *    callback - The callback to call when fd is ready.
 *    callback_data - Value passed back to the callback.
 * Returns:
 *    0 on success or a negative error code on failure.
 */
int cras_system_add_select_fd(int fd,
			      void (*callback)(void *data),
			      void *callback_data);

/* Removes the fd from the list of fds that are passed to select.
 * Args:
 *    fd - The file descriptor to remove from the list.
 */
void cras_system_rm_select_fd(int fd);

#endif /* CRAS_SYSTEM_STATE_H_ */
