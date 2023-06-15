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

/* This is a FLAC file loading framework */
#include <SDL_codec.h>
#include <FLAC/stream_decoder.h>
#include "CODEC_internals.h"
#include <string.h>

/** Macro for error checking */
#define CHECK_SOURCE(source, retval) \
    if (source == NULL) { \
        CODEC_SetError("Attempt to access a NULL codec source"); \
        return retval; \
    } else if (source->decoder == NULL) { \
        CODEC_SetError("Codec source has invalid state"); \
        return retval; \
    }

/**
 * The internal structure for decoding 
 */
typedef struct {
    /** The file stream for the audio */
    SDL_RWops* stream;
    /** The OGG decoder struct */
    FLAC__StreamDecoder* flac;
    
    /** The size of a decoder chunk */
    Uint32 pagesize;
    /** The current page in the stream */
    Uint32 currpage;
    /** The previous page in the stream */
    Uint32 lastpage;

    /** The intermediate buffer for uniformizing FLAC data */
    float* buffer;
    /** The size of the intermediate buffer */
    Uint64  buffsize;
    /** The last element read from the intermediate buffer */
    Uint64  bufflast;
    /** The number of bits used to encode the sample data */
    Uint32  sampsize;

} CODEC_FLAC;


#pragma mark -
#pragma mark FLAC Callbacks
/**
 * Performs a read of the underlying file stream for the FLAC decoder
 *
 * This method abstracts the file access to allow us to read the asset on
 * non-standard platforms (e.g. Android).  If method reads less than the
 * requested number of bytes, the actual value is stored in the provided
 * parameter pointer.
 *
 * @param decoder   The FLAC decoder struct
 * @param buffer    The buffer to start the data read
 * @param bytes     The number of bytes to read
 * @param cdata     The AUFLACDecoder instance
 *
 * @return the callback status (error or continue)
 */
FLAC__StreamDecoderReadStatus flac_read(const FLAC__StreamDecoder *flac,
                                        FLAC__byte buffer[], size_t *bytes,
                                        void *cdata) {
    //SDL_Log("flac read");
    CODEC_Source* source = (CODEC_Source*)cdata;
    CODEC_FLAC* decoder = (CODEC_FLAC*)source->decoder;
    if (SDL_RWtell(decoder->stream) == SDL_RWsize(decoder->stream)) {
        return FLAC__STREAM_DECODER_READ_STATUS_END_OF_STREAM;
    }
    *bytes = SDL_RWread(decoder->stream, buffer, 1, *bytes);
    if (*bytes == -1) {
        return FLAC__STREAM_DECODER_READ_STATUS_ABORT;
    }
    return FLAC__STREAM_DECODER_READ_STATUS_CONTINUE;
}

/**
 * Performs a seek of the underlying file stream for the FLAC decoder
 *
 * This method abstracts the file access to allow us to read the asset on
 * non-standard platforms (e.g. Android).  The offset provided is from
 * the file beginning (e.g. SEEK_SET).
 *
 * @param decoder   The FLAC decoder struct
 * @param offset    The number of bytes from the beginning of the file
 * @param cdata     The AUFLACDecoder instance
 *
 * @return the callback status (error or continue)
 */
FLAC__StreamDecoderSeekStatus flac_seek(const FLAC__StreamDecoder *flac,
                                        FLAC__uint64 offset,
                                        void *cdata) {
    //SDL_Log("flac seek");
    CODEC_Source* source = (CODEC_Source*)cdata;
    CODEC_FLAC* decoder = (CODEC_FLAC*)source->decoder;
    if (SDL_RWseek(decoder->stream,offset, RW_SEEK_SET) == -1) {
        return FLAC__STREAM_DECODER_SEEK_STATUS_ERROR;
    }
    return FLAC__STREAM_DECODER_SEEK_STATUS_OK;
}

/**
 * Performs a tell of the underlying file stream for the FLAC decoder
 *
 * This method abstracts the file access to allow us to read the asset on
 * non-standard platforms (e.g. Android).  The value computed is the
 * file offset relative to the beginning of the file.  The value read is
 * stored in the provided parameter pointer.
 *
 * @param decoder   The FLAC decoder struct
 * @param offset    The pointer to store the offset from the beginning
 * @param cdata     The CODEC_Source reference
 *
 * @return the callback status (error or continue)
 */
