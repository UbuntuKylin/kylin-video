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

#include "prefsubtitles.h"
#include "../smplayer/images.h"
#include "../smplayer/preferences.h"
#include "paths.h"
#include "../smplayer/filedialog.h"
#include "../smplayer/languages.h"
#include <QDebug>

#include <QInputDialog>

PrefSubtitles::PrefSubtitles(QWidget * parent, Qt::WindowFlags f)
	: PrefWidget(parent, f )
{
	setupUi(this);
    retranslateStrings();
}

PrefSubtitles::~PrefSubtitles()
{
}

void PrefSubtitles::setCurrentPage(int index)
{
    qDebug() << "PrefSubtitles::setCurrentPage index=" << index;
}

QString PrefSubtitles::sectionName() {
	return tr("Subtitles");
}

QPixmap PrefSubtitles::sectionIcon() {
    return Images::icon("pref_subtitles", 22);
}


void PrefSubtitles::retranslateStrings() {
	int font_autoload_item = font_autoload_combo->currentIndex();

	retranslateUi(this);

    groupBox_autoload->setStyleSheet("QGroupBox{border:none;margin-top:20px;font-size:14px;}QGroupBox:title{subcontrol-origin: margin;subcontrol-position: top left;padding: 0px 1px;color: #999999;font-family: 方正黑体_GBK;font-weight:bold;}");
    groupBox_encoding->setStyleSheet("QGroupBox{border:none;margin-top:20px;font-size:14px;}QGroupBox:title{subcontrol-origin: margin;subcontrol-position: top left;padding: 0px 1px;color: #999999;font-family: 方正黑体_GBK;font-weight:bold;}");

    font_autoload_combo->setStyleSheet("QComboBox{width:150px;height:24px;border:1px solid #000000;background:#0f0f0f;font-size:12px;font-family:方正黑体_GBK;background-position:center left;padding-left:5px;color:#999999;selection-color:#ffffff;selection-background-color:#1f1f1f;}QComboBox::hover{background-color:#0f0f0f;border:1px solid #0a9ff5;font-family:方正黑体_GBK;font-size:12px;color:#999999;}QComboBox:!enabled {background:#0f0f0f;color:#383838;}QComboBox::drop-down {width:17px;border:none;background:transparent;}QComboBox::drop-down:hover {background:transparent;}QComboBox::down-arrow{image:url(:/res/combobox_arrow_normal.png);}QComboBox::down-arrow:hover{image:url(:/res/combobox_arrow_hover.png);}QComboBox::down-arrow:pressed{image:url(:/res/combobox_arrow_press.png);}QComboBox QAbstractItemView{border:1px solid #0a9ff5;background:#262626;outline:none;}");
    font_encoding_combo->setStyleSheet("QComboBox{width:150px;height:24px;border:1px solid #000000;background:#0f0f0f;font-size:12px;font-family:方正黑体_GBK;background-position:center left;padding-left:5px;color:#999999;selection-color:#ffffff;selection-background-color:#1f1f1f;}QComboBox::hover{background-color:#0f0f0f;border:1px solid #0a9ff5;font-family:方正黑体_GBK;font-size:12px;color:#999999;}QComboBox:!enabled {background:#0f0f0f;color:#383838;}QComboBox::drop-down {width:17px;border:none;background:transparent;}QComboBox::drop-down:hover {background:transparent;}QComboBox::down-arrow{image:url(:/res/combobox_arrow_normal.png);}QComboBox::down-arrow:hover{image:url(:/res/combobox_arrow_hover.png);}QComboBox::down-arrow:pressed{image:url(:/res/combobox_arrow_press.png);}QComboBox QAbstractItemView{border:1px solid #0a9ff5;background:#262626;outline:none;}");
    enca_lang_combo->setStyleSheet("QComboBox{width:150px;height:24px;border:1px solid #000000;background:#0f0f0f;font-size:12px;font-family:方正黑体_GBK;background-position:center left;padding-left:5px;color:#999999;selection-color:#ffffff;selection-background-color:#1f1f1f;}QComboBox::hover{background-color:#0f0f0f;border:1px solid #0a9ff5;font-family:方正黑体_GBK;font-size:12px;color:#999999;}QComboBox:!enabled {background:#0f0f0f;color:#383838;}QComboBox::drop-down {width:17px;border:none;background:transparent;}QComboBox::drop-down:hover {background:transparent;}QComboBox::down-arrow{image:url(:/res/combobox_arrow_normal.png);}QComboBox::down-arrow:hover{image:url(:/res/combobox_arrow_hover.png);}QComboBox::down-arrow:pressed{image:url(:/res/combobox_arrow_press.png);}QComboBox QAbstractItemView{border:1px solid #0a9ff5;background:#262626;outline:none;}");
//    font_autoload_combo->setStyleSheet("QComboBox{width:150px;height:24px;border: 1px solid #000000;background: #0f0f0f;font-family:方正黑体_GBK;font-size:12px;color:#999999;}QComboBox::hover{background-color:#0f0f0f;border:1px solid #0a9ff5;font-family:方正黑体_GBK;font-size:12px;color:#999999;}QComboBox:enabled {background:#0f0f0f;color:#999999;}QComboBox:!enabled {background:#0f0f0f;color:#383838;}QComboBox:enabled:hover, QComboBox:enabled:focus {color: #1f1f1f;}QComboBox::drop-down {width: 17px;border: none;background: transparent;}QComboBox::drop-down:hover {background: transparent;}QComboBox::down-arrow{image:url(:/res/combobox_arrow_normal.png);}QComboBox::down-arrow:hover{image:url(:/res/combobox_arrow_hover.png);}QComboBox::down-arrow:pressed{image:url(:/res/combobox_arrow_press.png);}QComboBox QAbstractItemView {border: 1px solid #0a9ff5;background: #1f1f1f;outline: none;}");
//    font_encoding_combo->setStyleSheet("QComboBox{width:150px;height:24px;border: 1px solid #000000;background: #0f0f0f;font-family:方正黑体_GBK;font-size:12px;color:#999999;}QComboBox::hover{background-color:#0f0f0f;border:1px solid #0a9ff5;font-family:方正黑体_GBK;font-size:12px;color:#999999;}QComboBox:enabled {background:#0f0f0f;color:#999999;}QComboBox:!enabled {background:#0f0f0f;color:#383838;}QComboBox:enabled:hover, QComboBox:enabled:focus {color: #1f1f1f;}QComboBox::drop-down {width: 17px;border: none;background: transparent;}QComboBox::drop-down:hover {background: transparent;}QComboBox::down-arrow{image:url(:/res/combobox_arrow_normal.png);}QComboBox::down-arrow:hover{image:url(:/res/combobox_arrow_hover.png);}QComboBox::down-arrow:pressed{image:url(:/res/combobox_arrow_press.png);}QComboBox QAbstractItemView {border: 1px solid #0a9ff5;background: #1f1f1f;outline: none;}");
//    enca_lang_combo->setStyleSheet("QComboBox{width:150px;height:24px;border: 1px solid #000000;background: #0f0f0f;font-family:方正黑体_GBK;font-size:12px;color:#999999;}QComboBox::hover{background-color:#0f0f0f;border:1px solid #0a9ff5;font-family:方正黑体_GBK;font-size:12px;color:#999999;}QComboBox:enabled {background:#0f0f0f;color:#999999;}QComboBox:!enabled {background:#0f0f0f;color:#383838;}QComboBox:enabled:hover, QComboBox:enabled:focus {color: #1f1f1f;}QComboBox::drop-down {width: 17px;border: none;background: transparent;}QComboBox::drop-down:hover {background: transparent;}QComboBox::down-arrow{image:url(:/res/combobox_arrow_normal.png);}QComboBox::down-arrow:hover{image:url(:/res/combobox_arrow_hover.png);}QComboBox::down-arrow:pressed{image:url(:/res/combobox_arrow_press.png);}QComboBox QAbstractItemView {border: 1px solid #0a9ff5;background: #1f1f1f;outline: none;}");

    autoload_label->setStyleSheet("QLabel{background:transparent;font-size:12px;color:#999999;font-family:方正黑体_GBK;}");
    encoding_label->setStyleSheet("QLabel{background:transparent;font-size:12px;color:#999999;font-family:方正黑体_GBK;}");

    use_enca_check->setFocusPolicy(Qt::NoFocus);

	font_autoload_combo->setCurrentIndex(font_autoload_item);

    connect(use_enca_check, SIGNAL(toggled(bool)), enca_lang_combo, SLOT(setEnabled(bool)));

	// Encodings combo
    QString current_encoding = fontEncoding();
    QString current_enca_lang = encaLang();
    font_encoding_combo->clear();
    enca_lang_combo->clear();

    QMap<QString,QString> l = Languages::encodings();
    QMapIterator<QString, QString> i(l);
    while (i.hasNext()) {
        i.next();
        font_encoding_combo->addItem( i.value() + " (" + i.key() + ")", i.key() );
    }

    l = Languages::enca();
    i = l;
    while (i.hasNext()) {
        i.next();
        enca_lang_combo->addItem( i.value() + " (" + i.key() + ")", i.key() );
    }

    font_encoding_combo->model()->sort(0);
    enca_lang_combo->model()->sort(0);
    setFontEncoding(current_encoding);
    setEncaLang(current_enca_lang);

	createHelp();
}

