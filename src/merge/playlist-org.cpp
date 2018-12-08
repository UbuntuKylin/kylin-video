/*  smplayer, GUI front-end for mplayer.
    Copyright (C) 2006-2015 Ricardo Villalba <rvm@users.sourceforge.net>
    Copyright (C) 2013 ~ 2017 National University of Defense Technology(NUDT) & Tianjin Kylin Ltd.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "playlist.h"

#include <QToolBar>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QFileInfo>
#include <QMessageBox>
#include <QPushButton>
#include <QRegExp>
#include <QMenu>
#include <QDateTime>
#include <QSettings>
#include <QInputDialog>
#include <QToolButton>
#include <QTimer>
#include <QVBoxLayout>
#include <QUrl>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QHeaderView>
#include <QTextCodec>
#include <QApplication>
#include <QMimeData>
#include <QDebug>
#include <QString>
#include <QLabel>

#include "../smplayer/myaction.h"
#include "../smplayer/filedialog.h"
#include "../smplayer/helper.h"
#include "../smplayer/preferences.h"
#include "../smplayer/version.h"
#include "../smplayer/global.h"
#include "../smplayer/core.h"
#include "../smplayer/extensions.h"
#include "../kylin/playlistitem.h"
#include "../kylin/playlistview.h"
#include "../kylin/messagedialog.h"
#include <QPainter>
#include <QGraphicsOpacityEffect>
#include <stdlib.h>
#include "../smplayer/infoprovider.h"

using namespace Global;

Playlist::Playlist(Core *c, QWidget * parent, Qt::WindowFlags f)
    : QFrame(parent, Qt::SubWindow)
{
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setAutoFillBackground(true);
    this->setFixedWidth(220);
    setObjectName("PlaylistWidget");

    setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred);
    setAcceptDrops(true);
    setAttribute(Qt::WA_NoMousePropagation);

	automatically_play_next = true;
    modified = false;
	core = c;
    playlist_path = "";
    playlist_load_latest_dir = "";
    titleLabel = 0;
    btDel = 0;
    btAdd = 0;
    novideo_icon = 0;
    novideo_text = 0;
    add_Btn = 0;
    title_layout = NULL;

    createNoVideo();
	createTable();
    createToolbar();

    QVBoxLayout *layout = new QVBoxLayout(this);
    setFocusPolicy(Qt::ClickFocus);
    layout->setContentsMargins(0, 0, 0, 15);
    layout->setSpacing(16);
    btAddFrame = new QFrame;
//    btAddFrame->setFixedSize(156, 36);
    btAddFrame->setFixedSize(120, 36);
    btAddFrame->setObjectName("PlaylistWidgetAddFrame");
    btAddFrame->setFocusPolicy(Qt::NoFocus);
    QHBoxLayout *btAddFameLayout = new QHBoxLayout(btAddFrame);
    btAddFameLayout->setMargin(0);
    btAddFameLayout->setSpacing(0);
    btAddFameLayout->addWidget(btAdd, 0, Qt::AlignVCenter);
    btAddFameLayout->addWidget(btDel, 0, Qt::AlignVCenter);
//    btAddFameLayout->addWidget(btClose, 0, Qt::AlignVCenter);
    title_layout = new QHBoxLayout();
    title_layout->setSpacing(0);
    title_layout->setMargin(0);
    title_layout->addWidget(titleLabel, 0, Qt::AlignHCenter);
//    title_layout->addStretch();
    title_layout->addWidget(btAddFrame, 0, Qt::AlignHCenter);
    noVideoFrame = new QFrame();
    noVideoFrame->setFocusPolicy(Qt::NoFocus);
    QVBoxLayout *noVideoFameLayout = new QVBoxLayout(noVideoFrame);
    noVideoFameLayout->setMargin(0);
    noVideoFameLayout->setSpacing(30);
    noVideoFameLayout->addStretch();
    noVideoFameLayout->addWidget(novideo_icon, 0, Qt::AlignHCenter);
    noVideoFameLayout->addWidget(novideo_text, 0, Qt::AlignHCenter);
    noVideoFameLayout->addWidget(add_Btn, 0, Qt::AlignHCenter);
    noVideoFameLayout->addStretch();
    layout->addLayout(title_layout);

    layout->addWidget(listView, 0, Qt::AlignHCenter);
//    layout->addWidget(btAddFrame, 0, Qt::AlignHCenter | Qt::AlignBottom);
    layout->addWidget(noVideoFrame, 0, Qt::AlignHCenter);
    layout->addStretch();
    setLayout(layout);

    clear();

    connect(core, SIGNAL(mediaFinished()), this, SLOT(playNext()), Qt::QueuedConnection);
    connect(core, SIGNAL(mplayerFailed(QProcess::ProcessError)), this, SLOT(playerFailed(QProcess::ProcessError)) );
    connect(core, SIGNAL(mplayerFinishedWithError(int)), this, SLOT(playerFinishedWithError(int)) );
    connect(core, SIGNAL(mediaDataReceived(const MediaData &)), this, SLOT(getMediaInfo(const MediaData &)));
    connect(core, SIGNAL(mediaLoaded()), this, SLOT(getMediaInfo()) );

	// Random seed
	QTime t;
	t.start();
	srand( t.hour() * 3600 + t.minute() * 60 + t.second() );

	loadSettings();

	// Ugly hack to avoid to play next item automatically
	if (!automatically_play_next) {
		disconnect( core, SIGNAL(mediaFinished()), this, SLOT(playNext()) );
	}

	// Save config every 5 minutes.
	save_timer = new QTimer(this);
    connect(save_timer, SIGNAL(timeout()), this, SLOT(maybeSaveSettings()));
    save_timer->start(5 * 60000);

    if (this->count() > 0) {
        noVideoFrame->hide();
        listView->show();
    }
    else {
        noVideoFrame->show();
        listView->hide();
    }
}

Playlist::~Playlist() {
    if (popup) {
        delete popup;
        popup = NULL;
    }

    if (titleLabel != NULL) {
        delete titleLabel;
        titleLabel = NULL;
    }
    if (btDel != NULL) {
        delete btDel;
        btDel = NULL;
    }
    if (btAdd != NULL) {
        delete btAdd;
        btAdd = NULL;
    }
    if (novideo_icon != NULL) {
        delete novideo_icon;
        novideo_icon = NULL;
    }
    if (novideo_text != NULL) {
        delete novideo_text;
        novideo_text = NULL;
    }
    if (add_Btn != NULL) {
        delete add_Btn;
        add_Btn = NULL;
    }

    if (noVideoFrame != NULL) {
        delete noVideoFrame;
        noVideoFrame = NULL;
    }
    if (btAddFrame != NULL) {
        delete btAddFrame;
        btAddFrame = NULL;
    }

    for(int i=0; i<pl.count(); i++)
    {
        PlayListItem *item = pl.at(i);
        delete item;
        item = NULL;
    }
    pl.clear();
    if (title_layout) {
        delete title_layout;
        title_layout = NULL;
    }
    //kobe
//    foreach (QObject *child, listView->children()) {
//        QWidget *widget = static_cast<QWidget *>(child);
//        widget->deleteLater();
//    }

//    if (listView->count() > 0)
//        listView->clear();//kobe:WTF 会发生段错误
    if (listView != NULL) {
        delete listView;
        listView = NULL;
    }
    if (save_timer != NULL) {
        disconnect(save_timer, SIGNAL(timeout()), this, SLOT(maybeSaveSettings()));
        if(save_timer->isActive()) {
            save_timer->stop();
        }
        delete save_timer;
        save_timer = NULL;
    }
}

void Playlist::setTransparent(bool transparent) {
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

void Playlist::set_widget_opacity(const float &opacity) {
    QGraphicsOpacityEffect* opacityEffect = new QGraphicsOpacityEffect;
    this->setGraphicsEffect(opacityEffect);
    opacityEffect->setOpacity(opacity);
}

void Playlist::setModified(bool mod) {
    modified = mod;
//	emit modifiedChanged(modified);
}

void Playlist::createTable() {
    listView = new PlayListView(this);
//    listView->setFixedHeight(430);
//    QSizePolicy sp(QSizePolicy::Preferred, QSizePolicy::Preferred);
//    sp.setVerticalStretch(100);
//    listView->setSizePolicy(sp);
    connect(listView, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(doubleclicked(QListWidgetItem*)));
    connect(listView, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(showPopupMenu(const QPoint &)) );
}

void Playlist::setViewHeight()
{
    listView->setFixedHeight(this->height()-36-16);//36为顶部按钮和label的高度，16为顶部和列表的间隔
}

void Playlist::createNoVideo()
{
    novideo_icon = new QLabel();
    novideo_icon->setFixedSize(70, 70);
    novideo_icon->setPixmap(QPixmap(":/res/no-video.png"));
    novideo_text = new QLabel();
    novideo_text->setObjectName("VideoText");
    novideo_text->setText(tr("Playlist is empty"));
    add_Btn = new QPushButton();
    add_Btn->setFocusPolicy(Qt::NoFocus);
    add_Btn->setFixedSize(140, 38);
    add_Btn->setObjectName("PlaylistAddButton");
    add_Btn->setText(tr("Add File"));
    connect(add_Btn, SIGNAL(clicked(bool)), SLOT(popupDialogtoSelectFiles()));
}

void Playlist::createToolbar() {
    titleLabel = new QLabel();
    titleLabel->setAlignment(Qt::AlignCenter);
//    titleLabel->setFixedSize(220-156, 36);
    titleLabel->setFixedSize(220-120, 36);
    titleLabel->setStyleSheet("QLabel{font-size:12px;color:#ffffff;background:transparent;}");
    titleLabel->setText(tr("PlayList"));

    btDel = new QPushButton();
    btDel->setFocusPolicy(Qt::NoFocus);
    btDel->setFixedSize(60, 36);
    btDel->setText(tr("Clear"));
    btDel->setObjectName("PlaylistButton");
    connect(btDel, SIGNAL(clicked(bool)), SLOT(removeAll()));

    btAdd = new QPushButton();
    btAdd->setFocusPolicy(Qt::NoFocus);
    btAdd->setFixedSize(60, 36);
    btAdd->setObjectName("PlaylistButton");
    btAdd->setText(tr("Add"));
    //    connect(btAdd, SIGNAL(clicked(bool)), SIGNAL(addPlaylist(bool)));//emit this->addPlaylist(true);
    connect(btAdd, SIGNAL(clicked(bool)), SLOT(popupDialogtoSelectFiles()));

    playAct = new MyAction(this, "pl_play", false);
    connect(playAct, SIGNAL(triggered()), this, SLOT(playCurrent()) );
    playAct->change(tr("Play"));
    playAct->setIcon(QPixmap(":/res/playing_normal.png"));//playAct->setIcon(Images::icon("playing_normal"));

    // Remove actions
    removeSelectedAct = new MyAction(this, "pl_remove_selected", false);
    connect( removeSelectedAct, SIGNAL(triggered()), this, SLOT(removeTheSelected()) );
    removeSelectedAct->change(tr("Remove &selected"));
    removeSelectedAct->setIcon(QPixmap(":/res/delete.png"));

    deleteSelectedFileFromDiskAct = new MyAction(this, "pl_delete_from_disk");
    connect( deleteSelectedFileFromDiskAct, SIGNAL(triggered()), this, SLOT(deleteSelectedFileFromDisk()));
    deleteSelectedFileFromDiskAct->change(tr("&Delete file from disk") );
    deleteSelectedFileFromDiskAct->setIcon(QPixmap(":/res/delete.png"));

    // Popup menu
    popup = new QMenu(this);
    popup->addAction(playAct);
    popup->addAction(removeSelectedAct);
    popup->addAction(deleteSelectedFileFromDiskAct);
}

void Playlist::doubleclicked(QListWidgetItem *item)
{
//    qDebug() << item->text();
}

void Playlist::updateView() {
    QString time;

    for (int n=0; n < pl.count(); n++) {
//        name = pl[n]->name();
//        if (name.isEmpty()) name = pl[n]->filename();
        time = Helper::formatTime( (int) pl[n]->duration());
        if (listView) {
            QListWidgetItem *item = listView->item(n);
            PlayListItem *playlistItem = qobject_cast<PlayListItem *>(listView->itemWidget(item));
            playlistItem->update_time(time);//更新视频的时长
        }
    }
    setListCurrentItem(current_item);
}

void Playlist::setListCurrentItem(int current) {
    listView->setCurrentItem(listView->item(current));//启动加载时设置播放列表的默认选中项
    current_item = current;
    if ((current_item > -1) && (current_item < pl.count())) {
        pl[current_item]->setPlayed(true);
//        pl[current_item]->setActive(true);
    }
}

void Playlist::clear() {
    for(int i=0; i<pl.count(); i++)
    {
        PlayListItem *item = pl.at(i);
        delete item;
        item = NULL;
    }
    pl.clear();
    listView->reset();//kobe resolve segmentation fault
    listView->clearSelection();
    listView->clearFocus();
    listView->clear();//kobe:必须执行该语句，否则切换视频后点击显示播放列表会发生段错误
    listView->updateScrollbar();
    setListCurrentItem(0);
    setModified(false);

    emit this->update_playlist_count(0);

    noVideoFrame->show();
    listView->hide();
}

int Playlist::count() {
    return pl.count();
}

bool Playlist::isEmpty() {
    return pl.isEmpty();
}

void Playlist::addOneItem(QString filename, QString name, double duration) {
    // Test if already is in the list
    bool exists = false;
    for (int n = 0; n < pl.count(); n++) {
//        qDebug() << "pl[n]->filename()=" << pl[n]->filename();
        if ( pl[n]->filename() == filename.toUtf8().data()) {
            exists = true;
//            int last_item = pl.count()-1;
//            pl.move(n, last_item);
//            qDebug("Playlist::addOneItem: item already in list (%d), moved to %d and current_item=%d", n, last_item, current_item);
            current_item = n;//20170712
//            if (current_item > -1) {
//                if (current_item > n) current_item--;
//                else
//                if (current_item == n) current_item = last_item;
//            }
            break;
        }
    }

    if (!exists) {
        if (name.isEmpty()) {
            QFileInfo fi(filename.toUtf8().data());
            // Let's see if it looks like a file (no dvd://1 or something)
            if (filename.indexOf(QRegExp("^.*://.*")) == -1) {
                // Local file
                name = fi.fileName(); //fi.baseName(true);
            } else {
                // Stream
                name = filename;
            }
        }

        QListWidgetItem *item = new QListWidgetItem(listView);
        item->setSizeHint(QSize(220, 32));
////        item->setText(filename);
        listView->addItem(item);
        listView->setItemWidget(item, new PlayListItem("vedio", filename/*.toUtf8().data()*/, name/*.toUtf8().data()*/, duration));
        PlayListItem *playlistItem = qobject_cast<PlayListItem *>(listView->itemWidget(item));
        pl.append(playlistItem);//?????????????TODO:0518
        connect(playlistItem, SIGNAL(remove(QString)), this, SLOT(onPlayListItemDeleteBtnClicked(QString)));
        connect(playlistItem, SIGNAL(sig_doubleclicked_resource(QString)), this, SLOT(slot_doubleclicked_resource(QString)));

        setModified(true); // Better set the modified on a higher level

        listView->setCurrentItem(item);

        emit this->update_playlist_count(pl.count());
        if (pl.count() > 0) {
            noVideoFrame->hide();
            listView->show();
        }
        else {
            noVideoFrame->show();
            listView->hide();
        }
