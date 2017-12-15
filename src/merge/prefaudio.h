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

#ifndef _PREFAUDIO_H_
#define _PREFAUDIO_H_

#include "ui_prefaudio.h"
#include "../smplayer/prefwidget.h"
#include "../smplayer/inforeader.h"
#include "../smplayer/deviceinfo.h"
#include "../smplayer/preferences.h"

class PrefAudio : public PrefWidget, public Ui::PrefAudio
{
	Q_OBJECT

public:
    PrefAudio( QWidget * parent = 0, Qt::WindowFlags f = 0 );
    ~PrefAudio();

	// Pass data to the dialog
	void setData(Preferences * pref);

	// Apply changes
	void getData(Preferences * pref);

    bool fileSettingsMethodChanged() { return filesettings_method_changed; };

protected:
	virtual void createHelp();
	void setAO( QString ao_driver );
	QString AO();

	void setGlobalVolume(bool b);
	bool globalVolume();

    void setAutoSyncFactor(int factor);
    int autoSyncFactor();

    void setAutoSyncActivated(bool b);
    bool autoSyncActivated();

	void setMc(double value);
	double mc();

	void setMcActivated(bool b);
	bool mcActivated();

	void setSoftVol(bool b);
	bool softVol();

	void setInitialVolNorm(bool b);
	bool initialVolNorm();

	void setAmplification(int n);
	int amplification();

	void setAudioChannels(int ID);
	int audioChannels();

protected slots:
	void ao_combo_changed(int);

public slots:
    void update_driver_combobox();

protected:
	virtual void retranslateStrings();
	void updateDriverCombos();

	InfoList ao_list;

	DeviceList alsa_devices;
	DeviceList xv_adaptors;

private:
	bool filesettings_method_changed;
};

#endif
