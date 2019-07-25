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

#ifndef APPLICATION_H
#define APPLICATION_H

#include <QCoreApplication>
#include <QDebug>

class SignalManager;

#if defined(mycontroller)
#undef mycontroller
#endif
#define mycontroller (static_cast<Application *>(QCoreApplication::instance()))

class Application : public QCoreApplication {
    Q_OBJECT

public:
    Application(int& argc, char** argv);
    SignalManager *signalM = nullptr;
};

#endif  // APPLICATION_H