//        current_item = pl.count() - 1;//0526
//        this->setListCurrentItem(pl.count() - 1);//0526
    } else {
        this->setListCurrentItem(current_item);//20170712 正播放的文件存在播放列表中时更新播放列表的选择项
//        listView->setCurrentItem(listView->item(current_item));//20170712 正播放的文件存在播放列表中时更新播放列表的选择项
        qDebug("Playlist::addOneItem: item not added, already in the list current_item=%d", current_item);
    }
}

void Playlist::slot_listview_current_item_changed(QListWidgetItem *current, QListWidgetItem *previous)
{//kobe:此函数在启动加载配置时，会让配置的最后一个播放文件在列表中被选中
    PlayListItem *itemWidget = qobject_cast<PlayListItem *>(listView->itemWidget(previous));
    if (itemWidget) {
        itemWidget->setActive(false);
    }
    itemWidget = qobject_cast<PlayListItem *>(listView->itemWidget(current));
    if (itemWidget) {
        itemWidget->setActive(true);
    }
}

//双击播放列表的一项时进行播放
void Playlist::slot_doubleclicked_resource(QString filename)
{
    QFileInfo fi(filename);
    if (fi.exists()) {
        // Local file
        QString name = fi.fileName();
        emit this->sig_playing_title(name);
        core->open(filename/*, 0*/);//每次从头开始播放文件
    } else {
        emit this->playListFinishedWithError(filename);
    }
}

