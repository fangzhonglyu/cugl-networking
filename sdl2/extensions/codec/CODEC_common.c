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

/* This is the common file functions. It mostly dispatches on type */
#include <SDL_codec.h>
#include "CODEC_internals.h"


/**
 * Returns the (string) name for a given source type
 *
 * This function is typically used for debugging.
 *
 * @param type    The source type
 *
 * @return the (string) name for the type
 */
const char* CODEC_NameForType(CODEC_SourceType type) {
    switch(type) {
        case CODEC_TYPE_WAV:
            return "WAV";
        case CODEC_TYPE_VORBIS:
            return "OGG Vorbis";
        case CODEC_TYPE_FLAC:
            return "Xiph FLAC";
        case CODEC_TYPE_MPEG:
            return "MP3";
    }
    return "unknown";
}

/**
 * Closes a CODEC_Source, releasing all memory
 *
 * @param source    The source to close
 * 
 * @return 0 if the source was successfully closed, -1 otherwise.
 */
Sint32 CODEC_Close(CODEC_Source* source) {
    if (source == NULL) {
        CODEC_SetError("Attempt to access NULL codec source");
        return -1;
    }
    
    switch(source->type) {
    case CODEC_TYPE_WAV:
        return CODEC_WAV_Close(source);
    case CODEC_TYPE_VORBIS:
        return CODEC_Vorbis_Close(source);
    case CODEC_TYPE_FLAC:
        return CODEC_FLAC_Close(source);
    case CODEC_TYPE_MPEG:
        return CODEC_MPEG_Close(source);
    }

    CODEC_SetError("Unrecognized codec type: %d",source->type);
    return -1;
}

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
Sint32 CODEC_Seek(CODEC_Source* source, Uint32 page) {
    if (source == NULL) {
        CODEC_SetError("Attempt to access NULL codec source");
        return -1;
    }
    
    switch(source->type) {
    case CODEC_TYPE_WAV:
        return CODEC_WAV_Seek(source,page);
    case CODEC_TYPE_VORBIS:
        return CODEC_Vorbis_Seek(source,page);
    case CODEC_TYPE_FLAC:
        return CODEC_FLAC_Seek(source,page);
    case CODEC_TYPE_MPEG:
        return CODEC_MPEG_Seek(source,page);
    }

    CODEC_SetError("Unrecognized codec type: %d",source->type);
    return -1;
}

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
Sint32 CODEC_PageSize(CODEC_Source* source) {
    if (source == NULL) {
        CODEC_SetError("Attempt to access NULL codec source");
        return -1;
    }
    
    switch(source->type) {
    case CODEC_TYPE_WAV:
        return CODEC_WAV_PageSize(source);
    case CODEC_TYPE_VORBIS:
        return CODEC_Vorbis_PageSize(source);
    case CODEC_TYPE_FLAC:
        return CODEC_FLAC_PageSize(source);
    case CODEC_TYPE_MPEG:
        return CODEC_MPEG_PageSize(source);
    }

    CODEC_SetError("Unrecognized codec type: %d",source->type);
    return -1;
}

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
Sint32 CODEC_LastPage(CODEC_Source* source) {
    if (source == NULL) {
        CODEC_SetError("Attempt to access NULL codec source");
        return -1;
    }
    
    switch(source->type) {
    case CODEC_TYPE_WAV:
        return CODEC_WAV_LastPage(source);
    case CODEC_TYPE_VORBIS:
        return CODEC_Vorbis_LastPage(source);
    case CODEC_TYPE_FLAC:
        return CODEC_FLAC_LastPage(source);
    case CODEC_TYPE_MPEG:
        return CODEC_MPEG_LastPage(source);
    }

    CODEC_SetError("Unrecognized codec type: %d",source->type);
    return -1;
}

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
Sint32 CODEC_Tell(CODEC_Source* source) {
    if (source == NULL) {
        CODEC_SetError("Attempt to access NULL codec source");
        return -1;
    }
    
    switch(source->type) {
    case CODEC_TYPE_WAV:
        return CODEC_WAV_Tell(source);
    case CODEC_TYPE_VORBIS:
        return CODEC_Vorbis_Tell(source);
    case CODEC_TYPE_FLAC:
        return CODEC_FLAC_Tell(source);
    case CODEC_TYPE_MPEG:
        return CODEC_MPEG_Tell(source);
    }

    CODEC_SetError("Unrecognized codec type: %d",source->type);
    return -1;
}

/** 
 * Returns 1 if the audio source is at the end of the stream; 0 otherwise
 *
 * @param source    The audio source
 *
 * @return 1 if the audio source is at the end of the stream; 0 otherwise
 */
Uint32 CODEC_EOF(CODEC_Source* source) {
    if (source == NULL) {
        CODEC_SetError("Attempt to access NULL codec source");
        return 0;
    }
    
    switch(source->type) {
    case CODEC_TYPE_WAV:
        return CODEC_WAV_EOF(source);
    case CODEC_TYPE_VORBIS:
        return CODEC_Vorbis_EOF(source);
    case CODEC_TYPE_FLAC:
        return CODEC_FLAC_EOF(source);
    case CODEC_TYPE_MPEG:
        return CODEC_MPEG_EOF(source);
    }

    CODEC_SetError("Unrecognized codec type: %d",source->type);
    return 0;
}

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
Sint32 CODEC_Read(CODEC_Source* source, float* buffer) {
    if (source == NULL) {
        CODEC_SetError("Attempt to access NULL codec source");
        return -1;
    }
    
    switch(source->type) {
    case CODEC_TYPE_WAV:
        return CODEC_WAV_Read(source,buffer);
    case CODEC_TYPE_VORBIS:
        return CODEC_Vorbis_Read(source,buffer);
    case CODEC_TYPE_FLAC:
        return CODEC_FLAC_Read(source,buffer);
    case CODEC_TYPE_MPEG:
        return CODEC_MPEG_Read(source,buffer);
    }

    CODEC_SetError("Unrecognized codec type: %d",source->type);
    return -1;
}

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
 * @param stream    The MP3 stream
 * @param buffer    The buffer to hold the PCM data
 *
 * @return the number of audio frames (samples/channels) read
 */
Sint64 CODEC_Fill(CODEC_Source* source, float* buffer) {
    if (source == NULL) {
        CODEC_SetError("Attempt to access NULL codec source");
        return -1;
    }
    
    switch(source->type) {
    case CODEC_TYPE_WAV:
        return CODEC_WAV_Fill(source,buffer);
    case CODEC_TYPE_VORBIS:
        return CODEC_Vorbis_Fill(source,buffer);
    case CODEC_TYPE_FLAC:
        return CODEC_FLAC_Fill(source,buffer);
    case CODEC_TYPE_MPEG:
        return CODEC_MPEG_Fill(source,buffer);
    }

    CODEC_SetError("Unrecognized codec type: %d",source->type);
    return -1;
}
