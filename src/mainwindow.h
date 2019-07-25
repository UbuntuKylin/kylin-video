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

#ifndef _MAINWINDOW_H_
#define _MAINWINDOW_H_

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsProxyWidget>
#include <QStackedLayout>

#include "smplayer/mediadata.h"
#include "smplayer/mediasettings.h"
#include "smplayer/preferences.h"
#include "smplayer/core.h"

class QPushButton;
class QWidget;
class QMenu;
class VideoWindow;
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
class BottomController;
class FilterHandler;
//class ShortcutsWidget;
class CoverWidget;
class InfoWorker;
class MaskWidget;
class ControllerWorker;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    MainWindow(QString arch_type = "", QString snap = "", ControllerWorker *controller = NULL, QWidget* parent = 0);
    ~MainWindow();

    void initRegisterMeta();
    void createPanel();
    void createVideoWindow();
    void createCore();
    void createPlaylist();
    void createTopTitleBar();
    void createBottomToolBar();
    void createActionsAndMenus();
    void createTrayActions();
    void createTipWidget();
    void createEscWidget();
    void createMaskWidget();
    void createPreferencesDialog();
    void createFilePropertiesDialog();
    void initRemoteControllerConnections();
    void loadConfigForUI();
    void setStayOnTop(bool b);

    void setDataToFileProperties();
    void createAboutDialog();
    void createHelpDialog();
    void setDataToAboutDialog();


    void setActionsEnabled(bool);
    void updateRecents();

    void updateMuteWidgets();
    void updateOnTopWidgets();
    void updatePlayOrderWidgets();

    void setPlaylistVisible(bool visible);
    void slideEdgeWidget(QWidget *right, QRect start, QRect end, int delay, bool hide = false);
    void disableControl(int delay = 350);

    void parseArguments();
    void bindThreadWorker(InfoWorker *worker);

    Core * getCore() { return core; };

public slots:
    void changeStayOnTop(int);
    void checkStayOnTop(Core::State);
    void changePlayOrder(int play_order);
    void powerOffPC();


    void slot_mute(/*bool b*/);
    virtual void doOpen(QString file); // Generic open, autodetect type.
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

	void setForceCloseOnFinish(int n) { arg_close_on_finish = n; };
	int forceCloseOnFinish() { return arg_close_on_finish; };
	void setForceStartInFullscreen(int n) { arg_start_in_fullscreen = n; };
	int forceStartInFullscreen() { return arg_start_in_fullscreen; };
    void slot_min();
    void onResponseMaxWindow(bool b);
    void slot_close();
    void slot_menu();
    void disableSomeComponent();
    void setPlaylistProperty();
    void slot_playlist();
    void slot_set_fullscreen();
    void showTipWidget(const QString text);
    void onShowOrHideEscWidget(bool b);
    void open_screenshot_directory();
    void onSavePreviewImage(int time);
//    void showShortcuts();

    void startPlayPause();
    void onMeidaFilesAdded(const VideoPtrList medialist);

    void setBackgroudPixmap(QString pixmapDir);

protected slots:
	virtual void closeWindow();
    virtual void trayIconActivated(QSystemTrayIcon::ActivationReason);
    void toggleShowAll();
    void showAll(bool b);
    void showAll();
    void quit();
    void setJumpTexts();
    void openRecent();
    void enterFullscreenOnPlay();
    void exitFullscreenOnStop();
    void exitFullscreenIfNeeded();
    void playlistHasFinished();
    void displayState(Core::State state);
    void displayMessage(QString message);
    void gotCurrentTime(double, bool);
    void updateWidgets();
    void newMediaLoaded();
    void updateMediaInfo();
//    void displayABSection(int secs_a, int secs_b);
    void displayVideoInfo(int width, int height, double fps);
    void displayBitrateInfo(int vbitrate, int abitrate);
	void gotNoFileToPlay();
	void gotForbidden();
    void enableActionsOnPlaying();
    void disableActionsOnStop();
    void togglePlayAction(Core::State);
    void hidePanel();
	void resizeMainWindow(int w, int h);
	void resizeWindow(int w, int h);
	virtual void displayGotoTime(int);
	//! You can call this slot to jump to the specified percentage in the video, while dragging the slider.
    void goToPosOnDragging(int);
    void showPopupMenu();
    void showPopupMenu( QPoint p );
    void leftClickFunction();
    void rightClickFunction();
    void doubleClickFunction();
    void middleClickFunction();
    void xbutton1ClickFunction();
    void xbutton2ClickFunction();
    void processFunction(QString function);
    void dragEnterEvent( QDragEnterEvent * ) ;
    void dropEvent ( QDropEvent * );
    void applyNewPreferences();
    void applyFileProperties();
    void clearRecentsList();
    void moveWindowDiff(QPoint diff);
    void loadActions();
    void saveActions();

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

    //! Saves the mplayer log to a file every time a file is loaded
    void autosaveMplayerLog();

    void checkMplayerVersion();
    void displayWarningAboutOldMplayer();