void Playlist::playCurrent() {
    int current = listView->currentRow();
    if (current > -1) {
        playItem(current);
    }
//    this->slot_doubleclicked_resource();
}

void Playlist::itemDoubleClicked(int row) {
//	qDebug("Playlist::itemDoubleClicked: row: %d", row );
	playItem(row);
}

void Playlist::showPopupMenu(const QPoint & pos) {
//	qDebug("Playlist::showPopupMenu: x: %d y: %d", pos.x(), pos.y() );

    // get select
    QList<QListWidgetItem *> items = listView->selectedItems();
    if (items.length() != 1) {
        return;
    }

    PlayListItem *item = qobject_cast<PlayListItem *>(listView->itemWidget(items.first()));
    if (!item) {
        return;
    }
    QString m_data = item->data();
    if (m_data.isEmpty()) {
        return;
    }
//    current_item = 1;

    QPoint itemPos = item->mapFromParent(pos);
    if (!item->rect().contains(itemPos)) {
        return;
    }

//    QPoint globalPos = this->mapToGlobal(pos);
    if (!popup->isVisible()) {
        playAct->setDisabled(0 == m_data.length());
        popup->move(listView->viewport()->mapToGlobal(pos) );
        popup->show();
    }
}

//kobe:添加多个文件文件夹或拖拽进多个文件文件夹时才会走这里，如果是支持乱序，则乱序选择一个开始播放20170725
void Playlist::startPlayPause() {
	// Start to play
    if (pref->play_order == Preferences::RandomPlay) {//随机播放
        playItem(chooseRandomItem());
    }
    else {//顺序播放 列表循环
        playItem(0);
    }
}

