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

#include "playlistview.h"
#include "../smplayer/myaction.h"

#include <QScrollBar>
#include <QDebug>
#include <QMenu>
#include <QDir>
#include <QProcess>
#include <QFileInfo>
#include <QStyleFactory>
#include <QResizeEvent>
#include <QStandardItemModel>
#include <QScrollBar>

#include "playlistdelegate.h"
#include "playlistmodel.h"
#include "../smplayer/preferences.h"
#include "../smplayer/global.h"
using namespace Global;


PlayListItem::PlayListItem() : QStandardItem()
{
    setDuration(0);
}

PlayListItem::PlayListItem(const QString filename, const QString name, double duration) : QStandardItem()
{
    setFilename(filename);
    setName(name);
    setDuration(duration);
}

PlayListItem::~PlayListItem()
{
}

void PlayListItem::setFilename(const QString filename)
{
    m_filename = filename;
}

void PlayListItem::setName(const QString name)
{
    m_name = name;
}

void PlayListItem::setDuration(double duration)
{
    m_duration = duration;
}

QString PlayListItem::filename()
{
    return this->m_filename;
}

QString PlayListItem::name()
{
    return this->m_name;
}

double PlayListItem::duration()
{
    return this->m_duration;
}



PlayListView::PlayListView(QSettings *set, QWidget *parent)
    : QListView(parent)
    , m_set(set)
{
    this->setStyleSheet("QListView{background-color: #2e2e2e;}");

    m_playingFile = "";

    m_scrollBar = new QScrollBar(this);
    m_scrollBar->setOrientation(Qt::Vertical);
    m_scrollBar->raise();
    connect(m_scrollBar, SIGNAL(valueChanged(int)), this, SLOT(onValueChanged(int)));

    m_playlistModel = new PlaylistModel(this);
    m_playlistDelegate = new PlaylistDelegate;
    this->setItemDelegate(m_playlistDelegate);
    connect(m_playlistDelegate, &PlaylistDelegate::removeBtnClicked, this, [=] {
        this->removeSelectionByModelIndex(this->currentHoverIndex());
    });
    this->setModel(m_playlistModel);

    this->setSpacing(0);
    this->setContentsMargins(0, 0, 0, 0);
    this->setUpdatesEnabled(true);
    this->setMouseTracking(true);
    this->setVerticalScrollMode(QAbstractItemView::ScrollPerItem);//QListView::ScrollPerPixel
    this->setHorizontalScrollMode(QAbstractItemView::ScrollPerItem);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    this->viewport()->setAcceptDrops(true);
    this->setDragDropOverwriteMode(false);
    this->setDropIndicatorShown(true);
    this->setDragEnabled(true);
    this->setDefaultDropAction(Qt::MoveAction);
    this->setDragDropMode(QAbstractItemView::InternalMove/*QAbstractItemView::DragOnly*/);
    this->setMovement(QListView::Free);
    this->setSelectionMode(QListView::ExtendedSelection);//QListView::SingleSelection

    connect(this, &PlayListView::currentHoverChanged, this, &PlayListView::onCurrentHoverChanged);
    connect(this, &PlayListView::entered, this, &PlayListView::onItemEntered);

    connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
    connect(this, SIGNAL(doubleClicked(QModelIndex)), SLOT(onDoubleClicked(QModelIndex)));
}

PlayListView::~PlayListView()
{
    if (m_playlistDelegate) {
        delete m_playlistDelegate;
        m_playlistDelegate = nullptr;
    }
}

void PlayListView::onPlayListChanged(/*const VideoPtrList medialist*/)
{
    //清空原来的列表
    this->m_playlistModel->removeRows(0, this->m_playlistModel->rowCount());

    //重新加载新的列表
    for (auto video : pref->m_videoMap) {
        this->addPlayListItem(video->localpath(), video->name(), video->duration());
    }
//    for (auto &media : medialist) {
//        this->addPlayListItem(media->localpath(), media->name(), media->duration());
//    }
    this->updateScrollbarSize();
}

void PlayListView::onValueChanged(int value)
{
    verticalScrollBar()->setValue(value);
}

