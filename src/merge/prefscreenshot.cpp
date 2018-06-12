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

#include "prefscreenshot.h"
#include "../smplayer/preferences.h"
#include "../smplayer/filedialog.h"
#include "../smplayer/images.h"
#include "../smplayer/mediasettings.h"
#include "../smplayer/paths.h"
#include "../smplayer/playerid.h"
#include <QDebug>
#include "../smplayer/deviceinfo.h"
//#include <QWindowsStyle>//qt4
//#include <QCommonStyle>//qt5

PrefScreenShot::PrefScreenShot(QWidget * parent, Qt::WindowFlags f)
	: PrefWidget(parent, f )
{
	setupUi(this);

//	mplayerbin_edit->setDialogType(FileChooser::GetFileName);
	screenshot_edit->setDialogType(FileChooser::GetDirectory);

	retranslateStrings();
}

PrefScreenShot::~PrefScreenShot()
{
}

void PrefScreenShot::retranslateStrings() {
	retranslateUi(this);

    use_screenshots_check->setFocusPolicy(Qt::NoFocus);

    groupBox->setStyleSheet("QGroupBox{border:none;margin-top:20px;font-size:14px;}QGroupBox:title{subcontrol-origin: margin;subcontrol-position: top left;padding: 0px 1px;color: #999999;font-family: 方正黑体_GBK;font-weight:bold;}");

    screenshot_format_combo->setStyleSheet("QComboBox{width:150px;height:24px;border:1px solid #000000;background:#0f0f0f;font-size:12px;font-family:方正黑体_GBK;background-position:center left;padding-left:5px;color:#999999;selection-color:#ffffff;selection-background-color:#1f1f1f;}QComboBox::hover{background-color:#0f0f0f;border:1px solid #0a9ff5;font-family:方正黑体_GBK;font-size:12px;color:#999999;}QComboBox:!enabled {background:#0f0f0f;color:#383838;}QComboBox::drop-down {width:17px;border:none;background:transparent;}QComboBox::drop-down:hover {background:transparent;}QComboBox::down-arrow{image:url(:/res/combobox_arrow_normal.png);}QComboBox::down-arrow:hover{image:url(:/res/combobox_arrow_hover.png);}QComboBox::down-arrow:pressed{image:url(:/res/combobox_arrow_press.png);}QComboBox QAbstractItemView{border:1px solid #0a9ff5;background:#262626;outline:none;}");
//    screenshot_format_combo->setStyle(new QWindowsStyle);//qt4
//    screenshot_format_combo->setStyle(new QCommonStyle);//qt5
    //    screenshot_format_combo->setStyleSheet("QComboBox{width:150px;height:24px;border: 1px solid #000000;background: #0f0f0f;font-family:方正黑体_GBK;font-size:12px;color:#999999;}QComboBox::hover{background-color:#0f0f0f;border:1px solid #0a9ff5;font-family:方正黑体_GBK;font-size:12px;color:#999999;}QComboBox:enabled {background:#0f0f0f;color:#999999;}QComboBox:!enabled {background:#0f0f0f;color:#383838;}QComboBox:enabled:hover, QComboBox:enabled:focus {color: #1f1f1f;}QComboBox::drop-down {width: 17px;border: none;background: transparent;}QComboBox::drop-down:hover {background: transparent;}QComboBox::down-arrow{image:url(:/res/combobox_arrow_normal.png);}QComboBox::down-arrow:hover{image:url(:/res/combobox_arrow_hover.png);}QComboBox::down-arrow:pressed{image:url(:/res/combobox_arrow_press.png);}QComboBox QAbstractItemView {border: 1px solid #0a9ff5;background: #1f1f1f;outline: none;}");
    screenshot_template_edit->setStyleSheet("QLineEdit {height: 25px;border: 1px solid #000000;background: #0f0f0f;font-family:方正黑体_GBK;font-size:12px;color:#999999;}QLineEdit::hover{border: 1px solid #000000;background: #0a0a0a;font-family:方正黑体_GBK;font-size:12px;color:#ffffff;}QLineEdit:enabled {background: #0a0a0a;color:#999999;}QLineEdit:enabled:hover, QLineEdit:enabled:focus {background: #0a0a0a;color:#ffffff;}QLineEdit:!enabled {color: #383838;}");

    screenshots_dir_label->setStyleSheet("QLabel{background:transparent;font-size:12px;color:#999999;font-family:方正黑体_GBK;}");
    screenshot_format_label->setStyleSheet("QLabel{background:transparent;font-size:12px;color:#999999;font-family:方正黑体_GBK;}");
    screenshot_template_label->setStyleSheet("QLabel{background:transparent;font-size:12px;color:#999999;font-family:方正黑体_GBK;}");

    connect(use_screenshots_check, SIGNAL(toggled(bool)), screenshots_dir_label, SLOT(setEnabled(bool)));
    connect(use_screenshots_check, SIGNAL(toggled(bool)), screenshot_edit, SLOT(setEnabled(bool)));
    connect(use_screenshots_check, SIGNAL(toggled(bool)), screenshot_template_edit, SLOT(setEnabled(bool)));
    connect(use_screenshots_check, SIGNAL(toggled(bool)), screenshot_format_label, SLOT(setEnabled(bool)));
    connect(use_screenshots_check, SIGNAL(toggled(bool)), screenshot_format_combo, SLOT(setEnabled(bool)));
    connect(use_screenshots_check, SIGNAL(toggled(bool)), screenshot_template_label, SLOT(setEnabled(bool)));

	screenshot_edit->setCaption(tr("Select a directory"));

	createHelp();
}

