/*
 * SDL_codec:  An audio codec library for use with SDL
 * Copyright (C) 2022 Walker M. White
 *
 * This is a simple library to lad different types of audio files as PCM data.
 * We choose this over SDL_mixer because it gives us finer grain control over 
 * our audio layer (which we need for effects).
 * 
 * SDL License:
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
 *  \file SDL_codec.h
 *
 *  Header file for SDL_codec library
 *
 *  This is simple library to load different types of audio files as PCM data. This 
 *  library makes it easy to stream or buffer audio files while decoupling the
 *  decoding from the playback implementation.
 */
#ifndef SDL_CODEC_H_
#define SDL_CODEC_H_

#include "SDL.h"
#include "SDL_version.h"
#include "begin_code.h"

/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
extern "C" {
#endif

/** 
 * The file types supported by this library
 */
typedef enum {
    CODEC_TYPE_WAV    = 0x00000001,         /**< WAV File */
    CODEC_TYPE_VORBIS = 0x00000002,         /**< OGG Vorbis Audio */
    CODEC_TYPE_FLAC   = 0x00000003,         /**< Xiph FLAC */
    CODEC_TYPE_MPEG   = 0x00000004,         /**< MP3 Audio */
} CODEC_SourceType;

/** 
 * The representation of an audio source (as a stream).
 *
 * While we allow direct access to attributes like type, channels, rate, etc.
 * they should only be accessed read-only. Changing them can have disastrous
 * effects on the stream.
 */
typedef struct {
    /** The type for this source */
    CODEC_SourceType type;

    /** The number of channels in this sound source (max 32) */
    Uint8  channels;
    
    /** The sampling rate (frequency) of this sound source */
    Uint32 rate;
    
    /** The number of frames in this sounds source */
    Uint64 frames;

    /** An opaque reference to the format specific decoder */
    void* decoder;

} CODEC_Source;

/** 
 * Creates a new CODEC_Source from an OGG Vorbis file
 * 
 * This function will return NULL if the file cannot be located or is not an
 * proper OGG Vorbis file. The file will not be read into memory, but is instead
 * available for streaming.
 *
 * Is the responsibility of the caller of this function to close the CODEC_Source
 * (with {@link CODEC_Close}) when done.
 *
 * @param filename    The name of the file to load
 *
 * @return a new CODEC_Source from an OGG Vorbis file
 */
extern DECLSPEC CODEC_Source* CODEC_OpenVorbis(const char* filename);

/** 
 * Creates a new CODEC_Source from an Xiph FLAC file
 * 
 * This function will return NULL if the file cannot be located or is not an
 * proper Xiph FLAC file. The file will not be read into memory, but is instead
 * available for streaming.
 *
 * Is the responsibility of the caller of this function to close the CODEC_Source
 * (with {@link CODEC_Close}) when done.
 *
 * @param filename    The name of the file to load
 *
 * @return a new CODEC_Source from an Xiph FLAC file
 */
extern DECLSPEC CODEC_Source* CODEC_OpenFLAC(const char* filename);

/** 
 * Creates a new CODEC_Source from an MP3 file
 * 
 * This function will return NULL if the file cannot be located or is not an
 * proper MP3 file. The file will not be read into memory, but is instead
 * available for streaming.
 *
 * Is the responsibility of the caller of this function to close the CODEC_Source
 * (with {@link CODEC_Close}) when done.
 *
 * @param filename    The name of the file to load
 *
 * @return a new CODEC_Source from an MP3 file
 */
extern DECLSPEC CODEC_Source* CODEC_OpenMPEG(const char* filename);

/** 
 * Creates a new CODEC_Source from an WAV file
 * 
 * This function will return NULL if the file cannot be located or is not an
 * proper WAV file. The file will not be read into memory, but is instead
 * available for streaming.
 *
 * Note that WAV is a container type in addition to a codec. Not all WAV files
 * are supported.
 *
 * Is the responsibility of the caller of this function to close the CODEC_Source
 * (with {@link CODEC_Close}) when done.
 *
 * @param filename    The name of the file to load
 *
 * @return a new CODEC_Source from an WAV file
 */
extern DECLSPEC CODEC_Source* CODEC_OpenWAV(const char* filename);

/**
 * Closes a CODEC_Source, releasing all memory
 *
 * @param source    The source to close
 * 
 * @return 0 if the source was successfully closed, -1 otherwise.
 */
extern DECLSPEC Sint32 CODEC_Close(CODEC_Source* source);

/**
 * Seeks to the given page in the stream.
 *
 * Audio streams are processed in pages. A page is the minimal amount of information
 * that can be read into memory at a time. Our API only allows seeking at the page
 * level, not at the sample level.
 *
 * If the page is out of bounds, this function will seek to the last page.
 *
 * @param source    The audio source
 * @param page        The page to seek
 *
 * @return the page acquired, or -1 if there is an error
 */
extern DECLSPEC Sint32 CODEC_Seek(CODEC_Source* source, Uint32 page);

/**
 * Returns the number of audio frames in a page.
 *
 * An audio frame is a collection of simultaneous samples for different channels.
 * Multiplying the page size by the number of channels produces the number of samples
 * in a page.
 *
 * @param source    The audio source
 *
 * @return the number of audio frames in a page, or -1 if there is an error
 */
extern DECLSPEC Sint32 CODEC_PageSize(CODEC_Source* source);

/**
 * Returns the index of the last page in the stream.
 *
 * Audio streams are processed in pages. A page is the minimal amount of information
 * that can be read into memory at a time. Our API only allows seeking at the page
 * level, not at the sample level.
 *
 * @param source    The audio source
 *
 * @return the index of the last page in the stream, or -1 if there is an error
 */
extern DECLSPEC Sint32 CODEC_LastPage(CODEC_Source* source);

/**
 * Returns the index of the current page in the stream.
 *
 * Audio streams are processed in pages. A page is the minimal amount of information
 * that can be read into memory at a time. Our API only allows seeking at the page
 * level, not at the sample level.
 *
 * @param source    The audio source
 *
 * @return the index of the current page in the stream, or -1 if there is an error
 */
extern DECLSPEC Sint32 CODEC_Tell(CODEC_Source* source);

/** 
 * Returns 1 if the audio source is at the end of the stream; 0 otherwise
 *
 * @param source    The audio source
 *
 * @return 1 if the audio source is at the end of the stream; 0 otherwise
 */
extern DECLSPEC Uint32 CODEC_EOF(CODEC_Source* source);

/**
 * Reads a single page of audio data into the buffer
 *
 * This function reads in the current stream page into the buffer. The data written 
 * into the buffer is linear PCM data with interleaved channels. If the stream is
 * at the end, nothing will be written.
 *
 * The size of a page is given by {@link CODEC_PageSize}. This buffer should
 * large enough to hold this data. As the page size is in audio frames, that means
 * the buffer should be pagesize * # of channels * sizeof(float) bytes.
 *
 * @param source    The audio source
 * @param buffer    The buffer to hold the PCM data
 *
 * @return the number of audio frames (samples/channels) read
 */
extern DECLSPEC Sint32 CODEC_Read(CODEC_Source* source, float* buffer);

/**
 * Reads in the entire audio source into the buffer.
 *
 * The data written into the buffer is linear PCM data with interleaved channels. If the
 * stream is not at the initial page, it will rewind before writing the data. It will
 * restore the stream to the initial page when done.
 *
 * The buffer needs to be large enough to hold the entire audio source. As the length is
 * measured in audio frames, this means that the buffer should be 
 * frames * # of channels * sizeof(float) bytes.
 *
 * @param source    The MP3 stream
 * @param buffer    The buffer to hold the PCM data
 *
 * @return the number of audio frames (samples/channels) read
 */
extern DECLSPEC Sint64 CODEC_Fill(CODEC_Source* source, float* buffer);

/**
 * Report SDL_codec errors
 *
 * \sa IMG_GetError
 */
#define CODEC_SetError    SDL_SetError

/**
 * Get last SDL_codec error
 *
 * \sa IMG_SetError
 */
#define CODEC_GetError    SDL_GetError

/* Ends C function definitions when using C++ */
#ifdef __cplusplus
}
#endif
#include "close_code.h"

#endif /* SDL_CODEC_H_ */