const QModelIndex &PlayListView::currentHoverIndex() const
{
    return m_indexCurrent;
}

void PlayListView::enterEvent(QEvent *event)
{
//    lc 0827 列表刪除刪除按鈕不刪除
//    if (m_indexCurrent.isValid()) {
//        openPersistentEditor(m_indexCurrent);
//    }

    QWidget::enterEvent(event);
}

void PlayListView::leaveEvent(QEvent *event)
{
    if (m_indexCurrent.isValid()) {
        closePersistentEditor(m_indexCurrent);
        static_cast<PlaylistModel *>(this->m_playlistModel)->setHoverIndex(QModelIndex());
    }
    QWidget::leaveEvent(event);
}

void PlayListView::onCurrentHoverChanged(const QModelIndex &previous, const QModelIndex &current)
{
    if (previous.isValid()) {
        closePersistentEditor(previous);
    }
    openPersistentEditor(current);
}

void PlayListView::onItemEntered(const QModelIndex &index)
{
    m_indexCurrent = index;
    static_cast<PlaylistModel *>(this->m_playlistModel)->setHoverIndex(m_indexCurrent);
    if (m_indexPrevious != m_indexCurrent) {
        emit currentHoverChanged(m_indexPrevious, m_indexCurrent);
        m_indexPrevious = m_indexCurrent;
    }
    m_playlistDelegate->setItemHover(m_indexCurrent);
//  lc 0827 列表刪除刪除按鈕不刪除
    openPersistentEditor(m_indexCurrent);
}

void PlayListView::updateScrollbarSize()
{
    //Attentions: 48 is playlistitem's height, 12 is VerticalScrollBar's width
    QSize totalSize = this->size();

    int vscrollBarWidth = 12;
    m_scrollBar->setMinimum(0);
    m_scrollBar->setSingleStep(1);
    m_scrollBar->resize(vscrollBarWidth, totalSize.height() - 2);
    m_scrollBar->move(totalSize.width() - vscrollBarWidth - 2, 0);
    m_scrollBar->setPageStep(totalSize.height() / 48);

    if (this->m_playlistModel->rowCount() > totalSize.height() / 48) {
        m_scrollBar->setVisible(true);
        m_scrollBar->setMaximum(this->m_playlistModel->rowCount() - totalSize.height() / 48);
    } else {
        m_scrollBar->setVisible(false);
        m_scrollBar->setMaximum(0);
    }
}

void PlayListView::wheelEvent(QWheelEvent *event)
{
    /*event->accept();
    if (event->orientation() == Qt::Vertical) {
        if (event->delta() >= 0)
            emit wheelUp();
        else
            emit wheelDown();
    }*/
    QListView::wheelEvent(event);
    this->m_scrollBar->setSliderPosition(verticalScrollBar()->sliderPosition());
}

void PlayListView::resizeEvent(QResizeEvent *event)
{
    QListView::resizeEvent(event);
    this->updateScrollbarSize();
}

int PlayListView::getModelRowCount()
{
    return this->m_playlistModel->rowCount();
}

QStandardItem * PlayListView::getItemByRow(int row)
{
    QStandardItem *item = this->m_playlistModel->item(row, 0);
    return item;
//    PlayListItem *playItem = static_cast<PlayListItem *>(item);
//    return playItem;
}

QString PlayListView::getFileNameByRow(int row)
{
    return this->m_playlistModel->findFileNameByRow(row);

    /*for (int i = 0; i < this->m_playlistModel->rowCount(); ++i) {
        auto index = this->m_playlistModel->index(i, 0);
        auto filepath = this->m_playlistModel->data(index).toString();//没有重写model的data函数时
        Q_ASSERT(!filepath.isEmpty());
        return filepath;
    }*/
}

void PlayListView::setPlayingInfo(const QString &filepath, int row)
{
    this->m_playingFile = filepath;
    if (row <= this->m_playlistModel->rowCount()) {
        QModelIndex index = this->m_playlistModel->index(row, 0);
        this->setCurrentIndex(index);
    }
}

QString PlayListView::getPlayingFile()
{
    return this->m_playingFile;
}

