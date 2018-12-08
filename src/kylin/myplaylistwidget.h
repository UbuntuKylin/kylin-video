//#pragma once

#include <QDialog>


#include "mediameta.h"
#include <QLabel>
#include <QPushButton>

class PlayListView1;
class MediaLibrary;

class MyPlayListWidget : public QDialog
{
    Q_OBJECT
public:
    explicit MyPlayListWidget(QWidget *parent = 0);
    ~MyPlayListWidget();

    void setCustomSortType();


//    void initData(/*PlaylistPtr playlist*/);
    void initConntion();
//    void showEmptyHits(bool empty);
public slots:
    void onMusiclistChanged(/*PlaylistPtr playlist*/);
//    void onMusicPlayed(PlaylistPtr playlist, const MetaPtr info);
//    void onMusicPause(PlaylistPtr playlist, const MetaPtr meta);
    void onMusicError(/*PlaylistPtr playlist, */const MetaPtr meta, int error);
    void onMusicListAdded(/*PlaylistPtr playlist, */const MetaPtrList metalist);
    void onMusicListRemoved(/*PlaylistPtr playlist, */const MetaPtrList metalist);
    void onLocate(/*PlaylistPtr playlist, const MetaPtr meta*/);
//    void onCustomContextMenuRequest(const QPoint &pos/*,
//                                    PlaylistPtr selectedlist,
//                                    PlaylistPtr favlist,
//                                    QList<PlaylistPtr >newlists*/);

    void onaddBtnClicked();
//    void onMusiclistCustomSort();
    void onRemoveMusicList(const MetaPtrList  & metalist);
//    void onDeleteMusicList(const MetaPtrList  & metalist);
//    void onPlayMedia(const MetaPtr meta);

signals:
    void playMedia(/*const MetaPtr meta*/const QString &filepath);
//    void playall(PlaylistPtr playlist);
//    void resort(/*PlaylistPtr playlist, */int sortType);
//    void playMedia(PlaylistPtr playlist, const MetaPtr meta);
//    void showInfoDialog(const MetaPtr meta);
//    void updateMetaCodec(const MetaPtr meta);

    void requestCustomContextMenu(const QPoint &pos);
//    void addToPlaylist(PlaylistPtr playlist, const MetaPtrList  &metalist);
    void musiclistRemove(/*PlaylistPtr playlist, */const MetaPtrList  &metalist);
    void musiclistDelete(/*PlaylistPtr playlist, */const MetaPtrList  &metalist);
//    void importMusicListWidgetSelectFiles(PlaylistPtr playlist, QStringList urllist);

protected:
    //virtual void dragEnterEvent(QDragEnterEvent *event) Q_DECL_OVERRIDE;
    //virtual void dropEvent(QDropEvent *event) Q_DECL_OVERRIDE;
    virtual void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;

private:
    //QLabel              *emptyHits;
    QWidget             *actionBar;
    QPushButton         *m_addBtn;
    PlayListView1       *musiclist;

    MediaLibrary        *library;
};

