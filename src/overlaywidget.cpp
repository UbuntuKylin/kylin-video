/*
 * Copyright (C) 2013 ~ 2017 National University of Defense Technology(NUDT) & Kylin Ltd.
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

#include "overlaywidget.h"
#include <QTime>
#include <QWindowStateChangeEvent>
#include <QHBoxLayout>
#define WAIT_TIME_TO_MAXIMIZE_OVERLAY_MS 300
#include <QPainter>
#include <QDebug>

OverlayWidget::OverlayWidget(QWidget *parent)
    : QWidget(parent, Qt::SubWindow)
{
	setWindowFlags(windowFlags() | Qt::SubWindow);
	setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
	setTransparent(true);
	//setTransparent(false);
	
	QPushButton *m_modelButton = new QPushButton("Load model");
    m_modelButton->setFixedSize(400,300);
	QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(m_modelButton);
	this->setLayout(layout);
	//setFixedSize(20,20);
}

OverlayWidget::~OverlayWidget()
{

}

void OverlayWidget::setBackgroundWidget(QWidget *widget)
{
    m_pBackgroundWidget = widget;
//    m_pBackgroundWidget->installEventFilter(this);
}

//void OverlayWidget::show()
//{

//    setGeometry(m_pBackgroundWidget->x()+30,m_pBackgroundWidget->y()+30,400,300);
//	QWidget::show();

//}

void OverlayWidget::setTransparent(bool transparent)
{
	if (transparent)
	{
		setAttribute(Qt::WA_TranslucentBackground);
		setWindowFlags(windowFlags() | Qt::FramelessWindowHint|Qt::X11BypassWindowManagerHint);
		set_widget_opacity(0.5);
	}
	else
	{
		setAttribute(Qt::WA_TranslucentBackground,false);
		setWindowFlags(windowFlags() & ~Qt::FramelessWindowHint);
	}
}

void OverlayWidget::set_widget_opacity(const float &opacity)
{
	QGraphicsOpacityEffect* opacityEffect = new QGraphicsOpacityEffect;
	this->setGraphicsEffect(opacityEffect);
    opacityEffect->setOpacity(opacity);
}

/** receives the events from the event dispatcher before the background widget, if returns true the background widget will not receive the events
*	For backgreound to delegate event handling to this we have to call the inherited qobject function ==> background->installEventFilter(overlay);
*/
bool OverlayWidget::eventFilter(QObject *obj, QEvent *event)
 {
    qDebug() << "OverlayWidget::eventFilter+++++++++++++++++++++++++++++++++++++++++++++++";
     if (obj==m_pBackgroundWidget)
     {
         if (event->type() == QEvent::Show)
            this->show();
         if (event->type() == QEvent::Close)
         {
            m_pBackgroundWidget = NULL;// if you dont set is as null the qt refrence counter will not let the overlay to close
            this->close();
         }
         if (event->type()==QEvent::Resize)
            setGeometry(m_pBackgroundWidget->x()+30,m_pBackgroundWidget->y()+30,400,300);
         if (event->type()==QEvent::Move)
             setGeometry(m_pBackgroundWidget->x()+30,m_pBackgroundWidget->y()+30,400,300);
         if (event->type()==QEvent::WindowStateChange){
            if (!m_pBackgroundWidget->isMinimized()){

                // wait until window is restored from minimised state
                QWindowStateChangeEvent* e = static_cast< QWindowStateChangeEvent* >( event );
                if (e->oldState() & Qt::WindowMinimized) {
                    QTime dieTime= QTime::currentTime().addMSecs(WAIT_TIME_TO_MAXIMIZE_OVERLAY_MS);
                    while( QTime::currentTime() < dieTime );
                }

                show();
            }
            else	hide();
	
         }

         // if another window from another application is put on top hide overlay (because it is set to alwause stay on top of all windows)
         if (event->type()==QEvent::ActivationChange){
            if(!m_pBackgroundWidget->isActiveWindow() & !isActiveWindow() )
                hide();

            else
                show();
        }
     }

     return false;
 }

//void OverlayWidget::changeEvent(QEvent *event)
//{
//	if (event->type()==QEvent::ActivationChange){
//		if(!isActiveWindow() & !m_pBackgroundWidget->isActiveWindow() )
//			hide();
//		else
//			this->show();

//	}
//}

//void OverlayWidget::paintEvent(QPaintEvent *event)
//{
////    QPainter painter(this);
////    painter.set_widget_opacity(widget_opacity);
////    painter.setBrush(widget_color);
////    painter.setPen(Qt::NoPen);
////    painter.drawRect(rect());

//    QPainter painter(this);
//    painter.eraseRect( event->rect() );

//    //0531 kobe:使用Clear模式绘图，消除视频播放时该界面上控件的重影
////    QPainter p(this);
////    p.setCompositionMode(QPainter::CompositionMode_Clear);
////    p.fillRect(rect(), Qt::SolidPattern);//p.fillRect(0, 0, this->width(), this->height(), Qt::SolidPattern);


////    QPalette pal = palette();
////    pal.setColor(QPalette::Background, QColor(0x00,0xff,0x00,0x00));
////    setPalette(pal);
//}
