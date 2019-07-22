/*  smplayer, GUI front-end for mplayer.
    Copyright (C) 2006-2015 Ricardo Villalba <rvm@users.sourceforge.net>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <Qt>

#define TOP_TOOLBAR_HEIGHT 39
#define BOTTOM_TOOLBAR_HEIGHT 79
#define ANIMATIONDELAY 400
#define WINDOW_MIN_WIDTH 400
#define WINDOW_MIN_HEIGHT 300


// ENABLE_DELAYED_DRAGGING
// if 1, sends the dragging position of the time slider
// some ms later

#define ENABLE_DELAYED_DRAGGING 1


// if SEEKBAR_RESOLUTION is defined, it specified the
// maximum value of the time slider
#define SEEKBAR_RESOLUTION 1000


// DVDNAV_SUPPORT
// if 1, smplayer will be compiled with support for mplayer's dvdnav
//#ifdef MPLAYER_SUPPORT
//#define DVDNAV_SUPPORT 0
//#endif

#endif
