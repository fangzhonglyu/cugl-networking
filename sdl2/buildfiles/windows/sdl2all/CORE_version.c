//
//  CORE_version.c
//  sdl2
//
//  This is a dummy function so that the library will properly link.
//  Without it, XCode will remove the SDL libraries as part of code
//  optimization.
//
//  Created by Walker White on 12/4/22.
//  Copyright © 2022 Simple DirectMedia Layer. All rights reserved.
//
#include "CORE_version.h"

/** Returns the version of the SDL core library */
const char* CORE_get_version(void) {
    return "2.26.0";
}
