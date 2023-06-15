/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2022 Sam Lantinga <slouken@libsdl.org>

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

/**
 *  \file SDL_extras.h
 *
 *  \brief Include file for SDL extensions used in CUGL 
 *  \author Walker M. White
 */

#ifndef SDL_extras_h_
#define SDL_extras_h_

#include "SDL_stdinc.h"
#include "SDL_video.h"
#include "SDL_rect.h"

#include "begin_code.h"

/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Acquires the screen resolution (in pixels) for this display
 *
 * The standard function {@link SDL_GetDisplayBounds} is unreliable in that it
 * does not always extract the bounds in the same format. Sometimes it extracts
 * them in pixels; other times in points. This function always guarantees the
 * bounds represent pixels.
 *
 * @param displayIndex  The display to query
 * @param rect          Rectangle to store the display bounds
 *
 * @return 0 on success; -1 if on failure (displayIndex is invalid)
 */
extern DECLSPEC int SDL_GetDisplayPixelBounds(int displayIndex, SDL_Rect *rect);

/**
 * Returns the usable full screen resolution for this display
 *
 * Usable is a subjective term defined by the operating system. In general, it means 
 * the full screen minus any space used by important user interface elements, like a 
 * status bar (iPhone), menu bar (OS X), or task bar (Windows).
 *
 * Because the usable bounds depends on orientation, the bounds computed will use the
 * current device orientation. If the orientation is unknown or on face (either face-up
 * or face-down), this will use the current orientation of the display (not the device).
 *
 * The values stored in the rectangle represent pixels.
 * 
 * @param displayIndex  The display to query
 * @param rect          Rectangle to store the display bounds
 *
 * @return 0 on success; -1 if on failure (displayIndex is invalid)
 */
extern DECLSPEC int SDL_GetDisplaySafeBounds(int displayIndex, SDL_Rect *rect);

/**
 * Returns 1 if this device has a notch, 0 otherwise
 *
 * Notched devices are edgeless smartphones or tablets that include at dedicated area in 
 * the screen for a camera. Examples include modern iPhones
 *
 * If a device is notched you should call {@link SDL_GetDisplaySafeBounds()} before 
 * laying out UI elements. It is acceptable to animate and draw backgrounds behind the 
 * notch, but it is not acceptable to place UI elements outside of these bounds.
 * 
 * @param displayIndex  The display to query
 *
 * @return 1 if this device has a notch, 0 otherwise
 */
extern DECLSPEC int SDL_CheckDisplayNotch(int displayIndex);


/**
 * Returns the number of pixels for each point.
 *
 * A point is a logical screen pixel. If you are using a traditional display, points and 
 * pixels are the same. However, on Retina displays and other high dpi monitors, they may 
 * be different. In particular, the number of pixels per point is a scaling factor times 
 * the point.
 *
 * You should never need to use these scaling factor for anything, as it is not useful 
 * for determining anything other than whether a high DPI display is present. It does not 
 * necessarily refer to physical pixel on the screen. In some cases (macOS X Retina 
 * displays), it refers to the pixel density of the backing framebuffer, which may be 
 * different from the physical framebuffer.
 * 
 * @param displayIndex  The display to query
 *
 * @return the number of pixels for each point (-1 if displayIndex invalid)
 */
extern DECLSPEC float SDL_GetDisplayPixelDensity(int displayIndex);

/**
 * Returns the current device orientation.
 *
 * The device orientation is the orientation of a mobile device, as held by the user.
 * This is not necessarily the same as the display orientation (as returned by 
 * {@link SDL_GetDisplayOrientation}, as some applications may have locked their display
 * into a fixed orientation. Indeed, it is generally a bad idea to let an OpenGL context
 * auto-rotate when the device orientation changes.
 *
 * The purpose of this function is to use device orientation as a (discrete) control
 * input while still permitting the OpenGL context to be locked.
 *
 * If this display is not a mobile device, this function will always return
 * {@link SDL_ORIENTATION_UNKNOWN}.
 * 
 * @param displayIndex  The display to query
 *
 * @return the current device orientation.
 */
extern DECLSPEC SDL_DisplayOrientation SDL_GetDeviceOrientation(int displayIndex);

/**
 * Returns the default orientation of this device.
 *
 * The default orientation corresponds to the intended orientiation that this mobile 
 * device should be held. For devices with home buttons, this home button is always 
 * expected at the bottom. For the vast majority of devices, this means the intended 
 * orientation is Portrait. However, some Samsung tablets have the home button
 * oriented for Landscape.
 *
 * This function is important because the accelerometer axis is oriented relative to the 
 * default orientation. So a default landscape device will have a different accelerometer 
 * orientation than a portrait device.
 *
 * If this display is not a mobile device, this function will always return
 * {@link SDL_ORIENTATION_UNKNOWN}.
 * 
 * @param displayIndex  The display to query
 *
 * @return the default orientation of this device.
 */
extern DECLSPEC SDL_DisplayOrientation SDL_GetDefaultOrientation(int displayIndex);

/* Ends C function definitions when using C++ */
#ifdef __cplusplus
}
#endif
#include "close_code.h"

#endif /* SDL_misc_h_ */

/* vi: set ts=4 sw=4 expandtab: */