void Playlist::playItem( int n ) {
//    qDebug("Playlist::playItem: %d (count:%d)", n, pl.count());

    if ( (n >= pl.count()) || (n < 0) ) {
        qDebug("Playlist::playItem: out of range");
        emit playlistEnded();
        emit this->sig_playing_title("");
        return;
    }

//    qDebug("######playlist_path: '%s'", playlist_path.toUtf8().data() );

    QString filename = pl[n]->filename();
//    qDebug("######filename: '%s'", filename);
//    QString filename_with_path = playlist_path + "/" + filename;

    if (!filename.isEmpty()) {
        //pl[n].setPlayed(true);
//        setListCurrentItem(n);//0527 n - 1

        QFileInfo fi(filename);
        if (fi.exists()) {
            setListCurrentItem(n);//0527 n - 1
            // Local file
            QString name = fi.fileName();
            emit this->sig_playing_title(name);
            core->open(filename/*, 0*/);//每次从头开始播放文件
        }
        else {
            emit this->playListFinishedWithError(filename);
        }

        //0621 kobe:  core->open(filename, 0): 每次从头开始播放文件，    core->open(filename):每次从上次播放停止位置开始播放文件
//        core->open(filename);
        //        if (play_files_from_start)
//            core->open(filename/*, 0*/);//每次从头开始播放文件
//        else
//            core->open(filename);
        /* else {
            // Stream
            name = filename;
            //emit this->playListFinishedWithError(filename);
        }*/
    }
}

