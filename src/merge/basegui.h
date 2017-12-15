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
#ifndef _BASEGUI_H_
#define _BASEGUI_H_

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsProxyWidget>
#include <QStackedLayout>

#include "../smplayer/mediadata.h"
#include "../smplayer/mediasettings.h"
#include "../smplayer/preferences.h"
#include "../smplayer/core.h"
#include "../smplayer/config.h"

class QPushButton;
class QWidget;
class QMenu;
class MplayerWindow;
class QLabel;
class FilePropertiesDialog;
class AboutDialog;
class HelpDialog;
class Playlist;
class EscTip;
class TipWidget;
class MyAction;
class MyActionGroup;
class PreferencesDialog;
class TitleWidget;
class BottomWidget;
class PlayMask;
class VideoPreview;
//class ShortcutsWidget;

class BaseGui : public QMainWindow
{
    Q_OBJECT
    
public:
    BaseGui(QString arch_type = "", QWidget* parent = 0, Qt::WindowFlags flags = 0);
	~BaseGui();

	/* Return true if the window shouldn't show on startup */
	virtual bool startHidden() { return false; };

    void setTransparent(bool transparent);
    void set_widget_opacity(const float &opacity=0.8);

	Core * getCore() { return core; };
    Playlist * getPlaylist() { return playlistWidget; };

    void setPlaylisshowPlaylisttVisible(bool visible);
    void setPlaylistVisible(bool visible);
    void slideEdgeWidget(QWidget *right, QRect start, QRect end, int delay, bool hide = false);
    void disableControl(int delay = 350);
    bool mouseInControlsArea();

public slots:
    void slot_mute(/*bool b*/);
    void reset_mute_button();
    void start_top_and_bottom_timer();
	virtual void open(QString file); // Generic open, autodetect type.
	virtual void openFile();
	virtual void openFile(QString file);
	virtual void openFiles(QStringList files);
	virtual void openDirectory();
	virtual void openDirectory(QString directory);
    virtual void openURL();
    virtual void openURL(QString url);
    virtual void showAboutDialog();
    virtual void showHelpDialog();
	virtual void loadSub();
	virtual void loadAudioFile(); // Load external audio file
	void setInitialSubtitle(const QString & subtitle_file);
	virtual void showPreferencesDialog();
	virtual void showFilePropertiesDialog();
	virtual void showGotoDialog();
	virtual void showSubDelayDialog();
	virtual void showAudioDelayDialog();
	virtual void exitFullscreen();
	virtual void toggleFullscreen(bool);
    void setStayOnTop(bool b);
    virtual void changeStayOnTop(int);
    virtual void checkStayOnTop(Core::State);
    void changePlayOrder(int play_order);
	void setForceCloseOnFinish(int n) { arg_close_on_finish = n; };
	int forceCloseOnFinish() { return arg_close_on_finish; };
	void setForceStartInFullscreen(int n) { arg_start_in_fullscreen = n; };
	int forceStartInFullscreen() { return arg_start_in_fullscreen; };
    void slot_min();
    void slot_max();
    void slot_close();
    void slot_menu();
    void disableSomeComponent();
    void setPlaylistProperty();
    void slot_playlist();
    void slot_resize_corner();
    void slot_set_fullscreen();
    void showTipWidget(const QString text);
    void hideTipWidget();
    void showOrHideEscWidget(bool b);
    void open_screenshot_directory();
    void ready_save_pre_image(int time);
//    void showShortcuts();

protected slots:
	virtual void closeWindow();
    virtual void trayIconActivated(QSystemTrayIcon::ActivationReason);
    virtual void toggleShowAll();
    virtual void showAll(bool b);
    virtual void showAll();
    virtual void quit();
	virtual void setJumpTexts();
	virtual void openRecent();
	virtual void enterFullscreenOnPlay();
	virtual void exitFullscreenOnStop();
	virtual void exitFullscreenIfNeeded();
	virtual void playlistHasFinished();
	virtual void displayState(Core::State state);
	virtual void displayMessage(QString message);
    virtual void gotCurrentTime(double, bool);
	virtual void updateWidgets();
	virtual void newMediaLoaded();
	virtual void updateMediaInfo();
	void gotNoFileToPlay();
	void gotForbidden();
	virtual void enableActionsOnPlaying();
	virtual void disableActionsOnStop();
	virtual void togglePlayAction(Core::State);
	void resizeMainWindow(int w, int h);
	void resizeWindow(int w, int h);
	void centerWindow();
	virtual void displayGotoTime(int);
	//! You can call this slot to jump to the specified percentage in the video, while dragging the slider.
	virtual void goToPosOnDragging(int);
	virtual void showPopupMenu();
	virtual void showPopupMenu( QPoint p );
	virtual void leftClickFunction();
	virtual void rightClickFunction();
	virtual void doubleClickFunction();
	virtual void middleClickFunction();
	virtual void xbutton1ClickFunction();
	virtual void xbutton2ClickFunction();
	virtual void processFunction(QString function);
	virtual void dragEnterEvent( QDragEnterEvent * ) ;
	virtual void dropEvent ( QDropEvent * );
	virtual void applyNewPreferences();
	virtual void applyFileProperties();
	virtual void clearRecentsList();
	virtual void moveWindowDiff(QPoint diff);