void PrefSubtitles::setData(Preferences * pref) {
	setFontEncoding( pref->subcp );
	setUseEnca( pref->use_enca );
	setEncaLang( pref->enca_lang );
}

void PrefSubtitles::getData(Preferences * pref) {
	requires_restart = false;

	TEST_AND_SET(pref->subcp, fontEncoding());
	TEST_AND_SET(pref->use_enca, useEnca());
	TEST_AND_SET(pref->enca_lang, encaLang());
}

void PrefSubtitles::setFontEncoding(QString s) {
	int i = font_encoding_combo->findData(s);
	font_encoding_combo->setCurrentIndex(i);
}

QString PrefSubtitles::fontEncoding() {
	int index = font_encoding_combo->currentIndex();
	return font_encoding_combo->itemData(index).toString();
}

void PrefSubtitles::setEncaLang(QString s) {
	int i = enca_lang_combo->findData(s);
	enca_lang_combo->setCurrentIndex(i);
}

QString PrefSubtitles::encaLang() {
	int index = enca_lang_combo->currentIndex();
	return enca_lang_combo->itemData(index).toString();
}

void PrefSubtitles::setUseEnca(bool b) {
	use_enca_check->setChecked(b);
}

bool PrefSubtitles::useEnca() {
	return use_enca_check->isChecked();
}

void PrefSubtitles::createHelp() {
	clearHelp();

	addSectionTitle(tr("Subtitles"));

	setWhatsThis(font_autoload_combo, tr("Autoload"), 
        tr("Select the subtitle autoload method.") );

	setWhatsThis(font_encoding_combo, tr("Default subtitle encoding"), 
        tr("Select the encoding which will be used for subtitle files "
           "by default.") );

	setWhatsThis(use_enca_check, tr("Try to autodetect for this language"),
		tr("When this option is on, the encoding of the subtitles will be "
           "tried to be autodetected for the given language. "
           "It will fall back to the default encoding if the autodetection "
           "fails. This option requires a MPlayer compiled with ENCA "
           "support.") );

	setWhatsThis(enca_lang_combo, tr("Subtitle language"),
		tr("Select the language for which you want the encoding to be guessed "
           "automatically.") );

	addSectionTitle(tr("Font"));
}

//#include "moc_prefsubtitles.cpp"
