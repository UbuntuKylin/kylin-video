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
class AudioEqualizer;

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
    void createAudioEqualizer();
    void createActionsAndMenus();
    void createOpenActionsAndMenus();
    void createRecentsActionsAndMenus();
    void createTopActionsAndMenus();
    void createPlayCommandActionsAndMenus();
    void createPlayOrderActionsAndMenus();
    void createVideoAspectActionsAndMenus();
    void createVideoRotateActionsAndMenus();
    void createAudioActionsAndMenus();
    void createScreenshotActionsAndMenus();
    void createSubtitleActionsAndMenus();
    void createOsdActionsAndMenus();
    void createOthersActionsAndMenus();
    void createTrayActions();
    void createTipWidget();
    void createEscWidget();
    void createMaskWidget();
    void createPreferencesDialog();
    void createFilePropertiesDialog();
    void setDataToFileProperties();
    void createAboutDialog();
    void createHelpDialog();
    void initRemoteControllerConnections();
    void setStayOnTop(bool b);
    void setActionsEnabled(bool);
    void updateRecents();
    void updateMuteWidgets();
    void updateOnTopWidgets();
    void updatePlayOrderWidgets();
    void setPlaylistVisible(bool visible);
    void slideEdgeWidget(QWidget *right, QRect start, QRect end, int delay, bool hide = false);
    void bindThreadWorker(InfoWorker *worker);
    Core * getCore() { return m_core; };

public slots:
    void changeStayOnTop(int);
    void checkStayOnTop(Core::State);
    void changePlayOrder(int play_order);
    void powerOffPC();
    void onMediaStoppedByUser();
    void onMute();
    void onMinWindow();
    void onCloseWindow();
    void onShowMenu();
    void onMaxWindow(bool b);
    void onFullScreen();
    void onShowOrHidePlaylist();
    void exitFullscreen();
    void exitFullscreenOnStop();
    void exitFullscreenIfNeeded();
    void toggleFullscreen(bool);
    void onPlayPause();

    void showAboutDialog();
    void showHelpDialog();
    void showPreferencesDialog();
    void showFilePropertiesDialog();
    void showGotoDialog();
    void showSubDelayDialog();
    void showAudioDelayDialog();
    void showTipWidget(const QString text);
    void onShowOrHideEscWidget(bool b);
    void toggleShowOrHideMainWindow();
    void showMainWindow();
    void showAudioEqualizer(bool b);

    void updateWidgets();
    void updateAudioEqualizer();

    void doOpen(QString file);
    void openFile();
    void openFile(QString file);
    void openFiles(QStringList files);
    void openDirectory();
    void openDirectory(QString directory);
    void openURL();
    void loadSub();
	void setInitialSubtitle(const QString & subtitle_file);
    void onSavePreviewImage(int time);
//    void showShortcuts();
    void onMeidaFilesAdded(const VideoPtrList medialist);
    void setBackgroudPixmap(QString pixmapDir);
    void trayIconActivated(QSystemTrayIcon::ActivationReason);
    void setJumpTexts();
    void openRecent();
    void playlistHasFinished();
    void displayState(Core::State state);
    void displayMessage(QString message);
    void gotCurrentTime(double, bool);

    void newMediaLoaded();
    void updateMediaInfo();
    void displayVideoInfo(int width, int height, double fps);
    void displayBitrateInfo(int vbitrate, int arbitrate);
	void gotNoFileToPlay();
	void gotForbidden();
    void enableActionsOnPlaying();
    void disableActionsOnStop();
    void togglePlayAction(Core::State);
    void hideCentralWidget();
    void resizeMainWindow(int w, int h);
    void displayGotoTime(int);
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
    void applyNewPreferences();
    void applyFileProperties();
    void clearRecentsList();
    void moveWindowDiff(QPoint diff);
    void loadActions();
    void saveActions();
    void moveWindow();

	// Single instance stuff
