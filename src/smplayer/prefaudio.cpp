/*  smplayer, GUI front-end for mplayer.
    Copyright (C) 2006-2015 Ricardo Villalba <rvm@users.sourceforge.net>
    Copyright (C) 2013 ~ 2019 National University of Defense Technology(NUDT) & Tianjin Kylin Ltd.

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

#include "prefaudio.h"
#include "../smplayer/preferences.h"
#include "../smplayer/filedialog.h"
#include "../smplayer/images.h"
#include "../smplayer/mediasettings.h"
#include "../smplayer/paths.h"
#include "../utils.h"
#include <QDebug>
#include "../smplayer/deviceinfo.h"

PrefAudio::PrefAudio(QString snap, QWidget * parent, Qt::WindowFlags f)
	: PrefWidget(parent, f )
{
	setupUi(this);

    //add by zhaoyubiao for hide-volume-check
    global_volume_check->setVisible(false);
    softvol_check->setVisible(false);
    softvol_max_spin->setVisible(false);
    amplification_label->setVisible(false);
    //add end

    this->m_snap = snap;

	// Read driver info from InfoReader:
    InfoReader * i = InfoReader::obj(this->m_snap);//20181212
	i->getInfo();
	ao_list = i->aoList();

    pa_devices = DeviceInfo::paDevices();

	// Channels combo
    channels_combo->addItem( "0", MediaSettings::ChDefault );
	channels_combo->addItem( "2", MediaSettings::ChStereo );
	channels_combo->addItem( "4", MediaSettings::ChSurround );
	channels_combo->addItem( "6", MediaSettings::ChFull51 );
	channels_combo->addItem( "7", MediaSettings::ChFull61 );
	channels_combo->addItem( "8", MediaSettings::ChFull71 );
    connect(ao_combo, SIGNAL(currentIndexChanged(int)), this, SLOT(ao_combo_changed(int)));

	retranslateStrings();
}

PrefAudio::~PrefAudio()
{
}

void PrefAudio::retranslateStrings() {
	retranslateUi(this);

    global_volume_check->setFocusPolicy(Qt::NoFocus);
    softvol_check->setFocusPolicy(Qt::NoFocus);
    volnorm_check->setFocusPolicy(Qt::NoFocus);
    autosync_check->setFocusPolicy(Qt::NoFocus);

    groupBox_volume->setStyleSheet("QGroupBox{border:none;margin-top:20px;font-size:14px;}QGroupBox:title{subcontrol-origin: margin;subcontrol-position: top left;padding: 0px 1px;color: #999999;font-family: 方正黑体_GBK;font-weight:bold;}");
    groupBox_sync->setStyleSheet("QGroupBox{border:none;margin-top:20px;font-size:14px;}QGroupBox:title{subcontrol-origin: margin;subcontrol-position: top left;padding: 0px 1px;color: #999999;font-family: 方正黑体_GBK;font-weight:bold;}");

    ao_combo->setStyleSheet("QComboBox{width:150px;height:24px;border:1px solid #000000;background:#0f0f0f;font-size:12px;font-family:方正黑体_GBK;background-position:center left;padding-left:5px;color:#999999;selection-color:#ffffff;selection-background-color:#1f1f1f;}QComboBox::hover{background-color:#0f0f0f;border:1px solid #0a9ff5;font-family:方正黑体_GBK;font-size:12px;color:#999999;}QComboBox:!enabled {background:#0f0f0f;color:#383838;}QComboBox::drop-down {width:17px;border:none;background:transparent;}QComboBox::drop-down:hover {background:transparent;}QComboBox::down-arrow{image:url(:/res/combobox_arrow_normal.png);}QComboBox::down-arrow:hover{image:url(:/res/combobox_arrow_hover.png);}QComboBox::down-arrow:pressed{image:url(:/res/combobox_arrow_press.png);}QComboBox QAbstractItemView{border:1px solid #0a9ff5;background:#262626;outline:none;}");
    channels_combo->setStyleSheet("QComboBox{width:150px;height:24px;border:1px solid #000000;background:#0f0f0f;font-size:12px;font-family:方正黑体_GBK;background-position:center left;padding-left:5px;color:#999999;selection-color:#ffffff;selection-background-color:#1f1f1f;}QComboBox::hover{background-color:#0f0f0f;border:1px solid #0a9ff5;font-family:方正黑体_GBK;font-size:12px;color:#999999;}QComboBox:!enabled {background:#0f0f0f;color:#383838;}QComboBox::drop-down {width:17px;border:none;background:transparent;}QComboBox::drop-down:hover {background:transparent;}QComboBox::down-arrow{image:url(:/res/combobox_arrow_normal.png);}QComboBox::down-arrow:hover{image:url(:/res/combobox_arrow_hover.png);}QComboBox::down-arrow:pressed{image:url(:/res/combobox_arrow_press.png);}QComboBox QAbstractItemView{border:1px solid #0a9ff5;background:#262626;outline:none;}");
//    ao_combo->setStyleSheet("QComboBox{width:150px;height:24px;border: 1px solid #000000;background: #0f0f0f;font-family:方正黑体_GBK;font-size:12px;color:#999999;}QComboBox::hover{background-color:#0f0f0f;border:1px solid #0a9ff5;font-family:方正黑体_GBK;font-size:12px;color:#999999;}QComboBox:enabled {background:#0f0f0f;color:#999999;}QComboBox:!enabled {background:#0f0f0f;color:#383838;}QComboBox:enabled:hover, QComboBox:enabled:focus {color: #1f1f1f;}QComboBox::drop-down {width: 17px;border: none;background: transparent;}QComboBox::drop-down:hover {background: transparent;}QComboBox::down-arrow{image:url(:/res/combobox_arrow_normal.png);}QComboBox::down-arrow:hover{image:url(:/res/combobox_arrow_hover.png);}QComboBox::down-arrow:pressed{image:url(:/res/combobox_arrow_press.png);}QComboBox QAbstractItemView {border: 1px solid #0a9ff5;background: #1f1f1f;outline: none;}");
//    channels_combo->setStyleSheet("QComboBox{width:150px;height:24px;border: 1px solid #000000;background: #0f0f0f;font-family:方正黑体_GBK;font-size:12px;color:#999999;}QComboBox::hover{background-color:#0f0f0f;border:1px solid #0a9ff5;font-family:方正黑体_GBK;font-size:12px;color:#999999;}QComboBox:enabled {background:#0f0f0f;color:#999999;}QComboBox:!enabled {background:#0f0f0f;color:#383838;}QComboBox:enabled:hover, QComboBox:enabled:focus {color: #1f1f1f;}QComboBox::drop-down {width: 17px;border: none;background: transparent;}QComboBox::drop-down:hover {background: transparent;}QComboBox::down-arrow{image:url(:/res/combobox_arrow_normal.png);}QComboBox::down-arrow:hover{image:url(:/res/combobox_arrow_hover.png);}QComboBox::down-arrow:pressed{image:url(:/res/combobox_arrow_press.png);}QComboBox QAbstractItemView {border: 1px solid #0a9ff5;background: #1f1f1f;outline: none;}");
    softvol_max_spin->setStyleSheet("QSpinBox {height: 24px;min-width: 40px;border: 1px solid #000000;background: #0f0f0f;font-family:方正黑体_GBK;font-size:12px;color:#999999;}QSpinBox:hover {height: 24px;min-width: 40px;background-color:#0f0f0f;border:1px solid #0a9ff5;font-family:方正黑体_GBK;font-size:12px;color:#999999;}QSpinBox:enabled {color: #999999;}QSpinBox:enabled:hover, QSpinBox:enabled:focus {color: #999999;}QSpinBox:!enabled {color: #383838;background: transparent;}QSpinBox::up-button {border: none;width: 17px;height: 12px;image: url(:/res/spin_top_arrow_normal.png);}QSpinBox::up-button:hover {image: url(:/res/spin_top_arrow_hover.png);}QSpinBox::up-button:pressed {image: url(:/res/spin_top_arrow_press.png);}QSpinBox::up-button:!enabled {background: transparent;}QSpinBox::up-button:enabled:hover {background: rgb(255, 255, 255, 30);}QSpinBox::down-button {border: none;width: 17px;height: 12px;image: url(:/res/spin_bottom_arrow_normal.png);}QSpinBox::down-button:hover {image: url(:/res/spin_bottom_arrow_hover.png);}QSpinBox::down-button:pressed {image: url(:/res/spin_bottom_arrow_press.png);}QSpinBox::down-button:!enabled {background: transparent;}QSpinBox::down-button:hover{background: #0f0f0f;}");
    autosync_spin->setStyleSheet("QSpinBox {height: 24px;min-width: 40px;border: 1px solid #000000;background: #0f0f0f;font-family:方正黑体_GBK;font-size:12px;color:#999999;}QSpinBox:hover {height: 24px;min-width: 40px;background-color:#0f0f0f;border:1px solid #0a9ff5;font-family:方正黑体_GBK;font-size:12px;color:#999999;}QSpinBox:enabled {color: #999999;}QSpinBox:enabled:hover, QSpinBox:enabled:focus {color: #999999;}QSpinBox:!enabled {color: #383838;background: transparent;}QSpinBox::up-button {border: none;width: 17px;height: 12px;image: url(:/res/spin_top_arrow_normal.png);}QSpinBox::up-button:hover {image: url(:/res/spin_top_arrow_hover.png);}QSpinBox::up-button:pressed {image: url(:/res/spin_top_arrow_press.png);}QSpinBox::up-button:!enabled {background: transparent;}QSpinBox::up-button:enabled:hover {background: rgb(255, 255, 255, 30);}QSpinBox::down-button {border: none;width: 17px;height: 12px;image: url(:/res/spin_bottom_arrow_normal.png);}QSpinBox::down-button:hover {image: url(:/res/spin_bottom_arrow_hover.png);}QSpinBox::down-button:pressed {image: url(:/res/spin_bottom_arrow_press.png);}QSpinBox::down-button:!enabled {background: transparent;}QSpinBox::down-button:hover{background: #0f0f0f;}");

    amplification_label->setStyleSheet("QLabel{background:transparent;font-size:12px;color:#999999;font-family:方正黑体_GBK;}");
    factor_label->setStyleSheet("QLabel{background:transparent;font-size:12px;color:#999999;font-family:方正黑体_GBK;}");
    ao_label->setStyleSheet("QLabel{background:transparent;font-size:12px;color:#999999;font-family:方正黑体_GBK;}");
    label->setStyleSheet("QLabel{background:transparent;font-size:12px;color:#999999;font-family:方正黑体_GBK;}");

    connect(softvol_check, SIGNAL(toggled(bool)), softvol_max_spin, SLOT(setEnabled(bool)));
    connect(autosync_check, SIGNAL(toggled(bool)), autosync_spin, SLOT(setEnabled(bool)));
    connect(softvol_check, SIGNAL(toggled(bool)), amplification_label, SLOT(setEnabled(bool)));
    connect(autosync_check, SIGNAL(toggled(bool)), factor_label, SLOT(setEnabled(bool)));

    channels_combo->setItemText(0, tr("Default"));
	channels_combo->setItemText(0, tr("2 (Stereo)") );
	channels_combo->setItemText(1, tr("4 (4.0 Surround)") );
	channels_combo->setItemText(2, tr("6 (5.1 Surround)") );
	channels_combo->setItemText(3, tr("7 (6.1 Surround)") );
	channels_combo->setItemText(4, tr("8 (7.1 Surround)") );

	updateDriverCombos();

	createHelp();
}

void PrefAudio::setData(Preferences *pref) {
	QString ao = pref->ao;
    setAO(ao);
    setGlobalVolume(pref->global_volume);
    setSoftVol(pref->use_soft_vol);
    setInitialVolNorm(pref->initial_volnorm);
    setAmplification(pref->softvol_max);
    setAudioChannels(pref->initial_audio_channels);
    setAutoSyncActivated(pref->autosync);
    setAutoSyncFactor(pref->autosync_factor);
}

void PrefAudio::getData(Preferences * pref) {
	requires_restart = false;
	filesettings_method_changed = false;

    /*if (pref->mplayer_bin != mplayerPath()) {
        requires_restart = true;
        pref->mplayer_bin = mplayerPath();

        qDebug("PrefGeneral::getData: mplayer binary has changed, getting version number");
        // Forces to get info from mplayer to update version number
        InfoReader * i = InfoReader::obj();
        i->getInfo();
        // Update the drivers list at the same time
        ao_list = i->aoList();
        updateDriverCombos();
    }*/

    TEST_AND_SET(pref->ao, AO());
	TEST_AND_SET(pref->use_soft_vol, softVol());
	pref->global_volume = globalVolume();
	pref->initial_volnorm = initialVolNorm();
	TEST_AND_SET(pref->softvol_max, amplification());
    pref->initial_audio_channels = audioChannels();
    TEST_AND_SET(pref->autosync, autoSyncActivated());
    TEST_AND_SET(pref->autosync_factor, autoSyncFactor());
}

