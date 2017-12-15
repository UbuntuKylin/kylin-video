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

#ifndef _PREFVIDEO_H_
#define _PREFVIDEO_H_

#include "ui_prefvideo.h"
#include "../smplayer/prefwidget.h"
#include "../smplayer/inforeader.h"
#include "../smplayer/deviceinfo.h"
#include "../smplayer/preferences.h"

class PrefVideo : public PrefWidget, public Ui::PrefVideo
{
	Q_OBJECT

public:
    PrefVideo(QString arch_type = "", QWidget * parent = 0, Qt::WindowFlags f = 0 );
    ~PrefVideo();

	// Pass data to the dialog
	void setData(Preferences * pref);

	// Apply changes
	void getData(Preferences * pref);

    bool fileSettingsMethodChanged() { return filesettings_method_changed; };

protected:
	virtual void createHelp();


	void setVO( QString vo_driver );
	QString VO();

	// Tab video and audio
	void setEq2(bool b);
	bool eq2();

    void setInitialPostprocessing(bool b);
    bool initialPostprocessing();

	void setDirectRendering(bool b);
	bool directRendering();

	void setDoubleBuffer(bool b);
	bool doubleBuffer();

	void setUseSlices(bool b);
	bool useSlices();

	void setAmplification(int n);
	int amplification();

	void setAudioChannels(int ID);
	int audioChannels();

protected slots:
	void vo_combo_changed(int);

public slots:
    void update_driver_combobox();

protected:
	virtual void retranslateStrings();
	void updateDriverCombos();

	InfoList vo_list;
	DeviceList alsa_devices;
	DeviceList xv_adaptors;

private:
	bool filesettings_method_changed;
    QString arch;
};

#endif
