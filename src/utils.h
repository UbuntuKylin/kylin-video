/*
 * Copyright (C) 2013 ~ 2019 National University of Defense Technology(NUDT) & Kylin Ltd.
 *
 * Authors:
 *  Kobe Lee    xiangli@ubuntukylin.com/kobe24_lixiang@126.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef UTILS_H
#define UTILS_H

#include <Qt>
#include <QWidget>
#include <QString>
#include <QStringList>
#include "smplayer/preferences.h"

#define PLAYER_NAME "MPlayer/mpv"

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

enum DragState {NOT_DRAGGING, START_DRAGGING, DRAGGING};

class Utils {
public:
    enum PlayerId {
        MPLAYER = 0,
        MPV = 1
    };

    static PlayerId player(const QString &player_bin);
    static void setWidgetOpacity(QWidget *w, bool transparent, const float &opacity=0.7);

    // Format a time (hh:mm:ss)
    static QString formatTime(int secs);
    static QString timeForJumps(int secs);
    static bool directoryContainsDVD(QString directory);
    static QStringList filesForPlaylist(const QString & initial_file, Preferences::AutoAddToPlaylistFilter filter);
    static QString findExecutable(const QString & name);

    static void shutdown();

private:
    static QStringList searchForConsecutiveFiles(const QString & initial_file);
    static QStringList filesInDirectory(const QString & initial_file, const QStringList & filter);
};

#endif // UTILS_H
