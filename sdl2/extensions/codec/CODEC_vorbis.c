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

/* This is a OGG Vorbis file loading framework */

#include <SDL_codec.h>
#include <vorbis/vorbisfile.h>
#include "CODEC_internals.h"

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
    OggVorbis_File oggfile;
    /** Reference to the logical bitstream for decoding */
    Sint32 bitstream;
    
    /** The size of a decoder chunk */
    Uint32 pagesize;
    /** The current page in the stream */
    Uint32 currpage;
    /** The previous page in the stream */
    Uint32 lastpage;

} CODEC_Vorbis;


#pragma mark -
#pragma mark OGG Callbacks

/**
 * Returns the SDL channel for the given OGG channel
 *
 * The channel layout for Ogg data is nonstandard (e.g. channels > 3 are not 
 * stereo compatible), so this function standardizes the channel layout to 
 * agree with FLAC and other data encodings.
 *
 * @param ch        The OGG channel position
 * @param channels  The total number of channels
 *
 * @return the SDL channel for the given OGG channel
 */
static Uint32 ogg2sdl(Uint32 ch, Uint32 channels) {
    switch (channels) {
        case 3:
        case 5:
        {
            switch (ch) {
                case 1:
                    return 2;
                case 2:
                    return 1;
            }
        }
            break;
        case 6:
        {
            switch (ch) {
                case 1:
                    return 2;
                case 2:
                    return 1;
                case 3:
                    return 4;
                case 4:
                    return 5;
                case 5:
                    return 3;
            }
        }
    }
    return ch;
}

/**
 * Performs a read of the underlying file stream for the OGG decoder
 *
 * This method abstracts the file access to allow us to read the asset on
 * non-standard platforms (e.g. Android).  
 *
 * @param ptr           The buffer to start the data read
 * @param size          The number of bytes per element
 * @param nmemb         The number of elements to read
 * @param datasource    The file to read from
 * 
 * @return the number of elements actually read.
 */
static size_t ogg_read(void *ptr, size_t size, size_t nmemb, void *datasource) {
    return SDL_RWread((SDL_RWops*)datasource, ptr, size, nmemb);
}

/**
 * Performs a seek of the underlying file stream for the OGG decoder
 *
 * This method abstracts the file access to allow us to read the asset on
 * non-standard platforms (e.g. Android).  The value whence is one the 
 * classic values SEEK_CUR, SEEK_SET, or SEEK_END.
 *
 * @param datasource    The file to seek
 * @param offset        The offset to seek to
 * @param whence        The position to offset from
 *
 * @return the new file position
 */
static int    ogg_seek(void *datasource, ogg_int64_t offset, int whence) {
    if(datasource==NULL)return(-1);
    return (int)SDL_RWseek((SDL_RWops*)datasource,offset,whence);
}

/**
 * Performs a tell of the underlying file stream for the OGG decoder
 *
 * This method abstracts the file access to allow us to read the asset on
 * non-standard platforms (e.g. Android). This function returns the current
 * offset (from the beginning) of the file position.
 *
 * @param datasource    The file to query
 *
 * @return the current file position
 */
static long   ogg_tell(void *datasource) {
    return (long)SDL_RWtell((SDL_RWops*)datasource);
}


#pragma mark -
#pragma mark CODEC Functions
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
CODEC_Source* CODEC_OpenVorbis(const char* filename) {
    SDL_RWops* stream = SDL_RWFromFile(filename, "rb");
    if (stream == NULL) {
        return NULL;
    }
    
    CODEC_Vorbis* decoder = malloc(sizeof(CODEC_Vorbis));
    if (!decoder) {
        SDL_RWclose(stream);
        return NULL;
    }
    decoder->bitstream = -1;

    ov_callbacks calls;
    calls.read_func = ogg_read;
    calls.seek_func = ogg_seek;
    calls.tell_func = ogg_tell;
    calls.close_func = NULL;

    int error = ov_open_callbacks(stream, &(decoder->oggfile), NULL, 0, calls);
    if (error) {
        CODEC_SetError("File '%s' is not an OGG Vorbis file", filename);
        SDL_RWclose(stream);
        free(decoder);
        return NULL;
    }
    
    vorbis_info* info = ov_info(&(decoder->oggfile), decoder->bitstream);
    Uint32 frames =  (uint32_t)ov_pcm_total(&(decoder->oggfile), decoder->bitstream);
    
    decoder->stream = stream;
    decoder->pagesize = DECODER_PAGE_SIZE/(sizeof(float)*info->channels);
    decoder->lastpage = (Uint32)(frames/decoder->pagesize);;
    decoder->currpage = 0;
    
    CODEC_Source* result = malloc(sizeof(CODEC_Source));
    if (!result) {
        SDL_RWclose(stream);
        free(decoder);
        return NULL;
    }
    result->type = CODEC_TYPE_VORBIS;
    result->decoder = (void*)decoder;
    result->channels = info->channels;
    result->rate     = (Uint32)info->rate;
    result->frames   = frames;
    return result;
}

