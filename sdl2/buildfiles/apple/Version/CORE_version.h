//
//  Core_version.h
//  sdl2
//
//  This is a dummy function so that the library will properly link.
//  Without it, XCode will remove the SDL libraries as part of code
//  optimization.
//
//  Created by Walker White on 12/4/22.
//  Copyright © 2022 Simple DirectMedia Layer. All rights reserved.
//

#ifndef __CORE_VERSION_H__
#define __CORE_VERSION_H__
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
extern "C" {
#endif

/** Returns the version of the SDL core library */
extern const char* CORE_get_version(void);

/* Ends C function definitions when using C++ */
#ifdef __cplusplus
}
#endif

#endif /* __CORE_VERSION_H__ */
