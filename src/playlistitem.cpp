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

#include "playlistitem.h"

#include <QDebug>
#include <QTimer>
#include <QLabel>
#include <QStyle>
#include <QLineEdit>
#include <QMouseEvent>
#include <QHBoxLayout>
#include <QMenu>
#include <QStyleFactory>
#include <QMessageBox>
#include <QPushButton>
#include "../smplayer/helper.h"

PlayListItem::PlayListItem(QString mtype, QString filename, QString name, double duration, QWidget *parent) : QFrame(parent)
{
    m_filename = filename;
    if (name.isDetached())
        m_name = "lixiang";
    m_name = name;
    m_duration = duration;
    m_played = false;
    m_deleted = false;

    m_data = filename;

    this->setFixedSize(220, 32);

    this->setToolTip(filename);

    icon_label = new QLabel(this);
    icon_label->setObjectName("PlayListIcon");//0616
    icon_label->setGeometry(8,8,16,16);
    icon_label->setProperty("iconName", mtype);

    m_titleedit = new QLineEdit(this);
    m_titleedit->setObjectName("PlayListTitle");//0616
    m_titleedit->setDisabled(true);
    m_titleedit->setReadOnly(true);
    m_titleedit->setAlignment(Qt::AlignLeft);
    m_titleedit->setGeometry(30,0,105,32);

    time_label = new QLabel(this);
    time_label->setStyleSheet("QLabel{color: #ffffff;font-size: 12px;background: transparent;}");
    time_label->setText(Helper::formatTime((int)duration));
    time_label->setGeometry(140,0,50,32);

    delete_btn = new QPushButton(this);
    delete_btn->setGeometry(192,8,16,16);
    delete_btn->setFocusPolicy(Qt::NoFocus);
    delete_btn->setObjectName("PlayListDelete");
    connect(delete_btn, SIGNAL(clicked(bool)), this, SLOT(onDelete()));

    QFont font(m_titleedit->font());
    font.setPixelSize(12);
    QFontMetrics fm(font);
//    m_titleedit->setText(fm.elidedText(QString(m_name), Qt::ElideMiddle, m_titleedit->maximumWidth()));
    m_titleedit->setText(fm.elidedText(QString(m_name), Qt::ElideMiddle, 105));
}

void PlayListItem::update_widget_qss_property(QWidget *w, const char *name, const QVariant &value)
{
    w->setProperty(name, value);
    this->style()->unpolish(w);
    this->style()->polish(w);
    w->update();
}

void PlayListItem::setActive(bool active)
{
    if (active) {
        this->update_widget_qss_property(m_titleedit, "status", "active");
        this->update_widget_qss_property(delete_btn, "status", "active");

    } else {
        this->update_widget_qss_property(m_titleedit, "status", "");
        this->update_widget_qss_property(delete_btn, "status", "");
    }
}

void PlayListItem::mouseDoubleClickEvent(QMouseEvent *event)
{
//    QFrame::mouseDoubleClickEvent(event);
    emit this->sig_doubleclicked_resource(this->m_filename);

    QPoint lineeditMousePos = m_titleedit->mapFromParent(event->pos());
    if (!m_titleedit->rect().contains(lineeditMousePos)) {
        return;
    }

    if (m_titleedit->isReadOnly()) {
        return;
    }
}

void PlayListItem::onDelete()
{
    emit this->remove(m_data);
}

//kobe:更新视频的时长
void PlayListItem::update_time(QString duration)
{
    time_label->setText(duration);
}