//20170725
void Playlist::playNext() {
//    emit this->sig_playing_title("");
//    qDebug("Playlist::playNext  pl[current_item]->name()=%s", pl[current_item]->name());

    if (pref->play_order == Preferences::RandomPlay) {//随机播放
        int chosen_item = chooseRandomItem();
        if (chosen_item == -1) {
            clearPlayedTag();
            chosen_item = chooseRandomItem();
            if (chosen_item == -1) chosen_item = 0;
        }
        playItem(chosen_item);
    }
    else if (pref->play_order == Preferences::ListLoopPlay) {//列表循环
        bool finished_list = (current_item+1 >= pl.count());
        if (finished_list) {
            clearPlayedTag();
        }
        if (finished_list) {
            playItem(0);
        } else {
            playItem(current_item+1);
        }
    }
    else {//顺序播放
        bool finished_list = (current_item+1 >= pl.count());
        if (finished_list) {
            clearPlayedTag();
        }
        if (finished_list) {
//            emit this->finish_list();
            emit this->showMessage(tr("Reached the end of the playlist"));
        }
        else {
            playItem(current_item + 1);
        }
    }
}

void Playlist::playPrev() {
    if (current_item > 0) {
        playItem(current_item - 1);
    }
    else {
        if (pl.count() > 1) {
            playItem(pl.count() - 1);
        }
    }
}


void Playlist::resumePlay() {
    if (pl.count() > 0) {
        if (current_item < 0) current_item = 0;
//        current_item = 0;//20170713
        playItem(current_item);
    }
}

void Playlist::getMediaInfo() {
    QString filename = core->mdat.filename;
    double duration = core->mdat.duration;
//    QString artist = core->mdat.clip_artist;
    //kobe:有的rmvb视频的clip_name存在乱码
    QString name;
    //QString name = core->mdat.clip_name;
    //if (name.isEmpty()) name = core->mdat.stream_title;

    if (name.isEmpty()) {
        QFileInfo fi(filename);
        name = fi.fileName();//20170713
//        if (fi.exists()) {
//            // Local file
//            name = fi.fileName();
//        } else {
//            // Stream
//            name = filename;
//        }
    }
//    if (!artist.isEmpty()) name = artist + " - " + name;

    for (int n = 0; n < pl.count(); n++) {
        if (pl[n]->filename() == filename) {
            current_item = n;//kobe 20170712
            // Found item
            if (pl[n]->duration() < 1) {
                if (!name.isEmpty()) {
                    pl[n]->setName(name.toUtf8().data());//edit by kobe
                }
                pl[n]->setDuration(duration);
            }
            else
            // Edited name (sets duration to 1)
            if (pl[n]->duration() == 1) {
                pl[n]->setDuration(duration);
            }
        }
    }
    updateView();
}

//0707
void Playlist::popupDialogtoSelectFiles() {
    //打开一个或多个文件时，此时只是将选择的文件加入播放列表，并不会自动去播放选择的文件
    Extensions e;
    QStringList files = MyFileDialog::getOpenFileNames(
                            this, tr("Select one or more files to open"),
                            lastDir(),
                            tr("Multimedia") + e.multimedia().forFilter() + ";;" +
                            tr("All files") +" (*.*)" );

    if (files.count()!=0) addFiles(files);
}

void Playlist::addFiles(QStringList files, AutoGetInfo auto_get_info) {
    bool get_info = (auto_get_info == GetInfo);
    get_info = true;

//    MediaData data;
    setCursor(Qt::WaitCursor);

    QString initial_file;
    if (pl.count() == 1) {
        initial_file = pl[0]->filename()/*.toUtf8().data()*/;//0526
        qDebug() << "pl[0]->filename()=" << pl[0]->filename();
    }
    int new_current_item = -1;

    for (int n = 0; n < files.count(); n++) {
        QString name = "";
        double duration = 0;

        //kobe 0606 如果选择多个文件，此时读取信息会耗时很长，导致界面卡顿，此处暂时不获取视频信息，在双击播放后再在函数updateView中更新视频的时长
//        if ( (get_info) && (QFile::exists(files[n])) ) {
//            data = InfoProvider::getInfo(files[n]);
//            name = data.displayName();
//            duration = data.duration;
//        }

//        qDebug() << "USE_INFOPROVIDER name=" << name << " duration=" << duration << " current_item=" << current_item;
        if (!initial_file.isEmpty() && files[n] == initial_file) {
            PlayListItem *first_item = pl.takeFirst();//pl.takeFirst();
            name = first_item->name();
            duration = first_item->duration();
            new_current_item = n;
        }
        this->addOneItem(files[n], name, duration);

        if (QFile::exists(files[n])) {
            playlist_load_latest_dir = QFileInfo(files[n]).absolutePath();
        }
    }

    unsetCursor();
//    if (new_current_item != -1) setListCurrentItem(new_current_item);
//    else setListCurrentItem(pl.count() - 1);//0526 kobe
    updateView();
}

