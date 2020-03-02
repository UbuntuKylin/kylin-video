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

#include "previewmanager.h"

PreviewManager::PreviewManager(QObject *parent)
{
#ifdef Q_OS_LINUX
    m_path="/usr/bin/mplayer";
# endif

    m_process = new PreviewProcess(parent);
    QObject::connect(m_process, SIGNAL(lineAvailable(QByteArray)), this, SLOT(playerConsoleInfo(QByteArray)));
    QObject::connect(m_process, SIGNAL(finished(int)), this, SIGNAL(processFinished(int)));

    m_started = false;
}

PreviewManager::~PreviewManager()
{
    m_process->write("quit\n");
    m_process->close();
    m_process->kill();
    delete m_process;
}

void PreviewManager::play(QString filePath, float duration)
{
    m_duration = duration;
    m_arguments << "-slave" << "-identify" << "-noquiet" << "-stop-xscreensaver";
    m_arguments << "-nodr"
                << "-double"
                << "-nocolorkey"
                << "-wid" << QString::number(m_videoWinid)
                << "-nokeepaspect";
    m_arguments << filePath;
    emit requestPausePreview();//停止预览
    startMplayer();//重启
}

QString PreviewManager::parsevalue(QString serstr, QString sep, QString str)
{
    QString tmp;
    tmp = str.mid(str.indexOf(serstr));
    tmp = tmp.left(tmp.indexOf("\n"));

    return tmp.mid(tmp.indexOf(sep)+1).trimmed();
}

void PreviewManager::seek(int seconds)
{
    double pos = (double) seconds / 1.0;
    QString cmd =QString("pausing_keep seek " + QString::number(pos) + " 2\n");
    m_process->write(cmd.toStdString().c_str());
}

void PreviewManager::pause()
{
    QString cmd = QString("pause\n");
    m_process->write(cmd.toStdString().c_str());
}

void PreviewManager::stop()
{
    m_process->write(QString("quit\n").toStdString().c_str());
}

void  PreviewManager::startMplayer()
{
    QString strArgument;
    m_process->clearArguments();
    m_process->addArgument(m_path);

    foreach (strArgument, m_arguments) {
        m_process->addArgument(strArgument);
    }

    m_process->start();

    QString cmd = "osd "+ QString::number(0) + "\n";
    m_process->write(cmd.toStdString().c_str()) ;
}

void PreviewManager::playerConsoleInfo(QByteArray ba)
{
    QString outInfo(ba);

    if (!m_started) {
        if (outInfo.contains("ID_LENGTH", Qt::CaseInsensitive)){
            QString value = parsevalue("ID_LENGTH=", "=", outInfo);
            m_duration = value.toFloat();
        }

        if (outInfo.contains("Starting playback", Qt::CaseInsensitive)){
            m_started = true;
        }
    }
}