FLAC__StreamDecoderTellStatus flac_tell(const FLAC__StreamDecoder *flac,
                                        FLAC__uint64 *offset,
                                        void *cdata) {
    //SDL_Log("flac tell");
    CODEC_Source* source = (CODEC_Source*)cdata;
    CODEC_FLAC* decoder = (CODEC_FLAC*)source->decoder;
    *offset = SDL_RWtell(decoder->stream);
    return (*offset == -1 ? FLAC__STREAM_DECODER_TELL_STATUS_ERROR : FLAC__STREAM_DECODER_TELL_STATUS_OK);
}

/**
 * Performs a length computation of the underlying file for the FLAC decoder
 *
 * This method abstracts the file access to allow us to read the asset on
 * non-standard platforms (e.g. Android).  The value computed is the
 * length in bytes.  The value read is stored in the provided parameter
 * pointer.
 *
 * @param decoder   The FLAC decoder struct
 * @param length    The pointer to store the file length
 * @param cdata     The AUFLACDecoder instance
 *
 * @return the callback status (error or continue)
 */
FLAC__StreamDecoderLengthStatus flac_size(const FLAC__StreamDecoder *flac,
                                          FLAC__uint64 *length,
                                          void *cdata) {
    //SDL_Log("Flac size");
    CODEC_Source* source = (CODEC_Source*)cdata;
    CODEC_FLAC* decoder = (CODEC_FLAC*)source->decoder;
    *length = SDL_RWsize(decoder->stream);
    return (*length == -1 ? FLAC__STREAM_DECODER_LENGTH_STATUS_ERROR : FLAC__STREAM_DECODER_LENGTH_STATUS_OK);
}

/**
 * Performs an eof computation of the underlying file for the FLAC decoder
 *
 * This method abstracts the file access to allow us to read the asset on
 * non-standard platforms (e.g. Android).
 *
 * @param decoder   The FLAC decoder struct
 * @param cdata     The AUFLACDecoder instance
 *
 * @return true if the stream is at the end of the file
 */
FLAC__bool flac_eof(const FLAC__StreamDecoder *flac, void *cdata) {
    //SDL_Log("Flac eof");
    CODEC_Source* source = (CODEC_Source*)cdata;
    CODEC_FLAC* decoder = (CODEC_FLAC*)source->decoder;
    return SDL_RWtell(decoder->stream) == SDL_RWsize(decoder->stream);
}

/**
 * Performs a write of decoded sample data
 *
 * This method is the primary write method for decoded sample data.  The
 * data is converted to a float format and stored in the backing buffer
 * for later access.
 *
 * @param decoder   The FLAC decoder struct
 * @param frame     The frame header for the current data block
 * @param buffer    The decoded samples for this block
 * @param cdata     The AUFLACDecoder instance
 *
 * @return the callback status (error or continue)
 */
FLAC__StreamDecoderWriteStatus flac_write(const FLAC__StreamDecoder *flac,
                                          const FLAC__Frame *frame,
                                          const FLAC__int32 * const buffer[],
                                          void *cdata) {
    CODEC_Source* source = (CODEC_Source*)cdata;
    CODEC_FLAC* decoder = (CODEC_FLAC*)source->decoder;
    if(frame->header.channels != source->channels) {
        SDL_SetError("FLAC has changed number of channels from %d to %d", 
                     source->channels, frame->header.channels);
        decoder->buffsize = 0;
        return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;
    }
    
    decoder->buffsize = frame->header.blocksize;
    double factor = 1.0/(1L << decoder->sampsize);
    for(Uint32 ch = 0; ch < source->channels; ch++) {
        if (buffer[ch] == NULL) {
            SDL_SetError("FLAC channel %d is NULL", ch);
            decoder->buffsize = 0;
            return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;
        }
        
        //Sint32* output = decoder->buffer+ch;
        float* output = decoder->buffer+ch;
        const FLAC__int32* input = buffer[ch];
        for (Uint32 ii = 0; ii < decoder->buffsize; ii++) {
            *output = (float)(*input*factor);
            output += source->channels;
            input++;
        }
    }
    
    decoder->bufflast = 0;
    return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}

/**
 * Performs a write of the file metadata
 *
 * This method is called when the decoder is initialized to query the
 * stream info data. This is how the decoder gathers the important
 * decoding information like sample rate and channel layout.
 *
 * @param decoder   The FLAC decoder struct
 * @param metadata  The file metadata
 * @param cdata     The AUFLACDecoder instance
 *
 * @return the callback status (error or continue)
 */