void PrefAudio::update_driver_combobox() {
    InfoReader * i = InfoReader::obj(this->m_snap);//20181212
    i->getInfo();
    // Update the drivers list at the same time
    ao_list = i->aoList();
    updateDriverCombos();
}

void PrefAudio::updateDriverCombos() {
    QString current_ao = AO();
    ao_combo->clear();
    ao_combo->addItem(tr("Default"), "");

    QString ao;
    for ( int n = 0; n < ao_list.count(); n++) {
        ao = ao_list[n].name();
//        ao_combo->addItem( ao, ao );

        if (ao == "oss" || ao == "alsa" || ao == "pulse") {
            ao_combo->addItem( ao, ao );
        }
        //USE_PULSEAUDIO_DEVICES
        if ((ao == "pulse") && (!pa_devices.isEmpty())) {
            for (int n=0; n < pa_devices.count(); n++) {
                ao_combo->addItem( DeviceInfo::printableName("pulse", pa_devices[n]), DeviceInfo::internalName("pulse", pa_devices[n]) );
            }
        }
    }
//    ao_combo->addItem( tr("User defined..."), "user_defined" );
    setAO(current_ao);


    /*QString current_ao = AO();
	ao_combo->clear();
	ao_combo->addItem(tr("Default"), "player_default");

	QString ao;
	for ( int n = 0; n < ao_list.count(); n++) {
		ao = ao_list[n].name();
        if (ao == "oss" || ao == "alsa" || ao == "pulse") {
            ao_combo->addItem( ao, ao );
        }
		if ((ao == "alsa") && (!alsa_devices.isEmpty())) {
			for (int n=0; n < alsa_devices.count(); n++) {
				ao_combo->addItem( "alsa (" + alsa_devices[n].ID().toString() + " - " + alsa_devices[n].desc() + ")", 
                                   "alsa:device=hw=" + alsa_devices[n].ID().toString() );
			}
		}
	}
    setAO(current_ao);*/
}

