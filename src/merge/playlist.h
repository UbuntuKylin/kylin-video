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
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef _PLAYLIST_H_
#define _PLAYLIST_H_

#include <QList>
#include <QStringList>
#include <QWidget>
#include <QFrame>
#include <QProcess>
#include <QGraphicsOpacityEffect>

class MyAction;
class Core;
class QMenu;
class QSettings;
class QToolButton;
class QTimer;
class PlayListItem;
class PlayListView;
class QListWidgetItem;
class QPushButton;
class QLabel;
class QHBoxLayout;

class Playlist : public QFrame
{
	Q_OBJECT

public:
	enum AutoGetInfo { NoGetInfo = 0, GetInfo = 1, UserDefined = 2 };

    Playlist( Core *c, QWidget * parent = 0, Qt::WindowFlags f = Qt::Window);
	~Playlist();

	void clear();
	int count();
	bool isEmpty();

    bool isModified() { return modified; };

    //0606
    void setTransparent(bool transparent);
    void set_widget_opacity(const float &opacity=0.8);
    void setViewHeight();

protected:
    void paintEvent(QPaintEvent *event);

public slots:
    void addOneItem(QString filename, QString name, double duration);

	// Start playing, from item 0 if shuffle is off, or from
	// a random item otherwise
	void startPlay();

	void playItem(int n);

	virtual void playNext();
    virtual void playPrev();

	virtual void resumePlay();
    virtual void removeTheSelected();
    virtual void removeSelected(QString filename);
	virtual void removeAll();
    virtual void onPlayListItemDeleteBtnClicked(QString filename);

	virtual void moveItemUp(int);
	virtual void moveItemDown(int);

    virtual void popupDialogtoSelectFiles();
	virtual void addDirectory();

	virtual void addFile(QString file, AutoGetInfo auto_get_info = UserDefined);
	virtual void addFiles(QStringList files, AutoGetInfo auto_get_info = UserDefined);

	// Adds a directory, no recursive
	virtual void addOneDirectory(QString dir);

	// Adds a directory, maybe with recursion (depends on user config)
	virtual void addDirectory(QString dir);

	// EDIT BY NEO -->
//	virtual void sortBy(int section);
	// <--

	virtual void deleteSelectedFileFromDisk();

	virtual void getMediaInfo();

    void setModified(bool);

    void slot_listview_current_item_changed(QListWidgetItem * current, QListWidgetItem * previous);
    void slot_doubleclicked_resource(QString filename);
    void doubleclicked(QListWidgetItem *item);

signals:
	void playlistEnded();
	void visibilityChanged(bool visible);
    void sig_playing_title(QString title);
    void update_playlist_count(int count);
    void closePlaylist();
    void playListFinishedWithError(QString errorStr);
    void showMessage(QString text);
    void finish_list();

protected:
	void updateView();
    void setListCurrentItem(int current);
	void clearPlayedTag();
	int chooseRandomItem();
	void swapItems(int item1, int item2 );
	// EDIT BY NEO -->
//	void sortBy(int section, bool revert, int count);
	// <--
	QString lastDir();

protected slots:
	virtual void playCurrent();
	virtual void itemDoubleClicked(int row);
    virtual void showPopupMenu(const QPoint & pos);
	virtual void upItem();
	virtual void downItem();
	virtual void editCurrentItem();
	virtual void editItem(int item);

	virtual void saveSettings();
	virtual void loadSettings();

	virtual void maybeSaveSettings();

	void playerFailed(QProcess::ProcessError);
	void playerFinishedWithError(int);


protected:
    void createNoVideo();
	void createTable();
    void createToolbar();

protected:
	virtual void dragEnterEvent( QDragEnterEvent * ) ;
	virtual void dropEvent ( QDropEvent * );
	virtual void hideEvent ( QHideEvent * );
	virtual void showEvent ( QShowEvent * );
	virtual void closeEvent( QCloseEvent * e );

protected:
    QList<PlayListItem *> pl;
	int current_item;

	QString playlist_path;
    QString playlist_load_latest_dir;

	Core * core;
	QMenu * popup;
    QFrame      *noVideoFrame;
    QLabel      *novideo_icon;
    QLabel      *novideo_text;
    QPushButton *add_Btn;
    PlayListView *listView;
    QLabel      *titleLabel;
    QFrame *btAddFrame;
    QPushButton *btDel;
    QPushButton *btAdd;
    MyAction * playAct;
    MyAction * removeSelectedAct;
    MyAction * deleteSelectedFileFromDiskAct;

private:
    bool modified;
	QTimer * save_timer;
	int row_spacing;
	bool automatically_play_next;
    QHBoxLayout *title_layout;
};

#endif

