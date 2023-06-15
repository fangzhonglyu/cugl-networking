# SDL2 Codec
---
The standard audio extension for SDL2 is [SDL2 Mixer](https://github.com/libsdl-org/SDL_mixer/tree/main/external).
However, that library takes away a lot of audio control from the programmer,
especially if we want custom effects.  What SDL2 really needs is an API that
allows the programmer to extract linear PCM data (the default playback format 
for SDL2) from various audio formats.

That is the purpose of this extension. It only extracts data from audio files, 
and does not play them back. Hence this API can be used even when the SDL2 audio
subsystem is not initialized. 

The API provides a uniform "page-based" semantics for the audio data. Linear PCM
data is extracted from audio sources in pages, where the page size is determined
by the audio format. This makes it easy for the programmer to implement both
in-memory and streaming versions of audio playback. Playback streaming simply
pages in data as it is being played. As audio files can be quite large, streaming
audio can significantly reduce the footprint of your application.

The API does support seeking, but only at the page level. Sample level seeking
requires that the programmer buffer the page accesses.

## Codec Support

This version of the application creator is built from the following audio codecs:

- [FLAC](https://xiph.org/flac/download.html), version 1.4.2
- [OGG Vorbis](https://xiph.org/downloads/), version 1.3.7
- [Mini MP3](https://github.com/lieff/minimp3), version 1.0 (chosen for licensing reasons)

WAV support is based `SDL_Wave.c` from SDL2 version 2.12.0, adding page-based access
for WAV files. It is missing support for A-Law and mu-Law formats found in newer 
versions of `SDL_Wave.c`.

**TODO**: Add other Codecs provided by SDL2 Mixer.
