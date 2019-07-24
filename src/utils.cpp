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

#include "utils.h"
#include "smplayer/extensions.h"

#include <QGraphicsOpacityEffect>
#include <QFileInfo>
#include <QDir>
#include <QProcess>
#include <QtDBus>
#include <QDebug>

Utils::PlayerId Utils::player(const QString & player_bin) {
    PlayerId p;
    if (player_bin.toLower().contains("mplayer")) {
        p = MPLAYER;
    }
    else {
        p = MPV;
    }

    return p;
}

void Utils::setWidgetOpacity(QWidget *w, bool transparent, const float &opacity)
{
    if (transparent) {
        w->setAttribute(Qt::WA_TranslucentBackground);
        w->setWindowFlags(w->windowFlags() | Qt::FramelessWindowHint | Qt::X11BypassWindowManagerHint);
        QGraphicsOpacityEffect* opacityEffect = new QGraphicsOpacityEffect;
        w->setGraphicsEffect(opacityEffect);
        opacityEffect->setOpacity(opacity);
    }
    else {
        w->setAttribute(Qt::WA_TranslucentBackground, false);
        w->setWindowFlags(w->windowFlags() & ~Qt::FramelessWindowHint);
    }
}

QString Utils::formatTime(int secs)
{
    bool negative = (secs < 0);
    secs = qAbs(secs);

    int t = secs;
    int hours = (int) t / 3600;
    t -= hours*3600;
    int minutes = (int) t / 60;
    t -= minutes*60;
    int seconds = t;

    return QString("%1%2:%3:%4").arg(negative ? "-" : "").arg(hours, 2, 10, QChar('0')).arg(minutes, 2, 10, QChar('0')).arg(seconds, 2, 10, QChar('0'));
}

QString Utils::timeForJumps(int secs)
{
    int minutes = (int) secs / 60;
    int seconds = secs % 60;

    if (minutes == 0) {
        return QObject::tr("%n second(s)", "", seconds);
    }
    else {
        if (seconds == 0)
            return QObject::tr("%n minute(s)", "", minutes);
        else {
            QString m = QObject::tr("%n minute(s)", "", minutes);
            QString s = QObject::tr("%n second(s)", "", seconds);
            return QObject::tr("%1 and %2").arg(m).arg(s);
        }
    }
}

bool Utils::directoryContainsDVD(QString directory)
{
    QDir dir(directory);
    QStringList l = dir.entryList();
    bool valid = false;
    for (int n=0; n < l.count(); n++) {
        //qDebug("  * entry %d: '%s'", n, l[n].toUtf8().data());
        if (l[n].toLower() == "video_ts") valid = true;
    }

    return valid;
}

QStringList Utils::searchForConsecutiveFiles(const QString & initial_file)
{
    qDebug("Utils::searchForConsecutiveFiles: initial_file: '%s'", initial_file.toUtf8().constData());

    QStringList files_to_add;
    QStringList matching_files;

    QFileInfo fi(initial_file);
    QString basename = fi.completeBaseName();
    QString extension = fi.suffix();
    QString path = fi.absolutePath();

    QDir dir(path);
    dir.setFilter(QDir::Files);
    dir.setSorting(QDir::Name);

    QRegExp rx("(\\d+)");

    int digits;
    int current_number;
    int pos = 0;
    QString next_name;
    bool next_found = false;
    qDebug("Utils::searchForConsecutiveFiles: trying to find consecutive files");
    while  ( ( pos = rx.indexIn(basename, pos) ) != -1 ) {
        qDebug("Utils::searchForConsecutiveFiles: captured: %s",rx.cap(1).toUtf8().constData());
        digits = rx.cap(1).length();
        current_number = rx.cap(1).toInt() + 1;
        next_name = basename.left(pos) + QString("%1").arg(current_number, digits, 10, QLatin1Char('0'));
        next_name.replace(QRegExp("([\\[\\]?*])"), "[\\1]");
        next_name += "*." + extension;
        qDebug("Utils::searchForConsecutiveFiles: next name = %s",next_name.toUtf8().constData());
        matching_files = dir.entryList((QStringList)next_name);

        if ( !matching_files.isEmpty() ) {
            next_found = true;
            break;
        }
        qDebug("Utils::searchForConsecutiveFiles: pos = %d",pos);
        pos  += digits;
    }

    if (next_found) {
        while (!matching_files.isEmpty()) {
            QString filename = path  + "/" + matching_files[0];
            files_to_add << filename;
            current_number++;
            next_name = basename.left(pos) + QString("%1").arg(current_number, digits, 10, QLatin1Char('0'));
            next_name.replace(QRegExp("([\\[\\]?*])"), "[\\1]");
            next_name += "*." + extension;
            matching_files = dir.entryList((QStringList)next_name);
        }
    }

    return files_to_add;
}

