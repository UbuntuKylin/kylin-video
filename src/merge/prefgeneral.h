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

#ifndef _PREFGENERAL_H_
#define _PREFGENERAL_H_

#include "ui_prefgeneral.h"
#include "../smplayer/prefwidget.h"
#include "../smplayer/inforeader.h"
#include "../smplayer/deviceinfo.h"
#include "../smplayer/preferences.h"

class PrefGeneral : public PrefWidget, public Ui::PrefGeneral
{
	Q_OBJECT

public:
	PrefGeneral( QWidget * parent = 0, Qt::WindowFlags f = 0 );
	~PrefGeneral();

	// Pass data to the dialog
	void setData(Preferences * pref);

	// Apply changes
	void getData(Preferences * pref);

    bool fileSettingsMethodChanged() { return filesettings_method_changed; };

public slots:
    void selectRadioButton();

protected:
	virtual void createHelp();

	// Tab General
    void setMplayerPath(QString path);
	QString mplayerPath();

    void setPauseWhenHidden(bool b);
    bool pauseWhenHidden();

    void setPreviewWhenPlaying(bool b);
    bool previewWhenPlaying();

signals:
    void ready_to_update_driver();

protected:
	virtual void retranslateStrings();


private:
	bool filesettings_method_changed;
};

#endif
