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

#include "prefperformance.h"
#include "../smplayer/images.h"
#include "../smplayer/global.h"
#include "../smplayer/preferences.h"
#include <QDebug>

using namespace Global;

PrefPerformance::PrefPerformance(QWidget * parent, Qt::WindowFlags f)
	: PrefWidget(parent, f )
{
	setupUi(this);
    hwdec_combo->addItem(tr("None"), "no");
	hwdec_combo->addItem(tr("Auto"), "auto");
	hwdec_combo->addItem("auto-copy", "auto-copy");
    hwdec_combo->addItem("vdpau", "vdpau");
        hwdec_combo->addItem("vdpau-copy", "vdpau-copy");
	hwdec_combo->addItem("vaapi", "vaapi");
	hwdec_combo->addItem("vaapi-copy", "vaapi-copy");
	//hwdec_combo->addItem("rkmpp-copy", "rkmpp-copy");
	hwdec_combo->addItem("cuda", "cuda");
        hwdec_combo->addItem("cuda-copy", "cuda-copy");
        hwdec_combo->addItem("crystalhd", "crystalhd");
	retranslateStrings();
}

PrefPerformance::~PrefPerformance()
{
}

void PrefPerformance::setCurrentPage(int index)
{

}

QString PrefPerformance::sectionName() {
	return tr("Performance");
}

QPixmap PrefPerformance::sectionIcon() {
    return Images::icon("pref_performance", 22);
}

