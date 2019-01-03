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

#include "shortcutswidget.h"
#include <QDebug>
#include <QTimer>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QFormLayout>
#include "../smplayer/helper.h"

#include "../smplayer/global.h"
#include "../smplayer/preferences.h"
using namespace Global;

ShortcutsWidget *ShortcutsWidget::self = 0;
ShortcutsWidget::ShortcutsWidget(QWidget *parent) : QWidget(parent)
{
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);
    this->setWindowTitle(tr("Kylin Video - Shortcuts"));
    timer = NULL;
    parent_widget = 0;
    title_label = NULL;
    control_title_label = NULL;
    other_title_label = NULL;
    close_button = NULL;
//    control_form_layout = NULL;
    control_grid_layout = NULL;
    other_grid_layout = NULL;
    set_widget_opacity(0.7);
    set_background_color(QColor(0, 0, 0));

    this->init_ui();
}

ShortcutsWidget::~ShortcutsWidget()
{
    self = 0;
    if (timer != NULL) {
        disconnect(timer,SIGNAL(timeout()),this,SLOT(hide_mask_widget()));
        if(timer->isActive()) {
            timer->stop();
        }
        delete timer;
        timer = NULL;
    }
    if (title_label != NULL) {
        delete title_label;
        title_label = NULL;
    }
    if (control_title_label != NULL) {
        delete control_title_label;
        control_title_label = NULL;
    }
    if (other_title_label != NULL) {
        delete other_title_label;
        other_title_label = NULL;
    }
    if (close_button != NULL) {
        delete close_button;
        close_button = NULL;
    }
    while(control_grid_layout != NULL && control_grid_layout->count() > 0)
    {
        QWidget *widget = control_grid_layout->itemAt(0)->widget();
        control_grid_layout->removeWidget(widget);
        delete widget;
        widget = NULL;
    }
//    for (int i=0; i < control_label_list.count(); i++) {
//        control_grid_layout->removeWidget(control_label_list[i]);
////        control_form_layout->removeWidget(control_label_list[i]);
//        delete control_label_list[i];
//        control_label_list[i] = NULL;
//    }
//    control_label_list.clear();
    if (control_grid_layout != NULL) {
        delete control_grid_layout;
        control_grid_layout = NULL;
    }
    if (other_title_label != NULL) {
        delete other_title_label;
        other_title_label = NULL;
    }
//    for (int j=0; j < other_label_list.count(); j++) {
//        other_grid_layout->removeWidget(other_label_list[j]);
//        delete other_label_list[j];
//        other_label_list[j] = NULL;
//    }
//    other_label_list.clear();
    while(other_grid_layout != NULL && other_grid_layout->count() > 0)
    {
        QWidget *widget = other_grid_layout->itemAt(0)->widget();
        other_grid_layout->removeWidget(widget);
        delete widget;
        widget = NULL;
    }
    if (other_grid_layout != NULL) {
        delete other_grid_layout;
        other_grid_layout = NULL;
    }
//    if(control_form_layout != NULL)
//    {
//        delete control_form_layout;
//        control_form_layout = NULL;
//    }
}

void ShortcutsWidget::set_parent_widget(QWidget *widget)
{
    if (this->parent_widget != widget) {
        this->parent_widget = widget;
    }
}

void ShortcutsWidget::setPrefData(Preferences *pref)
{
    forward_10s_title->setText(tr("Forward %1").arg(Helper::timeForJumps(pref->seeking1)));
    forward_1m_title->setText(tr("Forward %1").arg(Helper::timeForJumps(pref->seeking2)));
    forward_10m_title->setText(tr("Forward %1").arg(Helper::timeForJumps(pref->seeking3)));

    back_10s_title->setText(tr("Rewind %1").arg(Helper::timeForJumps(pref->seeking1)));
    back_1m_title->setText(tr("Rewind %1").arg(Helper::timeForJumps(pref->seeking2)));
    back_10m_title->setText(tr("Rewind %1").arg(Helper::timeForJumps(pref->seeking3)));
}

