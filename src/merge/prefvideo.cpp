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
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "prefvideo.h"
#include "../smplayer/preferences.h"
#include "../smplayer/filedialog.h"
#include "../smplayer/images.h"
#include "../smplayer/mediasettings.h"
#include "paths.h"
#include "../smplayer/playerid.h"
#include <QDebug>
#include "../smplayer/deviceinfo.h"

PrefVideo::PrefVideo(QString arch_type, QWidget * parent, Qt::WindowFlags f)
    : PrefWidget(parent, f ), arch(arch_type)
{
	setupUi(this);

	// Read driver info from InfoReader:
	InfoReader * i = InfoReader::obj();
	i->getInfo();
	vo_list = i->voList();
	alsa_devices = DeviceInfo::alsaDevices();
	xv_adaptors = DeviceInfo::xvAdaptors();
    connect(vo_combo, SIGNAL(currentIndexChanged(int)), this, SLOT(vo_combo_changed(int)));

	retranslateStrings();
}

PrefVideo::~PrefVideo()
{
}

void PrefVideo::retranslateStrings() {
	retranslateUi(this);

    postprocessing_check->setFocusPolicy(Qt::NoFocus);
    eq2_check->setFocusPolicy(Qt::NoFocus);
    double_buffer_check->setFocusPolicy(Qt::NoFocus);
    direct_rendering_check->setFocusPolicy(Qt::NoFocus);
    use_slices_check->setFocusPolicy(Qt::NoFocus);
    vo_combo->setStyleSheet("QComboBox{width:150px;height:24px;border:1px solid #000000;background:#0f0f0f;font-size:12px;font-family:方正黑体_GBK;background-position:center left;padding-left:5px;color:#999999;selection-color:#ffffff;selection-background-color:#1f1f1f;}QComboBox::hover{background-color:#0f0f0f;border:1px solid #0a9ff5;font-family:方正黑体_GBK;font-size:12px;color:#999999;}QComboBox:!enabled {background:#0f0f0f;color:#383838;}QComboBox::drop-down {width:17px;border:none;background:transparent;}QComboBox::drop-down:hover {background:transparent;}QComboBox::down-arrow{image:url(:/res/combobox_arrow_normal.png);}QComboBox::down-arrow:hover{image:url(:/res/combobox_arrow_hover.png);}QComboBox::down-arrow:pressed{image:url(:/res/combobox_arrow_press.png);}QComboBox QAbstractItemView{border:1px solid #0a9ff5;background:#262626;outline:none;}");
//    vo_combo->setStyleSheet("QComboBox{width:150px;height:24px;border: 1px solid #000000;background: #0f0f0f;font-family:方正黑体_GBK;font-size:12px;color:#999999;}QComboBox::hover{background-color:#0f0f0f;border:1px solid #0a9ff5;font-family:方正黑体_GBK;font-size:12px;color:#999999;}QComboBox:enabled {background:#0f0f0f;color:#999999;}QComboBox:!enabled {background:#0f0f0f;color:#383838;}QComboBox:enabled:hover, QComboBox:enabled:focus {color: #1f1f1f;}QComboBox::drop-down {width: 17px;border: none;background: transparent;}QComboBox::drop-down:hover {background: transparent;}QComboBox::down-arrow{image:url(:/res/combobox_arrow_normal.png);}QComboBox::down-arrow:hover{image:url(:/res/combobox_arrow_hover.png);}QComboBox::down-arrow:pressed{image:url(:/res/combobox_arrow_press.png);}QComboBox QAbstractItemView {border: 1px solid #0a9ff5;background: #1f1f1f;outline: none;}");
    vo_label->setStyleSheet("QLabel{background:transparent;font-size:12px;color:#999999;font-family:方正黑体_GBK;}");


	updateDriverCombos();

	createHelp();
}

void PrefVideo::setData(Preferences *pref) {
	QString vo = pref->vo;
	if (vo.isEmpty()) {
        //kobe
        pref->vo = "xv";
        vo = "xv";
	}
	setVO( vo );
	setEq2( pref->use_soft_video_eq );
    setInitialPostprocessing(pref->initial_postprocessing);
    setDirectRendering(pref->use_direct_rendering);
    setDoubleBuffer(pref->use_double_buffer);
    setUseSlices(pref->use_slices);
}

void PrefVideo::getData(Preferences * pref) {
	requires_restart = false;
	filesettings_method_changed = false;

	TEST_AND_SET(pref->vo, VO());
	TEST_AND_SET(pref->use_soft_video_eq, eq2());
    pref->initial_postprocessing = initialPostprocessing();
	TEST_AND_SET(pref->use_direct_rendering, directRendering());
	TEST_AND_SET(pref->use_double_buffer, doubleBuffer());
	TEST_AND_SET(pref->use_slices, useSlices());
}

void PrefVideo::update_driver_combobox()
{
    InfoReader * i = InfoReader::obj();
    i->getInfo();
    // Update the drivers list at the same time
    vo_list = i->voList();
    updateDriverCombos();
}

