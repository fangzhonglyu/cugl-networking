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

/* This is a MP3 file loading framework */
#include <SDL_codec.h>
#include <mp3stream.h>
#include <limits.h>
#include <string.h>
#include "CODEC_internals.h"

/** Macro for error checking */
#define CHECK_SOURCE(source, retval) \
    if (source == NULL) { \
    	SDL_Log("Attempt to access a NULL codec source"); \
        CODEC_SetError("Attempt to access a NULL codec source"); \
        return retval; \
    } else if (source->decoder == NULL) { \
    	SDL_Log("Codec source has invalid state"); \
        CODEC_SetError("Codec source has invalid state"); \
        return retval; \
    }

/**
 * The internal structure for decoding 
 */
typedef struct {
    /** The MPEG decoder struct */
    MP3Stream* converter;
    /** A buffer to store the decoding data */
    Sint16* buffer;
    
    /** The size of a decoder chunk */
    Uint32 pagesize;
    /** The current page in the stream */
    Uint32 currpage;
    /** The previous page in the stream */
    Uint32 lastpage;

} CODEC_MPEG;

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
 * This function is a helper for {@link CODEC_Read} and {@link CODEC_Fill}.
 *
 * @param source    The audio source
 * @param decoder   The MP3 decoder
 * @param buffer    The buffer to hold the PCM data
 *
 * @return the number of audio frames (samples/channels) read
 */
Sint32 mpeg_read_page(CODEC_Source* source, CODEC_MPEG* decoder, float* buffer) {
	Sint32 amount = (Sint32)MP3Stream_ReadPage(decoder->converter,decoder->buffer);
	if (amount < 0) {
		return -1;
	}
	double factor = 1.0/(SHRT_MAX+1);

    Uint32 temp = amount;
    float* output = buffer;
    short* input  = decoder->buffer;
    while (temp--) {
        *output = (float)((*input)*factor);
        output++;
        input++;
    }
	decoder->currpage++;
    return amount/source->channels;
}

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
CODEC_Source* CODEC_OpenMPEG(const char* filename) {
	MP3Stream* converter = MP3Stream_Alloc(filename);
	if (converter == NULL) {
		return NULL;
	}
	
	CODEC_MPEG* decoder  = (CODEC_MPEG*)malloc(sizeof(CODEC_MPEG));
    if (!decoder) {
        MP3Stream_Free(converter);
        return NULL;
    }
	CODEC_Source* source = (CODEC_Source*)malloc(sizeof(CODEC_Source));
    if (!source) {
        MP3Stream_Free(converter);
        free(decoder);
        return NULL;
    }

	source->channels = MP3Stream_IsStereo(converter) ? 2 : 0;
	source->rate     = MP3Stream_GetFrequency(converter);
	decoder->pagesize = MP3Stream_GetPageSize(converter);
	source->frames = MP3Stream_GetLength(converter);

	SDL_Log("Rate %d",source->rate);
	SDL_Log("Page size %d",decoder->pagesize);

    decoder->lastpage = (Uint32)(source->frames/decoder->pagesize);
    decoder->currpage = 0;
    
    Sint16* buffer = (Sint16*)malloc(sizeof(Sint16)*decoder->pagesize*source->channels);
    memset(buffer,0,sizeof(Sint16)*decoder->pagesize*source->channels);

    decoder->buffer = buffer;
    decoder->converter = converter;
    source->decoder = decoder;
    source->type = CODEC_TYPE_MPEG;
	return source;
}

/**
 * The MP3 specific implementation of {@link CODEC_Close}.
 *
 * @param source    The source to close
 * 
 * @return 0 if the source was successfully closed, -1 otherwise.
 */
Sint32 CODEC_MPEG_Close(CODEC_Source* source) {
	CHECK_SOURCE(source,-1)
    
    CODEC_MPEG* decoder = (CODEC_MPEG*)(source->decoder);
    if (decoder->converter != NULL) {
	    MP3Stream_Free(decoder->converter);
	    decoder->converter = NULL;
	}
    if (decoder->buffer != NULL) {
        free(decoder->buffer);
        decoder->buffer = NULL;
    }
    free(decoder);
	source->decoder = NULL;
	free(source);	
    return 0;
}

