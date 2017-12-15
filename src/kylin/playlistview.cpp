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

#include "playlistview.h"

#include <QDebug>
#include <QMenu>
#include <QScrollBar>
#include <QStyleFactory>

#include "playlistitem.h"

PlayListView::PlayListView(QWidget *parent) : QListWidget(parent)
{
    setObjectName("PlayListView");//kobe:设置选中项的左侧的颜色栏

    scrollBarWidth = 8;//滚动条的宽度
    itemHeight = 32;//每个item的高度

    /*setAcceptDrops(true)来接受放下事件，通过设置setDropIndicatorShown(true)则可以清晰地看到放下过程中的图标指示。
     * 然后实现dragEnterEvent()、dropEvent()方法，当用户把一个对象拖动到这个窗体上时，就会调用dragEnterEvent()，
     * 如果对这个事件调用acceptProposedAction()，就表明可以在这个窗体上拖放对象。默认情况下窗口部件是不接受拖动的。
     * Qt会自动改变光标向用户说明这个窗口部件不是有效的放下点。
    */
    setDragEnabled(true);
//    viewport()->setAcceptDrops(true);//不允许listitem项拖动
    setDropIndicatorShown(true);
    setDefaultDropAction(Qt::MoveAction);
//    setDragDropMode(QAbstractItemView::DragOnly);


//    setResizeMode(QListWidget::Adjust);
    setMovement(QListWidget::Static);//设置单元项不可拖动

    setSelectionMode(QListView::SingleSelection);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    this->setContextMenuPolicy(Qt::CustomContextMenu);

//    this->verticalScrollBar()->setStyleSheet("QScrollBar:vertical {width: 12px;background: #141414;margin:0px 0px 0px 0px;border:1px solid #141414;}QScrollBar::handle:vertical {width: 12px;min-height: 45px;background: #292929;margin-left: 0px;margin-right: 0px;}QScrollBar::handle:vertical:hover {background: #3e3e3e;}QScrollBar::handle:vertical:pressed {background: #272727;}QScrollBar::sub-line:vertical {height: 6px;background: transparent;subcontrol-position: top;}QScrollBar::add-line:vertical {height: 6px;background: transparent;subcontrol-position: bottom;}QScrollBar::sub-line:vertical:hover {background: #292929;}QScrollBar::add-line:vertical:hover {background: #292929;}QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {background: transparent;}");
//    this->verticalScrollBar()->setObjectName("PlayListViewScrollBar");//kobe:让滚动条可以鼠标拖动
//    this->verticalScrollBar()->setFixedWidth(scrollBarWidth);
////    this->verticalScrollBar()->move(this->size().width() - scrollBarWidth, 0);
//    this->verticalScrollBar()->setSingleStep(1);

    vscrollBar = new QScrollBar(this);
    vscrollBar->setObjectName("PlayListViewScrollBar");//kobe:让滚动条可以鼠标拖动
    vscrollBar->setStyleSheet("QScrollBar:vertical {width: 12px;background: #141414;margin:0px 0px 0px 0px;border:1px solid #141414;}QScrollBar::handle:vertical {width: 12px;min-height: 45px;background: #292929;margin-left: 0px;margin-right: 0px;}QScrollBar::handle:vertical:hover {background: #3e3e3e;}QScrollBar::handle:vertical:pressed {background: #272727;}QScrollBar::sub-line:vertical {height: 6px;background: transparent;subcontrol-position: top;}QScrollBar::add-line:vertical {height: 6px;background: transparent;subcontrol-position: bottom;}QScrollBar::sub-line:vertical:hover {background: #292929;}QScrollBar::add-line:vertical:hover {background: #292929;}QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {background: transparent;}");
    vscrollBar->setOrientation(Qt::Vertical);
    vscrollBar->raise();
    connect(vscrollBar, SIGNAL(valueChanged(int)), this ,SLOT(slot_scrollbar_value_changed(int)));
}

PlayListView::~PlayListView()
{
    if (vscrollBar != NULL) {
        delete vscrollBar;
        vscrollBar = NULL;
    }
}

void PlayListView::checkScrollbarSize()
{
    int itemCount = this->model()->rowCount();
    QSize size = this->size();
    vscrollBar->resize(scrollBarWidth, size.height() - 2);
    vscrollBar->move(size.width() - scrollBarWidth , 0);
    vscrollBar->setSingleStep(1);
    vscrollBar->setPageStep(size.height() / itemHeight);

    if (itemCount > size.height() / itemHeight) {
        vscrollBar->show();
        vscrollBar->setMaximum(itemCount - size.height() / itemHeight);
    } else {
        vscrollBar->hide();
        vscrollBar->setMaximum(0);
    }
}

void PlayListView::slot_scrollbar_value_changed(int value)
{
    this->verticalScrollBar()->setValue(value);
}

void PlayListView::wheelEvent(QWheelEvent *event)
{
    QListWidget::wheelEvent(event);
    this->vscrollBar->setSliderPosition(verticalScrollBar()->sliderPosition());
}

void PlayListView::resizeEvent(QResizeEvent *event)
{
    QListWidget::resizeEvent(event);
    this->checkScrollbarSize();
}

void PlayListView::updateScrollbar()
{
    this->checkScrollbarSize();
}