void PrefPerformance::retranslateStrings() {
	retranslateUi(this);

    groupBox_cache->setStyleSheet("QGroupBox{border:none;margin-top:20px;font-size:14px;}QGroupBox:title{subcontrol-origin: margin;subcontrol-position: top left;padding: 0px 1px;color: #999999;font-family: 方正黑体_GBK;font-weight:bold;}");
    groupBox_decode->setStyleSheet("QGroupBox{border:none;margin-top:20px;font-size:14px;}QGroupBox:title{subcontrol-origin: margin;subcontrol-position: top left;padding: 0px 1px;color: #999999;font-family: 方正黑体_GBK;font-weight:bold;}");

    cache_streams_spin->setStyleSheet("QSpinBox {height: 24px;min-width: 40px;border: 1px solid #000000;background: #0f0f0f;font-family:方正黑体_GBK;font-size:12px;color:#999999;}QSpinBox:hover {height: 24px;min-width: 40px;background-color:#0f0f0f;border:1px solid #0a9ff5;font-family:方正黑体_GBK;font-size:12px;color:#999999;}QSpinBox:enabled {color: #999999;}QSpinBox:enabled:hover, QSpinBox:enabled:focus {color: #999999;}QSpinBox:!enabled {color: #383838;background: transparent;}QSpinBox::up-button {border: none;width: 17px;height: 12px;image: url(:/res/spin_top_arrow_normal.png);}QSpinBox::up-button:hover {image: url(:/res/spin_top_arrow_hover.png);}QSpinBox::up-button:pressed {image: url(:/res/spin_top_arrow_press.png);}QSpinBox::up-button:!enabled {background: transparent;}QSpinBox::up-button:enabled:hover {background: rgb(255, 255, 255, 30);}QSpinBox::down-button {border: none;width: 17px;height: 12px;image: url(:/res/spin_bottom_arrow_normal.png);}QSpinBox::down-button:hover {image: url(:/res/spin_bottom_arrow_hover.png);}QSpinBox::down-button:pressed {image: url(:/res/spin_bottom_arrow_press.png);}QSpinBox::down-button:!enabled {background: transparent;}QSpinBox::down-button:hover{background: #0f0f0f;}");
    cache_files_spin->setStyleSheet("QSpinBox {height: 24px;min-width: 40px;border: 1px solid #000000;background: #0f0f0f;font-family:方正黑体_GBK;font-size:12px;color:#999999;}QSpinBox:hover {height: 24px;min-width: 40px;background-color:#0f0f0f;border:1px solid #0a9ff5;font-family:方正黑体_GBK;font-size:12px;color:#999999;}QSpinBox:enabled {color: #999999;}QSpinBox:enabled:hover, QSpinBox:enabled:focus {color: #999999;}QSpinBox:!enabled {color: #383838;background: transparent;}QSpinBox::up-button {border: none;width: 17px;height: 12px;image: url(:/res/spin_top_arrow_normal.png);}QSpinBox::up-button:hover {image: url(:/res/spin_top_arrow_hover.png);}QSpinBox::up-button:pressed {image: url(:/res/spin_top_arrow_press.png);}QSpinBox::up-button:!enabled {background: transparent;}QSpinBox::up-button:enabled:hover {background: rgb(255, 255, 255, 30);}QSpinBox::down-button {border: none;width: 17px;height: 12px;image: url(:/res/spin_bottom_arrow_normal.png);}QSpinBox::down-button:hover {image: url(:/res/spin_bottom_arrow_hover.png);}QSpinBox::down-button:pressed {image: url(:/res/spin_bottom_arrow_press.png);}QSpinBox::down-button:!enabled {background: transparent;}QSpinBox::down-button:hover{background: #0f0f0f;}");
    threads_spin->setStyleSheet("QSpinBox {height: 24px;min-width: 40px;border: 1px solid #000000;background: #0f0f0f;font-family:方正黑体_GBK;font-size:12px;color:#999999;}QSpinBox:hover {height: 24px;min-width: 40px;background-color:#0f0f0f;border:1px solid #0a9ff5;font-family:方正黑体_GBK;font-size:12px;color:#999999;}QSpinBox:enabled {color: #999999;}QSpinBox:enabled:hover, QSpinBox:enabled:focus {color: #999999;}QSpinBox:!enabled {color: #383838;background: transparent;}QSpinBox::up-button {border: none;width: 17px;height: 12px;image: url(:/res/spin_top_arrow_normal.png);}QSpinBox::up-button:hover {image: url(:/res/spin_top_arrow_hover.png);}QSpinBox::up-button:pressed {image: url(:/res/spin_top_arrow_press.png);}QSpinBox::up-button:!enabled {background: transparent;}QSpinBox::up-button:enabled:hover {background: rgb(255, 255, 255, 30);}QSpinBox::down-button {border: none;width: 17px;height: 12px;image: url(:/res/spin_bottom_arrow_normal.png);}QSpinBox::down-button:hover {image: url(:/res/spin_bottom_arrow_hover.png);}QSpinBox::down-button:pressed {image: url(:/res/spin_bottom_arrow_press.png);}QSpinBox::down-button:!enabled {background: transparent;}QSpinBox::down-button:hover{background: #0f0f0f;}");
    hwdec_combo->setStyleSheet("QComboBox{width:150px;height:24px;border:1px solid #000000;background:#0f0f0f;font-size:12px;font-family:方正黑体_GBK;background-position:center left;padding-left:5px;color:#999999;selection-color:#ffffff;selection-background-color:#1f1f1f;}QComboBox::hover{background-color:#0f0f0f;border:1px solid #0a9ff5;font-family:方正黑体_GBK;font-size:12px;color:#999999;}QComboBox:!enabled {background:#0f0f0f;color:#383838;}QComboBox::drop-down {width:17px;border:none;background:transparent;}QComboBox::drop-down:hover {background:transparent;}QComboBox::down-arrow{image:url(:/res/combobox_arrow_normal.png);}QComboBox::down-arrow:hover{image:url(:/res/combobox_arrow_hover.png);}QComboBox::down-arrow:pressed{image:url(:/res/combobox_arrow_press.png);}QComboBox QAbstractItemView{border:1px solid #0a9ff5;background:#262626;outline:none;}");
//    hwdec_combo->setStyleSheet("QComboBox{width:150px;height:24px;border: 1px solid #000000;background: #0f0f0f;font-family:方正黑体_GBK;font-size:12px;color:#999999;}QComboBox::hover{background-color:#0f0f0f;border:1px solid #0a9ff5;font-family:方正黑体_GBK;font-size:12px;color:#999999;}QComboBox:enabled {background:#0f0f0f;color:#999999;}QComboBox:!enabled {background:#0f0f0f;color:#383838;}QComboBox:enabled:hover, QComboBox:enabled:focus {color: #1f1f1f;}QComboBox::drop-down {width: 17px;border: none;background: transparent;}QComboBox::drop-down:hover {background: transparent;}QComboBox::down-arrow{image:url(:/res/combobox_arrow_normal.png);}QComboBox::down-arrow:hover{image:url(:/res/combobox_arrow_hover.png);}QComboBox::down-arrow:pressed{image:url(:/res/combobox_arrow_press.png);}QComboBox QAbstractItemView {border: 1px solid #0a9ff5;background: #1f1f1f;outline: none;}");
    cache_local_label->setStyleSheet("QLabel{background:transparent;font-size:12px;color:#999999;font-family:方正黑体_GBK;}");
    cache_streams_label->setStyleSheet("QLabel{background:transparent;font-size:12px;color:#999999;font-family:方正黑体_GBK;}");
    kb_local_label->setStyleSheet("QLabel{background:transparent;font-size:12px;color:#999999;font-family:方正黑体_GBK;}");
    kb_streams_label->setStyleSheet("QLabel{background:transparent;font-size:12px;color:#999999;font-family:方正黑体_GBK;}");
    thread_label->setStyleSheet("QLabel{background:transparent;font-size:12px;color:#999999;font-family:方正黑体_GBK;}");
    hd_decode_label->setStyleSheet("QLabel{background:transparent;font-size:12px;color:#999999;font-family:方正黑体_GBK;}");

	createHelp();
}

