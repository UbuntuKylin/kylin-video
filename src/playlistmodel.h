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

#include <QStandardItemModel>

#include "datautils.h"

class PlaylistModel : public QStandardItemModel
{
    Q_OBJECT

public:
    explicit PlaylistModel(QObject *parent = 0);
    ~PlaylistModel();

    QModelIndex findModelIndex(const VideoPtr video);
    QString findFileNameByRow(int row);
    QString filePathData(const QModelIndex &index, int role = Qt::DisplayRole) const;
    VideoPtr videoData(const QModelIndex &index, int role = Qt::DisplayRole) const;

    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) Q_DECL_OVERRIDE;

public slots:
    void setHoverIndex(const QModelIndex &index);

protected:
    virtual Qt::DropActions supportedDropActions() const Q_DECL_OVERRIDE;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;

private:
    QModelIndex m_removeIndex;
    QModelIndex m_hoverIndex;
};

