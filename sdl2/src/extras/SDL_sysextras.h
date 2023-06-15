/*
  Simple DirectMedia Layer Extensions
  Copyright (C) 2022 Walker White

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/
#ifndef SDL_sysextras_h_
#define SDL_sysextras_h_
#include "../SDL_internal.h"
#include "SDL_extras.h"
#include "SDL_error.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * System dependent version of SDL_GetDisplayPixelBounds
 *
 * @param displayIndex  The display to query
 * @param rect          Rectangle to store the display bounds
 */
extern void SDL_SYS_GetDisplayPixelBounds(int displayIndex, SDL_Rect *rect);

/**
 * System dependent version of SDL_GetDisplaySafeBounds
 *
 * @param displayIndex  The display to query
 * @param rect          Rectangle to store the display bounds
 */
extern DECLSPEC void SDL_SYS_GetDisplaySafeBounds(int displayIndex, SDL_Rect *rect);

/**
 * System dependent version of SDL_CheckDisplayNotch
 *
 * @param displayIndex  The display to query
 *
 * @return 1 if this device has a notch, 0 otherwise
 */
extern DECLSPEC int SDL_SYS_CheckDisplayNotch(int displayIndex);

/**
 * System dependent version of SDL_GetPixelDensity
 *
 * @param displayIndex  The display to query
 *
 * @return the number of pixels for each point.
 */
extern DECLSPEC float SDL_SYS_GetPixelDensity(int displayIndex);

/**
 * System dependent version of SDL_GetDeviceOrientation
 *
 * @param displayIndex  The display to query
 *
 * @return the current device orientation.
 */
extern DECLSPEC SDL_DisplayOrientation SDL_SYS_GetDeviceOrientation(int displayIndex);

/**
 * System dependent version of SDL_GetDefaultOrientation
 *
 * @param displayIndex  The display to query
 *
 * @return the default orientation of this device.
 */
extern DECLSPEC SDL_DisplayOrientation SDL_SYS_GetDefaultOrientation(int displayIndex);

#ifdef __cplusplus
}
#endif

/* vi: set ts=4 sw=4 expandtab: */


#endif /* SDL_sysextras_h_ */

/* vi: set ts=4 sw=4 expandtab: */
