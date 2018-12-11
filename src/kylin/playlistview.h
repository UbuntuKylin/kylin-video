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

#include <QScopedPointer>
#include <QListView>
#include <QSettings>
#include <QTime>

#include "datautils.h"
class PlaylistModel;
class PlaylistDelegate;


#include <QStandardItem>
class PlayListItem : public QStandardItem {

public:
    PlayListItem();
    PlayListItem(const QString filename, const QString name, double duration);
    ~PlayListItem();

    void setFilename(const QString filename);
    void setName(const QString name);
    void setDuration(double duration);

    QString filename();
    QString name();
    double duration();

    QString m_filename;
    QString m_name;
    double m_duration;
};


class PlayListView : public QListView
{
    Q_OBJECT

public:
    explicit PlayListView(QSettings *set, QWidget *parent = 0);
    ~PlayListView();

    const QModelIndex &currentHoverIndex() const;
    QModelIndex findModelIndex(const VideoPtr video);
    void addPlayListItem(const QString &filepath, const QString &name, double duration);
    int getModelRowCount();
    QStandardItem *getItemByRow(int row);
    QString getFileNameByRow(int row);
    void setPlayingInfo(const QString &filepath, int row);
    QString getPlayingFile();
    void removeFilesFromPlayList(const QStringList &filelist);
    void updateScrollbarSize();

    PlaylistModel *m_playlistModel;
    void onPlayListChanged(/*const VideoPtrList medialist*/);

public slots:
    void onDoubleClicked(const QModelIndex & index);
    void onPlayActionTriggered();
    void onRemoveAcionTriggered();
    void onDeleteActionTriggered();
    void removeSelectionBySelctionModel(QItemSelectionModel *selection);
    void removeSelectionByModelIndex(const QModelIndex &index);
    void onValueChanged(int value);
    void onCurrentHoverChanged(const QModelIndex &previous, const QModelIndex &current);
    void onItemEntered(const QModelIndex &index);
    void showContextMenu(const QPoint &pos);

signals:
    void requestRemoveFiles(const QStringList &filepathlist);
    void requestDeleteVideos(const QStringList &filepathlist);
    void requestPlayVideo(int row, const QString filepath);
    void requestResortVideos(const QStringList &sortList, int index);
    void currentHoverChanged(const QModelIndex &previous, const QModelIndex &current);

protected:
    virtual void dragEnterEvent(QDragEnterEvent *event) Q_DECL_OVERRIDE;
    virtual void startDrag(Qt::DropActions supportedActions) Q_DECL_OVERRIDE;
    virtual void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
    virtual void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;
    virtual void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
    virtual void enterEvent(QEvent *event) Q_DECL_OVERRIDE;
    virtual void leaveEvent(QEvent *event) Q_DECL_OVERRIDE;

private:
    QScrollBar *m_scrollBar = nullptr;
    PlaylistDelegate *m_playlistDelegate = nullptr;
    QSettings *m_set = nullptr;
    QModelIndex m_selectedModelIndex;
    QModelIndex m_indexPrevious;
    QModelIndex m_indexCurrent;
    QString m_playingFile;
};