void PrefScreenShot::setData(Preferences *pref) {
    setUseScreenshots(pref->use_screenshot);
    setScreenshotDir(pref->screenshot_directory);

    screenshot_format_combo->addItems(QStringList() << "png" << "ppm" << "pgm" << "pgmyuv" << "tga" << "jpg" << "jpeg");

    screenshot_template_edit->setText(pref->screenshot_template);
    setScreenshotFormat(pref->screenshot_format);

    QString destPath = Paths::appPath() + "/mpv";
//    if (pref->mplayer_bin == "/usr/bin/mpv") {
    if (pref->mplayer_bin == destPath) {
            screenshot_template_label->show();
            screenshot_template_edit->show();
            screenshot_format_label->show();
            screenshot_format_combo->show();
    }
    else {
        screenshot_template_label->hide();
        screenshot_template_edit->hide();
        screenshot_format_label->hide();
        screenshot_format_combo->hide();
    }
}

void PrefScreenShot::getData(Preferences * pref) {
	requires_restart = false;
	filesettings_method_changed = false;

	TEST_AND_SET(pref->use_screenshot, useScreenshots());
	TEST_AND_SET(pref->screenshot_directory, screenshotDir());

    if (pref->mplayer_bin == "/usr/bin/mpv") {
        TEST_AND_SET(pref->screenshot_template, screenshot_template_edit->text());
        TEST_AND_SET(pref->screenshot_format, screenshotFormat());
    }
}

void PrefScreenShot::setUseScreenshots(bool b) {
	use_screenshots_check->setChecked(b);
}

bool PrefScreenShot::useScreenshots() {
	return use_screenshots_check->isChecked();
}

void PrefScreenShot::setScreenshotDir( QString path ) {
	screenshot_edit->setText( path );
}

QString PrefScreenShot::screenshotDir() {
	return screenshot_edit->text();
}

void PrefScreenShot::setScreenshotFormat(const QString format) {
    int i = screenshot_format_combo->findText(format);
    if (i < 0) i = 0;
    screenshot_format_combo->setCurrentIndex(i);
}

QString PrefScreenShot::screenshotFormat() {
    return screenshot_format_combo->currentText();
}

void PrefScreenShot::createHelp() {
	clearHelp();

	setWhatsThis(use_screenshots_check, tr("Enable screenshots"),
		tr("You can use this option to enable or disable the possibility to "
           "take screenshots.") );

	setWhatsThis(screenshot_edit, tr("Screenshots folder"),
		tr("Here you can specify a folder where the screenshots taken by "
           "Kylin Video will be stored. If the folder is not valid the "
           "screenshot feature will be disabled.") );

    setWhatsThis(screenshot_template_edit, tr("Template for screenshots"),
        tr("This option specifies the filename template used to save screenshots.") + " " +
        tr("For example %1 would save the screenshot as 'moviename_0001.png'.").arg("%F_%04n") + "<br>" +
        tr("%1 specifies the filename of the video without the extension, "
           "%2 adds a 4 digit number padded with zeros.").arg("%F").arg("%04n") + " " +
        tr("For a full list of the template specifiers visit this link:") +
        " <a href=\"http://mpv.io/manual/stable/#options-screenshot-template\">"
        "http://mpv.io/manual/stable/#options-screenshot-template</a>" + "<br>" +
        tr("This option only works with mpv.") );

    setWhatsThis(screenshot_format_combo, tr("Format for screenshots"),
        tr("This option allows one to choose the image file type used for saving screenshots.") + " " +
        tr("This option only works with mpv.") );
}

//#include "moc_prefscreenshot.cpp"
