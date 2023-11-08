/* KallistiOS ##version##

   dc/sound/stream2.h
   Copyright (C) 2002, 2004 Megan Potter
   Copyright (C) 2020 Lawrence Sebald
   Copyright (C) 2023 Ruslan Rostovtsev

*/

/** \file   dc/sound/stream2.h
    \brief  Sound streaming support.

    This file contains declarations for doing streams of sound. This underlies
    pretty much any decoded sounds you might use, including the Ogg Vorbis
    libraries. Note that this does not actually handle decoding, so you'll have
    to worry about that yourself (or use something in kos-ports).

    \author Megan Potter
    \author Florian Schulze
    \author Lawrence Sebald
    \author Ruslan Rostovtsev
    
    modified by kapodamy with the following changes:
        * increase the SND_STREAM_MAX (4 instances/8 channels) to SND_STREAM2_MAX (8 instances/16 channels)
        * declare snd_stream2_* functions from snd_stream2.c file
*/

#ifndef __DC_SOUND_STREAM2_H
#define __DC_SOUND_STREAM2_H

#include <dc/sound/stream.h>
__BEGIN_DECLS


/** \brief  The maximum number of streams that can be allocated at once. */
#define SND_STREAM2_MAX 8

/** \brief  The maximum buffer size (in bytes) for a ADPCM stream.

 * This value was rounded from 0x10000 to 0x8000. The AICA "loopend" register
 * only can hold 65535 (only 65534 usable) samples and the ADPCM format can go
 * up to 131070 samples, to avoid truncation, the buffer size is limited to 32764 bytes.
*/
#define SND_STREAM2_BUFFER_ADPCM_MAX 0x7ffc

/** \brief  Set the callback for a given stream.

    This function sets the get data callback function for a given stream,
    overwriting any old callback that may have been in place.

    \param  hnd             The stream handle for the callback.
    \param  cb              A pointer to the callback function.
*/
void snd_stream2_set_callback(snd_stream_hnd_t hnd, snd_stream_callback_t cb);

/** \brief  Set the user data for a given stream.

    This function sets the user data pointer for the given stream, overwriting
    any existing one that may have been in place. This is designed to allow the
    user the ability to associate a piece of data with the stream for instance
    to assist in identifying what sound is playing on a stream. The driver does
    not attempt to use this data in any way.

    \param  hnd             The stream handle to look up.
    \param  d               A pointer to the user data.
*/
void snd_stream2_set_userdata(snd_stream_hnd_t hnd, void *d);

/** \brief  Get the user data for a given stream.

    This function retrieves the set user data pointer for a given stream.

    \param  hnd             The stream handle to look up.
    \return                 The user data pointer set for this stream or NULL
                            if no data pointer has been set.
*/
void *snd_stream2_get_userdata(snd_stream_hnd_t hnd);

/** \brief  Add a filter to the specified stream.

    This function adds a filter to the specified stream. The filter will be
    called on each block of data input to the stream from then forward.

    \param  hnd             The stream to add the filter to.
    \param  filtfunc        A pointer to the filter function.
    \param  obj             Filter function user data.
*/
void snd_stream2_filter_add(snd_stream_hnd_t hnd, snd_stream_filter_t filtfunc,
                           void *obj);

/** \brief  Remove a filter from the specified stream.

    This function removes a filter that was previously added to the specified
    stream.

    \param  hnd             The stream to remove the filter from.
    \param  filtfunc        A pointer to the filter function to remove.
    \param  obj             The filter function's user data. Must be the same as
                            what was passed as obj to snd_stream2_filter_add().
*/
void snd_stream2_filter_remove(snd_stream_hnd_t hnd,
                              snd_stream_filter_t filtfunc, void *obj);

/** \brief  Prefill the stream buffers.

    This function prefills the stream buffers before starting it. This is
    implicitly called by snd_stream2_start(), so there's probably no good reason
    to call this yourself.

    \param  hnd             The stream to prefill buffers on.
*/
void snd_stream2_prefill(snd_stream_hnd_t hnd);

/** \brief  Initialize the stream system.

    This function initializes the sound stream system and allocates memory for
    it as needed. Note, this is not done by the default init, so if you're using
    the streaming support and not using something like the kos-ports Ogg Vorbis
    library, you'll need to call this yourself. This will implicitly call
    snd_init(), so it will potentially overwrite anything going on the AICA.

    \retval -1              On failure.
    \retval 0               On success.
*/
int snd_stream2_init(void);