void Playlist::addFile(QString file, AutoGetInfo auto_get_info) {
    //打开一个文件时
	addFiles( QStringList() << file, auto_get_info );
}

void Playlist::addDirectory() {
	QString s = MyFileDialog::getExistingDirectory(
                    this, tr("Choose a directory"),
                    lastDir() );

	if (!s.isEmpty()) {
		addDirectory(s);
        playlist_load_latest_dir = s;
	}
}

void Playlist::addOneDirectory(QString dir) {
	QStringList filelist;

	Extensions e;
	QRegExp rx_ext(e.multimedia().forRegExp());
	rx_ext.setCaseSensitivity(Qt::CaseInsensitive);

	QStringList dir_list = QDir(dir).entryList();

	QString filename;
    QStringList::Iterator it = dir_list.begin();
    while( it != dir_list.end() ) {
		filename = dir;
		if (filename.right(1)!="/") filename += "/";
		filename += (*it);
		QFileInfo fi(filename);
		if (!fi.isDir()) {
			if (rx_ext.indexIn(fi.suffix()) > -1) {
				filelist << filename;
			}
		}
		++it;
	}
	addFiles(filelist);
}

void Playlist::addDirectory(QString dir) {
	addOneDirectory(dir);

//	if (recursive_add_directory) {//递归
//		QFileInfoList dir_list = QDir(dir).entryInfoList(QStringList() << "*", QDir::AllDirs | QDir::NoDotAndDotDot);
//		for (int n=0; n < dir_list.count(); n++) {
//			if (dir_list[n].isDir()) {
//				qDebug("Playlist::addDirectory: adding directory: %s", dir_list[n].filePath().toUtf8().data());
//				addDirectory(dir_list[n].filePath());
//			}
//		}
//	}
}

void Playlist::removeTheSelected()
{
    int current = listView->currentRow();
    if ( (current >= pl.count()) || (current < 0) ) {
        qDebug("Playlist::playItem: out of range when remove");
        return;
    }

    QString filename = pl[current]->filename();
    if (!filename.isEmpty()) {
        MessageDialog msgDialog(0, tr("Confirm remove"),
                                tr("You're about to remove the file '%1' from the playlist.").arg(filename) + "<br>"+
                                tr("Are you sure you want to proceed?"), QMessageBox::Yes | QMessageBox::No);
        if (msgDialog.exec() != -1) {
            if (msgDialog.standardButton(msgDialog.clickedButton()) == QMessageBox::Yes) {
                removeSelected(filename);
            }
        }
//        removeSelected(filename);
    }
}

void Playlist::onPlayListItemDeleteBtnClicked(QString filename)
{
    if (!filename.isEmpty()) {
        MessageDialog msgDialog(0, tr("Confirm remove"),
                                tr("You're about to remove the file '%1' from the playlist.").arg(filename) + "<br>"+
                                tr("Are you sure you want to proceed?"), QMessageBox::Yes | QMessageBox::No);
        if (msgDialog.exec() != -1) {
            if (msgDialog.standardButton(msgDialog.clickedButton()) == QMessageBox::Yes) {
                removeSelected(filename);
            }
        }
    }
}

// Remove selected item
void Playlist::removeSelected(QString filename) {
    int first_selected = -1;
    int number_previous_item = 0;
    for (int i = 0; i < listView->count(); ++i) {
        QListWidgetItem *item = listView->item(i);
        PlayListItem *playlistItem = qobject_cast<PlayListItem *>(listView->itemWidget(item));
        if (playlistItem->data() == filename) {
            pl[i]->setMarkForDeletion(true);
            number_previous_item++;
            if (first_selected == -1) first_selected = i;
//            break;
            listView->removeItemWidget(item);
            delete listView->takeItem(listView->row(item));
            Q_ASSERT(listView->count() > 0);
            listView->updateScrollbar();
            listView->setCurrentItem(listView->item(0));
            break;
        }
    }

    QList<PlayListItem *>::Iterator it = pl.begin(), itend = pl.end();
    for(;it !=itend;it++)
    {
        if ( (*it)->markedForDeletion() ) {
            qDebug("remove '%s'", (*it)->filename().toUtf8().data());
            it = pl.erase(it);
//            it--;
            setModified(true);
            break;
        }
    }

    emit this->update_playlist_count(pl.count());
    if (pl.count() > 0) {
        noVideoFrame->hide();
        listView->show();
        this->playNext();//Fixed bug: #4915
    }
    else {
        noVideoFrame->show();
        listView->hide();
    }

    if (first_selected < current_item) {
        current_item -= number_previous_item;
    }

    if (isEmpty()) setModified(false);
    updateView();

    if (first_selected >= listView->count())
        first_selected = listView->count() - 1;
    if ( ( first_selected > -1) && ( first_selected < listView->count() ) ) {
//        listView->clearSelection();
        listView->setCurrentRow(first_selected);
    }
}