signals:
    void sigActionsEnabled(bool);
    void setPlayOrPauseEnabled(bool);
    void setStopEnabled(bool);
    void frameChanged(int);
    void ABMarkersChanged(int secs_a, int secs_b);
    void timeChanged(QString time_ready_to_print, QString all_time);

	//! Sent when the user wants to close the main window
	void quitSolicited();

	//! Sent when another instance requested to play a file
	void openFileRequested();
    void requestUpdatePlaylistBtnQssProperty(bool);
    void guiChanged();
    void send_save_preview_image_name(int time, QString filepath);

protected:
#if QT_VERSION < 0x050000
    virtual void hideEvent(QHideEvent *);
    virtual void showEvent(QShowEvent *);
#else
	virtual bool event(QEvent * e);
	bool was_minimized;
#endif
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;
//    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
//    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    virtual void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    virtual bool eventFilter(QObject *obj, QEvent *event) Q_DECL_OVERRIDE;
    virtual void resizeEvent(QResizeEvent *e) Q_DECL_OVERRIDE;
    //    void paintEvent(QPaintEvent *);


    virtual void keyPressEvent(QKeyEvent *event);

protected:
    QStackedLayout *contentLayout = nullptr;
    QWidget * panel = nullptr;
    TitleWidget *m_topToolbar = nullptr;
    BottomWidget *m_bottomToolbar = nullptr;
    BottomController *m_bottomController = nullptr;

    // Menu File
    QMenu *openMenu = nullptr;//打开
    MyAction *openFileAct = nullptr;//打开文件
    MyAction *openDirectoryAct = nullptr;//打开文件夹
    MyAction *openURLAct = nullptr;//打开URL
    MyAction *clearRecentsAct = nullptr;//清空最近的文件
    QMenu *recentfiles_menu = nullptr;//打开最近的文件

    QMenu *playMenu = nullptr;//播放控制
    QMenu * control_menu = nullptr;
    MyAction * rewind1Act = nullptr;
    MyAction * rewind2Act = nullptr;
    MyAction * rewind3Act = nullptr;
    MyAction * forward1Act = nullptr;
    MyAction * forward2Act = nullptr;
    MyAction * forward3Act = nullptr;
    QMenu * speed_menu = nullptr;
    MyAction * gotoAct = nullptr;
    // Menu Speed
    MyAction * normalSpeedAct = nullptr;
    MyAction * halveSpeedAct = nullptr;
    MyAction * doubleSpeedAct = nullptr;
    MyAction * decSpeed10Act = nullptr;
    MyAction * incSpeed10Act = nullptr;
    MyAction * decSpeed4Act = nullptr;
    MyAction * incSpeed4Act = nullptr;
    MyAction * decSpeed1Act = nullptr;
    MyAction * incSpeed1Act = nullptr;
    MyAction * playPrevAct = nullptr;
    MyAction * playNextAct = nullptr;

    QMenu * aspect_menu = nullptr;
    // Aspect Action Group
    MyActionGroup * aspectGroup = nullptr;
    MyAction * aspectDetectAct = nullptr;
    MyAction * aspectNoneAct = nullptr;
    MyAction * aspect11Act = nullptr;		// 1:1
    MyAction * aspect32Act = nullptr;		// 3:2
    MyAction * aspect43Act = nullptr;		// 4:3
    MyAction * aspect118Act = nullptr;	// 11:8
    MyAction * aspect54Act = nullptr;		// 5:4
    MyAction * aspect149Act = nullptr;	// 14:9
    MyAction * aspect1410Act = nullptr;	// 14:10
    MyAction * aspect169Act = nullptr;	// 16:9
    MyAction * aspect1610Act = nullptr;	// 16:10
    MyAction * aspect235Act = nullptr;	// 2.35:1

    // Rotate Group
    MyActionGroup * rotateGroup = nullptr;
    MyAction * rotateNoneAct = nullptr;
    MyAction * rotateClockwiseFlipAct = nullptr;
    MyAction * rotateClockwiseAct = nullptr;
    MyAction * rotateCounterclockwiseAct = nullptr;
    MyAction * rotateCounterclockwiseFlipAct = nullptr;

    MyAction * flipAct = nullptr;
    MyAction * mirrorAct = nullptr;

    // Rotate menu
    QMenu * rotate_flip_menu = nullptr;
    QMenu * rotate_menu = nullptr;

