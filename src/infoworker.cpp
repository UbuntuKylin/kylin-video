/*
 * Copyright (C) 2013 ~ 2019 National University of Defense Technology(NUDT) & Tianjin Kylin Ltd.
 *
 * Authors:
 *  Kobe Lee    lixiang@kylinos.cn/kobe24_lixiang@126.com
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

#include "infoworker.h"
#include "../smplayer/preferences.h"
#include "../smplayer/global.h"

#include <QDebug>
#include <QThread>
#include <unistd.h>

using namespace Global;

#include "../smplayer/infoprovider.h"

InfoWorker::InfoWorker(QObject *parent)
    :QObject(parent)
{
    //qDebug()<<"InfoWorker init"<<"线程id:" << QThread::currentThreadId();
}

InfoWorker::~InfoWorker()
{

}

void InfoWorker::onGetMediaInfo(const QStringList &filepaths)
{
    bool fileInPlaylist = false;
    VideoPtrList medialist;
    for (int n = 0; n < filepaths.count(); n++) {
        QFileInfo fileInfo(filepaths[n]);
        if (fileInfo.isDir()) {
            continue;
        }

        if (pref->m_videoMap.contains(filepaths[n])) {
            fileInPlaylist = true;
            continue;
        }

        // lc add 20200831 如果是网络视频流添加視頻信息
        if (filepaths[0].indexOf(QRegExp("^.*://.*")) == 0) {
            MediaData data = InfoProvider::getInfo(filepaths[n]);
            QString name = "";
            double duration = data.duration;
            name = filepaths[n];
            auto video = pref->generateVedioData(filepaths[n], name, duration);
            if (video.isNull()) {
                continue;
            }
            medialist << video;
        }
        // lc end

        if ((QFile::exists(filepaths[n]))) {
            MediaData data = InfoProvider::getInfo(filepaths[n]);
            QString name = "";
            double duration = 0;
            name = data.displayName();
            if (name.isEmpty()) {
                QFileInfo fi(filepaths[n]);
                // Let's see if it looks like a file (no dvd://1 or something)
                if (filepaths[n].indexOf(QRegExp("^.*://.*")) == -1) {
                    // Local file
                    name = fi.fileName();
                } else {
                    // Stream
                    name = filepaths[n];
                }
            }
            duration = data.duration;
            auto video = pref->generateVedioData(filepaths[n], name, duration);
            if (video.isNull()) {
                continue;
            }
            medialist << video;
        }
    }

    emit meidaFilesAdded(medialist, fileInPlaylist);
    medialist.clear();
}
