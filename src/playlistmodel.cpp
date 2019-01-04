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

#include "playlistmodel.h"
#include "../smplayer/preferences.h"
#include "../smplayer/global.h"
using namespace Global;

#include <QDebug>

PlaylistModel::PlaylistModel(QObject *parent) :
    QStandardItemModel(parent)
{
    m_removeIndex = QModelIndex();
    m_hoverIndex = QModelIndex();
}

PlaylistModel::~PlaylistModel()
{

}

QString PlaylistModel::findFileNameByRow(int row)
{
    QModelIndex itemIndex = this->index(row, 0);
    QString filepath = data(itemIndex).toString();
    return filepath;
}

QModelIndex PlaylistModel::findModelIndex(const VideoPtr video)
{
    Q_ASSERT(!video.isNull());

    QModelIndex itemIndex;
    for (int i = 0; i < rowCount(); ++i) {
        itemIndex = index(i, 0);
        QString filepath = data(itemIndex).toString();//没有重写model的data函数时
        if (filepath == video->m_localpath) {
            return itemIndex;
        }
    }

    return QModelIndex();
}

bool PlaylistModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    return QStandardItemModel::setData(index, value, role);
}

QString PlaylistModel::filePathData(const QModelIndex &index, int role) const
{
    QString filepath = this->data(index, role).toString();
    return filepath;
}

VideoPtr PlaylistModel::videoData(const QModelIndex &index, int role) const
{
    QString filepath = this->data(index, role).toString();
    auto video = pref->video(filepath);
    if (video.isNull()) {
        return VideoPtr();
    }

    return video;
}

Qt::DropActions PlaylistModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction;//return QAbstractItemModel::supportedDropActions();
}

Qt::ItemFlags PlaylistModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags defaultFlags = QAbstractItemModel::flags(index);
    if (index.isValid()) {
        return Qt::ItemIsDragEnabled | defaultFlags;//Qt::ItemIsEditable
    }
    else {
        return Qt::ItemIsDropEnabled | defaultFlags;
    }
}

void PlaylistModel::setHoverIndex(const QModelIndex &index)
{
    m_hoverIndex = index;
    emit dataChanged(m_hoverIndex, m_hoverIndex);
}
