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

#include "controller.h"
#include "application.h"
#include "signalmanager.h"

#include <QDebug>
#include <QCoreApplication>
#include <QtDBus>

#include <pthread.h>
#include <stdio.h>

#define CONTROL_DBUS_SERVICE_NAME  "com.kylin.kylinvideo.controller"


static void* request_controller_thread(void* arg)
{
    //开始循环，接收用户输入选择，并响应回复信息
    char buff[256];
    while (fgets(buff, sizeof(buff), stdin) != NULL)
    {
        int iCmdType = 0;
        if(sscanf(buff, "%d", &iCmdType) == -1) {
            fprintf(stderr, "Input error: %s(errno: %d) And please input again\n", strerror(errno), errno);
            continue;
        }
        int op = 0;

        switch(iCmdType)
        {
            case 0:
                printf("request for thread to exit!\n");
                op = 1;
                break;
            case 1:
                emit mycontroller->signalM->testControl("forward");
                break;
            case 2:
                emit mycontroller->signalM->testControl("rewind");
                break;
            case 3:
                emit mycontroller->signalM->testControl("playPause");
                break;
            case 4:
                emit mycontroller->signalM->testControl("stop");
                break;
            case 5:
                emit mycontroller->signalM->testControl("quit");
                break;
            default:
                printf("unrecognized command!\n");
                break;
        }

        if(op == 1)
            break;
    }

    return (void*)0;
}

Controller::Controller(QObject *parent)
    : QObject(parent)
    , m_iface(0)
{

    qDebug() << "test cmd: 1 -> forward; 2 -> rewind; 3 -> playPause; 4 -> stop; 5 -> quit;";
    pthread_t tid;
    pthread_create(&tid, NULL, request_controller_thread, NULL);
    QObject::connect(mycontroller->signalM, &SignalManager::testControl, this, [=] (const QString &cmd) {
        if (cmd == "forward") {
            this->seek_forward(10);
        }
        else if (cmd == "rewind") {
            this->seek_rewind(10);
        }
        else if (cmd == "playPause") {
            this->playPause();
        }
        else if (cmd == "stop") {
            this->stop();
        }
        else if (cmd == "quit") {
            this->quit();
        }
    });


#if QT_VERSION >= 0x050000
    m_iface = new QDBusInterface(CONTROL_DBUS_SERVICE_NAME, "/", CONTROL_DBUS_SERVICE_NAME, QDBusConnection::sessionBus(), this);
    //m_iface = new QDBusInterface(CONTROL_DBUS_SERVICE_NAME, "/com/kylin/kylinvideo/controller", CONTROL_DBUS_SERVICE_NAME, QDBusConnection::sessionBus(), this);
    if (!m_iface->isValid()) {
        fprintf(stderr, "%s\n",
                qPrintable(QDBusConnection::sessionBus().lastError().message()));
        QCoreApplication::instance()->quit();
    }
#endif
}

Controller::~Controller()
{
    if (m_iface) {
        delete m_iface;
        m_iface = 0;
    }
}

void Controller::start(const QString &name, const QString &oldValue, const QString &newValue)
{
    Q_UNUSED(oldValue);

    if (name != CONTROL_DBUS_SERVICE_NAME || newValue.isEmpty())
        return;

    if (m_iface) {
        delete m_iface;
        m_iface = 0;
    }
    m_iface = new QDBusInterface(CONTROL_DBUS_SERVICE_NAME, "/", CONTROL_DBUS_SERVICE_NAME, QDBusConnection::sessionBus(), this);
    if (!m_iface->isValid()) {
        fprintf(stderr, "%s\n",
                qPrintable(QDBusConnection::sessionBus().lastError().message()));
        QCoreApplication::instance()->quit();
    }

    //QDBusConnection::sessionBus().connect(CONTROL_DBUS_SERVICE_NAME, QString("/"), CONTROL_DBUS_SERVICE_NAME, QString("aboutToQuit"), this, SLOT(quit(QString)));
    connect(m_iface, SIGNAL(aboutToQuit()), QCoreApplication::instance(), SLOT(quit()));
}

void Controller::playPause()
{
    m_iface->call("playPause");
}

bool Controller::seek_forward(int seconds)
{
    QDBusReply<QDBusVariant> reply = m_iface->call("seek_forward", seconds);
    if (reply.isValid()) {
        qDebug() << "Reply was:" << reply.value().variant().toBool();
        return reply.value().variant().toBool();
    }
    else {
        if (m_iface->lastError().isValid())
            fprintf(stderr, "Call failed: %s\n", qPrintable(m_iface->lastError().message()));
    }

    return false;
}

bool Controller::seek_rewind(int seconds)
{
    QDBusReply<QDBusVariant> reply = m_iface->call("seek_rewind", seconds);
    if (reply.isValid()) {
        qDebug() << "Reply was:" << reply.value().variant().toBool();
        return reply.value().variant().toBool();
    }
    else {
        if (m_iface->lastError().isValid())
            fprintf(stderr, "Call failed: %s\n", qPrintable(m_iface->lastError().message()));
    }

    return false;
}

void Controller::stop()
{
    m_iface->call("stop");
}

void Controller::quit()
{
    m_iface->call("quit");
}
