/*
 * Copyright (C) 2013 ~ 2019 National University of Defense Technology(NUDT) & Tianjin Kylin Ltd.
 *
 * Authors:
 *  Kobe Lee    lixiang@kylinos.cn/kobe24_lixiang@126.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "supportformats.h"
#include <QDebug>
#include <QScrollBar>
#include "../smplayer/extensions.h"

SupportFormats::SupportFormats(QWidget * parent, Qt::WindowFlags f)
    : QWidget(parent, f )
{
	setupUi(this);
	retranslateStrings();
}

SupportFormats::~SupportFormats()
{
}

void SupportFormats::retranslateStrings() {
	retranslateUi(this);

    groupBox_video->setStyleSheet("QGroupBox{border:none;margin-top:20px;font-size:14px;}QGroupBox:title{subcontrol-origin: margin;subcontrol-position: top left;padding: 0px 1px;color: #999999;font-family: 方正黑体_GBK;font-weight:bold;}");
    groupBox_audio->setStyleSheet("QGroupBox{border:none;margin-top:20px;font-size:14px;}QGroupBox:title{subcontrol-origin: margin;subcontrol-position: top left;padding: 0px 1px;color: #999999;font-family: 方正黑体_GBK;font-weight:bold;}");
    groupBox_subtitles->setStyleSheet("QGroupBox{border:none;margin-top:20px;font-size:14px;}QGroupBox:title{subcontrol-origin: margin;subcontrol-position: top left;padding: 0px 1px;color: #999999;font-family: 方正黑体_GBK;font-weight:bold;}");

    vedio_edit->setStyleSheet("QTextEdit {border: 1px solid #000000;color: #999999;background: #0f0f0f;font-family:方正黑体_GBK;font-size: 12px;}");
    vedio_edit->verticalScrollBar()->setStyleSheet("QScrollBar:vertical {width: 12px;background: #141414;margin:0px 0px 0px 0px;border:1px solid #141414;}QScrollBar::handle:vertical {width: 12px;min-height: 45px;background: #292929;margin-left: 0px;margin-right: 0px;}QScrollBar::handle:vertical:hover {background: #3e3e3e;}QScrollBar::handle:vertical:pressed {background: #272727;}QScrollBar::sub-line:vertical {height: 6px;background: transparent;subcontrol-position: top;}QScrollBar::add-line:vertical {height: 6px;background: transparent;subcontrol-position: bottom;}QScrollBar::sub-line:vertical:hover {background: #292929;}QScrollBar::add-line:vertical:hover {background: #292929;}QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {background: transparent;}");

    audio_edit->setStyleSheet("QTextEdit {border: 1px solid #000000;color: #999999;background: #0f0f0f;font-family:方正黑体_GBK;font-size: 12px;}");
    audio_edit->verticalScrollBar()->setStyleSheet("QScrollBar:vertical {width: 12px;background: #141414;margin:0px 0px 0px 0px;border:1px solid #141414;}QScrollBar::handle:vertical {width: 12px;min-height: 45px;background: #292929;margin-left: 0px;margin-right: 0px;}QScrollBar::handle:vertical:hover {background: #3e3e3e;}QScrollBar::handle:vertical:pressed {background: #272727;}QScrollBar::sub-line:vertical {height: 6px;background: transparent;subcontrol-position: top;}QScrollBar::add-line:vertical {height: 6px;background: transparent;subcontrol-position: bottom;}QScrollBar::sub-line:vertical:hover {background: #292929;}QScrollBar::add-line:vertical:hover {background: #292929;}QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {background: transparent;}");

    subtitles_edit->setStyleSheet("QTextEdit {border: 1px solid #000000;color: #999999;background: #0f0f0f;font-family:方正黑体_GBK;font-size: 12px;}");
    subtitles_edit->verticalScrollBar()->setStyleSheet("QScrollBar:vertical {width: 12px;background: #141414;margin:0px 0px 0px 0px;border:1px solid #141414;}QScrollBar::handle:vertical {width: 12px;min-height: 45px;background: #292929;margin-left: 0px;margin-right: 0px;}QScrollBar::handle:vertical:hover {background: #3e3e3e;}QScrollBar::handle:vertical:pressed {background: #272727;}QScrollBar::sub-line:vertical {height: 6px;background: transparent;subcontrol-position: top;}QScrollBar::add-line:vertical {height: 6px;background: transparent;subcontrol-position: bottom;}QScrollBar::sub-line:vertical:hover {background: #292929;}QScrollBar::add-line:vertical:hover {background: #292929;}QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {background: transparent;}");

    QString video_tip = QString(tr("Some video formats do not support preview and seek by dragging, e.g. the swf."));
    vedio_edit->setToolTip("<qt>"+ video_tip +"</qt>");
}

void SupportFormats::setData() {
    vedio_edit->clear();
    audio_edit->clear();
    subtitles_edit->clear();
    Extensions e;
    vedio_edit->setText(e.video().forFilter());
    audio_edit->setText(e.audio().forFilter());
    subtitles_edit->setText(e.subtitles().forFilter());
    vedio_edit->verticalScrollBar()->setValue(0);
    audio_edit->verticalScrollBar()->setValue(0);
    subtitles_edit->verticalScrollBar()->setValue(0);
}

//#include "moc_supportformats.cpp"
