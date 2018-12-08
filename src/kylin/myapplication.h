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

#ifndef MYAPPLICATION_H
#define MYAPPLICATION_H

#include <QtGlobal>

#ifdef SINGLE_INSTANCE
#include "QtSingleApplication"

class MyApplication : public QtSingleApplication
{
	Q_OBJECT

public:
	MyApplication ( const QString & appId, int & argc, char ** argv ) 
		: QtSingleApplication(appId, argc, argv) {};

	virtual void commitData ( QSessionManager & /*manager*/ ) {
		// Nothing to do, let the application to close
	}

	inline static MyApplication * instance() {
		return qobject_cast<MyApplication*>(QApplication::instance());
	}
};

#else
#include <QApplication>

class MyApplication : public QApplication
{
	Q_OBJECT

public:
	MyApplication ( const QString & appId, int & argc, char ** argv ) : QApplication(argc, argv) {};

	virtual void commitData ( QSessionManager & /*manager*/ ) {
		// Nothing to do, let the application to close
	}
};

#endif

#endif

