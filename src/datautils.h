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

#ifndef DATA_UTILS_H
#define DATA_UTILS_H

#include <QObject>
#include <QString>
#include <QSharedPointer>
#include <QDataStream>

class VideoData
{
public:
    QString m_localpath;
    QString m_name;
    double m_duration;
//    bool m_active;

    void setLocalpath(QString localpath) { m_localpath = localpath; };
    void setName(QString name) { m_name = name; };
    void setDuration(double duration) { m_duration = duration; };

    QString localpath() const { return m_localpath; };
    QString name() const { return m_name; };
    double duration() const { return m_duration; };

//public:
//    void setActive(bool b) { active = b; }
};

//inline bool operator ==(const VideoData &data, const VideoData &other) {
//    return data.localpath == other.localpath;
//}

typedef QSharedPointer<VideoData> VideoPtr;
typedef QList<VideoPtr> VideoPtrList;

Q_DECLARE_METATYPE(VideoData)
Q_DECLARE_METATYPE(VideoPtr)
Q_DECLARE_METATYPE(VideoPtrList)


#endif // DATA_UTILS_H
