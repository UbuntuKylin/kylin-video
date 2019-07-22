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

#include "maskwidget.h"

#include <QMovie>
#include <QLabel>
#include <QVBoxLayout>
#include <QDebug>

MaskWidget *MaskWidget::self = 0;
MaskWidget::MaskWidget(QWidget *parent) : QWidget(parent)
{
    this->resize(0,0);
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_StyledBackground);
    this->setStyleSheet("QWidget{background:rgba(0,0,0,200);}");
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setWindowOpacity(0.7);

    QVBoxLayout *m_hlayout = new QVBoxLayout(this);
    m_hlayout->setContentsMargins(5, 5, 5, 5);
    m_hlayout->setSpacing(5);

    m_iconLabel = new QLabel(this);
    m_iconLabel->setFixedSize(16, 16);
    m_iconLabel->setStyleSheet("QLabel{border:none;background-color:transparent;}");

    m_textLabel = new QLabel(this);
    m_textLabel->setStyleSheet("QLabel{border:none;background-color:transparent;color:#ffffff;font-size:14px;}");
    m_textLabel->setText(tr("Loading..."));
    QFont font = m_textLabel->font();
    const QFontMetrics fm(font);
    m_textLabel->setFixedWidth(fm.width(m_textLabel->text()));

    m_hlayout->addStretch();
    m_hlayout->addWidget(m_iconLabel, 0, Qt::AlignHCenter);
    m_hlayout->addWidget(m_textLabel, 0, Qt::AlignHCenter);
    m_hlayout->addStretch();

    m_movie = new QMovie(":/res/move.gif");
    m_iconLabel->setMovie(m_movie);

    this->hide();
}

MaskWidget::~MaskWidget()
{
    if (m_movie) {
        delete m_movie;
    }
}

void MaskWidget::showMask()
{
    if (!parent() || !this->parentWidget())
        return;

    QRect parentRect = this->parentWidget()->window()->geometry();
    this->setGeometry(0, 0, parentRect.width(), parentRect.height());
    this->show();
}


void MaskWidget::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);

    if (m_movie) {
        m_movie->start();
    }
}

void MaskWidget::hideEvent(QHideEvent* event)
{
  QWidget::hideEvent(event);

  if (m_movie) {
      m_movie->stop();
  }
}
