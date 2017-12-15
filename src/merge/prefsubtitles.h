/*  smplayer, GUI front-end for mplayer.
    Copyright (C) 2006-2015 Ricardo Villalba <rvm@users.sourceforge.net>
    Copyright (C) 2013 ~ 2017 National University of Defense Technology(NUDT) & Tianjin Kylin Ltd.

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
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef _PREFSUBTITLES_H_
#define _PREFSUBTITLES_H_

#include "ui_prefsubtitles.h"
#include "../smplayer/prefwidget.h"

class Preferences;
class Encodings;

class PrefSubtitles : public PrefWidget, public Ui::PrefSubtitles
{
	Q_OBJECT

public:
	PrefSubtitles( QWidget * parent = 0, Qt::WindowFlags f = 0 );
	~PrefSubtitles();

	virtual QString sectionName();
	virtual QPixmap sectionIcon();

    // Pass data to the dialog
    void setData(Preferences * pref);

    // Apply changes
    void getData(Preferences * pref);

public slots:
    void setCurrentPage(int index);

protected:
	virtual void createHelp();

	void setFontEncoding(QString s);
	QString fontEncoding();

	void setUseEnca(bool v);
	bool useEnca();

	void setEncaLang(QString s);
	QString encaLang();

	void setFreetypeSupport(bool b);
	bool freetypeSupport();

protected:
	virtual void retranslateStrings();

private:
	Encodings * encodings;
	QString forced_ass_style;
};

#endif
