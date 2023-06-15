/*
 * mp3stream:  An simple mp3 streaming API
 * Copyright (C) 2022 Walker M. White
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

/**
 *  @file mp3stream.h
 *
 *  Header file for a mp3 streaming library
 *
 *  This is a simple library for decoding MP3s built on top of minimp3. It allows
 *  the user to either get the data all at once, or one frame at a time.
 */
#ifndef _MP3_STREAM_H_
#define _MP3_STREAM_H_

/* This is an internal header for Codec delegation */
#include <SDL.h>
#include <begin_code.h>

/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
extern "C" {
#endif

/** Internal, opaque type for decoding MP3s */
typedef struct MP3Stream MP3Stream;

/**
 * Allocates a new MP3 stream for the given file
 *
 * The file must be in the current working directory, or else this will return NULL.
 * It the responsibility of the user to free this stream when done.
 *
 * @param file	The name of the MP3 file
 *
 * @return a new MP3 stream for the given file
 */
extern DECLSPEC MP3Stream* MP3Stream_Alloc(const char* file);

/**
 * Frees the given MP3 stream
 *
 * It the responsibility of the user to free any allocated stream when done.
 *
 * @param stream	The MP3 stream to free
 *
 * @return 0 if successful
 */
extern DECLSPEC Sint32 MP3Stream_Free(MP3Stream* stream);

/**
 * Returns 1 if this MP3 stream is stereo
 *
 * MP3 files can only either be mono or stereo.
 *
 * @param stream	The MP3 stream
 *
 * @return 1 if stereo, 0 if mono
 */
extern DECLSPEC Uint32 MP3Stream_IsStereo(MP3Stream* stream);

/**
 * Returns the sample rate for this MP3 stream
 *
 * @param stream	The MP3 stream
 *
 * @return the sample rate for this MP3 stream
 */
extern DECLSPEC Uint32 MP3Stream_GetFrequency(MP3Stream* stream);

/**
 * Returns the page size for this MP3 stream
 *
 * The page size is measured in audio frames, not samples. An audio frame is a 
 * collection of simultaneous samples for all of the different channels.
 *
 * @param stream	The MP3 stream
 *
 * @return the page size for this MP3 stream
 */
extern DECLSPEC Uint32 MP3Stream_GetPageSize(MP3Stream* stream);

/**
 * Returns the last page for this MP3 stream
 *
 * @param stream	The MP3 stream
 *
 * @return the last page for this MP3 stream
 */
extern DECLSPEC Uint32 MP3Stream_GetLastPage(MP3Stream* stream);

/**
 * Returns the size of this MP3 stream in audio frames
 *
 * An audio frame (not to be confused with a page) is a collection of simultaneous 
 * samples for all of the different channels. 
 *
 * @param stream	The MP3 stream
 *
 * @return the size of this MP3 stream in audio frames
 */
extern DECLSPEC Uint32 MP3Stream_GetLength(MP3Stream* stream);

/**
 * Sets the current page for the MP3 stream
 *
 * Pages are more course grained than samples. We only allow seeking at the page
 * level. If the page is out of bounds, we will set to the last page.
 *
 * @param stream	The MP3 stream
 * @param page		The page to seek
 *
 * @return the new current page
 */
extern DECLSPEC Uint32 MP3Stream_SetPage(MP3Stream* stream, Uint32 page);

/**
 * Reads in a page of MP3 data.
 *
 * This function reads in the current stream page into the buffer. The data written 
 * into the buffer is linear PCM data with interleaved channels. If the stream is
 * at the end, nothing will be written.
 *
 * The size of a page is given by {@link MP3Stream_GetPageSize}. This buffer should
 * large enough to hold this data. As the page size is in audio frames, that means
 * the buffer should be pagesize * # of channels * sizeof(Sint16) bytes.
 *
 * @param stream	The MP3 stream
 * @param buffer	The buffer to hold the PCM data
 *
 * @return the number of audio frames (samples/channels) read
 */
extern DECLSPEC Sint64 MP3Stream_ReadPage(MP3Stream* stream, Sint16 *buffer);

/**
 * Reads in the entire MP3 stream into the buffer.
 *
 * The data written into the buffer is linear PCM data with interleaved channels. If the
 * stream is not at the initial page, it will rewind before writing the data. It will
 * restore the stream to the initial page when done.
 *
 * The buffer needs to be large enough to hold the entire MP3 stream. As the length is
 * measured in audio frames, this means that the buffer should be 
 * length * # of channels * sizeof(Sint16) bytes.
 *
 * @param stream	The MP3 stream
 * @param buffer	The buffer to hold the PCM data
 *
 * @return the number of audio frames (samples/channels) read
 */
extern DECLSPEC Sint64 MP3Stream_Read(MP3Stream* stream, Sint16 *buffer);

/* Ends C function definitions when using C++ */
#ifdef __cplusplus
}
#endif
#include <close_code.h>

#endif /* _MP3_STREAM_H_ */