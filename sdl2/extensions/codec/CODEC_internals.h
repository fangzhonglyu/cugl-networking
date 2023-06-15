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
#ifndef _CODEC_INTERNALS_H_
#define _CODEC_INTERNALS_H_

/* This is an internal header for Codec delegation */
#include <SDL_codec.h>
#include <begin_code.h>

/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
extern "C" {
#endif

/** The default page size (in bytes) */
#define DECODER_PAGE_SIZE   4096

/**
 * Returns the (string) name for a given source type
 *
 * This function is typically used for debugging.
 *
 * @param type    The source type
 *
 * @return the (string) name for the type
 */
extern DECLSPEC const char* CODEC_NameForType(CODEC_SourceType type);

/**
 * The Vorbis specific implementation of {@link CODEC_Close}.
 *
 * @param source    The source to close
 * 
 * @return 0 if the source was successfully closed, -1 otherwise.
 */
extern DECLSPEC Sint32 CODEC_Vorbis_Close(CODEC_Source* source);

/**
 * The Vorbis specific implementation of {@link CODEC_Seek}.
 *
 * @param source    The audio source
 * @param page        The page to seek
 *
 * @return the page acquired, or -1 if there is an error
 */
extern DECLSPEC Sint32 CODEC_Vorbis_Seek(CODEC_Source* source, Uint32 page);

/**
 * The Vorbis specific implementation of {@link CODEC_PageSize}.
 *
 * @param source    The audio source
 *
 * @return the number of audio frames in a page, or -1 if there is an error
 */
extern DECLSPEC Sint32 CODEC_Vorbis_PageSize(CODEC_Source* source);

/**
 * The Vorbis specific implementation of {@link CODEC_LastPage}.
 *
 * @param source    The audio source
 *
 * @return the index of the last page in the stream, or -1 if there is an error
 */
extern DECLSPEC Sint32 CODEC_Vorbis_LastPage(CODEC_Source* source);

/**
 * The Vorbis specific implementation of {@link CODEC_Tell}.
 *
 * @param source    The audio source
 *
 * @return the index of the current page in the stream, or -1 if there is an error
 */
extern DECLSPEC Sint32 CODEC_Vorbis_Tell(CODEC_Source* source);

/**
 * The Vorbis specific implementation of {@link CODEC_EOF}.
 *
 * @param source    The audio source
 *
 * @return 1 if the audio source is at the end of the stream; 0 otherwise
 */
extern DECLSPEC Uint32 CODEC_Vorbis_EOF(CODEC_Source* source);

/**
 * The Vorbis specific implementation of {@link CODEC_Read}.
 *
 * @param source    The audio source
 * @param buffer    The buffer to hold the PCM data
 *
 * @return the number of audio frames (samples/channels) read
 */
extern DECLSPEC Sint32 CODEC_Vorbis_Read(CODEC_Source* source, float* buffer);

/**
 * The Vorbis specific implementation of {@link CODEC_Fill}.
 *
 * @param source    The audio source
 * @param buffer    The buffer to hold the PCM data
 *
 * @return the number of audio frames (samples/channels) read
 */
extern DECLSPEC Sint64 CODEC_Vorbis_Fill(CODEC_Source* source, float* buffer);

/**
 * The FLAC specific implementation of {@link CODEC_Close}.
 *
 * @param source    The source to close
 * 
 * @return 0 if the source was successfully closed, -1 otherwise.
 */
extern DECLSPEC Sint32 CODEC_FLAC_Close(CODEC_Source* source);

/**
 * The FLAC specific implementation of {@link CODEC_Seek}.
 *
 * @param source    The audio source
 * @param page        The page to seek
 *
 * @return the page acquired, or -1 if there is an error
 */
extern DECLSPEC Sint32 CODEC_FLAC_Seek(CODEC_Source* source, Uint32 page);

/**
 * The FLAC specific implementation of {@link CODEC_PageSize}.
 *
 * @param source    The audio source
 *
 * @return the number of audio frames in a page, or -1 if there is an error
 */
extern DECLSPEC Sint32 CODEC_FLAC_PageSize(CODEC_Source* source);

/**
 * The FLAC specific implementation of {@link CODEC_LastPage}.
 *
 * @param source    The audio source
 *
 * @return the index of the last page in the stream, or -1 if there is an error
 */
extern DECLSPEC Sint32 CODEC_FLAC_LastPage(CODEC_Source* source);

/**
 * The FLAC specific implementation of {@link CODEC_Tell}.
 *
 * @param source    The audio source
 *
 * @return the index of the current page in the stream, or -1 if there is an error
 */
extern DECLSPEC Sint32 CODEC_FLAC_Tell(CODEC_Source* source);

/**
 * The FLAC specific implementation of {@link CODEC_EOF}.
 *
 * @param source    The audio source
 *
 * @return 1 if the audio source is at the end of the stream; 0 otherwise
 */
extern DECLSPEC Uint32 CODEC_FLAC_EOF(CODEC_Source* source);

/**
 * The FLAC specific implementation of {@link CODEC_Read}.
 *
 * @param source    The audio source
 * @param buffer    The buffer to hold the PCM data
 *
 * @return the number of audio frames (samples/channels) read
 */
extern DECLSPEC Sint32 CODEC_FLAC_Read(CODEC_Source* source, float* buffer);

/**
 * The FLAC specific implementation of {@link CODEC_Fill}.
 *
 * @param source    The audio source
 * @param buffer    The buffer to hold the PCM data
 *
 * @return the number of audio frames (samples/channels) read
 */
extern DECLSPEC Sint64 CODEC_FLAC_Fill(CODEC_Source* source, float* buffer);

/**
 * The WAV specific implementation of {@link CODEC_Close}.
 *
 * @param source    The source to close
 * 
 * @return 0 if the source was successfully closed, -1 otherwise.
 */
extern DECLSPEC Sint32 CODEC_MPEG_Close(CODEC_Source* source);

/**
 * The MP3 specific implementation of {@link CODEC_Seek}.
 *
 * @param source    The audio source
 * @param page        The page to seek
 *
 * @return the page acquired, or -1 if there is an error
 */
extern DECLSPEC Sint32 CODEC_MPEG_Seek(CODEC_Source* source, Uint32 page);

/**
 * The MP3 specific implementation of {@link CODEC_PageSize}.
 *
 * @param source    The audio source
 *
 * @return the number of audio frames in a page, or -1 if there is an error
 */
extern DECLSPEC Sint32 CODEC_MPEG_PageSize(CODEC_Source* source);

/**
 * The MP3 specific implementation of {@link CODEC_LastPage}.
 *
 * @param source    The audio source
 *
 * @return the index of the last page in the stream, or -1 if there is an error
 */
extern DECLSPEC Sint32 CODEC_MPEG_LastPage(CODEC_Source* source);

/**
 * The MP3 specific implementation of {@link CODEC_Tell}.
 *
 * @param source    The audio source
 *
 * @return the index of the current page in the stream, or -1 if there is an error
 */
extern DECLSPEC Sint32 CODEC_MPEG_Tell(CODEC_Source* source);

/**
 * The MP3 specific implementation of {@link CODEC_EOF}.
 *
 * @param source    The audio source
 *
 * @return 1 if the audio source is at the end of the stream; 0 otherwise
 */
extern DECLSPEC Uint32 CODEC_MPEG_EOF(CODEC_Source* source);

/**
 * The MP3 specific implementation of {@link CODEC_Read}.
 *
 * @param source    The audio source
 * @param buffer    The buffer to hold the PCM data
 *
 * @return the number of audio frames (samples/channels) read
 */
extern DECLSPEC Sint32 CODEC_MPEG_Read(CODEC_Source* source, float* buffer);

/**
 * The MP3 specific implementation of {@link CODEC_Fill}.
 *
 * @param source    The audio source
 * @param buffer    The buffer to hold the PCM data
 *
 * @return the number of audio frames (samples/channels) read
 */
extern DECLSPEC Sint64 CODEC_MPEG_Fill(CODEC_Source* source, float* buffer);

/**
 * The WAV specific implementation of {@link CODEC_Close}.
 *
 * @param source    The source to close
 * 
 * @return 0 if the source was successfully closed, -1 otherwise.
 */
extern DECLSPEC Sint32 CODEC_WAV_Close(CODEC_Source* source);

/**
 * The WAV specific implementation of {@link CODEC_Seek}.
 *
 * @param source    The audio source
 * @param page        The page to seek
 *
 * @return the page acquired, or -1 if there is an error
 */
extern DECLSPEC Sint32 CODEC_WAV_Seek(CODEC_Source* source, Uint32 page);

/**
 * The WAV specific implementation of {@link CODEC_PageSize}.
 *
 * @param source    The audio source
 *
 * @return the number of audio frames in a page, or -1 if there is an error
 */
extern DECLSPEC Sint32 CODEC_WAV_PageSize(CODEC_Source* source);

/**
 * The WAV specific implementation of {@link CODEC_LastPage}.
 *
 * @param source    The audio source
 *
 * @return the index of the last page in the stream, or -1 if there is an error
 */
extern DECLSPEC Sint32 CODEC_WAV_LastPage(CODEC_Source* source);

/**
 * The WAV specific implementation of {@link CODEC_Tell}.
 *
 * @param source    The audio source
 *
 * @return the index of the current page in the stream, or -1 if there is an error
 */
extern DECLSPEC Sint32 CODEC_WAV_Tell(CODEC_Source* source);

/**
 * The WAV specific implementation of {@link CODEC_EOF}.
 *
 * @param source    The audio source
 *
 * @return 1 if the audio source is at the end of the stream; 0 otherwise
 */
extern DECLSPEC Uint32 CODEC_WAV_EOF(CODEC_Source* source);

/**
 * The WAV specific implementation of {@link CODEC_Read}.
 *
 * @param source    The audio source
 * @param buffer    The buffer to hold the PCM data
 *
 * @return the number of audio frames (samples/channels) read
 */
extern DECLSPEC Sint32 CODEC_WAV_Read(CODEC_Source* source, float* buffer);

/**
 * The WAV specific implementation of {@link CODEC_Fill}.
 *
 * @param source    The audio source
 * @param buffer    The buffer to hold the PCM data
 *
 * @return the number of audio frames (samples/channels) read
 */
extern DECLSPEC Sint64 CODEC_WAV_Fill(CODEC_Source* source, float* buffer);


/* Ends C function definitions when using C++ */
#ifdef __cplusplus
}
#endif
#include <close_code.h>

#endif /* _CODEC_INTERNALS_H_ */