/**
 * The Vorbis specific implementation of {@link CODEC_Close}.
 *
 * @param source    The source to close
 * 
 * @return 0 if the source was successfully closed, -1 otherwise.
 */
Sint32 CODEC_Vorbis_Close(CODEC_Source* source) {
    CHECK_SOURCE(source,-1)
    
    CODEC_Vorbis* decoder = (CODEC_Vorbis*)(source->decoder);
    ov_clear(&(decoder->oggfile));
    if (decoder->stream != NULL) {
        SDL_RWclose(decoder->stream);
        decoder->stream = NULL;
    }
    free(source->decoder);
    source->decoder = NULL;
    free(source);
    return 0;
}

/**
 * The Vorbis specific implementation of {@link CODEC_Seek}.
 *
 * @param source    The audio source
 * @param page        The page to seek
 *
 * @return the page acquired, or -1 if there is an error
 */
Sint32 CODEC_Vorbis_Seek(CODEC_Source* source, Uint32 page) {
    CHECK_SOURCE(source,-1)
    
    CODEC_Vorbis* decoder = (CODEC_Vorbis*)(source->decoder);    
    if (page > decoder->lastpage) {
        CODEC_SetError("Page %d is out of bounds",page);
        return -1;
    }
    
    Uint64 frame = (Uint64)page*decoder->pagesize;
    if (frame > source->frames) {
        frame = source->frames;
    }
    
    int error = ov_pcm_seek(&(decoder->oggfile),frame);
    if (error) {
        if (error == OV_ENOSEEK) {
            CODEC_SetError("Bitstream is not seekable");
        } else if (error == OV_EINVAL) {
            CODEC_SetError("Invalid argument value (make sure file is open)");
        } else if (error == OV_EREAD) {
            CODEC_SetError("Internal logic fault (likely heap corruption)");
        } else if (error == OV_EFAULT) {
            CODEC_SetError("Media returned a read error");
        } else if (error == OV_EBADLINK) {
            CODEC_SetError("Invalid stream section");
        }
        return -1;
    }
    decoder->currpage = (uint32_t)(frame/decoder->pagesize);
    return decoder->currpage;
}

/**
 * The Vorbis specific implementation of {@link CODEC_PageSize}.
 *
 * @param source    The audio source
 *
 * @return the number of audio frames in a page, or -1 if there is an error
 */
Sint32 CODEC_Vorbis_PageSize(CODEC_Source* source) {
    CHECK_SOURCE(source,-1)
    return ((CODEC_Vorbis*)(source->decoder))->pagesize;
}

/**
 * The Vorbis specific implementation of {@link CODEC_LastPage}.
 *
 * @param source    The audio source
 *
 * @return the index of the last page in the stream, or -1 if there is an error
 */
Sint32 CODEC_Vorbis_LastPage(CODEC_Source* source) {
    CHECK_SOURCE(source,-1)
    return ((CODEC_Vorbis*)(source->decoder))->lastpage;
}

/**
 * The Vorbis specific implementation of {@link CODEC_Tell}.
 *
 * @param source    The audio source
 *
 * @return the index of the current page in the stream, or -1 if there is an error
 */
Sint32 CODEC_Vorbis_Tell(CODEC_Source* source) {
    CHECK_SOURCE(source,-1)
    return ((CODEC_Vorbis*)(source->decoder))->currpage;
}