    virtual void loadActions();
    virtual void saveActions();

	// Single instance stuff
#ifdef SINGLE_INSTANCE
	void handleMessageFromOtherInstances(const QString& message);
#endif

	//! Called when core can't parse the mplayer version and there's no
	//! version supplied by the user
	void askForMplayerVersion(QString);

	void showExitCodeFromMplayer(int exit_code);
	void showErrorFromMplayer(QProcess::ProcessError);
    void showErrorFromPlayList(QString errorStr);

//	//! Clears the mplayer log
    void clearMplayerLog();

//	//! Saves the line from the mplayer output
    void recordMplayerLog(QString line);

signals:
    void sigActionsEnabled(bool);
    void setPlayOrPauseEnabled(bool);
    void setStopEnabled(bool);
    void frameChanged(int);
    void ABMarkersChanged(int secs_a, int secs_b);
    void videoInfoChanged(int width, int height, double fps);
    void timeChanged(QString time_ready_to_print, QString all_time);
    void clear_playing_title();

	//! Sent when the user wants to close the main window
	void quitSolicited();

	//! Sent when another instance requested to play a file
	void openFileRequested();

    void change_playlist_btn_status(bool);//0619

    void guiChanged();//kobe 20170710
    void send_save_preview_image_name(int time, QString filepath);

protected:
#if QT_VERSION < 0x050000
	virtual void hideEvent( QHideEvent * );
	virtual void showEvent( QShowEvent * );
#else
	virtual bool event(QEvent * e);
	bool was_minimized;
#endif
    void closeEvent(QCloseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    virtual bool eventFilter(QObject *obj, QEvent *event) override;
    virtual void resizeEvent(QResizeEvent *e) override;//20170720
	void createCore();
	void createMplayerWindow();
	void createPlaylist();
	void createPanel();
	void createPreferencesDialog();
	void createFilePropertiesDialog();
	void setDataToFileProperties();
    void createAboutDialog();
    void createHelpDialog();
    void setDataToAboutDialog();
	void initializeGui();
    void createActionsAndMenus();
    void createTrayActions();
    void addTrayActions();
    void createHiddenActions();
	void setActionsEnabled(bool);
	void updateRecents();
    void loadConfig();
    virtual void keyPressEvent(QKeyEvent *event);

protected:
    QStackedLayout *contentLayout;
	QWidget * panel;
    TitleWidget *m_topToolbar;
    BottomWidget *m_bottomToolbar;

    // Menu File
    QMenu *openMenu;//打开
    MyAction *openFileAct;//打开文件
    MyAction *openDirectoryAct;//打开文件夹
    MyAction *openURLAct;//打开URL
    MyAction *clearRecentsAct;//清空最近的文件
    QMenu *recentfiles_menu;//打开最近的文件

    QMenu *playMenu;//播放控制
    QMenu * control_menu;
    MyAction * rewind1Act;
    MyAction * rewind2Act;
    MyAction * rewind3Act;
    MyAction * forward1Act;
    MyAction * forward2Act;
    MyAction * forward3Act;
    QMenu * speed_menu;
    MyAction * gotoAct;
    // Menu Speed
    MyAction * normalSpeedAct;
    MyAction * halveSpeedAct;
    MyAction * doubleSpeedAct;
    MyAction * decSpeed10Act;
    MyAction * incSpeed10Act;
    MyAction * decSpeed4Act;
    MyAction * incSpeed4Act;
    MyAction * decSpeed1Act;
    MyAction * incSpeed1Act;
    MyAction * playPrevAct;
    MyAction * playNextAct;

    QMenu * aspect_menu;
    // Aspect Action Group
    MyActionGroup * aspectGroup;
    MyAction * aspectDetectAct;
    MyAction * aspectNoneAct;
    MyAction * aspect11Act;		// 1:1
    MyAction * aspect32Act;		// 3:2
    MyAction * aspect43Act;		// 4:3
    MyAction * aspect118Act;	// 11:8
    MyAction * aspect54Act;		// 5:4
    MyAction * aspect149Act;	// 14:9
    MyAction * aspect1410Act;	// 14:10
    MyAction * aspect169Act;	// 16:9
    MyAction * aspect1610Act;	// 16:10
    MyAction * aspect235Act;	// 2.35:1

    // Rotate Group
    MyActionGroup * rotateGroup;
    MyAction * rotateNoneAct;
    MyAction * rotateClockwiseFlipAct;
    MyAction * rotateClockwiseAct;
    MyAction * rotateCounterclockwiseAct;
    MyAction * rotateCounterclockwiseFlipAct;

    MyAction * flipAct;
    MyAction * mirrorAct;

    // Rotate menu
    QMenu * rotate_flip_menu;
    QMenu * rotate_menu;

//    MyAction * shortcutsAct;
    MyAction * screenshotAct;

    QMenu * ontop_menu;
    // Menu StayOnTop
    MyActionGroup * onTopActionGroup;
    MyAction * onTopAlwaysAct;
    MyAction * onTopNeverAct;
    MyAction * onTopWhilePlayingAct;

    //play order
    QMenu * play_order_menu;
    MyActionGroup * playOrderActionGroup;
    MyAction * orderPlaysAct;
    MyAction * randomPlayAct;
    MyAction * listLoopPlayAct;

    QMenu *audioMenu;//声音
    MyAction * muteAct;
    MyAction * decVolumeAct;
    MyAction * incVolumeAct;
    MyAction * decAudioDelayAct;
    MyAction * incAudioDelayAct;
    MyAction * audioDelayAct; // Ask for delay

    // Stereo Mode Action Group
    MyActionGroup * stereoGroup;
    MyAction * stereoAct;
    MyAction * leftChannelAct;
    MyAction * rightChannelAct;
    MyAction * monoAct;
    MyAction * reverseAct;

    QMenu * stereomode_menu;

    QMenu *subtitlesMenu;//字幕
    MyAction * loadSubsAct;
    MyAction * subVisibilityAct;

    MyAction *showPreferencesAct;//设置
    MyAction *showPropertiesAct;//信息和属性
    MyAction *aboutAct;
    MyAction *helpAct;
    MyAction *quitAct;
    MyAction *openDirAct;

    QMenu *popup;
    QMenu *main_popup;
    QMenu * audiochannels_menu;
    MyActionGroup * channelsGroup;
    MyAction * channelsStereoAct;
    MyAction * channelsSurroundAct;
    MyAction * channelsFull51Act;
    MyAction * channelsFull61Act;
    MyAction * channelsFull71Act;

    //tray menu and actions
    QMenu *tray_menu;
    MyAction *action_show;
    MyAction *action_openshotsdir;


    //Hide actions
    MyAction *playlist_action;
    MyAction *play_pause_aciton;
    MyAction *stopAct;
    MyAction *fullscreenAct;

    PreferencesDialog *pref_dialog;
    FilePropertiesDialog *file_dialog;
    AboutDialog *aboutDlg;
    HelpDialog *helpDlg;
	Core * core;
	MplayerWindow *mplayerwindow;
    Playlist * playlistWidget;
	QString pending_actions_to_run;

	// Force settings from command line
	int arg_close_on_finish; // -1 = not set, 1 = true, 0 = false
	int arg_start_in_fullscreen; // -1 = not set, 1 = true, 0 = false

private:
	bool was_maximized;
    QString mplayer_log;
	bool ignore_show_hide_events;
    bool isFinished;
    bool isPlaying;
    bool fullscreen;
    QPoint mainwindow_pos;
    QPoint playlist_pos;
    bool trayicon_playlist_was_visible;
    QPushButton *resizeCorner;
    bool resizeFlag;
    QSystemTrayIcon *tray;

    PlayMask *play_mask;
    EscTip *escWidget;
    TipWidget *tipWidget;
    QTimer *tip_timer;
    QString arch;
    VideoPreview *video_preview;
//    ShortcutsWidget *shortcuts_widget;
};
    
#endif