void Playlist::removeAll() {
	clear();
}

void Playlist::clearPlayedTag() {
    for (int n = 0; n < pl.count(); n++) {
        pl[n]->setPlayed(false);
    }
    updateView();
}

int Playlist::chooseRandomItem() {
    QList <int> fi; //List of not played items (free items)
    for (int n = 0; n < pl.count(); n++) {
        if (!pl[n]->played()) fi.append(n);
    }

//    qDebug("Playlist::chooseRandomItem: free items: %d", fi.count() );

    if (fi.count() == 0) return -1; // none free

    for (int i = 0; i < fi.count(); i++) {
        qDebug("Playlist::chooseRandomItem: * item: %d", fi[i]);
    }

    int selected = (int) ((double) fi.count() * rand()/(RAND_MAX+1.0));
//    qDebug("Playlist::chooseRandomItem: selected item: %d (%d)", selected, fi[selected]);
    return fi[selected];
}

void Playlist::swapItems(int item1, int item2 ) {
}


void Playlist::upItem() {
//	qDebug("Playlist::upItem");

    int current = listView->currentRow();
    qDebug(" currentRow: %d", current );

    moveItemUp(current);

}

void Playlist::downItem() {
    qDebug("Playlist::downItem");

    int current = listView->currentRow();
    qDebug(" currentRow: %d", current );

    moveItemDown(current);
}

void Playlist::moveItemUp(int current){

}
void Playlist::moveItemDown(int current	){

}

void Playlist::editCurrentItem() {
//	int current = listView->currentRow();
//	if (current > -1) editItem(current);
}

void Playlist::editItem(int item) {
}

void Playlist::deleteSelectedFileFromDisk() {
    qDebug("Playlist::deleteSelectedFileFromDisk");

    int current = listView->currentRow();
    if (current > -1) {
//		// If more that one row is selected, select only the current one
//		listView->clearSelection();
//		listView->setCurrentCell(current, 0);

        QString filename = pl[current]->filename();
        qDebug() << "Playlist::deleteSelectedFileFromDisk: current file:" << filename;

        //20170715
        QFileInfo fi(filename);
        if (fi.exists() && fi.isFile() && fi.isWritable()) {
            // Ask the user for confirmation
            MessageDialog msgDialog(0, tr("Confirm deletion"),
                                    tr("You're about to DELETE the file '%1' from your drive.").arg(filename) + "<br>"+
                                    tr("This action cannot be undone. Are you sure you want to proceed?"), QMessageBox::Yes | QMessageBox::No);
            if (msgDialog.exec() != -1) {
                if (msgDialog.standardButton(msgDialog.clickedButton()) == QMessageBox::Yes) {
                    // Delete file
                    bool success = QFile::remove(filename);
                    if (success) {
                        // Remove item from the playlist
                        removeSelected(filename);
                    } else {
//                        QMessageBox::warning(this, tr("Deletion failed"),
//                            tr("It wasn't possible to delete '%1'").arg(filename));
                        MessageDialog warnDialog(0, tr("Deletion failed"), tr("It wasn't possible to delete '%1'").arg(filename), QMessageBox::Ok);
                        warnDialog.exec();
                    }
                }
            }
        } else {
//            qDebug("Playlist::deleteSelectedFileFromDisk: file doesn't exists, it's not a file or it's not writable");
            MessageDialog infoDialog(0, tr("Error deleting the file"), tr("It's not possible to delete '%1' from the filesystem.").arg(filename), QMessageBox::Ok);
            infoDialog.exec();
        }
    }
}

// Drag&drop
void Playlist::dragEnterEvent( QDragEnterEvent *e ) {
//	qDebug("Playlist::dragEnterEvent");

	if (e->mimeData()->hasUrls()) {
		e->acceptProposedAction();
	}
}

void Playlist::dropEvent( QDropEvent *e ) {
//	qDebug("Playlist::dropEvent");

	QStringList files;

	if (e->mimeData()->hasUrls()) {
		QList <QUrl> l = e->mimeData()->urls();
		QString s;
		for (int n=0; n < l.count(); n++) {
			if (l[n].isValid()) {
				qDebug("Playlist::dropEvent: scheme: '%s'", l[n].scheme().toUtf8().data());
				if (l[n].scheme() == "file") 
					s = l[n].toLocalFile();
				else
					s = l[n].toString();
				/*
				qDebug(" * '%s'", l[n].toString().toUtf8().data());
				qDebug(" * '%s'", l[n].toLocalFile().toUtf8().data());
				*/
				qDebug("Playlist::dropEvent: file: '%s'", s.toUtf8().data());
				files.append(s);
			}
		}
	}

	files.sort();

	QStringList only_files;
	for (int n = 0; n < files.count(); n++) {
		if ( QFileInfo( files[n] ).isDir() ) {
			addDirectory( files[n] );
		} else {
			only_files.append( files[n] );
		}
	}

	addFiles( only_files );
}