void PlayListView::onDoubleClicked(const QModelIndex & index)
{
    /*int row = this->currentIndex().row();
    if (row != -1 && row != 0) {
            QList <QStandardItem *>curRow;
            curRow = this->m_playlistModel->takeRow(row);
            this->m_playlistModel->insertRow(row-1, curRow);
            this->selectRow(row-1);
    }*/

    m_indexCurrent = index;
//    m_playlistDelegate->setItemHover(index);

    //没有重写model的data函数时
    QString filepath = this->m_playlistModel->filePathData(index);
    emit requestPlayVideo(this->currentIndex().row(), filepath);
    m_playingFile = filepath;
}

QModelIndex PlayListView::findModelIndex(const VideoPtr video)
{
    Q_ASSERT(!video.isNull());

    return this->m_playlistModel->findModelIndex(video);
}

void PlayListView::removeFilesFromPlayList(const QStringList &filelist)
{
    setAutoScroll(false);
    foreach (QString filepath, filelist) {
        if (filepath.isEmpty())
            continue;
        for (int i = 0; i < this->m_playlistModel->rowCount(); ++i) {
            auto index = this->m_playlistModel->index(i, 0);
            QString fileName = this->m_playlistModel->data(index).toString();//没有重写model的data函数时
            if (fileName == filepath) {
                this->m_playlistModel->removeRow(i);
            }
        }
    }

    updateScrollbarSize();
    setAutoScroll(true);


    /*QStandardItemModel *m_playlistModel = dynamic_cast<QStandardItemModel*>(this->m_playlistModel);
    m_playlistModel->removeRow(this->currentIndex().row());*/
}

void PlayListView::keyPressEvent(QKeyEvent *event)
{
    switch (event->modifiers()) {
    case Qt::NoModifier:
        switch (event->key()) {
        case Qt::Key_Delete:
            QItemSelectionModel *selection = this->selectionModel();
            this->removeSelectionBySelctionModel(selection);
            break;
        }
        break;
    case Qt::ShiftModifier:
        switch (event->key()) {
        case Qt::Key_Delete:
            break;
        }
        break;
    default: break;
    }

    QAbstractItemView::keyPressEvent(event);
}

void PlayListView::addPlayListItem(const QString &filepath, const QString &name, double duration)
{
    //PlayListItem *newItem = new PlayListItem(filepath, name, duration);
    //m_playlistModel->appendRow(static_cast<QStandardItem *>(newItem));
    QStandardItem *newItem = new QStandardItem;
    newItem->setToolTip(name);
    m_playlistModel->appendRow(newItem);
    int row = m_playlistModel->rowCount() - 1;
    QModelIndex index = m_playlistModel->index(row, 0, QModelIndex());
    m_playlistModel->setData(index, filepath);//TODO: kobe for data(hash), 如果Model中重写的了data()函数，则该设置的值被覆盖，由data()重新设置值
}

void PlayListView::removeSelectionBySelctionModel(QItemSelectionModel *selection)
{
    Q_ASSERT(selection != NULL);

    QStringList filePathList;
    for (const QModelIndex &index : selection->selectedRows()) {
        QString filepath = this->m_playlistModel->filePathData(index);
        filePathList.append(filepath);
    }
    emit this->requestRemoveFiles(filePathList);
}

void PlayListView::removeSelectionByModelIndex(const QModelIndex &index)
{
    if (!index.isValid()) {
        return;
    }
    QStringList filePathList;
    QString filepath = this->m_playlistModel->filePathData(index);
    filePathList.append(filepath);
//    if (this->m_playingFile == filepath) {

//    }

    emit this->requestRemoveFiles(filePathList);
}