void PrefAudio::setAO( QString ao_driver ) {
	int idx = ao_combo->findData( ao_driver );
	if (idx != -1) {
		ao_combo->setCurrentIndex(idx);
	} else {
        //kobe
        idx = ao_combo->findData("pulse");
        ao_combo->setCurrentIndex(idx);
//        ao_combo->setCurrentIndex(ao_combo->findData("user_defined"));
//        ao_user_defined_edit->setText(ao_driver);
	}
	ao_combo_changed(ao_combo->currentIndex());
}

QString PrefAudio::AO() {
	QString ao = ao_combo->itemData(ao_combo->currentIndex()).toString();
//	if (ao == "user_defined") {
//		ao = ao_user_defined_edit->text();
//		/*
//		if (ao.isEmpty()) {
//			ao = ao_combo->itemData(0).toString();
//			qDebug("PrefGeneral::AO: user defined ao is empty, using %s", ao.toUtf8().constData());
//		}
//		*/
//	}
	return ao;
}

void PrefAudio::setSoftVol(bool b) {
	//softvol_check->setChecked(b);
	softvol_check->setChecked(false);//disable SoftVol
}

void PrefAudio::setGlobalVolume(bool b) {
	//global_volume_check->setChecked(b);
	global_volume_check->setChecked(false);//disable GlobalVolume
}

