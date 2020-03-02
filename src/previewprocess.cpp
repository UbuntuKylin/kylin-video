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

#include "previewprocess.h"
#include <QDebug>

PreviewProcess::PreviewProcess(QObject * parent) : QProcess(parent)
{
    clearArguments();
    setProcessChannelMode(QProcess::MergedChannels);
    connect(this, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(procFinished()));
}

void PreviewProcess::clearArguments()
{
    m_program = "";
    m_arg.clear();
}

void PreviewProcess::addArgument(const QString &a)
{
    if (m_program.isEmpty()) {
        m_program = a;
    }
    else {
        m_arg.append(a);
    }
}

void PreviewProcess::start()
{
    m_remainingOutput.clear();
    QProcess::start(m_program, m_arg);
}

int PreviewProcess::canReadLine(const QByteArray &ba, int from)
{
    int pos1 = ba.indexOf('\n', from);
    int pos2 = ba.indexOf('\r', from);

    if ((pos1 == -1) && (pos2 == -1)) {
        return -1;
    }

    int pos = pos1;
    if ((pos1 != -1) && (pos2 != -1)) {
        if (pos1 < pos2) {
            pos = pos1;
        }
        else {
            pos = pos2;
        }
    }
    else {
        if (pos1 == -1) {
            pos = pos2;
        }
        else {
            if (pos2 == -1) {
                pos = pos1;
            }
        }
    }

    return pos;
}

void PreviewProcess::procFinished()
{
    if (bytesAvailable() > 0 ) {
        QByteArray buffer = readAllStandardOutput();
        QByteArray ba = m_remainingOutput + buffer;
        int start = 0;
        int from_pos = 0;
        int pos = canReadLine(ba, from_pos);

        while (pos > -1) {
            QByteArray line = ba.mid(start, pos-start);
            from_pos = pos + 1;
            start = from_pos;
            emit lineAvailable(line);
            pos = canReadLine(ba, from_pos);
        }
        m_remainingOutput = ba.mid(from_pos);
    }
}
