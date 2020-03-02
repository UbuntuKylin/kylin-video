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

#ifndef _PLAYLIST_H_
#define _PLAYLIST_H_

#include <QList>
#include <QStringList>
#include <QWidget>
#include <QFrame>
#include <QProcess>
#include <QGraphicsOpacityEffect>

#include "../smplayer/mediadata.h"

class PlayListView;
class MyAction;
class Core;
class QMenu;
class QSettings;
class QToolButton;
class QTimer;
class QPushButton;
class QLabel;
class QHBoxLayout;

class Playlist : public QFrame
{
	Q_OBJECT

public:
	enum AutoGetInfo { NoGetInfo = 0, GetInfo = 1, UserDefined = 2 };
    enum M3UFormat { M3U = 0, M3U8 = 1, DetectFormat = 2 };

    Playlist(Core *c, QWidget * parent = 0, Qt::WindowFlags f = Qt::Window);
	~Playlist();

    void setConfigPath(const QString & config_path);
	void clear();

    int count();
	bool isEmpty();
    bool rowIsEmpty();

    bool isModified() { return modified; };
    void setPlaying(const QString &filepath, int index);

    void setViewHeight();

    void createNoVideo();
    void createTable();
    void createToolbar();
    void clearPlayedTag();
    int chooseRandomItem();

    // EDIT BY NEO -->
//	void sortBy(int section, bool revert, int count);
    // <--
    QString lastDir();

    void setPlaylistFilename(const QString &);
    QString playlistFilename() { return playlist_filename; };

    void updateWindowTitle();

    // Preferences
    void setDirectoryRecursion(bool b) { recursive_add_directory = b; };
    void setAutoGetInfo(bool b) { automatically_get_info = b; };
    void setPlayFilesFromStart(bool b) { play_files_from_start = b; };
    void setIgnorePlayerErrors(bool b) { ignore_player_errors = b; };
    void setStartPlayOnLoad(bool b) { start_play_on_load = b; };
    void setAutomaticallyPlayNext(bool b) { automatically_play_next = b; };

    bool directoryRecursion() { return recursive_add_directory; };
    bool autoGetInfo() { return automatically_get_info; };
    bool playFilesFromStart() { return play_files_from_start; };
    bool ignorePlayerErrors() { return ignore_player_errors; };
    bool startPlayOnLoad() { return start_play_on_load; };
    bool automaticallyPlayNext() { return automatically_play_next; };

    void allowDeleteFromDisk(bool enabled) { allow_delete_from_disk = enabled; };
    bool isDeleteFromDiskAllowed() { return allow_delete_from_disk; };


public slots:
    void loadSingleItem(QString filename, QString name, double duration);
    void loadItemWithoutUI(QString filename, QString name, double duration);

	// Start playing, from item 0 if shuffle is off, or from
	// a random item otherwise
	void startPlayPause();

	void playItem(int n);

	virtual void playNext();
    virtual void playPrev();

    void playNextAuto(); // Called from GUI when a file finished

	virtual void resumePlay();
	virtual void removeAll();
    virtual void onPlayListItemDeleteBtnClicked(const QStringList &filepathlist);

	virtual void moveItemUp(int);
	virtual void moveItemDown(int);

    virtual void popupDialogtoSelectFiles();
	virtual void addDirectory();

//    void addCurrentFile();
    void addFiles();
    void addUrls();

	virtual void addFile(QString file, AutoGetInfo auto_get_info = UserDefined);
	virtual void addFiles(QStringList files, AutoGetInfo auto_get_info = UserDefined);

	// Adds a directory, no recursive
	virtual void addOneDirectory(QString dir);

	// Adds a directory, maybe with recursion (depends on user config)
	virtual void addDirectory(QString dir);

	// EDIT BY NEO -->
//	virtual void sortBy(int section);
	// <--

    void deleteSelectedFileFromDisk(const QStringList &filelist);

    bool maybeSave();
    void load();

    bool saveCurrentPlaylist();
    bool save(const QString & filename = QString::null);

    void load_m3u(QString file, M3UFormat format = DetectFormat);
    bool save_m3u(QString file);

    void load_pls(QString file);
    bool save_pls(QString file);

    void loadXSPF(const QString & filename);
    bool saveXSPF(const QString & filename);

    void setModified(bool);

    // Slots to connect from basegui
    void getMediaInfo(const MediaData &);
    void playerFailed(QProcess::ProcessError);
    void playerFinishedWithError(int);

    void itemActivated(const QModelIndex & index );
    void upItem();
    void downItem();

    void copyURL();
    void openFolder();

    void saveSettings();
    void updatePlayOrderSettings();
    void loadSettings();
    void maybeSaveSettings();

    void onResortVideos(const QStringList &sortList, int index);
    void onPlayListItemDoubleClicked(int row, const QString &filename);
    void onPlayListChanged(const VideoPtrList medialist);

signals:
	void playlistEnded();
    void cleanPlaylistFinished();
    void requestSetPlayingTitle(QString title);
    void update_playlist_count(int count);
    void closePlaylist();
    void playListFinishedWithError(QString errorStr);
    void showMessage(QString text);
    void finish_list();
//    void requestToAddCurrentFile();
    void modifiedChanged(bool);
    void windowTitleChanged(const QString & title);
    void requestGetMediaInfo(const QStringList &files);

#ifdef PREVIEW_TEST
    void requestDestoryPreview();
#endif

protected:
    virtual void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    virtual void dragEnterEvent(QDragEnterEvent *) Q_DECL_OVERRIDE;
    virtual void dropEvent(QDropEvent *) Q_DECL_OVERRIDE;
    virtual void hideEvent(QHideEvent *) Q_DECL_OVERRIDE;
    virtual void showEvent(QShowEvent *) Q_DECL_OVERRIDE;

private:
	QString playlist_path;
    QString playlist_filename;
    QString latest_dir;

    Core * m_core = nullptr;
    QFrame *noVideoFrame = nullptr;
    QLabel *novideo_icon = nullptr;
    QLabel *novideo_text = nullptr;
    QPushButton *add_Btn = nullptr;
    PlayListView *m_playlistView = nullptr;
    QLabel *titleLabel = nullptr;
    QFrame *btAddFrame = nullptr;
    QPushButton *btDel = nullptr;
    QPushButton *btAdd = nullptr;
    QSettings *set = nullptr;
    bool modified;
    int m_currentItemIndex;
    QTimer *save_timer = nullptr;
    QHBoxLayout *title_layout = nullptr;

    //Preferences
    bool recursive_add_directory;
    bool automatically_get_info;
    bool play_files_from_start;

    bool start_play_on_load;
    bool automatically_play_next;
    bool ignore_player_errors;

    bool allow_delete_from_disk;
};

#endif