QStringList Utils::filesInDirectory(const QString & initial_file, const QStringList & filter)
{
    qDebug("Utils::filesInDirectory: initial_file: %s", initial_file.toUtf8().constData());
    //qDebug() << "Utils::filesInDirectory: filter:" << filter;

    QFileInfo fi(initial_file);
    QString current_file = fi.fileName();
    QString path = fi.absolutePath();

    QDir d(path);
    QStringList all_files = d.entryList(filter, QDir::Files);

    QStringList r;
    for (int n = 0; n < all_files.count(); n++) {
        //if (all_files[n] != current_file) {
            QString s = path +"/" + all_files[n];
            r << s;
        //}
    }

    return r;
}

QStringList Utils::filesForPlaylist(const QString & initial_file, Preferences::AutoAddToPlaylistFilter filter)
{
    QStringList res;

    if (filter == Preferences::ConsecutiveFiles) {
        res = searchForConsecutiveFiles(initial_file);
    } else {
        Extensions e;
        QStringList exts;
        switch (filter) {
            case Preferences::VideoFiles: exts = e.video().forDirFilter(); break;
            case Preferences::AudioFiles: exts = e.audio().forDirFilter(); break;
            case Preferences::MultimediaFiles: exts = e.multimedia().forDirFilter(); break;
            default: ;
        }
        if (!exts.isEmpty()) res = Utils::filesInDirectory(initial_file, exts);
    }

    return res;
}

QString Utils::findExecutable(const QString & name)
{
    QByteArray env = qgetenv("PATH");
    QStringList search_paths = QString::fromLocal8Bit(env.constData()).split(':', QString::SkipEmptyParts);
    for (int n = 0; n < search_paths.count(); n++) {
        QString candidate = search_paths[n] + "/" + name;
        //qDebug("Utils::findExecutable: candidate: %s", candidate.toUtf8().constData());
        QFileInfo info(candidate);
        if (info.isFile() && info.isExecutable()) {
            //qDebug("Utils::findExecutable: executable found: %s", candidate.toUtf8().constData());
            return candidate;
        }
    }
    return QString::null;
}

void Utils::shutdown()
{
    bool works = false;

    QDBusMessage response;

    QDBusInterface gnomeSessionManager("org.gnome.SessionManager", "/org/gnome/SessionManager", "org.gnome.SessionManager", QDBusConnection::sessionBus());
    response = gnomeSessionManager.call("RequestShutdown");
    if (response.type() == QDBusMessage::ErrorMessage) {
        qDebug() << "Shutdown::shutdown: error:" << response.errorName() << ":" << response.errorMessage();
    } else {
        works = true;
    }

    if (!works) {
        QDBusInterface kdeSessionManager("org.kde.ksmserver", "/KSMServer", "org.kde.KSMServerInterface", QDBusConnection::sessionBus());
        response = kdeSessionManager.call("logout", 0, 2, 2);
        if (response.type() == QDBusMessage::ErrorMessage) {
            qDebug() << "Shutdown::shutdown: error:" << response.errorName() << ":" << response.errorMessage();
        } else {
            works = true;
        }
    }

    if (!works) {
        QDBusInterface powermanagement("org.freedesktop.Hal", "/org/freedesktop/Hal/devices/computer",
            "org.freedesktop.Hal.Device.SystemPowerManagement", QDBusConnection::systemBus());
        response = powermanagement.call("Shutdown");
        if (response.type() == QDBusMessage::ErrorMessage) {
            qDebug() << "Shutdown::shutdown: error:" << response.errorName() << ":" << response.errorMessage();
        } else {
            works = true;
        }
    }

    if (!works) {
        QDBusInterface powermanagement("org.freedesktop.ConsoleKit", "/org/freedesktop/ConsoleKit/Manager", "org.freedesktop.ConsoleKit.Manager",
            QDBusConnection::systemBus());
        response = powermanagement.call("Stop");
        if (response.type() == QDBusMessage::ErrorMessage) {
            qDebug() << "Shutdown::shutdown: error:" << response.errorName() << ":" << response.errorMessage();
        } else {
            works = true;
        }
    }

    if (!works) {
        qDebug("Shutdown::shutdown: shutdown failed");

        QProcess::startDetached("xmessage", QStringList() << "-buttons" << "Accept:0" << "-center" <<
            "This is a message from Kylin Video\n"
            "The computer should shut down now.\n"
            "However shutdown failed.");
    }
}