void PrefPerformance::setData(Preferences * pref) {
	setCacheForFiles( pref->cache_for_files );
	setCacheForStreams( pref->cache_for_streams );
    setThreads(pref->threads);
    setHwdec(pref->hwdec);
}

void PrefPerformance::getData(Preferences * pref) {
	requires_restart = false;

	TEST_AND_SET(pref->cache_for_files, cacheForFiles());
	TEST_AND_SET(pref->cache_for_streams, cacheForStreams());
	TEST_AND_SET(pref->threads, threads());
	TEST_AND_SET(pref->hwdec, hwdec());
}

void PrefPerformance::setCacheForFiles(int n) {
	cache_files_spin->setValue(n);
}

int PrefPerformance::cacheForFiles() {
	return cache_files_spin->value();
}

void PrefPerformance::setCacheForStreams(int n) {
	cache_streams_spin->setValue(n);
}

int PrefPerformance::cacheForStreams() {
	return cache_streams_spin->value();
}

void PrefPerformance::setThreads(int v) {
	threads_spin->setValue(v);
}

int PrefPerformance::threads() {
	return threads_spin->value();
}

void PrefPerformance::setHwdec(const QString & v) {
	int idx = hwdec_combo->findData(v);
	if (idx < 0) idx = 0;
	hwdec_combo->setCurrentIndex(idx);
}

QString PrefPerformance::hwdec() {
	int idx = hwdec_combo->currentIndex();
	return hwdec_combo->itemData(idx).toString();
}

void PrefPerformance::createHelp() {
	clearHelp();

	addSectionTitle(tr("Performance"));

	setWhatsThis(threads_spin, tr("Threads for decoding"),
		tr("Sets the number of threads to use for decoding. Only for "
           "MPEG-1/2 and H.264") );

	setWhatsThis(hwdec_combo, tr("Hardware decoding"),
		tr("Sets the hardware video decoding API. "
		   "If hardware decoding is not possible, software decoding will be used instead.") + " " +
		tr("Available options:") +
			"<ul>"
			"<li>" + tr("None: only software decoding will be used.") + "</li>"
			"<li>" + tr("Auto: it tries to automatically enable hardware decoding using the first available method.") + "</li>"
			#ifdef Q_OS_LINUX
            "<li>" + tr("vdpau: for the vdpau and opengl video outputs.") + "</li>"
			"<li>" + tr("vaapi: for the opengl and vaapi video outputs. For Intel GPUs only.") + "</li>"
			"<li>" + tr("vaapi-copy: it copies video back into system RAM. For Intel GPUs only.") + "</li>"
			#endif

			"</ul>" +
		tr("This option only works with mpv."));

	addSectionTitle(tr("Cache"));

	setWhatsThis(cache_files_spin, tr("Cache for files"), 
		tr("This option specifies how much memory (in kBytes) to use when "
           "precaching a file.") );

	setWhatsThis(cache_streams_spin, tr("Cache for streams"), 
		tr("This option specifies how much memory (in kBytes) to use when "
           "precaching a URL.") );
}

//#include "moc_prefperformance.cpp"