/** \brief  Shut down the stream system.

    This function shuts down the stream system and frees the memory associated
    with it. This does not call snd_shutdown().
*/
void snd_stream2_shutdown(void);

/** \brief  Allocate a stream.

    This function allocates a stream and sets its parameters. The function will
    return SND_STREAM_INVALID if there not enough unused AICA channels.

    \param  cb              The get data callback for the stream.
    \param  bufsize         The size of the buffer for the stream.
    \return                 A handle to the new stream on success,
                            SND_STREAM_INVALID on failure.
*/
snd_stream_hnd_t snd_stream2_alloc(snd_stream_callback_t cb, int bufsize);

/** \brief  Reinitialize a stream.

    This function reinitializes a stream, resetting its callback function.

    \param  hnd             The stream handle to reinit.
    \param  cb              The new get data callback for the stream.
    \return                 hnd
*/
int snd_stream2_reinit(snd_stream_hnd_t hnd, snd_stream_callback_t cb);

/** \brief  Destroy a stream.

    This function destroys a previously created stream, freeing all memory
    associated with it.

    \param  hnd             The stream to clean up.
*/
void snd_stream2_destroy(snd_stream_hnd_t hnd);

/** \brief  Enable queueing on a stream.

    This function enables queueing on the specified stream. This will make it so
    that you must call snd_stream2_queue_go() to actually start the stream, after
    scheduling the start. This is useful for getting something ready but not
    firing it right away.

    \param  hnd             The stream to enable queueing on.
*/
void snd_stream2_queue_enable(snd_stream_hnd_t hnd);

/** \brief  Disable queueing on a stream.

    This function disables queueing on the specified stream. This does not imply
    that a previously queued start on the stream will be fired if queueing was
    enabled before.

    \param  hnd             The stream to disable queueing on.
*/
void snd_stream2_queue_disable(snd_stream_hnd_t hnd);

/** \brief  Start a stream after queueing the request.

    This function makes the stream start once a start request has been queued,
    if queueing mode is enabled on the stream.

    \param  hnd             The stream to start the queue on.
*/
void snd_stream2_queue_go(snd_stream_hnd_t hnd);

/** \brief  Start a 16-bit PCM stream.

    This function starts processing the given stream, prefilling the buffers as
    necessary. In queueing mode, this will not start playback.

    \param  hnd             The stream to start.
    \param  freq            The frequency of the sound.
    \param  st              1 if the sound is stereo, 0 if mono.
*/
void snd_stream2_start(snd_stream_hnd_t hnd, uint32 freq, int st);

/** \brief  Start a 8-bit PCM stream.

    This function starts processing the given stream, prefilling the buffers as
    necessary. In queueing mode, this will not start playback.

    \param  hnd             The stream to start.
    \param  freq            The frequency of the sound.
    \param  st              1 if the sound is stereo, 0 if mono.
*/
void snd_stream2_start_pcm8(snd_stream_hnd_t hnd, uint32 freq, int st);

/** \brief  Start a 4-bit ADPCM stream.

    This function starts processing the given stream, prefilling the buffers as
    necessary. In queueing mode, this will not start playback.
    
    Note: stereo samples MUST BE interlaced per byte, this
    means the data layout must be "LLRR" (2 samples) instead of standard layout "LR"

    \param  hnd             The stream to start.
    \param  freq            The frequency of the sound.
    \param  st              1 if the sound is stereo, 0 if mono.
*/
void snd_stream2_start_adpcm(snd_stream_hnd_t hnd, uint32 freq, int st);

/** \brief  Stop a stream.

    This function stops a stream, stopping any sound playing from it. This will
    happen immediately, regardless of whether queueing is enabled or not.

    \param  hnd             The stream to stop.
*/
void snd_stream2_stop(snd_stream_hnd_t hnd);

/** \brief  Poll a stream.

    This function polls the specified stream to load more data if necessary. If
    using the streaming support, you must call this function periodically (most
    likely in a thread), or you won't get any sound output.

    \param  hnd             The stream to poll.
    \retval -3              If NULL was returned from the callback.
    \retval -1              If no callback is set, or if the state has been
                            corrupted.
    \retval 0               On success.
*/
int snd_stream2_poll(snd_stream_hnd_t hnd);

/** \brief  Set the volume on the stream.

    This function sets the volume of the specified stream.

    \param  hnd             The stream to set volume on.
    \param  vol             The volume to set. Valid values are 0-255.
*/
void snd_stream2_volume(snd_stream_hnd_t hnd, int vol);

__END_DECLS

#endif  /* __DC_SOUND_STREAM_H */
