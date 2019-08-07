/*  smplayer, GUI front-end for mplayer.
    Copyright (C) 2006-2015 Ricardo Villalba <rvm@users.sourceforge.net>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef _MY_PROCESS_H_
#define _MY_PROCESS_H_

#include <QProcess>
#include <QTemporaryFile>
#include <QTimer>

//! MyProcess is a specialized QProcess designed to properly work with mplayer.

class MyProcess : public QProcess
{
	Q_OBJECT

public:
	MyProcess ( QObject * parent = 0 );

	virtual void setExecutable(const QString & p) { program = p; };
	QString executable() { return program; };

	virtual void addArgument(const QString & a); 	//!< Add an argument

	void clearArguments(); 		//!< Clear the list of arguments
	QStringList arguments(); 	//!< Return the list of arguments

	void start();				//!< Start the process
	bool isRunning();			//!< Return true if the process is running

	static QStringList splitArguments(const QString & args);

signals:
	//! Emitted when there's a line available
	void lineAvailable(QByteArray ba);

protected slots:
	void readStdOut();			//!< Called for reading from standard output
//	void readTmpFile();			//!< Called for reading from the temp file
	void procFinished();		//!< Called when the process has finished

protected:
	//! Return true if it's possible to read an entire line.
	/*! @param from specifies the position to begin. */
	int canReadLine(const QByteArray & ba, int from = 0);
	//! Called from readStdOut() and readTmpFile() to do all the work
	void genericRead(QByteArray buffer);

protected:
	QString program;
	QStringList arg;

	QByteArray remaining_output;

//	QTemporaryFile temp_file;
	QTimer timer;
};

#endif
