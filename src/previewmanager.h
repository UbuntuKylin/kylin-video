/*
 * Copyright (C) 2013 ~ 2020 National University of Defense Technology(NUDT) & Tianjin Kylin Ltd.
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

#ifndef PREVIEW_MANAGER_H
#define PREVIEW_MANAGER_H

#include <QtCore>
#include <QtGui>
#include <QDebug>
#include <QPointer>

#include "previewprocess.h"

class PreviewManager : public QObject
{
 Q_OBJECT

public:
    PreviewManager(QObject *parent = 0);
   ~PreviewManager();

    void setVideoWinid(long int id) { m_videoWinid = id; }

    QStringList m_arguments;
    QString m_path;
    PreviewProcess *m_process;

signals:
    void requestPausePreview();
    void processFinished(int rc);

public slots:
    void startMplayer();
    void play(QString filePath, float duration);
    void pause();
    void stop();
    void seek(int seconds);
    void playerConsoleInfo(QByteArray ba);

private:
    QString parsevalue(QString serstr,QString sep, QString str);
    bool m_started;
    float m_duration;
    long int m_videoWinid;
};

#endif // PREVIEW_MANAGER_H