void flac_metadata(const FLAC__StreamDecoder *flac,
                   const FLAC__StreamMetadata *metadata,
                   void *cdata) {
    CODEC_Source* source = (CODEC_Source*)cdata;
    CODEC_FLAC* decoder = (CODEC_FLAC*)source->decoder;
    if(metadata->type == FLAC__METADATA_TYPE_STREAMINFO) {
        decoder->pagesize = metadata->data.stream_info.max_blocksize;
        source->channels = metadata->data.stream_info.channels;
        decoder->sampsize = metadata->data.stream_info.bits_per_sample;
        source->frames = metadata->data.stream_info.total_samples;
        source->rate   = metadata->data.stream_info.sample_rate;
        decoder->lastpage = (Uint32)(source->frames/decoder->pagesize);
    }
}

/**
 * Records an error in the underlying decoder
 *
 * This method does not abort decoding. Instead, it records the error
 * with SDL_SetError for later retrieval.
 *
 * @param flac      The FLAC decoder struct
 * @param state     The error status
 * @param cdata     The AUFLACDecoder instance
 *
 * @return the callback status (error or continue)
 */
void flac_error(const FLAC__StreamDecoder *flac,
                FLAC__StreamDecoderErrorStatus status,
                void *cdata) {
    SDL_SetError("FLAC error: %s",FLAC__StreamDecoderErrorStatusString[status]);
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
 * This function is a helper for {@link CODEC_Read} and {@link CODEC_Fill}.
 *
 * @param source    The audio source
 * @param decoder   The FLAC decoder
 * @param buffer    The buffer to hold the PCM data
 *
 * @return the number of audio frames (samples/channels) read
 */
Sint32 flac_read_page(CODEC_Source* source, CODEC_FLAC* decoder, float* buffer) {
    Sint32 read = 0;
    Uint32 limit = decoder->pagesize;
    while (read < (Sint32)limit) {
        // First see how much is available
        Sint32 avail = (Sint32)(decoder->buffsize - decoder->bufflast);
        avail = ((Sint32)(decoder->pagesize - read) < avail ? decoder->pagesize - read : avail);
        
        float* output = buffer+(decoder->bufflast*source->channels);
        memcpy(output,decoder->buffer,avail*source->channels*sizeof(float));
        read += avail;
        decoder->bufflast += avail;
        
        // Page in more if we need it
        if (read < (Sint32)limit) {
            if (!FLAC__stream_decoder_process_single(decoder->flac) || decoder->bufflast == decoder->buffsize) {
                return read;
            }
        }
    }
    decoder->currpage++;
    return read;
}


#pragma mark -
#pragma mark CODEC Functions

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
CODEC_Source* CODEC_OpenFLAC(const char* filename) {
    SDL_RWops* stream = SDL_RWFromFile(filename, "r");
    if (stream == NULL) {
        SDL_SetError("Could not open '%s'",filename);
        return NULL;
    }

    FLAC__StreamDecoder *flac;
    if (!(flac = FLAC__stream_decoder_new())) {
        SDL_SetError("Could not allocate FLAC decoder");
        SDL_RWclose(stream);
        return NULL;
    }
    
    (void)FLAC__stream_decoder_set_md5_checking(flac, true);
    FLAC__StreamDecoderInitStatus status;
    
    CODEC_FLAC* decoder = (CODEC_FLAC*)malloc(sizeof(CODEC_FLAC));
    if (!decoder) { 
        SDL_RWclose(stream);
        return NULL;
    }
    CODEC_Source* source = (CODEC_Source*)malloc(sizeof(CODEC_Source));
    if (!source) {
        SDL_RWclose(stream);
        free(decoder);
        return NULL;
    }
    source->type = CODEC_TYPE_FLAC;
    source->decoder = decoder;
    decoder->stream = stream;
    decoder->flac = flac;
    decoder->currpage = 0;
    decoder->buffer = NULL;
    decoder->buffsize = 0;
    decoder->bufflast = 0;

    
    status = FLAC__stream_decoder_init_stream(flac,
                                              flac_read, flac_seek, flac_tell,
                                              flac_size, flac_eof,
                                              flac_write, flac_metadata, flac_error,
                                              source);
    if (status != FLAC__STREAM_DECODER_INIT_STATUS_OK) {
        SDL_SetError("FLAC initialization error: %s",FLAC__StreamDecoderInitStatusString[status]);
        FLAC__stream_decoder_delete(flac);
        free(decoder);
        free(source);
        SDL_RWclose(stream);
        return NULL;
    }

    int ok = FLAC__stream_decoder_process_until_end_of_metadata(flac);
    if (!ok || decoder->pagesize == 0) {
        SDL_SetError("FLAC '%s' does not have a stream_info header",filename);
        FLAC__stream_decoder_delete(flac);
        free(decoder);
        free(source);
        SDL_RWclose(stream);
        return NULL;
    }
    
    decoder->buffer = (float*)malloc(decoder->pagesize*source->channels*sizeof(float));
    return source;
}

/**
 * The FLAC specific implementation of {@link CODEC_Close}.
 *
 * @param source    The source to close
 * 
 * @return 0 if the source was successfully closed, -1 otherwise.
 */
Sint32 CODEC_FLAC_Close(CODEC_Source* source) {
    CHECK_SOURCE(source,-1)
    
    CODEC_FLAC* decoder = (CODEC_FLAC*)(source->decoder);
    if (decoder->flac != NULL) {
        FLAC__stream_decoder_delete(decoder->flac);
        decoder->flac = NULL;
    }
    if (decoder->buffer != NULL) {
        free(decoder->buffer);
        decoder->buffer = NULL;
    }
    if (decoder->stream != NULL) {
        SDL_RWclose(decoder->stream);
        decoder->stream = NULL;
    }
    SDL_free(decoder);
    source->decoder = NULL;
    SDL_free(source);
    return 0;
}

/**
 * The FLAC specific implementation of {@link CODEC_Seek}.
 *
 * @param source    The audio source
 * @param page        The page to seek
 *
 * @return the page acquired, or -1 if there is an error
 */
Sint32 CODEC_FLAC_Seek(CODEC_Source* source, Uint32 page) {
    CHECK_SOURCE(source,-1)

    CODEC_FLAC* decoder = (CODEC_FLAC*)(source->decoder);
    if (page > decoder->lastpage) {
        page = decoder->lastpage;
    }
    
    if (!FLAC__stream_decoder_seek_absolute(decoder->flac,page*decoder->pagesize)) {
        SDL_SetError("Seek is not supported");
    }
    decoder->currpage = page;
    return page;
}

/**
 * The FLAC specific implementation of {@link CODEC_PageSize}.
 *
 * @param source    The audio source
 *
 * @return the number of audio frames in a page, or -1 if there is an error
 */
Sint32 CODEC_FLAC_PageSize(CODEC_Source* source) {
    CHECK_SOURCE(source,-1)
    return ((CODEC_FLAC*)(source->decoder))->pagesize;
}

/**
 * The FLAC specific implementation of {@link CODEC_LastPage}.
 *
 * @param source    The audio source
 *
 * @return the index of the last page in the stream, or -1 if there is an error
 */
Sint32 CODEC_FLAC_LastPage(CODEC_Source* source) {
    CHECK_SOURCE(source,-1)
    return ((CODEC_FLAC*)(source->decoder))->lastpage;
}

/**
 * The FLAC specific implementation of {@link CODEC_Tell}.
 *
 * @param source    The audio source
 *
 * @return the index of the current page in the stream, or -1 if there is an error
 */
Sint32 CODEC_FLAC_Tell(CODEC_Source* source) {
    CHECK_SOURCE(source,-1)
    return ((CODEC_FLAC*)(source->decoder))->currpage;
}

/**
 * The FLAC specific implementation of {@link CODEC_EOF}.
 *
 * @param source    The audio source
 *
 * @return 1 if the audio source is at the end of the stream; 0 otherwise
 */
Uint32 CODEC_FLAC_EOF(CODEC_Source* source) {
    CHECK_SOURCE(source,0)
    CODEC_FLAC* decoder = (CODEC_FLAC*)(source->decoder);    
    return (decoder->currpage == decoder->lastpage);
}

/**
 * The FLAC specific implementation of {@link CODEC_Read}.
 *
 * @param source    The audio source
 * @param buffer    The buffer to hold the PCM data
 *
 * @return the number of audio frames (samples/channels) read
 */
Sint32 CODEC_FLAC_Read(CODEC_Source* source, float* buffer) {
    CHECK_SOURCE(source,-1)
    CODEC_FLAC* decoder = (CODEC_FLAC*)(source->decoder);
    return flac_read_page(source,decoder,buffer);
}

/**
 * The WAV specific implementation of {@link CODEC_Fill}.
 *
 * @param source    The audio source
 * @param buffer    The buffer to hold the PCM data
 *
 * @return the number of audio frames (samples/channels) read
 */
Sint64 CODEC_FLAC_Fill(CODEC_Source* source, float* buffer) {
    CHECK_SOURCE(source,-1)
    CODEC_FLAC* decoder = (CODEC_FLAC*)(source->decoder);
    
    Sint64 read = 0;
    Sint64 limit = (Sint64)source->frames;
    while (read < limit) {
        Sint32 take = flac_read_page(source,decoder,buffer+read*source->channels);
        if (take == 0) {
            limit = read;
        } else {
            read += take;
        }
    }
    return read;
}