void Playlist::hideEvent( QHideEvent * ) {
	emit visibilityChanged(false);
}

void Playlist::showEvent( QShowEvent * ) {
	emit visibilityChanged(true);
}

void Playlist::closeEvent( QCloseEvent * e )  {
	saveSettings();
	e->accept();
}

void Playlist::getMediaInfo(const MediaData & mdat) {
    /*qDebug("Playlist::getMediaInfo");

    QString filename = mdat.filename;
    double duration = mdat.duration;
    QString artist = mdat.clip_artist;
    QString video_url = mdat.stream_path;

//	#if defined(Q_OS_WIN) || defined(Q_OS_OS2)
//	filename = Helper::changeSlashes(filename);
//	#endif

    QString name;
    if (change_name) {
        name = mdat.clip_name;
        if (name.isEmpty()) name = mdat.stream_title;

        if (name.isEmpty()) {
            QFileInfo fi(filename);
            if (fi.exists()) {
                // Local file
                name = fi.fileName();
            } else {
                // Stream
                name = filename;
            }
        }
        if (!artist.isEmpty()) name = artist + " - " + name;
    }

    for (int n = 0; n < count(); n++) {
        PLItem * i = itemData(n);
        if (i->filename() == filename) {
            // Found item
            bool modified_name = !(i->filename().endsWith(i->name()));
            if (i->duration() < 1) {
                if (!modified_name && !name.isEmpty()) {
                    i->setName(name);
                }
                i->setDuration(duration);
            }
            else
            // Edited name (sets duration to 1)
            if (i->duration() == 1) {
                i->setDuration(duration);
            }
            i->setVideoURL(video_url);
        }
    }*/
}

void Playlist::playerFailed(QProcess::ProcessError e) {
	qDebug("Playlist::playerFailed");
    emit this->sig_playing_title("");
//	if (ignore_player_errors) {
//        if (e != QProcess::FailedToStart) {
//            playNext();
//        }
//	}
}

void Playlist::playerFinishedWithError(int e) {
    emit this->sig_playing_title("");
//    qDebug("@@@@@@@@@@@@@@@@@@@Playlist::playerFinishedWithError: %d", e);
//    if (ignore_player_errors) {
//        playNext();
//    }
}

void Playlist::maybeSaveSettings() {
    if (isModified()) saveSettings();
}

void Playlist::saveSettings() {
	QSettings * set = settings;

    //Save current list
    set->beginGroup( "playlist_contents");
    set->setValue( "playlist_load_latest_dir", playlist_load_latest_dir );
    set->setValue("count", (int)pl.count());
    for ( int n=0; n < pl.count(); n++ ) {
        set->setValue(QString("item_%1_filename").arg(n), pl[n]->filename());
        set->setValue(QString("item_%1_duration").arg(n), pl[n]->duration());
        set->setValue(QString("item_%1_name").arg(n), pl[n]->name());
    }
    set->setValue("current_item", current_item );
    set->setValue("play_order", (int) pref->play_order);//20170725

    set->endGroup();
}

void Playlist::loadSettings() {
	QSettings * set = settings;

    //Load latest list
    set->beginGroup( "playlist_contents");
    playlist_load_latest_dir = set->value( "laylist_load_latest_dir", playlist_load_latest_dir ).toString();//latest_dir  0526

    pref->play_order = (Preferences::PlayOrder) set->value("play_order", (int) pref->play_order).toInt();//20170725

    int count = set->value("count", 0).toInt();
    QString filename, name;
    double duration;
    for (int n=0; n < count; n++ ) {
        filename = set->value(QString("item_%1_filename").arg(n), "").toString()/*.toUtf8().data()*/;
        duration = set->value(QString("item_%1_duration").arg(n), -1).toDouble();
        name = set->value( QString("item_%1_name").arg(n), "" ).toString()/*.toUtf8().data()*/;
//        qDebug() << "===============kobe test filename:" << filename << "   name=" << name << " duration=" << duration;//.toUtf8().data();
        this->addOneItem(filename, name, duration);//kobe add playlist contents
    }

    int index = set->value("current_item", -1).toInt();
    setListCurrentItem(index);

    emit this->update_playlist_count(pl.count());
    if (pl.count() > 0) {
        noVideoFrame->hide();
        listView->show();
    }
    else {
        noVideoFrame->show();
        listView->hide();
    }

    updateView();

    set->endGroup();
}

QString Playlist::lastDir() {
    QString last_dir = playlist_load_latest_dir;
	if (last_dir.isEmpty()) last_dir = pref->latest_dir;
	return last_dir;
}

void Playlist::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

//    QPainter p(this);
//    p.setCompositionMode(QPainter::CompositionMode_Clear);
//    p.fillRect(rect(), Qt::SolidPattern);//p.fillRect(0, 0, this->width(), this->height(), Qt::SolidPattern);
}

//#include "moc_playlist.cpp"