bool PrefAudio::globalVolume() {
	return global_volume_check->isChecked();
}

bool PrefAudio::softVol() {
	return softvol_check->isChecked();
}

void PrefAudio::setAutoSyncFactor(int factor) {
    autosync_spin->setValue(factor);
}

int PrefAudio::autoSyncFactor() {
    return autosync_spin->value();
}

void PrefAudio::setAutoSyncActivated(bool b) {
    autosync_check->setChecked(b);
}

bool PrefAudio::autoSyncActivated() {
    return autosync_check->isChecked();
}

void PrefAudio::setInitialVolNorm(bool b) {
	volnorm_check->setChecked(b);
}

bool PrefAudio::initialVolNorm() {
	return volnorm_check->isChecked();
}

void PrefAudio::setAmplification(int n) {
	softvol_max_spin->setValue(n);
}

int PrefAudio::amplification() {
	return softvol_max_spin->value();
}

void PrefAudio::setAudioChannels(int ID) {
	int pos = channels_combo->findData(ID);
	if (pos != -1) {
		channels_combo->setCurrentIndex(pos);
	} else {
		qWarning("PrefGeneral::setAudioChannels: ID: %d not found in combo", ID);
	}
}

int PrefAudio::audioChannels() {
	if (channels_combo->currentIndex() != -1) {
		return channels_combo->itemData( channels_combo->currentIndex() ).toInt();
	} else {
		qWarning("PrefGeneral::audioChannels: no item selected");
		return 0;
	}
}

