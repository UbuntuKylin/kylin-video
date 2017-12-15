/*
 * Copyright (C) 2013 ~ 2017 National University of Defense Technology(NUDT) & Tianjin Kylin Ltd.
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

#include "supportshortcuts.h"
#include <QDebug>
#include <QScrollBar>
#include "../smplayer/extensions.h"
#include "../smplayer/helper.h"

#include "../smplayer/global.h"
#include "../smplayer/preferences.h"
using namespace Global;

SupportShortcuts::SupportShortcuts(QWidget * parent, Qt::WindowFlags f)
    : QWidget(parent, f )
{
	setupUi(this);
//    groupBox_play->setParent(this);
//    groupBox_other->setParent(this);
	retranslateStrings();
}

SupportShortcuts::~SupportShortcuts()
{
}

void SupportShortcuts::retranslateStrings() {
	retranslateUi(this);

    groupBox_play->setStyleSheet("QGroupBox{border:none;margin-top:20px;font-size:14px;}QGroupBox:title{subcontrol-origin: margin;subcontrol-position: top left;padding: 0px 1px;color: #999999;font-family: 方正黑体_GBK;font-weight:bold;}");
    groupBox_other->setStyleSheet("QGroupBox{border:none;margin-top:20px;font-size:14px;}QGroupBox:title{subcontrol-origin: margin;subcontrol-position: top left;padding: 0px 1px;color: #999999;font-family: 方正黑体_GBK;font-weight:bold;}");

    play_edit->setStyleSheet("QTextEdit {border: 1px solid #000000;color: #999999;background: #0f0f0f;font-family:方正黑体_GBK;font-size: 12px;}");
    play_edit->verticalScrollBar()->setStyleSheet("QScrollBar:vertical {width: 12px;background: #141414;margin:0px 0px 0px 0px;border:1px solid #141414;}QScrollBar::handle:vertical {width: 12px;min-height: 45px;background: #292929;margin-left: 0px;margin-right: 0px;}QScrollBar::handle:vertical:hover {background: #3e3e3e;}QScrollBar::handle:vertical:pressed {background: #272727;}QScrollBar::sub-line:vertical {height: 6px;background: transparent;subcontrol-position: top;}QScrollBar::add-line:vertical {height: 6px;background: transparent;subcontrol-position: bottom;}QScrollBar::sub-line:vertical:hover {background: #292929;}QScrollBar::add-line:vertical:hover {background: #292929;}QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {background: transparent;}");

    other_edit->setStyleSheet("QTextEdit {border: 1px solid #000000;color: #999999;background: #0f0f0f;font-family:方正黑体_GBK;font-size: 12px;}");
    other_edit->verticalScrollBar()->setStyleSheet("QScrollBar:vertical {width: 12px;background: #141414;margin:0px 0px 0px 0px;border:1px solid #141414;}QScrollBar::handle:vertical {width: 12px;min-height: 45px;background: #292929;margin-left: 0px;margin-right: 0px;}QScrollBar::handle:vertical:hover {background: #3e3e3e;}QScrollBar::handle:vertical:pressed {background: #272727;}QScrollBar::sub-line:vertical {height: 6px;background: transparent;subcontrol-position: top;}QScrollBar::add-line:vertical {height: 6px;background: transparent;subcontrol-position: bottom;}QScrollBar::sub-line:vertical:hover {background: #292929;}QScrollBar::add-line:vertical:hover {background: #292929;}QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {background: transparent;}");
}

void SupportShortcuts::setData(Preferences * pref) {
    play_edit->clear();
    other_edit->clear();

    play_edit->append(tr("Play/Pause:   Space"));
//    play_edit->append(tr("Previous:     <, Media Previous"));
//    play_edit->append(tr("Next:     >, Media Next"));
    play_edit->append(tr("Previous:     %1").arg(pref->prev_key));
    play_edit->append(tr("Next:     %1").arg(pref->next_key));

    QString line = QString(tr("Forward %1:     Right(→)").arg(Helper::timeForJumps(pref->seeking1)));
    play_edit->append(line);
    line = QString(tr("Forward %1:     Up(↑)").arg(Helper::timeForJumps(pref->seeking2)));
    play_edit->append(line);
    line = QString(tr("Forward %1:     PgUp").arg(Helper::timeForJumps(pref->seeking3)));
    play_edit->append(line);
    line = QString(tr("Rewind %1:     Left(←)").arg(Helper::timeForJumps(pref->seeking1)));
    play_edit->append(line);
    line = QString(tr("Rewind %1:     Down(↓)").arg(Helper::timeForJumps(pref->seeking2)));
    play_edit->append(line);
    line = QString(tr("Rewind %1:     PgDn").arg(Helper::timeForJumps(pref->seeking3)));
    play_edit->append(line);
    play_edit->append(tr("Jump to...:     Ctrl + J"));
    play_edit->append(tr("Mute:     M"));
    play_edit->append(tr("Volume +:     9"));
    play_edit->append(tr("Volume -:     0"));
    play_edit->append(tr("Set audio delay:     Y"));
    play_edit->append(tr("Increase or decrease audio delay:     + / - / ="));


    other_edit->append(tr("Playlist:   %1").arg(pref->playlist_key));//F3
    other_edit->append(tr("Open File:     Ctrl + F"));
    other_edit->append(tr("Screenshot:     S"));
    other_edit->append(tr("Preferences:     Ctrl + P"));
    other_edit->append(tr("View info and properties...:     Ctrl + I"));
    other_edit->append(tr("About:   Ctrl + A"));
    other_edit->append(tr("Quit:   Ctrl + Q"));
    other_edit->append(tr("FullScreen/Cancel fullScreen:   Ctrl + Enter"));

    play_edit->verticalScrollBar()->setValue(0);//滚动到最顶层  无效??????????????????????
//    play_edit->verticalScrollBar()->setValue(play_edit->verticalScrollBar()->maximum());//滚动到最底层
    other_edit->verticalScrollBar()->setValue(0);
}

//#include "moc_supportshortcuts.cpp"