//    MyAction * shortcutsAct;
    MyAction * screenshotAct = nullptr;
    /*MyAction * screenshotsAct;
    MyAction * screenshotWithSubsAct;
    MyAction * screenshotWithNoSubsAct;*/

    QMenu * ontop_menu = nullptr;
    // Menu StayOnTop
    MyActionGroup * onTopActionGroup = nullptr;
    MyAction * onTopAlwaysAct = nullptr;
    MyAction * onTopNeverAct = nullptr;
    MyAction * onTopWhilePlayingAct = nullptr;

    //play order
    QMenu * play_order_menu = nullptr;
    MyActionGroup * playOrderActionGroup = nullptr;
    MyAction * orderPlaysAct = nullptr;
    MyAction * randomPlayAct = nullptr;
    MyAction * listLoopPlayAct = nullptr;

    QMenu *audioMenu = nullptr;//声音
    MyAction * muteAct = nullptr;
    MyAction * decVolumeAct = nullptr;
    MyAction * incVolumeAct = nullptr;
    MyAction * decAudioDelayAct = nullptr;
    MyAction * incAudioDelayAct = nullptr;
    MyAction * audioDelayAct = nullptr; // Ask for delay

    // Stereo Mode Action Group
    MyActionGroup * stereoGroup = nullptr;
    MyAction * stereoAct = nullptr;
    MyAction * leftChannelAct = nullptr;
    MyAction * rightChannelAct = nullptr;
    MyAction * monoAct = nullptr;
    MyAction * reverseAct = nullptr;

    QMenu * stereomode_menu = nullptr;

    QMenu *subtitlesMenu = nullptr;//字幕
    MyAction * loadSubsAct = nullptr;
    MyAction * subVisibilityAct = nullptr;

    MyAction *showPreferencesAct = nullptr;//设置
    MyAction *showPropertiesAct = nullptr;//信息和属性
    MyAction *aboutAct = nullptr;
    MyAction *helpAct = nullptr;
    MyAction *quitAct = nullptr;
    MyAction *m_poweroffAct = nullptr;
    //MyAction *openDirAct;

    //20181120
    QMenu * osd_menu = nullptr;
    //MyAction *showFilenameAct;
    MyAction *showMediaInfoAct = nullptr;
    //MyAction *showTimeAct;
    // OSD
    MyAction *incOSDScaleAct = nullptr;
    MyAction *decOSDScaleAct = nullptr;
//#ifdef MPV_SUPPORT
    MyAction *OSDFractionsAct = nullptr;
//#endif
    // OSD Action Group
    MyActionGroup * osdGroup = nullptr;
    MyAction * osdNoneAct = nullptr;
    MyAction * osdSeekAct = nullptr;
    MyAction * osdTimerAct = nullptr;
    MyAction * osdTotalAct = nullptr;

    QMenu *popup = nullptr;
    QMenu *main_popup = nullptr;
    QMenu *audiochannels_menu = nullptr;
    MyActionGroup *channelsGroup = nullptr;
    MyAction *channelsStereoAct = nullptr;
    MyAction *channelsSurroundAct = nullptr;
    MyAction *channelsFull51Act = nullptr;
    MyAction *channelsFull61Act = nullptr;
    MyAction *channelsFull71Act = nullptr;

    //tray menu and actions
    QMenu *tray_menu = nullptr;
    MyAction *action_show = nullptr;
    MyAction *action_openshotsdir = nullptr;


    //Hide actions
    MyAction *playlist_action = nullptr;
    MyAction *play_pause_aciton = nullptr;
    MyAction *stopAct = nullptr;
    MyAction *fullscreenAct = nullptr;

    PreferencesDialog *pref_dialog = nullptr;
    FilePropertiesDialog *file_dialog = nullptr;
    AboutDialog *aboutDlg = nullptr;
    HelpDialog *helpDlg = nullptr;
    Core *core = nullptr;
    VideoWindow *mplayerwindow = nullptr;
    Playlist *m_playlistWidget = nullptr;
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
    QPoint mainwindow_pos;
    QPoint playlist_pos;
    bool trayicon_playlist_was_visible;
    QPushButton *resizeCorner = nullptr;
    bool resizeFlag;
    QSystemTrayIcon *tray = nullptr;

    PlayMask *play_mask = nullptr;
    EscTip *escWidget = nullptr;
    TipWidget *tipWidget = nullptr;
    QTimer *tip_timer = nullptr;
    QString m_arch;
    VideoPreview *video_preview = nullptr;
//    ShortcutsWidget *shortcuts_widget;

    QString m_snap;
    FilterHandler *m_mouseFilterHandler = nullptr;

//    CoverWidget *m_coverWidget = nullptr;
    MaskWidget *m_maskWidget = nullptr;

    bool  m_leftPressed;// 鼠标是否按下
    QPixmap currentBackground;

    ControllerWorker *m_controller = nullptr;
};
    
#endif // _MAINWINDOW_H_