/**
 * The MP3 specific implementation of {@link CODEC_Seek}.
 *
 * @param source    The audio source
 * @param page        The page to seek
 *
 * @return the page acquired, or -1 if there is an error
 */
Sint32 CODEC_MPEG_Seek(CODEC_Source* source, Uint32 page) {
	CHECK_SOURCE(source,-1)

    CODEC_MPEG* decoder = (CODEC_MPEG*)(source->decoder);
    if (page*decoder->pagesize > source->frames) {
        page = MP3Stream_GetLastPage(decoder->converter);
    }
    
    page = MP3Stream_SetPage(decoder->converter,page);
    decoder->currpage = page;
    return page;
}

/**
 * The MP3 specific implementation of {@link CODEC_PageSize}.
 *
 * @param source    The audio source
 *
 * @return the number of audio frames in a page, or -1 if there is an error
 */
Sint32 CODEC_MPEG_PageSize(CODEC_Source* source) {
    CHECK_SOURCE(source,-1)
    return ((CODEC_MPEG*)(source->decoder))->pagesize;
}

/**
 * The MP3 specific implementation of {@link CODEC_LastPage}.
 *
 * @param source    The audio source
 *
 * @return the index of the last page in the stream, or -1 if there is an error
 */
Sint32 CODEC_MPEG_LastPage(CODEC_Source* source) {
    CHECK_SOURCE(source,-1)
    return ((CODEC_MPEG*)(source->decoder))->lastpage;
}

/**
 * The MP3 specific implementation of {@link CODEC_Tell}.
 *
 * @param source    The audio source
 *
 * @return the index of the current page in the stream, or -1 if there is an error
 */
Sint32 CODEC_MPEG_Tell(CODEC_Source* source) {
    CHECK_SOURCE(source,-1)
    return ((CODEC_MPEG*)(source->decoder))->currpage;
}

/**
 * The MP3 specific implementation of {@link CODEC_EOF}.
 *
 * @param source    The audio source
 *
 * @return 1 if the audio source is at the end of the stream; 0 otherwise
 */
Uint32 CODEC_MPEG_EOF(CODEC_Source* source) {
    CHECK_SOURCE(source,0)
    CODEC_MPEG* decoder = (CODEC_MPEG*)(source->decoder);    
    return (decoder->currpage == decoder->lastpage);
}

/**
 * The MP3 specific implementation of {@link CODEC_Read}.
 *
 * @param source    The audio source
 * @param buffer    The buffer to hold the PCM data
 *
 * @return the number of audio frames (samples/channels) read
 */
Sint32 CODEC_MPEG_Read(CODEC_Source* source, float* buffer) {
	CHECK_SOURCE(source,-1)

    CODEC_MPEG* decoder = (CODEC_MPEG*)(source->decoder);
	if (decoder->currpage < decoder->lastpage) {
    	return mpeg_read_page(source,decoder,buffer);
    }
    return 0;
}

/**
 * The MP3 specific implementation of {@link CODEC_Fill}.
 *
 * @param source    The audio source
 * @param buffer    The buffer to hold the PCM data
 *
 * @return the number of audio frames (samples/channels) read
 */
Sint64 CODEC_MPEG_Fill(CODEC_Source* source, float* buffer) {
    CHECK_SOURCE(source,-1)
    
    CODEC_MPEG* decoder = (CODEC_MPEG*)(source->decoder);
    Uint32 currpage = decoder->currpage;
    if (currpage != 0) {
        MP3Stream_SetPage(decoder->converter,0);
    }
	
    Sint32 amt  = 0;
	Sint64 read = 0;
	Sint64 offset = 0;
	while (decoder->currpage < decoder->lastpage) {
		amt = mpeg_read_page(source,decoder,buffer+offset);
		read += amt;
		offset += amt*source->channels;
	}
    if (decoder->currpage == decoder->lastpage) {
        amt = mpeg_read_page(source,decoder,buffer+offset);
        read += amt;
    }
    
    if (currpage != 0) {
        MP3Stream_SetPage(decoder->converter,currpage);
    }
    return read;
}