void PlayListView::showContextMenu(const QPoint &pos)
{
    QItemSelectionModel *selection = this->selectionModel();

    if (selection->selectedRows().length() <= 0) {
        return;
    }

    QPoint globalPos = this->mapToGlobal(pos);

    bool singleSelect = (1 == selection->selectedRows().length());

    QMenu myMenu;
    MyAction *playAct = NULL;
    if (singleSelect) {
        playAct = new MyAction(this, "pl_play", false);
        connect(playAct, SIGNAL(triggered()), this, SLOT(onPlayActionTriggered()));
        playAct->change(tr("Play"));
        playAct->setIcon(QPixmap(":/res/playing_normal.png"));//playAct->setIcon(Images::icon("playing_normal"));
    }

    // Remove actions
    auto removeSelectedAct = new MyAction(this, "pl_remove_selected", false);
    connect( removeSelectedAct, SIGNAL(triggered()), this, SLOT(onRemoveAcionTriggered()));
    removeSelectedAct->change(tr("Remove &selected"));
    removeSelectedAct->setIcon(QPixmap(":/res/delete.png"));

    auto deleteSelectedFileFromDiskAct = new MyAction(this, "pl_delete_from_disk");
    connect( deleteSelectedFileFromDiskAct, SIGNAL(triggered()), this, SLOT(onDeleteActionTriggered()));
    deleteSelectedFileFromDiskAct->change(tr("&Delete file from disk") );
    deleteSelectedFileFromDiskAct->setIcon(QPixmap(":/res/delete.png"));

    auto index = selection->selectedRows().first();
    m_selectedModelIndex = index;

    if (playAct) {
        myMenu.addAction(playAct);
    }

    if (removeSelectedAct) {
        myMenu.addAction(removeSelectedAct);
    }

    if (deleteSelectedFileFromDiskAct) {
        myMenu.addAction(deleteSelectedFileFromDiskAct);
    }

    myMenu.exec(globalPos);
}

void PlayListView::onPlayActionTriggered()
{
    QString filepath = this->m_playlistModel->filePathData(m_selectedModelIndex);//TODO: m_selectedModelIndex
    emit requestPlayVideo(this->currentIndex().row(), filepath);//TODO: currentIndex is right???
}

void PlayListView::onRemoveAcionTriggered()
{
    this->removeSelectionBySelctionModel(this->selectionModel());
}

void PlayListView::onDeleteActionTriggered()
{
    QItemSelectionModel *selection = this->selectionModel();

    Q_ASSERT(selection != NULL);

    QStringList fileList;
    for (auto index : selection->selectedRows()) {
        QString filepath = this->m_playlistModel->filePathData(index);
        fileList.append(filepath);
    }
    emit this->requestDeleteVideos(fileList);
}

void PlayListView::dragEnterEvent(QDragEnterEvent *event)
{
    QListView::dragEnterEvent(event);
}

void PlayListView::startDrag(Qt::DropActions supportedActions)
{
//    QModelIndex index = this->currentIndex();

    // save selected list
//    VideoPtrList selectedMediaList;
//    for (QModelIndex index : this->selectionModel()->selectedIndexes()) {
//        selectedMediaList << this->m_playlistModel->videoData(index);
//    }

    setAutoScroll(false);
    QListView::startDrag(supportedActions);
    setAutoScroll(true);

    QMap<QString, int> maps;
    int newCurrentIndex = -1;
    QStringList newSortList;
    for (int i = 0; i < this->m_playlistModel->rowCount(); ++i) {
        auto index = this->m_playlistModel->index(i, 0);
        auto filepath = this->m_playlistModel->data(index).toString();//没有重写model的data函数时
        Q_ASSERT(!filepath.isEmpty());
        maps.insert(filepath, i);
        if (this->m_playingFile == filepath) {
            newCurrentIndex = i;
        }
//        newSortList.append(filepath);
    }

    QMap<int, QString> sortMaps;
    for (QString filepath : maps.keys()) {
        sortMaps.insert(maps.value(filepath), filepath);
    }
    for (int i = 0; i < sortMaps.size(); ++i) {
        newSortList.append(sortMaps.value(i));
    }
    //qDebug() << "newSortList=" << newSortList;
    if (!newSortList.isEmpty()) {
        emit requestResortVideos(newSortList, newCurrentIndex);
    }

    // restore selected list
//    QItemSelection selections;
//    for (VideoPtr media : selectedMediaList) {
//        if (!media.isNull()) {
//            QModelIndex index = this->findModelIndex(media);
//            selections.append(QItemSelectionRange(index));
//        }
//    }
//    if (!selections.isEmpty()) {
//        this->selectionModel()->select(selections, QItemSelectionModel::Select);
//    }
}