void ShortcutsWidget::init_ui()
{
    control_grid_layout = new QGridLayout;
    control_grid_layout->setSpacing(10);
    control_grid_layout->setContentsMargins(0, 0, 0, 0);

    other_grid_layout = new QGridLayout;
    other_grid_layout->setSpacing(10);
    other_grid_layout->setContentsMargins(0, 0, 0, 0);

    title_label = new QLabel();
    title_label->setText(tr("Kylin Video - Shortcuts"));
    title_label->setFrameShape(QFrame::NoFrame);
    title_label->setStyleSheet("QLabel{background:transparent;font-size:16px;color:#ffffff;font-family:方正黑体_GBK;}");
    title_label->adjustSize();

    control_title_label = new QLabel();
    control_title_label->setText(tr("Play control"));
    control_title_label->setFrameShape(QFrame::NoFrame);
    control_title_label->setStyleSheet("QLabel{background:transparent;font-size:14px;color:#ffffff;font-family:方正黑体_GBK;}");
    control_title_label->adjustSize();

    other_title_label = new QLabel();
    other_title_label->setText(tr("Other control"));
    other_title_label->setFrameShape(QFrame::NoFrame);
    other_title_label->setStyleSheet("QLabel{background:transparent;font-size:14px;color:#ffffff;font-family:方正黑体_GBK;}");
    other_title_label->adjustSize();

    close_button = new QPushButton();
    close_button->setFixedSize(91, 25);
    close_button->setText(tr("Close"));
    close_button->setFocusPolicy(Qt::NoFocus);
    close_button->setStyleSheet("QPushButton{font-size:12px;background:#0f0f0f;border:1px solid #0a9ff5;color:#999999;}QPushButton:hover{background-color:#0a9ff5;border:1px solid #2db0f6;color:#ffffff;} QPushButton:pressed{background-color:#0993e3;border:1px solid #0a9ff5;color:#ffffff;}");
    connect(close_button, SIGNAL(clicked(bool)), this, SLOT(onCloseButtonClicked()));
//    close_button->setAutoDefault(true);
//    close_button->setFocus();
//    QWidget::setTabOrder(close_button, title_label);

    /*control_form_layout = new QFormLayout();
//    control_form_layout->setSizeConstraint(QLayout::SetFixedSize);//frame will fixed with content's width
    control_form_layout->setSpacing(10);
    control_form_layout->setHorizontalSpacing(10);
    control_form_layout->setRowWrapPolicy(QFormLayout::DontWrapRows);
    control_form_layout->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);*/

    play_title = new QLabel();
    play_title->setText(tr("Play/Pause"));
    play_title->setFrameShape(QFrame::NoFrame);
    play_title->setStyleSheet("QLabel{background:transparent;font-size:12px;color:#ffffff;font-family:方正黑体_GBK;}");
    play_title->adjustSize();
    control_grid_layout->addWidget(play_title,0,0);
//    control_label_list.append(play_title);

    play_label = new QLabel();
    play_label->setText("Space");
    play_label->setFrameShape(QFrame::NoFrame);
    play_label->setStyleSheet("QLabel{background:transparent;font-size:12px;color:#ffffff;font-family:方正黑体_GBK;}");
    play_label->adjustSize();
    control_grid_layout->addWidget(play_label,0,1);
//    control_label_list.append(play_label);
//    control_form_layout->addRow(tr("Play/Pause"), play_label);

    play_pre_title = new QLabel();
    play_pre_title->setText(tr("Previous"));
    play_pre_title->setFrameShape(QFrame::NoFrame);
    play_pre_title->setStyleSheet("QLabel{background:transparent;font-size:12px;color:#ffffff;font-family:方正黑体_GBK;}");
    play_pre_title->adjustSize();
    control_grid_layout->addWidget(play_pre_title,1,0);
//    control_label_list.append(play_pre_title);

    play_pre_label = new QLabel();
    play_pre_label->setText(pref->prev_key);
    play_pre_label->setFrameShape(QFrame::NoFrame);
    play_pre_label->setStyleSheet("QLabel{background:transparent;font-size:12px;color:#ffffff;font-family:方正黑体_GBK;}");
    play_pre_label->adjustSize();
    control_grid_layout->addWidget(play_pre_label,1,1);
//    control_label_list.append(play_pre_label);

    play_next_title = new QLabel();
    play_next_title->setText(tr("Next"));
    play_next_title->setFrameShape(QFrame::NoFrame);
    play_next_title->setStyleSheet("QLabel{background:transparent;font-size:12px;color:#ffffff;font-family:方正黑体_GBK;}");
    play_next_title->adjustSize();
    control_grid_layout->addWidget(play_next_title,2,0);
//    control_label_list.append(play_next_title);

    play_next_label = new QLabel();
    play_next_label->setText(pref->next_key);//">, Media Next"
    play_next_label->setFrameShape(QFrame::NoFrame);
    play_next_label->setStyleSheet("QLabel{background:transparent;font-size:12px;color:#ffffff;font-family:方正黑体_GBK;}");
    play_next_label->adjustSize();
    control_grid_layout->addWidget(play_next_label,2,1);
//    control_label_list.append(play_next_label);

    forward_10s_title = new QLabel();
    forward_10s_title->setFrameShape(QFrame::NoFrame);
    forward_10s_title->setStyleSheet("QLabel{background:transparent;font-size:12px;color:#ffffff;font-family:方正黑体_GBK;}");
    forward_10s_title->adjustSize();
    control_grid_layout->addWidget(forward_10s_title,3,0);
//    control_label_list.append(forward_10s_title);

    forward_10s_label = new QLabel();
    forward_10s_label->setText("Right(→)");
    forward_10s_label->setFrameShape(QFrame::NoFrame);
    forward_10s_label->setStyleSheet("QLabel{background:transparent;font-size:12px;color:#ffffff;font-family:方正黑体_GBK;}");
    forward_10s_label->adjustSize();
    control_grid_layout->addWidget(forward_10s_label,3,1);
//    control_label_list.append(forward_10s_label);

    forward_1m_title = new QLabel();
    forward_1m_title->setFrameShape(QFrame::NoFrame);
    forward_1m_title->setStyleSheet("QLabel{background:transparent;font-size:12px;color:#ffffff;font-family:方正黑体_GBK;}");
    forward_1m_title->adjustSize();
    control_grid_layout->addWidget(forward_1m_title,4,0);
//    control_label_list.append(forward_1m_title);

    forward_1m_label = new QLabel();
    forward_1m_label->setText("Up(↑)");
    forward_1m_label->setFrameShape(QFrame::NoFrame);
    forward_1m_label->setStyleSheet("QLabel{background:transparent;font-size:12px;color:#ffffff;font-family:方正黑体_GBK;}");
    forward_1m_label->adjustSize();
    control_grid_layout->addWidget(forward_1m_label,4,1);
//    control_label_list.append(forward_1m_label);

    forward_10m_title = new QLabel();
    forward_10m_title->setFrameShape(QFrame::NoFrame);
    forward_10m_title->setStyleSheet("QLabel{background:transparent;font-size:12px;color:#ffffff;font-family:方正黑体_GBK;}");
    forward_10m_title->adjustSize();
    control_grid_layout->addWidget(forward_10m_title,5,0);
//    control_label_list.append(forward_10m_title);

    forward_10m_label = new QLabel();
    forward_10m_label->setText("PgUp");
    forward_10m_label->setFrameShape(QFrame::NoFrame);
    forward_10m_label->setStyleSheet("QLabel{background:transparent;font-size:12px;color:#ffffff;font-family:方正黑体_GBK;}");
    forward_10m_label->adjustSize();
    control_grid_layout->addWidget(forward_10m_label,5,1);
//    control_label_list.append(forward_10m_label);

    back_10s_title = new QLabel();
    back_10s_title->setFrameShape(QFrame::NoFrame);
    back_10s_title->setStyleSheet("QLabel{background:transparent;font-size:12px;color:#ffffff;font-family:方正黑体_GBK;}");
    back_10s_title->adjustSize();
    control_grid_layout->addWidget(back_10s_title,6,0);
//    control_label_list.append(back_10s_title);

    back_10s_label = new QLabel();
    back_10s_label->setText("Left(←)");
    back_10s_label->setFrameShape(QFrame::NoFrame);
    back_10s_label->setStyleSheet("QLabel{background:transparent;font-size:12px;color:#ffffff;font-family:方正黑体_GBK;}");
    back_10s_label->adjustSize();
    control_grid_layout->addWidget(back_10s_label,6,1);
//    control_label_list.append(back_10s_label);

    back_1m_title = new QLabel();
    back_1m_title->setFrameShape(QFrame::NoFrame);
    back_1m_title->setStyleSheet("QLabel{background:transparent;font-size:12px;color:#ffffff;font-family:方正黑体_GBK;}");
    back_1m_title->adjustSize();
    control_grid_layout->addWidget(back_1m_title,7,0);
//    control_label_list.append(back_1m_title);

    back_1m_label = new QLabel();
    back_1m_label->setText("Down(↓)");
    back_1m_label->setFrameShape(QFrame::NoFrame);
    back_1m_label->setStyleSheet("QLabel{background:transparent;font-size:12px;color:#ffffff;font-family:方正黑体_GBK;}");
    back_1m_label->adjustSize();
    control_grid_layout->addWidget(back_1m_label,7,1);
//    control_label_list.append(back_1m_label);

    back_10m_title = new QLabel();
    back_10m_title->setFrameShape(QFrame::NoFrame);
    back_10m_title->setStyleSheet("QLabel{background:transparent;font-size:12px;color:#ffffff;font-family:方正黑体_GBK;}");
    back_10m_title->adjustSize();
    control_grid_layout->addWidget(back_10m_title,8,0);
//    control_label_list.append(back_10m_title);

    back_10m_label = new QLabel();
    back_10m_label->setText("PgDn");
    back_10m_label->setFrameShape(QFrame::NoFrame);
    back_10m_label->setStyleSheet("QLabel{background:transparent;font-size:12px;color:#ffffff;font-family:方正黑体_GBK;}");
    back_10m_label->adjustSize();
    control_grid_layout->addWidget(back_10m_label,8,1);
//    control_label_list.append(back_10m_label);

    jump_title = new QLabel();
    jump_title->setText(tr("Jump to..."));
    jump_title->setFrameShape(QFrame::NoFrame);
    jump_title->setStyleSheet("QLabel{background:transparent;font-size:12px;color:#ffffff;font-family:方正黑体_GBK;}");
    play_next_title->adjustSize();
    control_grid_layout->addWidget(jump_title,9,0);
//    control_label_list.append(jump_title);

    jump_label = new QLabel();
    jump_label->setText("Ctrl + J");
    jump_label->setFrameShape(QFrame::NoFrame);
    jump_label->setStyleSheet("QLabel{background:transparent;font-size:12px;color:#ffffff;font-family:方正黑体_GBK;}");
    jump_label->adjustSize();
    control_grid_layout->addWidget(jump_label,9,1);
//    control_label_list.append(jump_label);

    mute_title = new QLabel();
    mute_title->setText(tr("Mute"));
    mute_title->setFrameShape(QFrame::NoFrame);
    mute_title->setStyleSheet("QLabel{background:transparent;font-size:12px;color:#ffffff;font-family:方正黑体_GBK;}");
    mute_title->adjustSize();
    control_grid_layout->addWidget(mute_title,10,0);
//    control_label_list.append(mute_title);

    mute_label = new QLabel();
    mute_label->setText("M");
    mute_label->setFrameShape(QFrame::NoFrame);
    mute_label->setStyleSheet("QLabel{background:transparent;font-size:12px;color:#ffffff;font-family:方正黑体_GBK;}");
    mute_label->adjustSize();
    control_grid_layout->addWidget(mute_label,10,1);
//    control_label_list.append(mute_label);

    vol_up_title = new QLabel();
    vol_up_title->setText(tr("Volume +"));
    vol_up_title->setFrameShape(QFrame::NoFrame);
    vol_up_title->setStyleSheet("QLabel{background:transparent;font-size:12px;color:#ffffff;font-family:方正黑体_GBK;}");
    vol_up_title->adjustSize();
    control_grid_layout->addWidget(vol_up_title,11,0);
//    control_label_list.append(vol_up_title);

    vol_up_label = new QLabel();
    vol_up_label->setText("0");
    vol_up_label->setFrameShape(QFrame::NoFrame);
    vol_up_label->setStyleSheet("QLabel{background:transparent;font-size:12px;color:#ffffff;font-family:方正黑体_GBK;}");
    vol_up_label->adjustSize();
    control_grid_layout->addWidget(vol_up_label,11,1);
//    control_label_list.append(vol_up_label);

    vol_down_title = new QLabel();
    vol_down_title->setText(tr("Volume -"));
    vol_down_title->setFrameShape(QFrame::NoFrame);
    vol_down_title->setStyleSheet("QLabel{background:transparent;font-size:12px;color:#ffffff;font-family:方正黑体_GBK;}");
    vol_down_title->adjustSize();
    control_grid_layout->addWidget(vol_down_title,12,0);
//    control_label_list.append(vol_down_title);

    vol_down_label = new QLabel();
    vol_down_label->setText("9");
    vol_down_label->setFrameShape(QFrame::NoFrame);
    vol_down_label->setStyleSheet("QLabel{background:transparent;font-size:12px;color:#ffffff;font-family:方正黑体_GBK;}");
    vol_down_label->adjustSize();
    control_grid_layout->addWidget(vol_down_label,12,1);
//    control_label_list.append(vol_down_label);

    set_audio_delay_title = new QLabel();
    set_audio_delay_title->setText(tr("Set audio delay"));
    set_audio_delay_title->setFrameShape(QFrame::NoFrame);
    set_audio_delay_title->setStyleSheet("QLabel{background:transparent;font-size:12px;color:#ffffff;font-family:方正黑体_GBK;}");
    set_audio_delay_title->adjustSize();
    control_grid_layout->addWidget(set_audio_delay_title,13,0);
//    control_label_list.append(set_audio_delay_title);

    set_audio_delay_label = new QLabel();
    set_audio_delay_label->setText("Y");
    set_audio_delay_label->setFrameShape(QFrame::NoFrame);
    set_audio_delay_label->setStyleSheet("QLabel{background:transparent;font-size:12px;color:#ffffff;font-family:方正黑体_GBK;}");
    set_audio_delay_label->adjustSize();
    control_grid_layout->addWidget(set_audio_delay_label,13,1);
//    control_label_list.append(set_audio_delay_label);

    audio_delay_title = new QLabel();
    audio_delay_title->setText(tr("Increase or decrease audio delay"));
    audio_delay_title->setFrameShape(QFrame::NoFrame);
    audio_delay_title->setStyleSheet("QLabel{background:transparent;font-size:12px;color:#ffffff;font-family:方正黑体_GBK;}");
    audio_delay_title->adjustSize();
    control_grid_layout->addWidget(audio_delay_title,14,0);
//    control_label_list.append(audio_delay_title);

    audio_delay_label = new QLabel();
    audio_delay_label->setText("+ / - / =");
    audio_delay_label->setFrameShape(QFrame::NoFrame);
    audio_delay_label->setStyleSheet("QLabel{background:transparent;font-size:12px;color:#ffffff;font-family:方正黑体_GBK;}");
    audio_delay_label->adjustSize();
    control_grid_layout->addWidget(audio_delay_label,14,1);
//    control_label_list.append(audio_delay_label);


    playlist_title = new QLabel();
    playlist_title->setText(tr("Playlist"));
    playlist_title->setFrameShape(QFrame::NoFrame);
    playlist_title->setStyleSheet("QLabel{background:transparent;font-size:12px;color:#ffffff;font-family:方正黑体_GBK;}");
    playlist_title->adjustSize();
    other_grid_layout->addWidget(playlist_title,0,0);

    playlist_label = new QLabel();
    playlist_label->setText(pref->playlist_key);//F3
    playlist_label->setFrameShape(QFrame::NoFrame);
    playlist_label->setStyleSheet("QLabel{background:transparent;font-size:12px;color:#ffffff;font-family:方正黑体_GBK;}");
    playlist_label->adjustSize();
    other_grid_layout->addWidget(playlist_label,0,1);

    open_title = new QLabel();
    open_title->setText(tr("Open File"));
    open_title->setFrameShape(QFrame::NoFrame);
    open_title->setStyleSheet("QLabel{background:transparent;font-size:12px;color:#ffffff;font-family:方正黑体_GBK;}");
    open_title->adjustSize();
    other_grid_layout->addWidget(open_title,1,0);
//    other_label_list.append(open_title);

    open_label = new QLabel();
    open_label->setText("Ctrl + F");
    open_label->setFrameShape(QFrame::NoFrame);
    open_label->setStyleSheet("QLabel{background:transparent;font-size:12px;color:#ffffff;font-family:方正黑体_GBK;}");
    open_label->adjustSize();
    other_grid_layout->addWidget(open_label,1,1);
//    other_label_list.append(open_label);

    screenshot_title = new QLabel();
    screenshot_title->setText(tr("Screenshot"));
    screenshot_title->setFrameShape(QFrame::NoFrame);
    screenshot_title->setStyleSheet("QLabel{background:transparent;font-size:12px;color:#ffffff;font-family:方正黑体_GBK;}");
    screenshot_title->adjustSize();
    other_grid_layout->addWidget(screenshot_title,2,0);
//    other_label_list.append(screenshot_title);

    screenshot_label = new QLabel();
    screenshot_label->setText("S");
    screenshot_label->setFrameShape(QFrame::NoFrame);
    screenshot_label->setStyleSheet("QLabel{background:transparent;font-size:12px;color:#ffffff;font-family:方正黑体_GBK;}");
    screenshot_label->adjustSize();
    other_grid_layout->addWidget(screenshot_label,2,1);
//    other_label_list.append(screenshot_label);

    pref_title = new QLabel();
    pref_title->setText(tr("Preferences"));
    pref_title->setFrameShape(QFrame::NoFrame);
    pref_title->setStyleSheet("QLabel{background:transparent;font-size:12px;color:#ffffff;font-family:方正黑体_GBK;}");
    pref_title->adjustSize();
    other_grid_layout->addWidget(pref_title,3,0);
//    other_label_list.append(pref_title);

    pref_label = new QLabel();
    pref_label->setText("Ctrl + P");
    pref_label->setFrameShape(QFrame::NoFrame);
    pref_label->setStyleSheet("QLabel{background:transparent;font-size:12px;color:#ffffff;font-family:方正黑体_GBK;}");
    pref_label->adjustSize();
    other_grid_layout->addWidget(pref_label,3,1);
//    other_label_list.append(pref_label);

    info_title = new QLabel();
    info_title->setText(tr("View info and properties..."));
    info_title->setFrameShape(QFrame::NoFrame);
    info_title->setStyleSheet("QLabel{background:transparent;font-size:12px;color:#ffffff;font-family:方正黑体_GBK;}");
    info_title->adjustSize();
    other_grid_layout->addWidget(info_title,4,0);
//    other_label_list.append(info_title);

    info_label = new QLabel();
    info_label->setText("Ctrl + I");
    info_label->setFrameShape(QFrame::NoFrame);
    info_label->setStyleSheet("QLabel{background:transparent;font-size:12px;color:#ffffff;font-family:方正黑体_GBK;}");
    info_label->adjustSize();
    other_grid_layout->addWidget(info_label,4,1);
//    other_label_list.append(info_label);

    about_title = new QLabel();
    about_title->setText(tr("About"));
    about_title->setFrameShape(QFrame::NoFrame);
    about_title->setStyleSheet("QLabel{background:transparent;font-size:12px;color:#ffffff;font-family:方正黑体_GBK;}");
    about_title->adjustSize();
    other_grid_layout->addWidget(about_title,5,0);

    about_label = new QLabel();
    about_label->setText("Ctrl + A");
    about_label->setFrameShape(QFrame::NoFrame);
    about_label->setStyleSheet("QLabel{background:transparent;font-size:12px;color:#ffffff;font-family:方正黑体_GBK;}");
    about_label->adjustSize();
    other_grid_layout->addWidget(about_label,5,1);

    quit_title = new QLabel();
    quit_title->setText(tr("Quit"));
    quit_title->setFrameShape(QFrame::NoFrame);
    quit_title->setStyleSheet("QLabel{background:transparent;font-size:12px;color:#ffffff;font-family:方正黑体_GBK;}");
    quit_title->adjustSize();
    other_grid_layout->addWidget(quit_title,6,0);

    quit_label = new QLabel();
    quit_label->setText("Ctrl + Q");
    quit_label->setFrameShape(QFrame::NoFrame);
    quit_label->setStyleSheet("QLabel{background:transparent;font-size:12px;color:#ffffff;font-family:方正黑体_GBK;}");
    quit_label->adjustSize();
    other_grid_layout->addWidget(quit_label,6,1);

    fullscreen_title = new QLabel();
    fullscreen_title->setText(tr("FullScreen/Cancel fullScreen"));
    fullscreen_title->setFrameShape(QFrame::NoFrame);
    fullscreen_title->setStyleSheet("QLabel{background:transparent;font-size:12px;color:#ffffff;font-family:方正黑体_GBK;}");
    fullscreen_title->adjustSize();
    other_grid_layout->addWidget(fullscreen_title,7,0);

    fullscreen_label = new QLabel();
    fullscreen_label->setText("Ctrl + Enter");
    fullscreen_label->setFrameShape(QFrame::NoFrame);
    fullscreen_label->setStyleSheet("QLabel{background:transparent;font-size:12px;color:#ffffff;font-family:方正黑体_GBK;}");
    fullscreen_label->adjustSize();
    other_grid_layout->addWidget(fullscreen_label,7,1);

    QVBoxLayout *lhlayout = new QVBoxLayout;
    lhlayout->setSpacing(10);
    lhlayout->addWidget(control_title_label);
    lhlayout->addLayout(control_grid_layout);
    lhlayout->addStretch();

    QVBoxLayout *rhlayout = new QVBoxLayout;
    rhlayout->setSpacing(10);
    rhlayout->addWidget(other_title_label);
    rhlayout->addLayout(other_grid_layout);
    rhlayout->addStretch();

    QHBoxLayout *hlayout = new QHBoxLayout;
    hlayout->addStretch();
//    hlayout->addLayout(control_form_layout);
    hlayout->addLayout(lhlayout);
    hlayout->addStretch();
    hlayout->addLayout(rhlayout);
    hlayout->addStretch();

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(10, 40, 10, 30);
    layout->setSpacing(20);
//    layout->setSizeConstraint(QLayout::SetFixedSize);
    layout->addWidget(title_label);
    layout->addStretch();
    layout->addLayout(hlayout);
    layout->addStretch();
    layout->addWidget(close_button, 0, Qt::AlignHCenter);

    this->setLayout(layout);
}

void ShortcutsWidget::onCloseButtonClicked()
{
    if (timer->isActive())
        timer->stop();
    this->hide_mask_widget();
}

void ShortcutsWidget::restart_timer()
{
    if (timer == NULL) {
        timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), this, SLOT(hide_mask_widget()));
        timer->setInterval(10000);
    }
    if (timer->isActive())
        timer->stop();
    timer->start();
}

void ShortcutsWidget::set_widget_opacity(double opacity)
{
    this->setWindowOpacity(opacity);
}

void ShortcutsWidget::set_background_color(const QColor &bgColor)
{
    QPalette palette = this->palette();
    palette.setBrush(QPalette::Background, bgColor);
    this->setPalette(palette);
}

void ShortcutsWidget::showEvent(QShowEvent *)
{
    if (parent_widget != 0) {
        this->setGeometry(parent_widget->geometry());
    }
}

void ShortcutsWidget::hide_mask_widget()
{
    if (this->isVisible()) {
        this->hide();
    }
}

//void ShortcutsWidget::keyReleaseEvent(QKeyEvent *event) {
//    if (this->isVisible()) {
//        this->hide();
//    }
//    QWidget::keyReleaseEvent(event);
//}