void PrefVideo::updateDriverCombos() {
	QString current_vo = VO();
	vo_combo->clear();
	vo_combo->addItem(tr("Default"), "player_default");
	QString vo;
	for ( int n = 0; n < vo_list.count(); n++ ) {
		vo = vo_list[n].name();
        if (vo == "x11") {
            vo_combo->addItem("x11 (" + tr("slow") + ")", vo);
        }
        //当播放引擎为mplayer时如果选择sdl，视频显示尺寸可能异常，当播放引擎为mpv时如果选择sdl，视频窗口将分离。
        else if (vo == "xv" || vo == "gl_nosw"  /* || vo == "sdl"*/) {
            vo_combo->addItem(vo, vo);
        }
        else if (vo == "vdpau") {//kobe for arm 硬件解码基于飞腾上的只能基于vdpau接口来实现
            if (arch == "aarch64") {//kobe 20180612
                vo_combo->addItem(vo, vo);
            }
        }

        /*else
        if (vo == "gl") {//kobe:此类驱动在arm64上会导致机器卡死 Bug:3152
//			vo_combo->addItem( vo, vo);//kobe
//			vo_combo->addItem( "gl (" + tr("fast") + ")", "gl:yuv=2:force-pbo");//kobe
//			vo_combo->addItem( "gl (" + tr("fast - ATI cards") + ")", "gl:yuv=2:force-pbo:ati-hack");//kobe
//            vo_combo->addItem( "gl (yuv)", "gl:yuv=3");//kobe
		}
		else
		if (vo == "gl2") {
			vo_combo->addItem( vo, vo);
			vo_combo->addItem( "gl2 (yuv)", "gl2:yuv=3");
		}
		else
		if (vo == "gl_tiled") {
			vo_combo->addItem( vo, vo);
			vo_combo->addItem( "gl_tiled (yuv)", "gl_tiled:yuv=3");
		}
		else
		if (vo == "null" || vo == "png" || vo == "jpeg" || vo == "gif89a" || 
            vo == "tga" || vo == "pnm" || vo == "md5sum" ) 
		{
			; // Nothing to do
		}
        else
        if (vo == "fbdev" || vo == "aa" || vo == "directfb" || vo == "mpegpes")
        {//kobe:此类驱动在arm64上会导致机器卡死 Bug:3152
            ; // Nothing to do
        }
        else
        if (vo == "vdpau") {
            ; // kobe: Nothing to do
        }
		else
        vo_combo->addItem( vo, vo );*/
	}
//	vo_combo->addItem( tr("User defined..."), "user_defined" );//kobe:去掉自定义驱动选项

	setVO(current_vo);
}

void PrefVideo::setVO( QString vo_driver ) {
	int idx = vo_combo->findData( vo_driver );
	if (idx != -1) {
		vo_combo->setCurrentIndex(idx);
	} else {
        //kobe
        idx = vo_combo->findData("xv");
        vo_combo->setCurrentIndex(idx);
//		vo_combo->setCurrentIndex(vo_combo->findData("user_defined"));
//		vo_user_defined_edit->setText(vo_driver);
	}
	vo_combo_changed(vo_combo->currentIndex());
}

QString PrefVideo::VO() {
	QString vo = vo_combo->itemData(vo_combo->currentIndex()).toString();
//	if (vo == "user_defined") {
//		vo = vo_user_defined_edit->text();
//		/*
//		if (vo.isEmpty()) {
//			vo = vo_combo->itemData(0).toString();
//			qDebug("PrefGeneral::VO: user defined vo is empty, using %s", vo.toUtf8().constData());
//		}
//		*/
//	}
	return vo;
}

void PrefVideo::setEq2(bool b) {
	eq2_check->setChecked(b);
}

bool PrefVideo::eq2() {
	return eq2_check->isChecked();
}

void PrefVideo::setInitialPostprocessing(bool b) {
	postprocessing_check->setChecked(b);
}

bool PrefVideo::initialPostprocessing() {
	return postprocessing_check->isChecked();
}

void PrefVideo::setDirectRendering(bool b) {
	direct_rendering_check->setChecked(b);
}

bool PrefVideo::directRendering() {
	return direct_rendering_check->isChecked();
}

void PrefVideo::setDoubleBuffer(bool b) {
	double_buffer_check->setChecked(b);
}

bool PrefVideo::doubleBuffer() {
	return double_buffer_check->isChecked();
}

void PrefVideo::setUseSlices(bool b) {
	use_slices_check->setChecked(b);
}

bool PrefVideo::useSlices() {
	return use_slices_check->isChecked();
}

void PrefVideo::vo_combo_changed(int idx) {
//	qDebug("PrefGeneral::vo_combo_changed: %d", idx);
//	bool visible = (vo_combo->itemData(idx).toString() == "user_defined");
//	vo_user_defined_edit->setVisible(visible);
//	vo_user_defined_edit->setFocus();
}

void PrefVideo::createHelp() {
	clearHelp();

	setWhatsThis(vo_combo, tr("Video output driver"),
		tr("Select the video output driver. %1 provides the best performance.")
		  .arg("<b><i>xv</i></b>")
		);

	setWhatsThis(postprocessing_check, tr("Enable postprocessing by default"),
		tr("Postprocessing will be used by default on new opened files.") );

	setWhatsThis(eq2_check, tr("Software video equalizer"),
		tr("You can check this option if video equalizer is not supported by "
           "your graphic card or the selected video output driver.<br>"
           "<b>Note:</b> this option can be incompatible with some video "
           "output drivers.") );

	setWhatsThis(direct_rendering_check, tr("Direct rendering"),
		tr("If checked, turns on direct rendering (not supported by all "
           "codecs and video outputs)<br>"
           "<b>Warning:</b> May cause OSD/SUB corruption!") );

	setWhatsThis(double_buffer_check, tr("Double buffering"),
		tr("Double buffering fixes flicker by storing two frames in memory, "
           "and displaying one while decoding another. If disabled it can "
           "affect OSD negatively, but often removes OSD flickering.") );

	setWhatsThis(use_slices_check, tr("Draw video using slices"),
		tr("Enable/disable drawing video by 16-pixel height slices/bands. "
           "If disabled, the whole frame is drawn in a single run. "
           "May be faster or slower, depending on video card and available "
           "cache. It has effect only with libmpeg2 and libavcodec codecs.") );
}

//#include "moc_prefvideo.cpp"