void PrefAudio::ao_combo_changed(int idx) {
//	qDebug("PrefGeneral::ao_combo_changed: %d", idx);
//	bool visible = (ao_combo->itemData(idx).toString() == "user_defined");
//	ao_user_defined_edit->setVisible(visible);
//	ao_user_defined_edit->setFocus();
}

void PrefAudio::createHelp() {
	clearHelp();

	setWhatsThis(ao_combo, tr("Audio output driver"),
		tr("Select the audio output driver.") 
        + " " + 
		tr("%1 is the recommended one. Try to avoid %2 and %3, they are slow "
           "and can have an impact on performance.")
           .arg("<b><i>alsa</i></b>")
           .arg("<b><i>esd</i></b>")
           .arg("<b><i>arts</i></b>")
		);

	setWhatsThis(channels_combo, tr("Channels by default"),
		tr("Requests the number of playback channels. MPlayer "
           "asks the decoder to decode the audio into as many channels as "
           "specified. Then it is up to the decoder to fulfill the "
           "requirement. This is usually only important when playing "
           "videos with AC3 audio (like DVDs). In that case liba52 does "
           "the decoding by default and correctly downmixes the audio "
           "into the requested number of channels. "
           "<b>Note</b>: This option is honored by codecs (AC3 only), "
           "filters (surround) and audio output drivers (OSS at least).") );

	setWhatsThis(global_volume_check, tr("Global volume"),
		tr("If this option is checked, the same volume will be used for "
           "all files you play. If the option is not checked each "
           "file uses its own volume.") + "<br>" +
        tr("This option also applies for the mute control.") );

	setWhatsThis(softvol_check, tr("Software volume control"),
		tr("Check this option to use the software mixer, instead of "
           "using the sound card mixer.") );

	setWhatsThis(softvol_max_spin, tr("Max. Amplification"),
		tr("Sets the maximum amplification level in percent (default: 110). "
           "A value of 200 will allow you to adjust the volume up to a "
           "maximum of double the current level. With values below 100 the "
           "initial volume (which is 100%) will be above the maximum, which "
           "e.g. the OSD cannot display correctly.") );

	setWhatsThis(volnorm_check, tr("Volume normalization by default"),
		tr("Maximizes the volume without distorting the sound.") );

    setWhatsThis(autosync_check, tr("Audio/video auto synchronization"),
        tr("Gradually adjusts the A/V sync based on audio delay "
           "measurements.") );
}

//#include "moc_prefaudio.cpp"