/**
 * The Vorbis specific implementation of {@link CODEC_EOF}.
 *
 * @param source    The audio source
 *
 * @return 1 if the audio source is at the end of the stream; 0 otherwise
 */
Uint32 CODEC_Vorbis_EOF(CODEC_Source* source) {
    CHECK_SOURCE(source,0)
    CODEC_Vorbis* decoder = (CODEC_Vorbis*)(source->decoder);    
    return (decoder->currpage == decoder->lastpage);
}

/**
 * The Vorbis specific implementation of {@link CODEC_Read}.
 *
 * @param source    The audio source
 * @param buffer    The buffer to hold the PCM data
 *
 * @return the number of audio frames (samples/channels) read
 */
Sint32 CODEC_Vorbis_Read(CODEC_Source* source, float* buffer) {
    CHECK_SOURCE(source,-1)
    
    CODEC_Vorbis* decoder = (CODEC_Vorbis*)(source->decoder);    
    if (decoder->currpage == decoder->lastpage) {
        return 0;
    }
    
    // Now read from the stream
    Sint32 read = 0;
    Uint32 size = decoder->pagesize;
    
    float** pcmb;
    while (read < (Sint32)size) {
        Sint32 avail = (size - read >= size ? size : size - read);
        avail = (int)ov_read_float(&(decoder->oggfile), &pcmb, avail, &(decoder->bitstream));
        if (avail < 0) {
            if (avail == OV_HOLE) {
                CODEC_SetError("OGG file experienced an interruption in data");
            } else if (avail == OV_EBADLINK) {
                CODEC_SetError("OGG file has an invalid stream section");
            } else if (avail == OV_EINVAL) {
                CODEC_SetError("The OGG file headers cannot be read");
            }
            return avail;
        } else if (avail == 0) {
            break;
        }
        
        // Copy everything into its place
        for (Uint32 ch = 0; ch < source->channels; ++ch) {
            // OGG representation differs from SDL representation
            Uint32 outch = ogg2sdl(ch,source->channels);
            
            float* output = buffer+(read*source->channels)+outch;
            float* input  = pcmb[ch];
            Uint32 temp = avail;
            while (temp--) {
                *output = *input;
                output += source->channels;
                input++;
            }
        }
        
        read += avail;
    }
    
    decoder->currpage++;
    return read;
}

/**
 * The Vorbis specific implementation of {@link CODEC_Fill}.
 *
 * @param source    The audio source
 * @param buffer    The buffer to hold the PCM data
 *
 * @return the number of audio frames (samples/channels) read
 */

Sint64 CODEC_Vorbis_Fill(CODEC_Source* source, float* buffer) {
    CHECK_SOURCE(source,-1)
    
    CODEC_Vorbis* decoder = (CODEC_Vorbis*)(source->decoder);
    Uint32 currpage = decoder->currpage;
    if (currpage != 0) {
        CODEC_Vorbis_Seek(source,0);
    }
    
    // Now read from the stream
    Sint64 read = 0;
    Sint64 size = (Sint64)source->frames;
    
    float** pcmb;
    while (read < size) {
        Sint64 avail = (size - read >= size ? size : size - read);
        avail = (int)ov_read_float(&(decoder->oggfile), &pcmb, (int)avail, &(decoder->bitstream));
        if (avail < 0) {
            if (avail == OV_HOLE) {
                CODEC_SetError("OGG file experienced an interruption in data");
            } else if (avail == OV_EBADLINK) {
                CODEC_SetError("OGG file has an invalid stream section");
            } else if (avail == OV_EINVAL) {
                CODEC_SetError("The OGG file headers cannot be read");
            }
            return avail;
        } else if (avail == 0) {
            break;
        }
        
        // Copy everything into its place
        for (Uint32 ch = 0; ch < source->channels; ++ch) {
            // OGG representation differs from SDL representation
            Uint32 outch = ogg2sdl(ch,source->channels);
            
            float* output = buffer+(read*source->channels)+outch;
            float* input  = pcmb[ch];
            Uint32 temp = (Uint32)avail;
            while (temp--) {
                *output = *input;
                output += source->channels;
                input++;
            }
        }
        
        read += avail;
    }
    
    if (currpage != 0) {
        CODEC_Vorbis_Seek(source,currpage);
    }
    return read;
}
