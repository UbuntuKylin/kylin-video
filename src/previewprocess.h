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

#ifndef _PREVIEW_PROCESS_H_
#define _PREVIEW_PROCESS_H_

#include <QProcess>

class PreviewProcess : public QProcess
{
    Q_OBJECT

public:
    PreviewProcess(QObject * parent = 0);

    void addArgument(const QString &arg);
    void clearArguments();
    void start();

signals:
    void lineAvailable(QByteArray ba);

protected slots:
    void procFinished();

protected:
    int canReadLine(const QByteArray &ba, int from = 0);

private:
    QString m_program;
    QStringList m_arg;
    QByteArray m_remainingOutput;
};

#endif // _PREVIEW_PROCESS_H_