#ifdef SINGLE_INSTANCE
	void handleMessageFromOtherInstances(const QString& message);
#endif

	void showExitCodeFromMplayer(int exit_code);
	void showErrorFromMplayer(QProcess::ProcessError);
    void showErrorFromPlayList(QString errorStr);
    void clearMplayerLog();
    void recordMplayerLog(QString line);

signals:
    void requestActionsEnabled(bool);
    void requestPlayOrPauseEnabled(bool);
    void requestUpdatePlaylistBtnQssProperty(bool);
    void requestGuiChanged();

protected:
#if QT_VERSION < 0x050000
    void hideEvent(QHideEvent *event) Q_DECL_OVERRIDE;
    void showEvent(QShowEvent *event) Q_DECL_OVERRIDE;
#else
    bool event(QEvent * event) Q_DECL_OVERRIDE;
	bool was_minimized;
#endif
    void dragEnterEvent(QDragEnterEvent *event) Q_DECL_OVERRIDE;
    void dropEvent(QDropEvent *event) Q_DECL_OVERRIDE;
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    bool eventFilter(QObject *obj, QEvent *event) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
    //    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;

private:
    bool m_isMaximized;
    QString m_mplayerLogMsg;
    bool m_ignoreShowHideEvents;
    QPoint m_windowPos;
    QString m_arch;
    QString m_snap;
    bool  m_leftPressed;
    QPixmap currentBackground;
    bool m_dragWindow;
    int m_lastPlayingSeek;
    bool m_resizeFlag;
    QPushButton *m_resizeCornerBtn = nullptr;
    QSystemTrayIcon *m_mainTray = nullptr;
    PlayMask *m_playMaskWidget = nullptr;
    EscTip *m_escWidget = nullptr;
    TipWidget *m_tipWidget = nullptr;
    QTimer *m_tipTimer = nullptr;
    VideoPreview *m_videoPreview = nullptr;
//    ShortcutsWidget *m_shortcutsWidget;
    FilterHandler *m_mouseFilterHandler = nullptr;
//    CoverWidget *m_coverWidget = nullptr;
    MaskWidget *m_maskWidget = nullptr;
    ControllerWorker *m_controllerWorker = nullptr;

    QWidget *m_centralWidget = nullptr;
    QStackedLayout/*QVBoxLayout*/ *m_centralLayout = nullptr;
    TitleWidget *m_topToolbar = nullptr;
    BottomWidget *m_bottomToolbar = nullptr;
    BottomController *m_bottomController = nullptr;
    PreferencesDialog *m_prefDialog = nullptr;
    FilePropertiesDialog *m_propertyDialog = nullptr;
    AboutDialog *m_aboutDialog = nullptr;
    HelpDialog *m_helpDialog = nullptr;
    Core *m_core = nullptr;
    VideoWindow *m_mplayerWindow = nullptr;
    Playlist *m_playlistWidget = nullptr;

    AudioEqualizer *audio_equalizer = nullptr;

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
    MyAction * loadAudioAct = nullptr;
    MyAction * unloadAudioAct = nullptr;
    QMenu * audiofilter_menu = nullptr;
    MyAction * extrastereoAct = nullptr;
    MyAction * karaokeAct = nullptr;
    MyAction * volnormAct = nullptr;
    MyAction * earwaxAct = nullptr;

    MyAction * audioEqualizerAct = nullptr;
    MyAction * resetAudioEqualizerAct = nullptr;

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
//    MyAction *m_poweroffAct = nullptr;
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

    QMenu *m_mainMenu = nullptr;
    QMenu *m_toolbarMenu = nullptr;
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

    // The actions which are invisible shortcuts
    MyAction *playlist_action = nullptr;
    MyAction *play_pause_aciton = nullptr;
    MyAction *stopAct = nullptr;
    MyAction *fullscreenAct = nullptr;
};
    
#endif // _MAINWINDOW_H_

