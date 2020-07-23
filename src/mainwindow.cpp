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

#include "mainwindow.h"

#include <QMessageBox>
#include <QMouseEvent>
#include <QLabel>
#include <QMenu>
#include <QFileInfo>
#include <QApplication>
#include <QMenuBar>
#include <QHBoxLayout>
#include <QCursor>
#include <QTimer>
#include <QStyle>
#include <QRegExp>
#include <QStatusBar>
#include <QActionGroup>
#include <QUrl>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QDesktopServices>
#include <QInputDialog>
#include <QClipboard>
#include <QMimeData>
#include <QDesktopWidget>
#include <cmath>
#include <QtCore/qmath.h>
#include <QGraphicsOpacityEffect>
#include <QVariant>
#include <QDataStream>
#include <QtX11Extras/QX11Info>
#include <QtDBus>
#include <QDBusConnection>
#include <QEvent>
#include <QKeyEvent>
#include <QBitmap>

#include "playlist.h"
#include "titlewidget.h"
#include "bottomwidget.h"
#include "playmask.h"
#include "aboutdialog.h"
#include "esctip.h"
#include "tipwidget.h"
#include "messagedialog.h"
//#include "shortcutswidget.h"
#include "helpdialog.h"
#include "bottomcontroller.h"
#include "filterhandler.h"
#include "coverwidget.h"
#include "datautils.h"
#include "infoworker.h"
#include "maskwidget.h"
#include "videowindow.h"
#include "poweroffdialog.h"
//#include "controllerworker.h"

#include "smplayer/desktopinfo.h"
#include "smplayer/paths.h"
#include "smplayer/colorutils.h"
#include "smplayer/global.h"
#include "smplayer/translator.h"
#include "smplayer/images.h"
#include "smplayer/preferences.h"
#include "smplayer/filepropertiesdialog.h"
#include "smplayer/recents.h"
#include "smplayer/urlhistory.h"
#include "smplayer/errordialog.h"
#include "smplayer/timedialog.h"
#include "smplayer/audiodelaydialog.h"
#include "smplayer/filedialog.h"
#include "smplayer/mplayerversion.h"
#include "smplayer/actionseditor.h"
#include "smplayer/preferencesdialog.h"
#include "smplayer/prefshortcut.h"
#include "smplayer/myaction.h"
#include "smplayer/myactiongroup.h"
#include "smplayer/extensions.h"
#include "smplayer/version.h"
#include "smplayer/inputurl.h"
#include "smplayer/audioequalizer.h"
#include "smplayer/eqslider.h"

#ifdef PREVIEW_TEST
#include "previewdialog.h"
#else
#include "smplayer/videopreview.h"
#endif

//注意: x11的头文件需要放置在QJson/QDbus的后面
//#include <X11/Xlib.h>

using namespace Global;

QDataStream &operator<<(QDataStream &dataStream, const VideoPtr &objectA)
{
    auto ptr = objectA.data();
    auto ptrval = reinterpret_cast<qulonglong>(ptr);
    auto var = QVariant::fromValue(ptrval);
    dataStream << var;
    return  dataStream;
}

QDataStream &operator>>(QDataStream &dataStream, VideoPtr &objectA)
{
    QVariant var;
    dataStream >> var;
    qulonglong ptrval = var.toULongLong();
    auto ptr = reinterpret_cast<VideoData *>(ptrval);
    objectA = VideoPtr(ptr);
    return dataStream;
}

MainWindow::MainWindow(QString arch_type, QString snap, /*ControllerWorker *controller, */QWidget *parent, Qt::WindowFlags flags)
    : QMainWindow(parent, flags)
#if QT_VERSION >= 0x050000
	, was_minimized(false)
#endif
    , m_bottomController(new BottomController(this))
    , m_mouseFilterHandler(new FilterHandler(*this, *qApp))
    , m_leftPressed(false)
    , m_resizeFlag(false)
    , m_ignoreShowHideEvents(false)
    , m_arch(arch_type)
    , m_snap(snap)
    , m_maskWidget(new MaskWidget(this))
//    , m_controllerWorker(controller)
    , m_dragWindow(false)
    , m_lastPlayingSeek(0)
    , m_oldIsMaxmized(false)
{

    //qDebug() << "qApp->devicePixelRatio():" << qApp->devicePixelRatio();



    this->setWindowFlags(Qt::FramelessWindowHint/* | Qt::WindowStaysOnTopHint*/);//设置窗体标题栏隐藏并设置位于顶层
    this->setMouseTracking(true);//可获取鼠标跟踪效果，界面拉伸需要这个属性
    this->setAutoFillBackground(true);
    this->setAttribute(Qt::WA_TranslucentBackground);//设置窗口背景透明
    this->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    this->setMinimumSize(WINDOW_MIN_WIDTH, WINDOW_MIN_HEIGHT);
    this->resize(900, 600);
    this->setWindowTitle(tr("Kylin Video"));
    this->setWindowIcon(QIcon::fromTheme("kylin-video", QIcon(":/res/kylin-video.png")));
//    this->setWindowIcon(QIcon::fromTheme("kylin-video", QIcon(":/res/kylin-video.png")).pixmap(QSize(64, 64)).scaled(64, 64, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    this->setAcceptDrops(true);

//    this->setFocusPolicy(Qt::ClickFocus);
    this->setFocusPolicy(Qt::StrongFocus);//让空格等按钮事件可以在keyPressEvent函数中获取
    installEventFilter(this);//add event filter

    m_windowPos = pos();

    this->initRegisterMeta();

    //遮罩
//    m_shortcutsWidget = ShortcutsWidget::Instance();
//    m_shortcutsWidget->set_parent_widget(this);

	createPanel();
    createVideoWindow();
	createCore();
    createPlaylist();
    createTopTitleBar();
    createBottomToolBar();
    createAudioEqualizer();
    createActionsAndMenus();
    createTrayActions();
    createTipWidget();
    createEscWidget();
    createMaskWidget();
    initRemoteControllerConnections();

//    m_coverWidget = new CoverWidget(this);
//    m_coverWidget->setContentsMargins(0, 0, 0, 0);

    m_centralLayout = new /*QVBoxLayout*/QStackedLayout(m_centralWidget);
    m_centralLayout->setContentsMargins(20, 20, 20, 20);
    m_centralLayout->setMargin(1);
    m_centralLayout->setSpacing(0);
    m_centralLayout->addWidget(m_topToolbar);
    m_centralLayout->addWidget(m_mplayerWindow);
//    m_centralLayout->addWidget(m_coverWidget);
    m_centralLayout->addWidget(m_bottomToolbar);

    m_topToolbar->show();
    m_mplayerWindow->show();
    m_bottomToolbar->show();
    m_bottomToolbar->setFocus();

    this->setActionsEnabled(false);
    if (m_playlistWidget->count() > 0) {
        emit this->requestPlayOrPauseEnabled(true);
        m_bottomToolbar->updateLabelCountNumber(m_playlistWidget->count());
    }
    else {
        m_bottomToolbar->updateLabelCountNumber(0);
    }

    QTimer::singleShot(20, this, SLOT(loadActions()));

    this->move((QApplication::desktop()->screenGeometry(0).width() - this->width()) / 2, (QApplication::desktop()->screenGeometry(0).height() - this->height()) / 2);

    this->changeStayOnTop(pref->stay_on_top);
    this->changePlayOrder(pref->play_order);

//    QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect(this);
//    effect->setBlurRadius(50);
//    effect->setColor(Qt::red);
//    effect->setOffset(0);
//    this->setGraphicsEffect(effect);


#ifdef PREVIEW_TEST
    m_previewDlg = new PreviewDialog(this);
    m_previewDlg->hide();
#endif

    connect(m_mouseFilterHandler, &FilterHandler::mouseMoved, [=] () {
        if (this->isFullScreen()) {
            if (!m_topToolbar->isVisible() || !m_bottomToolbar->isVisible()) {
                m_bottomController->temporaryShow();
            }
        }
    });
}

MainWindow::~MainWindow()
{
    this->clearMplayerLog();

    if (m_tipTimer) {
        disconnect(m_tipTimer, SIGNAL(timeout()), m_tipWidget, SLOT(hide()));
        if(m_tipTimer->isActive()) {
            m_tipTimer->stop();
        }
        delete m_tipTimer;
        m_tipTimer = nullptr;
    }

    if (m_core) {
        delete m_core; // delete before m_mplayerWindow, otherwise, segfault...
        m_core = nullptr;
    }
    if (m_playMaskWidget) {
        delete m_playMaskWidget;
        m_playMaskWidget = nullptr;
    }
    if (m_escWidget) {
        delete m_escWidget;
        m_escWidget = nullptr;
    }
    if (m_tipWidget) {
        delete m_tipWidget;
        m_tipWidget = nullptr;
    }
    if (m_mplayerWindow) {
        delete m_mplayerWindow;
        m_mplayerWindow = nullptr;
    }
    if (m_playlistWidget) {
        delete m_playlistWidget;
        m_playlistWidget = nullptr;
    }
#ifdef PREVIEW_TEST
    if (m_previewDlg) {
        delete m_previewDlg;
        m_previewDlg = nullptr;
    }
    if (m_previewMgr) {
        m_previewMgr->stop();
        delete m_previewMgr;
    }
#else
    if (m_videoPreview) {
        delete m_videoPreview;
        m_videoPreview = nullptr;
    }
#endif
//    if (m_shortcutsWidget) {
//        delete m_shortcutsWidget;
//        m_shortcutsWidget = nullptr;
//    }
    if (m_prefDialog) {
        delete m_prefDialog;
        m_prefDialog = nullptr;
    }
    if (m_propertyDialog) {
        delete m_propertyDialog;
        m_propertyDialog = nullptr;
    }
//    if (m_aboutDialog) {
//        delete m_aboutDialog;
//        m_aboutDialog = nullptr;
//    }
    if (m_helpDialog) {
        delete m_helpDialog;
        m_helpDialog = nullptr;
    }
    if (m_mainTray) {
        delete m_mainTray;
        m_mainTray = nullptr;
    }
    if (m_mainMenu) {
        delete m_mainMenu;
        m_mainMenu = nullptr;
    }
    if (m_toolbarMenu) {
        delete m_toolbarMenu;
        m_toolbarMenu = nullptr;
    }
    if (m_resizeCornerBtn) {
        delete m_resizeCornerBtn;
        m_resizeCornerBtn = nullptr;
    }
    if (m_topToolbar) {
        delete m_topToolbar;
        m_topToolbar = nullptr;
    }
    if (m_bottomToolbar) {
        delete m_bottomToolbar;
        m_bottomToolbar = nullptr;
    }
    if (m_centralLayout) {
        delete m_centralLayout;
        m_centralLayout = nullptr;
    }
    if (m_centralWidget) {
        delete m_centralWidget;
        m_centralWidget = nullptr;
    }

}

void MainWindow::initRegisterMeta()
{
    qRegisterMetaType<VideoPtr>();
    qRegisterMetaTypeStreamOperators<VideoPtr>();
    qRegisterMetaType<VideoPtrList>();
    qRegisterMetaType<QList<VideoData>>();
}

void MainWindow::createPanel()
{
    m_centralWidget = new QWidget(this);
    m_centralWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_centralWidget->setMinimumSize(QSize(1,1));
    m_centralWidget->setFocusPolicy(Qt::StrongFocus);
    this->setCentralWidget(m_centralWidget);

//    //给主界面设置6px的圆角
////    this->setStyleSheet("QMainWindow{background-color: transparent;}");
//    this->setStyleSheet("QWidget{border:none; border-radius:6px;}");
    this->centralWidget()->setStyleSheet("QWidget{border:none; border-radius:6px; background: #000000;}");//background:rgba(0,0,0,0.9);
//    //border:1px solid rgba(255,255,255,0.05);


    m_centralWidget->setFocus();
}

void MainWindow::createVideoWindow()
{
    m_mplayerWindow = new VideoWindow(m_centralWidget);
    m_mplayerWindow->setColorKey("121212");//视频显示区域背景色设置为黑色
    m_mplayerWindow->setContentsMargins(0, 0, 0, 0);
    m_mplayerWindow->allowVideoMovement(pref->allow_video_movement);
    m_mplayerWindow->delayLeftClick(pref->delay_left_click);
    m_mplayerWindow->setAnimatedLogo(pref->animated_logo);

//    QVBoxLayout * layout = new QVBoxLayout;
//    layout->setSpacing(0);
//    layout->setMargin(0);
//    layout->addWidget(m_mplayerWindow);
//    m_centralWidget->setLayout(layout);

    connect(m_mplayerWindow, SIGNAL(doubleClicked()), this, SLOT(doubleClickFunction()));
    connect(m_mplayerWindow, SIGNAL(leftClicked()), this, SLOT(leftClickFunction()));
    connect(m_mplayerWindow, SIGNAL(rightClicked()), this, SLOT(rightClickFunction()));
    connect(m_mplayerWindow, SIGNAL(middleClicked()), this, SLOT(middleClickFunction()));
    connect(m_mplayerWindow, SIGNAL(xbutton1Clicked()), this, SLOT(xbutton1ClickFunction()));
    connect(m_mplayerWindow, SIGNAL(xbutton2Clicked()), this, SLOT(xbutton2ClickFunction()));
    connect(m_mplayerWindow, SIGNAL(mouseMovedDiff(QPoint)), this, SLOT(moveWindowDiff(QPoint)), Qt::QueuedConnection);
    m_mplayerWindow->activateMouseDragTracking(true);
}

void MainWindow::createCore()
{
    m_core = new Core(m_mplayerWindow, this->m_snap, this);
    connect(m_core, SIGNAL(widgetsNeedUpdate()), this, SLOT(updateWidgets()));
    connect(m_core, SIGNAL(audioEqualizerNeedsUpdate()), this, SLOT(updateAudioEqualizer()));
    connect(m_core, &Core::showFrame, this, [=] (int frame) {
        //qDebug() << "Frame changed: " << frame;
    });
    connect(m_core, &Core::ABMarkersChanged, this, [=] (int secs_a, int secs_b) {
        QString s;
        if (secs_a > -1) s = tr("A:%1").arg(Utils::formatTime(secs_a));

        if (secs_b > -1) {
            if (!s.isEmpty()) s += " ";
            s += tr("B:%1").arg(Utils::formatTime(secs_b));
        }
        qDebug() << "ABMarkersChanged: " << s;
    });
    connect(m_core, SIGNAL(showTime(double, bool)), this, SLOT(gotCurrentTime(double, bool)));
    connect(m_core, SIGNAL(needResize(int, int)), this, SLOT(resizeMainWindow(int,int)));
    connect(m_core, SIGNAL(showMessage(QString)), this, SLOT(displayMessage(QString)));
    connect(m_core, SIGNAL(stateChanged(Core::State)), this, SLOT(displayState(Core::State)));
    connect(m_core, SIGNAL(stateChanged(Core::State)), this, SLOT(checkStayOnTop(Core::State)), Qt::QueuedConnection);
    connect(m_core, &Core::mediaStartPlay, this, [=] () {
        if ((pref->mplayer_detected_version > 0) && (!MplayerVersion::isMplayerAtLeast(25158))) {
            QTimer::singleShot(1000, this, [=] {
                if (!pref->reported_mplayer_is_old) {
                    QMessageBox::warning(this, tr("Warning - Using old MPlayer"),
                        tr("The version of MPlayer (%1) installed on your system "
                           "is obsolete. kylin-video can't work well with it: some "
                           "options won't work, subtitle selection may fail...")
                           .arg(MplayerVersion::toString(pref->mplayer_detected_version)) +
                        "<br><br>" +
                        tr("Please, update your MPlayer.") +
                        "<br><br>" +
                        tr("(This warning won't be displayed anymore)") );

                    pref->reported_mplayer_is_old = true;
                }
            });
        }
    }, Qt::QueuedConnection);
    connect(m_core, SIGNAL(mediaStoppedByUser()), this, SLOT(onMediaStoppedByUser()));
    connect(m_core, SIGNAL(requestShowLogo(bool)), m_mplayerWindow, SLOT(setLogoVisible(bool)));
    connect(m_core, SIGNAL(mediaLoaded()), this, SLOT(enableActionsOnPlaying()));
    connect(m_core, SIGNAL(noFileToPlay()), this, SLOT(gotNoFileToPlay()));
    connect(m_core, SIGNAL(audioTracksInitialized()), this, SLOT(enableActionsOnPlaying()));
    connect(m_core, &Core::mediaFinished, this, [=] () {
        m_lastPlayingSeek = 0;
        this->disableActionsOnStop();
    });
    connect(m_core, SIGNAL(stateChanged(Core::State)), this, SLOT(togglePlayAction(Core::State)));
    connect(m_core, SIGNAL(mediaStartPlay()), this, SLOT(newMediaLoaded()), Qt::QueuedConnection);
    connect(m_core, SIGNAL(mediaInfoChanged()), this, SLOT(updateMediaInfo()));
    connect(m_core, &Core::failedToParseMplayerVersion, this, [=] (QString line) {
        qDebug() << "failedToParseMplayerVersion: " << line;
    });
    connect(m_core, SIGNAL(mplayerFailed(QProcess::ProcessError)), this, SLOT(showErrorFromMplayer(QProcess::ProcessError)));
    connect(m_core, SIGNAL(mplayerFinishedWithError(int)), this, SLOT(showExitCodeFromMplayer(int)));
    connect(m_core, SIGNAL(noVideo()), m_mplayerWindow, SLOT(showLogo()));//hideCentralWidget():
    connect(m_core, SIGNAL(aboutToStartPlaying()), this, SLOT(clearMplayerLog()));
    connect(m_core, SIGNAL(logLineAvailable(QString)), this, SLOT(recordMplayerLog(QString)));
    connect(m_core, SIGNAL(receivedForbidden()), this, SLOT(gotForbidden()));
    connect(m_core, SIGNAL(bitrateChanged(int,int)), this, SLOT(displayBitrateInfo(int,int)));
    connect(m_mplayerWindow, SIGNAL(wheelUp()), m_core, SLOT(wheelUp()));
    connect(m_mplayerWindow, SIGNAL(wheelDown()), m_core, SLOT(wheelDown()));
}

void MainWindow::createPlaylist()
{
    m_playlistWidget = new Playlist(m_core, this, 0);
    m_playlistWidget->setFixedSize(220, this->height() - TOP_TOOLBAR_HEIGHT - BOTTOM_TOOLBAR_HEIGHT);
    m_playlistWidget->setViewHeight();
    m_playlistWidget->move(this->width()-220, TOP_TOOLBAR_HEIGHT);
    m_playlistWidget->hide();
    connect(m_playlistWidget, SIGNAL(cleanPlaylistFinished()), this, SLOT(onCleanPlaylistFinished()));
    connect(m_playlistWidget, SIGNAL(playlistEnded()), this, SLOT(playlistHasFinished()));
    connect(m_playlistWidget, SIGNAL(playlistEnded()), m_mplayerWindow, SLOT(showLogo()));
    connect(m_playlistWidget, SIGNAL(closePlaylist()), this, SLOT(onShowOrHidePlaylist()));
    connect(m_playlistWidget, SIGNAL(playListFinishedWithError(QString)), this, SLOT(showErrorFromPlayList(QString)));
    connect(m_playlistWidget, SIGNAL(showMessage(QString)), this, SLOT(displayMessage(QString)));
#ifdef PREVIEW_TEST
    connect(m_playlistWidget, &Playlist::requestDestoryPreview, [=] () {
        if (m_previewMgr) {
            m_previewMgr->stop();
            delete m_previewMgr;
        }
    });
#endif
}

void MainWindow::createTopTitleBar()
{
    m_topToolbar = new TitleWidget(this);
    m_topToolbar->setFixedHeight(TOP_TOOLBAR_HEIGHT);
    this->setMenuWidget(m_topToolbar);
    m_topToolbar->setMouseTracking(true);
    //m_topToolbar->setAttribute(Qt::WA_X11DoNotAcceptFocus, true);

    connect(m_topToolbar, SIGNAL(requestShowMenu()), this, SLOT(onShowMenu()));
    connect(m_topToolbar, SIGNAL(requestMinWindow()), this, SLOT(onMinWindow()));
    connect(m_topToolbar, SIGNAL(requestCloseWindow()), this, SLOT(onCloseWindow()));
    connect(m_topToolbar, SIGNAL(requestMaxWindow(bool)), this, SLOT(onMaxWindow(bool)));
    connect(m_topToolbar, SIGNAL(mouseMovedDiff(QPoint)), this, SLOT(moveWindowDiff(QPoint)), Qt::QueuedConnection );
    connect(m_playlistWidget, SIGNAL(requestSetPlayingTitle(QString)), m_topToolbar, SLOT(onSetPlayingTitleName(QString)));
}

void MainWindow::createBottomToolBar()
{
    m_bottomToolbar = new BottomWidget(this);
    m_bottomToolbar->setFixedHeight(BOTTOM_TOOLBAR_HEIGHT);
    m_bottomToolbar->setLayoutDirection(Qt::LeftToRight);
    connect(m_bottomToolbar, SIGNAL(requestVolumeChanged(int)), m_core, SLOT(setVolume(int)));
    connect(m_core, SIGNAL(volumeChanged(int)), m_bottomToolbar, SIGNAL(valueChanged(int)));
    connect(m_bottomToolbar, SIGNAL(toggleFullScreen()), this, SLOT(onFullScreen()));
    connect(m_bottomToolbar, SIGNAL(togglePlaylist()), this, SLOT(onShowOrHidePlaylist()));

    connect(m_bottomToolbar, &BottomWidget::toggleStop, this, [=] {
        m_lastPlayingSeek = (int) m_core->mset.current_sec;
        m_core->stop();
        //disconnect(m_mouseFilterHandler, SIGNAL(mouseMoved()), m_bottomController, SLOT(temporaryShow()));
        m_bottomController->permanentShow();
    });
    connect(m_bottomToolbar, SIGNAL(togglePrev()), m_playlistWidget, SLOT(playPrev()));

    connect(m_bottomToolbar, &BottomWidget::togglePlayPause, this, &MainWindow::onPlayPause);
    connect(m_bottomToolbar, &BottomWidget::requestTemporaryShow, this, [=] {
        m_bottomController->temporaryShow();
    });

    connect(m_bottomToolbar, SIGNAL(toggleNext()), m_playlistWidget, SLOT(playNext()));
    connect(m_bottomToolbar, SIGNAL(toggleMute()), this, SLOT(onMute()));
    connect(this, SIGNAL(requestActionsEnabled(bool)), m_bottomToolbar, SLOT(onSetActionsEnabled(bool)));
    connect(this, SIGNAL(requestPlayOrPauseEnabled(bool)), m_bottomToolbar, SLOT(setPlayOrPauseEnabled(bool)));
    connect(this, SIGNAL(requestUpdatePlaylistBtnQssProperty(bool)), m_bottomToolbar, SLOT(updatePlaylistBtnQssProperty(bool)));
    connect(m_bottomToolbar, SIGNAL(posChanged(int)), m_core, SLOT(goToPosition(int)));
    connect(m_bottomToolbar, SIGNAL(delayedDraggingPos(int)), this, SLOT(goToPosOnDragging(int)));
    connect(m_core, SIGNAL(positionChanged(int)), m_bottomToolbar, SLOT(setPos(int)));
    connect(m_bottomToolbar, SIGNAL(draggingPos(int)), this, SLOT(displayGotoTime(int)));
    connect(m_bottomToolbar, SIGNAL(wheelUp()), m_core, SLOT(wheelUp()));
    connect(m_bottomToolbar, SIGNAL(wheelDown()), m_core, SLOT(wheelDown()));
    connect(m_bottomToolbar, SIGNAL(mouseMovedDiff(QPoint)), this, SLOT(moveWindowDiff(QPoint)), Qt::QueuedConnection );
    connect(m_core, SIGNAL(newDuration(double)), m_bottomToolbar, SLOT(setDuration(double)));

    connect(m_playlistWidget, &Playlist::update_playlist_count, this, [=] (int count) {
            m_bottomToolbar->updateLabelCountNumber(count);
            if (count == 0) {
                this->setActionsEnabled(false);
                emit this->requestPlayOrPauseEnabled(false);
            }
            else {
                this->enableActionsOnPlaying();
            }
        });
    //connect(m_playlistWidget, SIGNAL(update_playlist_count(int)), m_bottomToolbar, SLOT(updateLabelCountNumber(int)));
    connect(m_bottomToolbar, SIGNAL(requestSavePreviewImage(int, QPoint)), this, SLOT(onSavePreviewImage(int, QPoint)));
    connect(m_bottomToolbar, &BottomWidget::requestHideTip, [=] () {
#ifdef PREVIEW_TEST
        if (m_previewDlg) {
            m_previewDlg->resize(1,1);
            m_previewDlg->close();
        }
#endif
    });

    m_resizeCornerBtn = new QPushButton(m_bottomToolbar);
    m_resizeCornerBtn->setFocusPolicy(Qt::NoFocus);
    m_resizeCornerBtn->setStyleSheet("QPushButton{background-image:url(':/res/dragbar_normal.png');border:0px;}QPushButton:hover{background-image:url(':/res/dragbar_normal.png')}QPushButton:pressed{background-image:url(':/res/dragbar_normal.png')}");
    m_resizeCornerBtn->setFixedSize(15, 15);
    m_resizeCornerBtn->setCursor(Qt::SizeFDiagCursor);
    m_resizeCornerBtn->move(m_bottomToolbar->width()-15, m_bottomToolbar->height()-15);
    m_resizeCornerBtn->installEventFilter(this);
    m_resizeCornerBtn->raise();

    connect(m_bottomController, &BottomController::requestShow, this, [=] {
        m_bottomToolbar->show();
        m_topToolbar->show();

//        if (this->isFullScreen()) {
//            this->onShowOrHideEscWidget(true);
//        }
    });

    connect(m_bottomController, &BottomController::requestHide, this, [=] {
        if (this->isFullScreen()) {
            m_bottomToolbar->hide();
            m_topToolbar->hide();
            this->onShowOrHideEscWidget(false);
        }
    });
}

void MainWindow::createAudioEqualizer()
{
    // Audio Equalizer
    audio_equalizer = new AudioEqualizer(this);
    audio_equalizer->setVisible(false);
    connect(audio_equalizer->eq[0], SIGNAL(valueChanged(int)), m_core, SLOT(setAudioEq0(int)));
    connect(audio_equalizer->eq[1], SIGNAL(valueChanged(int)), m_core, SLOT(setAudioEq1(int)));
    connect(audio_equalizer->eq[2], SIGNAL(valueChanged(int)), m_core, SLOT(setAudioEq2(int)));
    connect(audio_equalizer->eq[3], SIGNAL(valueChanged(int)), m_core, SLOT(setAudioEq3(int)));
    connect(audio_equalizer->eq[4], SIGNAL(valueChanged(int)), m_core, SLOT(setAudioEq4(int)));
    connect(audio_equalizer->eq[5], SIGNAL(valueChanged(int)), m_core, SLOT(setAudioEq5(int)));
    connect(audio_equalizer->eq[6], SIGNAL(valueChanged(int)), m_core, SLOT(setAudioEq6(int)));
    connect(audio_equalizer->eq[7], SIGNAL(valueChanged(int)), m_core, SLOT(setAudioEq7(int)));
    connect(audio_equalizer->eq[8], SIGNAL(valueChanged(int)), m_core, SLOT(setAudioEq8(int)));
    connect(audio_equalizer->eq[9], SIGNAL(valueChanged(int)), m_core, SLOT(setAudioEq9(int)));
    connect(audio_equalizer, SIGNAL(applyClicked(AudioEqualizerList)), m_core, SLOT(setAudioAudioEqualizerRestart(AudioEqualizerList)));
    connect(audio_equalizer, SIGNAL(valuesChanged(AudioEqualizerList)), m_core, SLOT(setAudioEqualizer(AudioEqualizerList)));
    connect(audio_equalizer, SIGNAL(visibilityChanged()), this, SLOT(updateWidgets()));
}


void MainWindow::createActionsAndMenus()
{
    if (!m_mainMenu)
        m_mainMenu = new QMenu(this);
    else
        m_mainMenu->clear();

    this->createOpenActionsAndMenus();
    this->createRecentsActionsAndMenus();
    this->createTopActionsAndMenus();
    this->createPlayCommandActionsAndMenus();
    this->createPlayOrderActionsAndMenus();
    this->createVideoAspectActionsAndMenus();
    this->createVideoRotateActionsAndMenus();
    this->createAudioActionsAndMenus();
    this->createScreenshotActionsAndMenus();
    this->createSubtitleActionsAndMenus();
    this->createOsdActionsAndMenus();
    this->createOthersActionsAndMenus();

    m_mainMenu->addMenu(openMenu);
    m_mainMenu->addMenu(recentfiles_menu);
    m_mainMenu->addMenu(ontop_menu);
    m_mainMenu->addMenu(playMenu);
    m_mainMenu->addMenu(play_order_menu);
    m_mainMenu->addMenu(aspect_menu);
    m_mainMenu->addMenu(rotate_flip_menu);
    m_mainMenu->addMenu(audioMenu);
    m_mainMenu->addMenu(subtitlesMenu);
    m_mainMenu->addMenu(osd_menu);
//    m_mainMenu->addAction(shortcutsAct);
    m_mainMenu->addAction(screenshotAct);
    m_mainMenu->addAction(showPreferencesAct);
    m_mainMenu->addAction(showPropertiesAct);
    m_mainMenu->addAction(aboutAct);

    if (!m_toolbarMenu)
        m_toolbarMenu = new QMenu(this);
    else
        m_toolbarMenu->clear();
    m_toolbarMenu->addAction(openFileAct);
    m_toolbarMenu->addAction(screenshotAct);
    m_toolbarMenu->addAction(showPreferencesAct);
    m_toolbarMenu->addAction(helpAct);
    m_toolbarMenu->addAction(aboutAct);
    m_toolbarMenu->addSeparator();
    m_toolbarMenu->addAction(quitAct);
}

void MainWindow::createOpenActionsAndMenus()
{
    openMenu = new QMenu(this);
    openMenu->menuAction()->setText(tr("Open"));
    openMenu->setIcon(Images::icon("open_file_normal"));

    // Menu File
    openFileAct = new MyAction(QKeySequence("Ctrl+F"), this, "open_file");
    connect(openFileAct, SIGNAL(triggered()), this, SLOT(openFile()));
    openFileAct->change(QPixmap(":/res/open_file_normal.png"), tr("Open &File..."));

    openDirectoryAct = new MyAction( this, "open_directory");
    connect(openDirectoryAct, SIGNAL(triggered()), this, SLOT(openDirectory()));
    openDirectoryAct->change(/*QPixmap(":/res/openfolder.png"), */tr("Directory..."));

    openURLAct = new MyAction(QKeySequence("Ctrl+U"), this, "open_url" );
    connect(openURLAct, SIGNAL(triggered()), this, SLOT(openURL()) );
    openURLAct->change(tr("&URL..."));

    openMenu->addAction(openFileAct);
    openMenu->addAction(openDirectoryAct);
    openMenu->addAction(openURLAct);
}

void MainWindow::createRecentsActionsAndMenus()
{
    clearRecentsAct = new MyAction(this, "clear_recents");
    connect(clearRecentsAct, SIGNAL(triggered()), this, SLOT(clearRecentsList()));
    clearRecentsAct->change(QPixmap(":/res/delete_normal.png"), tr("&Clear"));

    recentfiles_menu = new QMenu(this);
    recentfiles_menu->addSeparator();
    recentfiles_menu->addAction(clearRecentsAct);
    recentfiles_menu->menuAction()->setText(tr("Recent files"));
//    recentfiles_menu->menuAction()->setIcon(QPixmap(":/res/delete.png"));

    this->updateRecents();
}

void MainWindow::createTopActionsAndMenus()
{
    // On Top : 在FT上有时候设置置顶无效，在X86下正常`
    onTopActionGroup = new MyActionGroup(this);
    onTopAlwaysAct = new MyActionGroupItem(this,onTopActionGroup,"on_top_always",Preferences::AlwaysOnTop);
    onTopNeverAct = new MyActionGroupItem(this,onTopActionGroup,"on_top_never",Preferences::NeverOnTop);
    onTopWhilePlayingAct = new MyActionGroupItem(this,onTopActionGroup,"on_top_playing",Preferences::WhilePlayingOnTop);
    onTopAlwaysAct->change(tr("&Always"));
    onTopNeverAct->change(tr("&Never"));
    onTopWhilePlayingAct->change(tr("While &playing"));
    connect(onTopActionGroup, SIGNAL(activated(int)), this, SLOT(changeStayOnTop(int)));
    // Ontop submenu
    ontop_menu = new QMenu(this);
    ontop_menu->menuAction()->setObjectName("ontop_menu");
    ontop_menu->addActions(onTopActionGroup->actions());
    ontop_menu->menuAction()->setText(tr("S&tay on top"));
//	ontop_menu->menuAction()->setIcon(Images::icon("ontop"));
}

void MainWindow::createPlayCommandActionsAndMenus()
{
    playMenu = new QMenu(this);
    playMenu->menuAction()->setText(tr("Play control"));
    control_menu = new QMenu(this);
    control_menu->menuAction()->setText(tr("Forward and rewind"));//快进快退
//    control_menu->menuAction()->setIcon(QPixmap(":/res/speed.png"));
    control_menu->menuAction()->setObjectName("control_menu");
    rewind1Act = new MyAction( Qt::Key_Left, this, "rewind1");
    rewind1Act->addShortcut(QKeySequence("Shift+Ctrl+B")); // MCE remote key
    connect(rewind1Act, SIGNAL(triggered()), m_core, SLOT(srewind()));
    rewind2Act = new MyAction( Qt::Key_Down, this, "rewind2");
    connect(rewind2Act, SIGNAL(triggered()), m_core, SLOT(rewind()));
    rewind3Act = new MyAction( Qt::Key_PageDown, this, "rewind3");
    connect(rewind3Act, SIGNAL(triggered()), m_core, SLOT(fastrewind()));
    forward1Act = new MyAction( Qt::Key_Right, this, "forward1");
    forward1Act->addShortcut(QKeySequence("Shift+Ctrl+F")); // MCE remote key
    connect(forward1Act, SIGNAL(triggered()), m_core, SLOT(sforward()));
    forward2Act = new MyAction( Qt::Key_Up, this, "forward2");
    connect(forward2Act, SIGNAL(triggered()), m_core, SLOT(forward()));
    forward3Act = new MyAction( Qt::Key_PageUp, this, "forward3" );
    connect(forward3Act, SIGNAL(triggered()), m_core, SLOT(fastforward()));

//    setAMarkerAct = new MyAction( this, "set_a_marker" );
//	connect( setAMarkerAct, SIGNAL(triggered()),
//             m_core, SLOT(setAMarker()) );

//	setBMarkerAct = new MyAction( this, "set_b_marker" );
//	connect( setBMarkerAct, SIGNAL(triggered()),
//             m_core, SLOT(setBMarker()) );

//	clearABMarkersAct = new MyAction( this, "clear_ab_markers" );
//	connect( clearABMarkersAct, SIGNAL(triggered()),
//             m_core, SLOT(clearABMarkers()) );

//    repeatAct = new MyAction( this, "repeat" );
//	repeatAct->setCheckable( true );
//	connect( repeatAct, SIGNAL(toggled(bool)),
//             m_core, SLOT(toggleRepeat(bool)) );

    gotoAct = new MyAction( QKeySequence("Ctrl+J"), this, "jump_to");
    connect(gotoAct, SIGNAL(triggered()), this, SLOT(showGotoDialog()));
//    gotoAct->change( Images::icon("jumpto"), tr("&Jump to..."));
    gotoAct->change(tr("&Jump to..."));

    control_menu->addAction(rewind1Act);
    control_menu->addAction(forward1Act);
    control_menu->addSeparator();
    control_menu->addAction(rewind2Act);
    control_menu->addAction(forward2Act);
    control_menu->addSeparator();
    control_menu->addAction(rewind3Act);
    control_menu->addAction(forward3Act);
    playMenu->addMenu(control_menu);
    // Speed submenu
    speed_menu = new QMenu(this);
    speed_menu->menuAction()->setText(tr("Play Speed"));
//    speed_menu->menuAction()->setIcon(QPixmap(":/res/speed.png"));
    speed_menu->menuAction()->setObjectName("speed_menu");
    normalSpeedAct = new MyAction(Qt::Key_Backspace, this, "normal_speed");
    connect(normalSpeedAct, SIGNAL(triggered()), m_core, SLOT(normalSpeed()));
    halveSpeedAct = new MyAction(Qt::Key_BraceLeft, this, "halve_speed");
    connect(halveSpeedAct, SIGNAL(triggered()), m_core, SLOT(halveSpeed()));
    doubleSpeedAct = new MyAction(Qt::Key_BraceRight, this, "double_speed");
    connect(doubleSpeedAct, SIGNAL(triggered()), m_core, SLOT(doubleSpeed()));
    decSpeed10Act = new MyAction(Qt::Key_BracketLeft, this, "dec_speed");
    connect(decSpeed10Act, SIGNAL(triggered()), m_core, SLOT(decSpeed10()));
    incSpeed10Act = new MyAction(Qt::Key_BracketRight, this, "inc_speed");
    connect(incSpeed10Act, SIGNAL(triggered()), m_core, SLOT(incSpeed10()));
    decSpeed4Act = new MyAction(this, "dec_speed_4");
    connect(decSpeed4Act, SIGNAL(triggered()), m_core, SLOT(decSpeed4()));
    incSpeed4Act = new MyAction(this, "inc_speed_4");
    connect(incSpeed4Act, SIGNAL(triggered()), m_core, SLOT(incSpeed4()));
    decSpeed1Act = new MyAction(this, "dec_speed_1");
    connect(decSpeed1Act, SIGNAL(triggered()), m_core, SLOT(decSpeed1()));
    incSpeed1Act = new MyAction(this, "inc_speed_1");
    connect(incSpeed1Act, SIGNAL(triggered()), m_core, SLOT(incSpeed1()));
    normalSpeedAct->change( tr("Normal speed"));
    halveSpeedAct->change(tr("Half speed") );
    doubleSpeedAct->change(tr("Double speed"));
    decSpeed10Act->change(tr("Speed -10%"));
    incSpeed10Act->change(tr("Speed +10%"));
    decSpeed4Act->change(tr("Speed -4%"));
    incSpeed4Act->change(tr("Speed +4%"));
    decSpeed1Act->change(tr("Speed -1%"));
    incSpeed1Act->change(tr("Speed +1%"));
    speed_menu->addAction(normalSpeedAct);//正常速度  1
    speed_menu->addSeparator();
    speed_menu->addAction(halveSpeedAct);//半速  0.5
    speed_menu->addAction(doubleSpeedAct);//双倍速度  2
    speed_menu->addSeparator();
    speed_menu->addAction(decSpeed10Act);//速度-10%
    speed_menu->addAction(incSpeed10Act);//速度+10%
    speed_menu->addSeparator();
    speed_menu->addAction(decSpeed4Act);//速度-4%
    speed_menu->addAction(incSpeed4Act);//速度+4%
    speed_menu->addSeparator();
    speed_menu->addAction(decSpeed1Act);//速度-1%
    speed_menu->addAction(incSpeed1Act);//速度+1%
    playMenu->addMenu(speed_menu);
    playMenu->addSeparator();
    playMenu->addAction(gotoAct);
    playMenu->addSeparator();
    playPrevAct = new MyAction(Qt::Key_Less, this, "play_prev");
    playPrevAct->addShortcut(Qt::Key_MediaPrevious); // MCE remote key
    connect(playPrevAct, SIGNAL(triggered()), m_playlistWidget, SLOT(playPrev()));
    playNextAct = new MyAction(Qt::Key_Greater, this, "play_next");
    playNextAct->addShortcut(Qt::Key_MediaNext); // MCE remote key
    connect(playNextAct, SIGNAL(triggered()), m_playlistWidget, SLOT(playNext()));
    playNextAct->change(tr("Next") );
    playPrevAct->change(tr("Previous"));
    playNextAct->setIcon(QPixmap(":/res/next_normal.png"));
    playPrevAct->setIcon(QPixmap(":/res/previous_normal.png"));
    playMenu->addAction(playPrevAct);
    playMenu->addAction(playNextAct);

    this->setJumpTexts();
}

void MainWindow::createPlayOrderActionsAndMenus()
{
    //play order
    playOrderActionGroup = new MyActionGroup(this);
    orderPlaysAct = new MyActionGroupItem(this,playOrderActionGroup,"order_play",Preferences::OrderPlay);
    randomPlayAct = new MyActionGroupItem(this,playOrderActionGroup,"random_play",Preferences::RandomPlay);
    listLoopPlayAct = new MyActionGroupItem(this,playOrderActionGroup,"list_loop_play",Preferences::ListLoopPlay);
    orderPlaysAct->change(tr("Order play"));
    randomPlayAct->change(tr("Random play"));
    listLoopPlayAct->change(tr("List loop play"));
    connect(playOrderActionGroup, SIGNAL(activated(int)), this, SLOT(changePlayOrder(int)));
    play_order_menu = new QMenu(this);
    play_order_menu->menuAction()->setObjectName("play_order_menu");
    play_order_menu->addActions(playOrderActionGroup->actions());
    play_order_menu->menuAction()->setText(tr("Play order"));
    //    play_order_menu->menuAction()->setIcon(Images::icon("list_cycle_normal"));
}

void MainWindow::createVideoAspectActionsAndMenus()
{
    // Video aspect
    aspectGroup = new MyActionGroup(this);
    aspectDetectAct = new MyActionGroupItem(this, aspectGroup, "aspect_detect", MediaSettings::AspectAuto);
    aspect11Act = new MyActionGroupItem(this, aspectGroup, "aspect_1:1", MediaSettings::Aspect11 );
    aspect54Act = new MyActionGroupItem(this, aspectGroup, "aspect_5:4", MediaSettings::Aspect54 );
    aspect43Act = new MyActionGroupItem(this, aspectGroup, "aspect_4:3", MediaSettings::Aspect43);
    aspect118Act = new MyActionGroupItem(this, aspectGroup, "aspect_11:8", MediaSettings::Aspect118 );
    aspect1410Act = new MyActionGroupItem(this, aspectGroup, "aspect_14:10", MediaSettings::Aspect1410 );
    aspect32Act = new MyActionGroupItem(this, aspectGroup, "aspect_3:2", MediaSettings::Aspect32);
    aspect149Act = new MyActionGroupItem(this, aspectGroup, "aspect_14:9", MediaSettings::Aspect149 );
    aspect1610Act = new MyActionGroupItem(this, aspectGroup, "aspect_16:10", MediaSettings::Aspect1610 );
    aspect169Act = new MyActionGroupItem(this, aspectGroup, "aspect_16:9", MediaSettings::Aspect169 );
    aspect235Act = new MyActionGroupItem(this, aspectGroup, "aspect_2.35:1", MediaSettings::Aspect235);

    QAction * sep = new QAction(aspectGroup);
    sep->setSeparator(true);

    aspectNoneAct = new MyActionGroupItem(this, aspectGroup, "aspect_none", MediaSettings::AspectNone);
    connect(aspectGroup, SIGNAL(activated(int)), m_core, SLOT(changeAspectRatio(int)));
    aspectDetectAct->change(tr("&Auto"));
    aspect11Act->change("1&:1");
    aspect32Act->change("&3:2");
    aspect43Act->change("&4:3");
    aspect118Act->change("11:&8");
    aspect54Act->change("&5:4");
    aspect149Act->change("&14:9");
    aspect1410Act->change("1&4:10");
    aspect169Act->change("16:&9");
    aspect1610Act->change("1&6:10");
    aspect235Act->change("&2.35:1");
    aspectNoneAct->change(tr("&Disabled"));

    // Aspect submenu
    aspect_menu = new QMenu(this);
    aspect_menu->menuAction()->setObjectName("aspect_menu");
    aspect_menu->addActions(aspectGroup->actions());
    aspect_menu->menuAction()->setText(tr("Aspect ratio"));//宽高比  画面比例
}

void MainWindow::createVideoRotateActionsAndMenus()
{
    // Rotate
    rotateGroup = new MyActionGroup(this);
    rotateNoneAct = new MyActionGroupItem(this, rotateGroup, "rotate_none", MediaSettings::NoRotate);
    rotateClockwiseFlipAct = new MyActionGroupItem(this, rotateGroup, "rotate_clockwise_flip", MediaSettings::Clockwise_flip);
    rotateClockwiseAct = new MyActionGroupItem(this, rotateGroup, "rotate_clockwise", MediaSettings::Clockwise);
    rotateCounterclockwiseAct = new MyActionGroupItem(this, rotateGroup, "rotate_counterclockwise", MediaSettings::Counterclockwise);
    rotateCounterclockwiseFlipAct = new MyActionGroupItem(this, rotateGroup, "rotate_counterclockwise_flip", MediaSettings::Counterclockwise_flip);
    connect(rotateGroup, SIGNAL(activated(int)), m_core, SLOT(changeRotate(int)));
    rotateNoneAct->change(tr("&Off"));
    rotateClockwiseFlipAct->change(tr("&Rotate by 90 degrees clockwise and flip"));
    rotateClockwiseAct->change(tr("Rotate by 90 degrees &clockwise"));
    rotateCounterclockwiseAct->change(tr("Rotate by 90 degrees counterclock&wise"));
    rotateCounterclockwiseFlipAct->change(tr("Rotate by 90 degrees counterclockwise and &flip"));

    flipAct = new MyAction(this, "flip");
    flipAct->setCheckable(true);
    connect(flipAct, SIGNAL(toggled(bool)), m_core, SLOT(toggleFlip(bool)));
    flipAct->change(tr("Fli&p image"));

    mirrorAct = new MyAction(this, "mirror");
    mirrorAct->setCheckable(true);
    connect(mirrorAct, SIGNAL(toggled(bool)), m_core, SLOT(toggleMirror(bool)));
    mirrorAct->change(tr("Mirr&or image"));

    // Rotate menu
    rotate_flip_menu = new QMenu(this);
    rotate_flip_menu->menuAction()->setText(tr("Frame rotation"));//画面旋转
    // Rotate submenu
    rotate_menu = new QMenu(this);
    rotate_menu->menuAction()->setObjectName("rotate_menu");
    rotate_menu->addActions(rotateGroup->actions());
    rotate_menu->menuAction()->setText(tr("&Rotate") );
    rotate_flip_menu->addMenu(rotate_menu);
    rotate_flip_menu->addAction(flipAct);
    rotate_flip_menu->addAction(mirrorAct);
}

void MainWindow::createAudioActionsAndMenus()
{
    // Menu Audio
    // Audio channels
//    channelsDefaultAct = new MyActionGroupItem(this, channelsGroup, "channels_default", MediaSettings::ChDefault);
    channelsGroup = new MyActionGroup(this);
    channelsStereoAct = new MyActionGroupItem(this, channelsGroup, "channels_stereo", MediaSettings::ChStereo);
    channelsSurroundAct = new MyActionGroupItem(this, channelsGroup, "channels_surround", MediaSettings::ChSurround);
    channelsFull51Act = new MyActionGroupItem(this, channelsGroup, "channels_ful51", MediaSettings::ChFull51);
    channelsFull61Act = new MyActionGroupItem(this, channelsGroup, "channels_ful61", MediaSettings::ChFull61);
    channelsFull71Act = new MyActionGroupItem(this, channelsGroup, "channels_ful71", MediaSettings::ChFull71);
    connect(channelsGroup, SIGNAL(activated(int)), m_core, SLOT(setAudioChannels(int)));
    channelsStereoAct->change(tr("&Stereo"));
    channelsSurroundAct->change(tr("&4.0 Surround"));
    channelsFull51Act->change(tr("&5.1 Surround"));
    channelsFull61Act->change(tr("&6.1 Surround"));
    channelsFull71Act->change(tr("&7.1 Surround"));
    // Audio channels submenu
    audiochannels_menu = new QMenu(this);
    audiochannels_menu->menuAction()->setObjectName("audiochannels_menu");
    audiochannels_menu->addActions(channelsGroup->actions());
    audiochannels_menu->menuAction()->setText(tr("&Channels"));

    audioMenu = new QMenu(this);
    audioMenu->menuAction()->setText(tr("Audio"));
//    audioMenuAct->setIcon(Images::icon("audio_menu"));
    muteAct = new MyAction(Qt::Key_M, this, "mute" );//kobe:音频菜单里面的静音
    muteAct->addShortcut(Qt::Key_VolumeMute); // MCE remote key
    muteAct->setCheckable(true);
    connect(muteAct, SIGNAL(toggled(bool)), m_core, SLOT(mute(bool)));
    QIcon icset(Images::icon("volume_low_normal"));
    icset.addPixmap(Images::icon("volume_mute_normal"), QIcon::Normal, QIcon::On);
    muteAct->change(icset, tr("&Mute"));

    // Submenu Filters
    extrastereoAct = new MyAction(this, "extrastereo_filter");
    extrastereoAct->setCheckable(true);
    connect(extrastereoAct, SIGNAL(toggled(bool)), m_core, SLOT(toggleExtrastereo(bool)));

    karaokeAct = new MyAction(this, "karaoke_filter");
    karaokeAct->setCheckable(true);
    connect(karaokeAct, SIGNAL(toggled(bool)), m_core, SLOT(toggleKaraoke(bool)));

    volnormAct = new MyAction(this, "volnorm_filter");
    volnormAct->setCheckable(true);
    connect(volnormAct, SIGNAL(toggled(bool)), m_core, SLOT(toggleVolnorm(bool)));

    earwaxAct = new MyAction(this, "earwax_filter");
    earwaxAct->setCheckable(true);
    connect(earwaxAct, SIGNAL(toggled(bool)), m_core, SLOT(toggleEarwax(bool)));

    extrastereoAct->change(tr("&Extrastereo"));
    karaokeAct->change(tr("&Karaoke"));
    volnormAct->change(tr("Volume &normalization") );
    earwaxAct->change(tr("&Headphone optimization") + " (earwax)");

    // Audio filter submenu
    audiofilter_menu = new QMenu(this);
    audiofilter_menu->menuAction()->setObjectName("audiofilter_menu");
    audiofilter_menu->addAction(extrastereoAct);
    audiofilter_menu->addAction(karaokeAct);
    audiofilter_menu->addAction(earwaxAct);
    audiofilter_menu->addAction(volnormAct);
    audiofilter_menu->menuAction()->setText(tr("&Filters"));
    audiofilter_menu->menuAction()->setIcon(Images::icon("audio_filters"));

    decVolumeAct = new MyAction(Qt::Key_9, this, "dec_volume");
    connect(decVolumeAct, SIGNAL(triggered()), m_core, SLOT(decVolume()));
    decVolumeAct->change(tr("Volume -"));
    incVolumeAct = new MyAction(Qt::Key_0, this, "inc_volume");
    connect(incVolumeAct, SIGNAL(triggered()), m_core, SLOT(incVolume()));
    incVolumeAct->change(tr("Volume +") );
    decAudioDelayAct = new MyAction(Qt::Key_Minus, this, "dec_audio_delay");
    connect( decAudioDelayAct, SIGNAL(triggered()), m_core, SLOT(decAudioDelay()));
    decAudioDelayAct->change(tr("Delay -"));
    incAudioDelayAct = new MyAction(Qt::Key_Plus, this, "inc_audio_delay");
    connect(incAudioDelayAct, SIGNAL(triggered()), m_core, SLOT(incAudioDelay()));
    incAudioDelayAct->change(tr("Delay +"));
    audioDelayAct = new MyAction(Qt::Key_Y, this, "audio_delay");
    connect(audioDelayAct, SIGNAL(triggered()), this, SLOT(showAudioDelayDialog()));
    audioDelayAct->change(tr("Set dela&y..."));
    loadAudioAct = new MyAction( this, "load_audio_file");
    connect(loadAudioAct, &MyAction::triggered, this, [=] () {
        exitFullscreenIfNeeded();

        Extensions e;
        QString s = MyFileDialog::getOpenFileName(
            this, tr("Choose a file"),
            pref->latest_dir,
            tr("Audio") + e.audio().forFilter()+";;" +
            tr("All files") +" (*.*)" );

        if (!s.isEmpty()) m_core->loadAudioFile(s);
    });
    loadAudioAct->change(tr("&Load external file..."));
    unloadAudioAct = new MyAction(this, "unload_audio_file");
    connect(unloadAudioAct, SIGNAL(triggered()), m_core, SLOT(unloadAudioFile()));
    unloadAudioAct->change(tr("U&nload"));

    audioEqualizerAct = new MyAction(this, "audio_equalizer");
    audioEqualizerAct->setCheckable(true);
    audioEqualizerAct->change(tr("E&qualizer"));
    connect(audioEqualizerAct, SIGNAL(toggled(bool)),this, SLOT(showAudioEqualizer(bool)));
    resetAudioEqualizerAct = new MyAction(this, "reset_audio_equalizer");
    connect(resetAudioEqualizerAct, SIGNAL(triggered()), audio_equalizer, SLOT(reset()));
    resetAudioEqualizerAct->change(tr("Reset audio equalizer"));

    // Stereo mode
    stereoGroup = new MyActionGroup(this);
    stereoAct = new MyActionGroupItem(this, stereoGroup, "stereo", MediaSettings::Stereo);
    leftChannelAct = new MyActionGroupItem(this, stereoGroup, "left_channel", MediaSettings::Left);
    rightChannelAct = new MyActionGroupItem(this, stereoGroup, "right_channel", MediaSettings::Right);
    monoAct = new MyActionGroupItem(this, stereoGroup, "mono", MediaSettings::Mono);
    reverseAct = new MyActionGroupItem(this, stereoGroup, "reverse_channels", MediaSettings::Reverse);
    connect(stereoGroup, SIGNAL(activated(int)), m_core, SLOT(setStereoMode(int)));
    stereoAct->change(tr("&Stereo"));
    leftChannelAct->change(tr("&Left channel"));
    rightChannelAct->change(tr("&Right channel"));
    monoAct->change(tr("&Mono"));
    reverseAct->change(tr("Re&verse"));

    // Stereo mode submenu
    stereomode_menu = new QMenu(this);
    stereomode_menu->menuAction()->setObjectName("stereomode_menu");
    stereomode_menu->addActions(stereoGroup->actions());
    stereomode_menu->menuAction()->setText(tr("&Stereo mode"));
//	stereomode_menu->menuAction()->setIcon(Images::icon("stereo_mode"));

    audioMenu->addAction(muteAct);
    audioMenu->addSeparator();
    audioMenu->addAction(loadAudioAct);
    audioMenu->addAction(unloadAudioAct);
    audioMenu->addSeparator();
    audioMenu->addMenu(audiofilter_menu);
    audioMenu->addAction(audioEqualizerAct);
    audioMenu->addSeparator();
    audioMenu->addMenu(audiochannels_menu);
    audioMenu->addMenu(stereomode_menu);
    audioMenu->addSeparator();
    audioMenu->addAction(decVolumeAct);
    audioMenu->addAction(incVolumeAct);
    audioMenu->addSeparator();
    audioMenu->addAction(decAudioDelayAct);
    audioMenu->addAction(incAudioDelayAct);
    audioMenu->addSeparator();
    audioMenu->addAction(audioDelayAct);
}

void MainWindow::createScreenshotActionsAndMenus()
{
    //    shortcutsAct = new MyAction(QKeySequence("Shift+?"), this, "Shortcuts");//快捷键  Qt::Key_Question
    ////    shortcutsAct->addShortcut(QKeySequence("Shift+Ctrl+?"));
    //    connect(shortcutsAct, SIGNAL(triggered()), this, SLOT(showShortcuts()));
    //    shortcutsAct->change(tr("Shortcuts"));

    // Single screenshot
    screenshotAct = new MyAction(Qt::Key_S, this, "screenshot");//屏幕截图
    connect(screenshotAct, SIGNAL(triggered()), m_core, SLOT(screenshot()));
    screenshotAct->change(Images::icon("screenshot_normal"), tr("&Screenshot"));

    /*screenshotWithSubsAct = new MyAction( QKeySequence("Ctrl+Shift+S"), this, "screenshot_with_subtitles" );
    connect( screenshotWithSubsAct, SIGNAL(triggered()),
             m_core, SLOT(screenshotWithSubtitles()) );

    screenshotWithNoSubsAct = new MyAction( QKeySequence("Ctrl+Alt+S"), this, "screenshot_without_subtitles" );
    connect( screenshotWithNoSubsAct, SIGNAL(triggered()),
             m_core, SLOT(screenshotWithoutSubtitles()) );

    // Multiple screenshots
    screenshotsAct = new MyAction( QKeySequence("Shift+D"), this, "multiple_screenshots" );
    connect( screenshotsAct, SIGNAL(triggered()),
             m_core, SLOT(screenshots()) );*/
}

void MainWindow::createSubtitleActionsAndMenus()
{
    subtitlesMenu = new QMenu(this);
    subtitlesMenu->menuAction()->setText( tr("Subtitles") );
//    subtitlesMenuAct->setIcon(Images::icon("subtitles_menu"));
    loadSubsAct = new MyAction(this, "load_subs" );
    connect(loadSubsAct, SIGNAL(triggered()), this, SLOT(loadSub()));
    loadSubsAct->change(tr("Load..."));
//    subVisibilityAct = new MyAction(Qt::Key_V, this, "subtitle_visibility");
    subVisibilityAct = new MyAction(this, "sub_visibility");
    subVisibilityAct->setCheckable(true);
    connect(subVisibilityAct, SIGNAL(toggled(bool)), m_core, SLOT(changeSubVisibility(bool)));
    subVisibilityAct->change(tr("Subtitle &visibility"));
    subtitlesMenu->addAction(loadSubsAct);
    subtitlesMenu->addAction(subVisibilityAct);
}

void MainWindow::createOsdActionsAndMenus()
{
    showMediaInfoAct = new MyAction(Qt::SHIFT | Qt::Key_I, this, "show_info_osd");
    connect( showMediaInfoAct, SIGNAL(triggered()), m_core, SLOT(showMediaInfoOnOSD()) );
    showMediaInfoAct->change( tr("Show &info on OSD") );

//    showTimeAct = new MyAction(Qt::Key_I, this, "show_time");
//    connect( showTimeAct, SIGNAL(triggered()), m_core, SLOT(showTimeOnOSD()) );
//    showTimeAct->change( tr("Show playback time on OSD") );//在 OSD 上显示播放时间

    // OSD
    incOSDScaleAct = new MyAction(Qt::SHIFT | Qt::Key_U, this, "inc_osd_scale");
    connect(incOSDScaleAct, SIGNAL(triggered()), m_core, SLOT(incOSDScale()));
    incOSDScaleAct->change(tr("Size &+"));

    decOSDScaleAct = new MyAction(Qt::SHIFT | Qt::Key_Y, this, "dec_osd_scale");
    connect(decOSDScaleAct, SIGNAL(triggered()), m_core, SLOT(decOSDScale()));
    decOSDScaleAct->change(tr("Size &-"));

//#ifdef MPV_SUPPORT
    OSDFractionsAct = new MyAction(this, "osd_fractions");
    OSDFractionsAct->change(tr("Show times with &milliseconds"));
    OSDFractionsAct->setCheckable(true);
    connect(OSDFractionsAct, SIGNAL(toggled(bool)), m_core, SLOT(setOSDFractions(bool)));
//#endif
    osdGroup = new MyActionGroup(this);
    osdNoneAct = new MyActionGroupItem(this, osdGroup, "osd_none", Preferences::NoneOsd);
    osdSeekAct = new MyActionGroupItem(this, osdGroup, "osd_seek", Preferences::Seek);
    osdTimerAct = new MyActionGroupItem(this, osdGroup, "osd_timer", Preferences::SeekTimer);
    osdTotalAct = new MyActionGroupItem(this, osdGroup, "osd_total", Preferences::SeekTimerTotal);
    connect( osdGroup, SIGNAL(activated(int)), m_core, SLOT(changeOSD(int)) );
    // Action groups
    osdNoneAct->change( tr("Subtitles onl&y") );
    osdSeekAct->change( tr("Volume + &Seek") );
    osdTimerAct->change( tr("Volume + Seek + &Timer") );
    osdTotalAct->change( tr("Volume + Seek + Timer + T&otal time") );

    // OSD submenu
    osd_menu = new QMenu(this);
    osd_menu->menuAction()->setObjectName("osd_menu");
    osd_menu->addActions(osdGroup->actions());
    osd_menu->addSeparator();
    osd_menu->addAction(showMediaInfoAct);
    osd_menu->addSeparator();
    osd_menu->addAction(decOSDScaleAct);
    osd_menu->addAction(incOSDScaleAct);
    osd_menu->addSeparator();
    osd_menu->addAction(OSDFractionsAct);
    // Menu Options
    osd_menu->menuAction()->setText( tr("&OSD") );
    osd_menu->menuAction()->setIcon( Images::icon("osd") );
}

void MainWindow::createOthersActionsAndMenus()
{
    showPreferencesAct = new MyAction(QKeySequence("Ctrl+P"), this, "show_preferences");
    connect(showPreferencesAct, SIGNAL(triggered()), this, SLOT(showPreferencesDialog()));
    showPreferencesAct->change(QPixmap(":/res/prefs.png"), tr("Preferences"));//首选项

    showPropertiesAct = new MyAction(QKeySequence("Ctrl+I"), this, "show_file_properties");
    connect(showPropertiesAct, SIGNAL(triggered()), this, SLOT(showFilePropertiesDialog()));
    showPropertiesAct->change(QPixmap(":/res/info.png"), tr("View &info and properties..."));//查看信息和属性

    helpAct = new MyAction(QKeySequence("Ctrl+H"), this, "show_help" );
    connect(helpAct, SIGNAL(triggered()), this, SLOT(showHelpDialog()));
    helpAct->change(QPixmap(":/res/help_normal.png"), tr("Help"));

    aboutAct = new MyAction(QKeySequence("Ctrl+A"), this, "about_kylin_video");
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(showAboutDialog()));
    aboutAct->change(Images::icon("about_normal"), tr("About &Kylin Video"));

    quitAct = new MyAction(QKeySequence("Ctrl+Q"), this, "quit");
    quitAct->change(Images::icon("quit_normal"), tr("Quit"));
    connect(quitAct, SIGNAL(triggered()), this, SLOT(onCloseWindow()));


    /*m_poweroffAct = new MyAction(this, "poweroff");
    m_poweroffAct->change(Images::icon("poweroff"), tr("PowerOff"));
    connect(m_poweroffAct, SIGNAL(triggered()), this, SLOT(powerOffPC()));*/

    //20181120
    //showFilenameAct = new MyAction(Qt::SHIFT | Qt::Key_O, this, "show_filename_osd");
    //connect( showFilenameAct, SIGNAL(triggered()), m_core, SLOT(showFilenameOnOSD()) );
//    showFilenameAct->change( tr("Show filename on OSD") );//在OSD中显示文件名


    // The invisible shortcuts: playlist_action, play_pause_aciton, stopAct and fullscreenAct
    playlist_action = new MyAction(QKeySequence("F3"), this, "playlist_open_close");
    playlist_action->change(tr("PlayList"));
    connect(playlist_action, SIGNAL(triggered()), this, SLOT(onShowOrHidePlaylist()));

    //这里设置快捷键后，则在keyPressEvent中无法获取到该快捷键的QKeySequence
    /*play_pause_aciton = new MyAction(Qt::Key_Space, this, "play_pause");
    //play_pause_aciton = new MyAction(QKeySequence(Qt::Key_Space), this, "play_pause");
    play_pause_aciton->change(tr("Play/Pause"));
    connect(playlist_action, &MyAction::triggered, this, &MainWindow::onPlayPause);*/

    stopAct = new MyAction(Qt::Key_MediaStop, this, "stop");
    stopAct->change(tr("Stop"));
    connect(stopAct, SIGNAL(triggered()), m_core, SLOT(stop()));

    fullscreenAct = new MyAction(QKeySequence("Ctrl+Return")/*Qt::Key_F*/, this, "fullscreen");
    fullscreenAct->change(tr("Fullscreen"));
    fullscreenAct->setCheckable(true);
    connect(fullscreenAct, SIGNAL(toggled(bool)), this, SLOT(toggleFullscreen(bool)));
}

void MainWindow::createTrayActions()
{
    //m_mainTray = new QSystemTrayIcon(Images::icon("logo", 22), this);
    m_mainTray = new QSystemTrayIcon(this);
    QIcon icon = QIcon::fromTheme("kylin-video", QIcon(":/res/kylin-video.png"));
    /*QIcon icon(QIcon::fromTheme("kylin-video"));
    if (icon.isNull()) {
        icon = QIcon(":/res/kylin-video.png");
    }*/
    m_mainTray->setIcon(icon);
    m_mainTray->setToolTip(tr("Kylin Video"));
    m_mainTray->show();
    connect(m_mainTray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayIconActivated(QSystemTrayIcon::ActivationReason)));
    //m_mainTray->setIcon(Images::icon("logo", 22));
    tray_menu = new QMenu(this);

    action_show = new MyAction(this, "open_window");
    action_show->change(Images::icon("open_window_normal"), tr("Open Homepage"));

    action_openshotsdir = new MyAction(this, "open_shots_dir");
    action_openshotsdir->change(Images::icon("open_screen"), tr("Open screenshots folder"));

    connect(action_show, SIGNAL(triggered()), this, SLOT(showMainWindow()));
    connect(action_openshotsdir, &MyAction::toggled, this, [=] () {
        bool open_enabled = ((!pref->screenshot_directory.isEmpty()) && (QFileInfo(pref->screenshot_directory).isDir()));
        if (open_enabled) {
            QDesktopServices::openUrl(QUrl(QString("file:%1").arg(pref->screenshot_directory), QUrl::TolerantMode));
        }
        else {
            m_mainTray->showMessage(tr("Information"), tr("The screenshot folder does not exist!"), QSystemTrayIcon::Information, 2000);//QSystemTrayIcon::Warning
        }
    });
    tray_menu->setFixedWidth(250);

    //添加菜单项
    tray_menu->addAction(action_show);
    tray_menu->addAction(action_openshotsdir);
    tray_menu->addSeparator();
    tray_menu->addAction(aboutAct);
    tray_menu->addAction(helpAct);
    tray_menu->addSeparator();
    tray_menu->addAction(showPreferencesAct);
    tray_menu->addAction(quitAct);
//    tray_menu->addAction(m_poweroffAct);

    m_mainTray->setContextMenu(tray_menu);
}

void MainWindow::createTipWidget()
{
    m_tipWidget = new TipWidget("Hello, Kylin!", this);
    m_tipWidget->setFixedHeight(30);
    m_tipWidget->move(10, TOP_TOOLBAR_HEIGHT);
    m_tipWidget->hide();

    m_tipTimer = new QTimer(this);
    connect(m_tipTimer, SIGNAL(timeout()), m_tipWidget, SLOT(hide()));
    m_tipTimer->setInterval(2000);
}

void MainWindow::createEscWidget()
{
    m_escWidget = new EscTip(this);
    m_escWidget->setFixedSize(440, 64);
    int windowWidth = QApplication::desktop()->screenGeometry(0).width();
    int windowHeight = QApplication::desktop()->screenGeometry(0).height();
    m_escWidget->move((windowWidth - m_escWidget->width()) / 2,(windowHeight - m_escWidget->height()) / 2);
    m_escWidget->hide();
}

void MainWindow::createMaskWidget()
{
    m_playMaskWidget = new PlayMask(m_mplayerWindow);
    m_mplayerWindow->setCornerWidget(m_playMaskWidget);
    m_playMaskWidget->hide();
    connect(m_playMaskWidget, &PlayMask::signal_play_continue, this, &MainWindow::onPlayPause);
}

void MainWindow::changeStayOnTop(int stay_on_top)
{
    switch (stay_on_top) {
        case Preferences::AlwaysOnTop : setStayOnTop(true); break;
        case Preferences::NeverOnTop  : setStayOnTop(false); break;
        case Preferences::WhilePlayingOnTop : setStayOnTop((m_core->state() == Core::Playing)); break;
    }

    pref->stay_on_top = (Preferences::OnTop) stay_on_top;
    this->updateOnTopWidgets();
}

void MainWindow::checkStayOnTop(Core::State state)
{
    if ((!pref->fullscreen) && (pref->stay_on_top == Preferences::WhilePlayingOnTop)) {
        setStayOnTop((state == Core::Playing));
    }
}

void MainWindow::changePlayOrder(int play_order)
{
    pref->play_order = (Preferences::PlayOrder) play_order;
    this->updatePlayOrderWidgets();
    if (m_playlistWidget) {
        m_playlistWidget->updatePlayOrderSettings();
    }
}

void MainWindow::bindThreadWorker(InfoWorker *worker)
{
    connect(m_playlistWidget, SIGNAL(requestGetMediaInfo(QStringList)), worker, SLOT(onGetMediaInfo(QStringList)));
    connect(worker, SIGNAL(meidaFilesAdded(VideoPtrList,bool)), this, SLOT(onMeidaFilesAdded(VideoPtrList,bool)));
}

void MainWindow::onMeidaFilesAdded(const VideoPtrList medialist, bool isFileInPlaylist)
{
    if (medialist.length() == 0) {
        if (!isFileInPlaylist) {//当执行了新加文件操作，而得到的新加文件结果为0时，此时如果isFileInPlaylist为true，表示新增加的文件在播放列表中早已存在，故不再次添加，此时不显示添加失败信息
            QString message = QString(tr("Failed to add files!"));
            this->displayMessage(message);
        }
    }
    else {
        this->m_playlistWidget->onPlayListChanged(medialist);
    }
}

void MainWindow::onPlayPause()
{
    m_topToolbar->show();
    m_bottomToolbar->show();
    //connect(m_mouseFilterHandler, SIGNAL(mouseMoved()), m_bottomController, SLOT(temporaryShow()));
//    if (m_playlistWidget && m_playlistWidget->count() > 0) {
        m_core->playOrPause(m_lastPlayingSeek);
//    }
}

//void MainWindow::showShortcuts()
//{
//    m_shortcutsWidget->setPrefData(pref);
//    m_shortcutsWidget->show();
//    m_shortcutsWidget->restart_timer();
//}

void MainWindow::keyPressEvent(QKeyEvent *event) {
//    if (event->key() == Qt::Key_Up) {
//    }
//    if (event->key() == Qt::Key_Down) {
//    }
    if (event->key() == Qt::Key_Space) {
        this->leftClickFunction();
    }
    if (event->key() == Qt::Key_Escape) {
        if (pref->fullscreen) {
            toggleFullscreen(false);
        }
    }
//    if (event->key() == Qt::Key_Question) {
//        this->showShortcuts();
//    }
//    QWidget::keyPressEvent(event);
    QMainWindow::keyPressEvent(event);
}

void MainWindow::onMediaStoppedByUser()
{
    this->exitFullscreenOnStop();
    m_mplayerWindow->showLogo();
    this->disableActionsOnStop();
#ifdef PREVIEW_TEST
    if (m_previewMgr) {
        m_previewMgr->stop();
        delete m_previewMgr;
    }
#endif
}

void MainWindow::onMute()
{
    bool muted = pref->global_volume ? pref->mute : m_core->mset.mute;
    m_core->mute(!muted);
    if (muted && pref->volume <= 0) {
        m_core->setVolume(50, true);
    }

    this->updateMuteWidgets();
}

void MainWindow::onMaxWindow(bool b)
{
    this->onShowOrHideEscWidget(false);
    pref->fullscreen = false;
    m_bottomToolbar->onUnFullScreen();//让全屏/取消全屏的按钮更换图片为全屏
    if (isFullScreen()) {
        m_topToolbar->updateMaxButtonStatus(false);
        this->showNormal();
    }
    else {
        if (!this->isMaximized()) {
            m_topToolbar->updateMaxButtonStatus(true);
            this->showMaximized();
        }
        else {
            m_topToolbar->updateMaxButtonStatus(false);
            this->showNormal();
        }
    }
}

void MainWindow::onMinWindow()
{
    /*if( windowState() != Qt::WindowMinimized ){
        setWindowState( Qt::WindowMinimized );
    }*/
    this->onShowOrHideEscWidget(false);
    pref->fullscreen = false;
    if (this->window()->windowState() == Qt::WindowMaximized) {
        m_oldIsMaxmized = true;
    }
    else {
        m_oldIsMaxmized = false;
    }
    this->showMinimized();
}

void MainWindow::onShowMenu()
{
    QPoint p = rect().topRight();
    p.setX(p.x() - 38*4);
    p.setY(p.y() + 36);
    m_toolbarMenu->exec(this->mapToGlobal(p));
}

void MainWindow::onCloseWindow()
{
    if (m_core->state() != Core::Stopped) {
        m_lastPlayingSeek = 0;
        m_core->stop();
    }

    m_playlistWidget->close();

    qApp->quit();
}

void MainWindow::onFullScreen()
{
    if (pref->fullscreen) {
        toggleFullscreen(false);
    }
    else {
        toggleFullscreen(true);
    }
}

void MainWindow::onShowOrHidePlaylist()
{
    setPlaylistVisible(!m_playlistWidget->isVisible());
}

void MainWindow::setPlaylistVisible(bool visible)
{
    m_playlistWidget->setProperty("moving", true);
    int titleBarHeight = TOP_TOOLBAR_HEIGHT;

    double factor = 0.6;
    QRect start(this->width(), titleBarHeight, m_playlistWidget->width(), m_playlistWidget->height());
    QRect end(this->width() - m_playlistWidget->width(), titleBarHeight, m_playlistWidget->width(), m_playlistWidget->height());
    if (!visible) {
        this->slideEdgeWidget(m_playlistWidget, end, start, ANIMATIONDELAY * factor, true);
        emit this->requestUpdatePlaylistBtnQssProperty(false);
    } else {
        m_playlistWidget->setFocus();
        this->slideEdgeWidget(m_playlistWidget, start, end, ANIMATIONDELAY * factor);
        emit this->requestUpdatePlaylistBtnQssProperty(true);
    }
    QTimer::singleShot(ANIMATIONDELAY * factor, this, [=] {
        m_playlistWidget->setEnabled(true);
    });
    m_topToolbar->raise();
    QTimer::singleShot(ANIMATIONDELAY * factor * 1, this, [=] {
        m_playlistWidget->setProperty("moving", false);
    });
}

void MainWindow::exitFullscreen()
{
    if (pref->fullscreen) {
        toggleFullscreen(false);
    }
}

void MainWindow::exitFullscreenOnStop()
{
    if (pref->fullscreen) {
        toggleFullscreen(false);
    }

    if (m_topToolbar) {
        m_topToolbar->cleaTitleName();
    }
}

void MainWindow::exitFullscreenIfNeeded()
{
    if (pref->fullscreen) {
        toggleFullscreen(false);
    }
}

void MainWindow::toggleFullscreen(bool b)
{
    if (b == pref->fullscreen) {
        // Nothing to do
        qDebug("MainWindow::toggleFullscreen: nothing to do, returning");
        return;
    }

    pref->fullscreen = b;

    if (!m_centralWidget->isVisible()) {
        return;
    }

    m_mplayerWindow->hideLogoForTemporary();

    if (pref->fullscreen) {
        if (m_bottomController)
            m_bottomController->temporaryShow();
        m_isMaximized = isMaximized();

        if (pref->stay_on_top == Preferences::WhilePlayingOnTop && m_core->state() == Core::Playing) {
            setStayOnTop(false);
        }

        showFullScreen();
        m_bottomToolbar->onFullScreen();//让全屏/取消全屏的按钮更换图片为取消全屏
        this->m_resizeCornerBtn->hide();
        m_topToolbar->updateMaxButtonStatus(true);

        QString state = m_core->stateToString().toUtf8().data();
        if (state == "Playing" || state == "Paused") {//全屏的时候如果不是正在播放或暂停，则显示标题栏和控制栏
        }

        this->onShowOrHideEscWidget(true);
    }
    else {
        if (m_bottomController)
            m_bottomController->permanentShow();
        m_topToolbar->updateMaxButtonStatus(false);
        this->showNormal();
//        if (m_isMaximized) {
//            m_topToolbar->updateMaxButtonStatus(true);
//            this->showMaximized(); // It has to be called after showNormal()
//        }
        m_bottomToolbar->onUnFullScreen();//让全屏/取消全屏的按钮更换图片为全屏
        this->m_resizeCornerBtn->show();
        if (this->m_escWidget->isVisible()) {//For Esc Key：不能用this->onShowOrHideEscWidget(false)
            this->m_escWidget->hide();
        }

        QString state = m_core->stateToString().toUtf8().data();
        if (state == "Playing" || state == "Paused") {// Stopped Playing Paused

        }
    }
    updateWidgets();
    setFocus();
    QTimer::singleShot(100, m_mplayerWindow, SLOT(updateLogoPosition()));
}

#ifdef SINGLE_INSTANCE
//单实例情况下，麒麟影音正在运行时，此时如果对其他视频文件进行右键选择使用麒麟影音播放，则会走这里打开文件进行播放
void MainWindow::handleMessageFromOtherInstances(const QString& message) {
	int pos = message.indexOf(' ');
	if (pos > -1) {
		QString command = message.left(pos);
		QString arg = message.mid(pos+1);

		if (command == "open_file") {
            this->showMainWindow();
            doOpen(arg);
		} 
		else
		if (command == "open_files") {
			QStringList file_list = arg.split(" <<sep>> ");
            this->showMainWindow();
			openFiles(file_list);
		}
		else
		if (command == "add_to_playlist") {
//			QStringList file_list = arg.split(" <<sep>> ");
            /* if (m_core->state() == Core::Stopped) { this->showMainWindow(); } */
//			playlist->addFiles(file_list);
		}
		else
		if (command == "media_title") {
			QStringList list = arg.split(" <<sep>> ");
            m_core->addForcedTitle(list[0], list[1]);
		}
        else
        if (command == "action") {
            processFunction(arg);
        }
		else
		if (command == "load_sub") {
			setInitialSubtitle(arg);
            if (m_core->state() != Core::Stopped) {
                m_core->loadSub(arg);
			}
		}
	}
}
#endif

void MainWindow::enableActionsOnPlaying()
{
    this->setActionsEnabled(true);
    // Screenshot option
    bool screenshots_enabled = ((pref->use_screenshot) &&
                                 (!pref->screenshot_directory.isEmpty()) &&
                                 (QFileInfo(pref->screenshot_directory).isDir()));
    screenshotAct->setEnabled(screenshots_enabled);

    // Enable or disable the audio equalizer
    audioEqualizerAct->setEnabled(pref->use_audio_equalizer);

    // Disable audio actions if there's not audio track
//    if ((m_core->mdat.audios.numItems()==0) && (m_core->mset.external_audio.isEmpty())) {
    if ((m_core->mset.audios.numItems()==0) && (m_core->mset.external_audio.isEmpty())) {
        audioEqualizerAct->setEnabled(false);
        muteAct->setEnabled(false);
        decVolumeAct->setEnabled(false);
        incVolumeAct->setEnabled(false);
        decAudioDelayAct->setEnabled(false);
        incAudioDelayAct->setEnabled(false);
        audioDelayAct->setEnabled(false);
        extrastereoAct->setEnabled(false);
        karaokeAct->setEnabled(false);
        volnormAct->setEnabled(false);
        earwaxAct->setEnabled(false);
        channelsGroup->setActionsEnabled(false);
        stereoGroup->setActionsEnabled(false);
    }

    // Disable video actions if it's an audio file
    if (m_core->mdat.novideo) {
        screenshotAct->setEnabled(false);
        flipAct->setEnabled(false);
        mirrorAct->setEnabled(false);
        aspectGroup->setActionsEnabled(false);
        rotateGroup->setActionsEnabled(false);
    }

//    if (pref->hwdec.startsWith("vdpau") && pref->mplayer_bin.contains("/usr/bin/mpv")) {
//        screenshotAct->setEnabled(false);
//    }

    // Disable video filters if using vdpau
    if ((pref->vdpau.disable_video_filters) && (pref->vo.startsWith("vdpau"))) {
//        screenshotAct->setEnabled(false);
        flipAct->setEnabled(false);
        mirrorAct->setEnabled(false);
        rotateGroup->setActionsEnabled(false);
        displayMessage(tr("Video filters are disabled when using vdpau"));//使用 VDPAU 时将禁用视频过滤器
    }
}

void MainWindow::disableActionsOnStop()
{
    this->setActionsEnabled(false);
    emit this->requestPlayOrPauseEnabled(true);

//    if (m_bottomController)
//        m_bottomController->permanentShow();
}

void MainWindow::togglePlayAction(Core::State state)
{
    if (state == Core::Playing) {//Stopped = 0, Playing = 1, Paused = 2
        m_bottomToolbar->onMusicPlayed();
        m_playMaskWidget->hide();
    }
    else if (state == Core::Stopped) {
        m_bottomToolbar->onMusicPause();
        m_playMaskWidget->hide();
    }
    else if (state == Core::Paused) {
        m_bottomToolbar->onMusicPause();
        if (m_mplayerWindow) {
            m_mplayerWindow->hideLogo();
        }
        m_playMaskWidget->show();
    }
    else {//Core::Buffering  缓冲的时候不改变m_playMaskWidget的显示隐藏状态
        // do nothing
        /*m_bottomToolbar->onMusicPause();
        if (m_mplayerWindow) {
            m_mplayerWindow->hideLogo();
        }*/
    }
}

void MainWindow::setJumpTexts()
{
    rewind1Act->change(tr("-%1").arg(Utils::timeForJumps(pref->seeking1)));
    rewind2Act->change(tr("-%1").arg(Utils::timeForJumps(pref->seeking2)));
    rewind3Act->change(tr("-%1").arg(Utils::timeForJumps(pref->seeking3)));

    forward1Act->change(tr("+%1").arg(Utils::timeForJumps(pref->seeking1)));
    forward2Act->change(tr("+%1").arg(Utils::timeForJumps(pref->seeking2)));
    forward3Act->change(tr("+%1").arg(Utils::timeForJumps(pref->seeking3)));
}

void MainWindow::createPreferencesDialog()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    m_prefDialog = new PreferencesDialog(m_arch, this->m_snap, this);
    //m_prefDialog->setStyleSheet("QDialog{border: 1px solid #121212;border-radius:20px;background-color:transparent;}");
    m_prefDialog->setModal(false);
    connect(m_prefDialog, SIGNAL(applied()), this, SLOT(applyNewPreferences()));
    QApplication::restoreOverrideCursor();
}

void MainWindow::createFilePropertiesDialog()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    m_propertyDialog = new FilePropertiesDialog(this);
    m_propertyDialog->setModal(false);
    connect( m_propertyDialog, SIGNAL(applied()), this, SLOT(applyFileProperties()) );
    QApplication::restoreOverrideCursor();
}

void MainWindow::setDataToFileProperties()
{
    InfoReader *i = InfoReader::obj(this->m_snap);
    i->getInfo();
    m_propertyDialog->setCodecs(i->vcList(), i->acList(), i->demuxerList());

    // Save a copy of the original values
    if (m_core->mset.original_demuxer.isEmpty())
        m_core->mset.original_demuxer = m_core->mdat.demuxer;

    if (m_core->mset.original_video_codec.isEmpty())
        m_core->mset.original_video_codec = m_core->mdat.video_codec;

    if (m_core->mset.original_audio_codec.isEmpty())
        m_core->mset.original_audio_codec = m_core->mdat.audio_codec;

    QString demuxer = m_core->mset.forced_demuxer;
    if (demuxer.isEmpty()) demuxer = m_core->mdat.demuxer;

    QString ac = m_core->mset.forced_audio_codec;
    if (ac.isEmpty()) ac = m_core->mdat.audio_codec;

    QString vc = m_core->mset.forced_video_codec;
    if (vc.isEmpty()) vc = m_core->mdat.video_codec;

    m_propertyDialog->setDemuxer(demuxer, m_core->mset.original_demuxer);
    m_propertyDialog->setAudioCodec(ac, m_core->mset.original_audio_codec);
    m_propertyDialog->setVideoCodec(vc, m_core->mset.original_video_codec);
    //m_propertyDialog->setMediaData(m_core->mdat);
    m_propertyDialog->setMediaData(m_core->mdat, m_core->mset.videos, m_core->mset.audios, m_core->mset.subs);
}

//void MainWindow::createAboutDialog()
//{
//    QApplication::setOverrideCursor(Qt::WaitCursor);
//    m_aboutDialog = new AboutDialog(this->m_snap);
//    m_aboutDialog->setModal(false);
//    QApplication::restoreOverrideCursor();
//}

void MainWindow::createHelpDialog()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    m_helpDialog = new HelpDialog(this);
    m_helpDialog->setModal(false);
    QApplication::restoreOverrideCursor();
}

void MainWindow::initRemoteControllerConnections()
{
//    connect(m_controllerWorker, SIGNAL(requestSeekForward(int)), m_core, SLOT(forward(int)));
//    connect(m_controllerWorker, SIGNAL(requestSeekRewind(int)), m_core, SLOT(rewind(int)));
//    connect(m_controllerWorker, &ControllerWorker::requestPlayPause, this, [=] () {
//        m_core->playOrPause(m_lastPlayingSeek);
//    });
//    connect(m_controllerWorker, &ControllerWorker::requestStop, m_core, &Core::stop);
}

void MainWindow::setStayOnTop(bool b)
{
    if ((b && (windowFlags() & Qt::WindowStaysOnTopHint)) || (!b && (!(windowFlags() & Qt::WindowStaysOnTopHint)))) {
        return;
    }

    m_ignoreShowHideEvents = true;

    bool visible = isVisible();
    QPoint old_pos = pos();
    if (b) {
        //TODO:在Mate系列桌面环境上，Qt5编写的程序在启动时设置置顶有效，程序运行过程中动态切换到置顶无效。后续可根据libwnck-dev中x11的接口去实现：./libwnck/window-action-menu.c:wnck_window_make_above (window)------->libwnck/window.c:_wnck_change_state
        setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
    }
    else {
        setWindowFlags(windowFlags() & ~Qt::WindowStaysOnTopHint);
    }

    move(old_pos);

    if (visible) {
        show();
    }

    m_ignoreShowHideEvents = false;
}

void MainWindow::setActionsEnabled(bool b)
{
    emit this->requestActionsEnabled(b);

    rewind1Act->setEnabled(b);
    rewind2Act->setEnabled(b);
    rewind3Act->setEnabled(b);
    forward1Act->setEnabled(b);
    forward2Act->setEnabled(b);
    forward3Act->setEnabled(b);
    gotoAct->setEnabled(b);

    // Menu Speed
    normalSpeedAct->setEnabled(b);
    halveSpeedAct->setEnabled(b);
    doubleSpeedAct->setEnabled(b);
    decSpeed10Act->setEnabled(b);
    incSpeed10Act->setEnabled(b);
    decSpeed4Act->setEnabled(b);
    incSpeed4Act->setEnabled(b);
    decSpeed1Act->setEnabled(b);
    incSpeed1Act->setEnabled(b);
    screenshotAct->setEnabled(b);
    showPropertiesAct->setEnabled(b);

    // Menu Audio
    audioEqualizerAct->setEnabled(b);
    muteAct->setEnabled(b);
    decVolumeAct->setEnabled(b);
    incVolumeAct->setEnabled(b);
    decAudioDelayAct->setEnabled(b);
    incAudioDelayAct->setEnabled(b);
    audioDelayAct->setEnabled(b);

    extrastereoAct->setEnabled(b);
    karaokeAct->setEnabled(b);
    volnormAct->setEnabled(b);
    earwaxAct->setEnabled(b);
    loadAudioAct->setEnabled(b);


    flipAct->setEnabled(b);
    mirrorAct->setEnabled(b);

    // Menu Subtitles
    loadSubsAct->setEnabled(b);
    aspectGroup->setActionsEnabled(b);
    rotateGroup->setActionsEnabled(b);
    channelsGroup->setActionsEnabled(b);

    stereoGroup->setActionsEnabled(b);
}

void MainWindow::updateRecents()
{
    recentfiles_menu->clear();
    int cur_items = 0;
    if (pref->history_recents == NULL) {
        return;
    }
    if (pref->history_recents->count() > 0) {
        for (int n=0; n < pref->history_recents->count(); n++) {
            QString i = QString::number( n+1 );
            QString fullname = pref->history_recents->item(n);
            QString filename = fullname;
            QFileInfo fi(fullname);
            //if (fi.exists()) filename = fi.fileName(); // Can be slow
            // Let's see if it looks like a file (no dvd://1 or something)
            if (fullname.indexOf(QRegExp("^.*://.*")) == -1) filename = fi.fileName();

            if (filename.size() > 85) {
                filename = filename.left(80) + "...";
            }

            QString show_name = filename;
            QString title = pref->history_recents->title(n);
            if (!title.isEmpty()) show_name = title;
//            qDebug() << "kobe recents=" << QString("%1. " + show_name ).arg( i.insert( i.size()-1, '&' ), 3, ' ' );
            QAction * a = recentfiles_menu->addAction( QString("%1. " + show_name ).arg( i.insert( i.size()-1, '&' ), 3, ' ' ));
            a->setStatusTip(fullname);
            a->setData(n);
            connect(a, SIGNAL(triggered()), this, SLOT(openRecent()));
            cur_items++;
        }
    } else {
        QAction * a = recentfiles_menu->addAction( tr("<empty>") );
        a->setEnabled(false);
    }
    recentfiles_menu->menuAction()->setVisible( cur_items > 0 );
    if (cur_items  > 0) {
        recentfiles_menu->addSeparator();
        recentfiles_menu->addAction( clearRecentsAct );
    }
}

void MainWindow::updateMuteWidgets()
{
    muteAct->setChecked((pref->global_volume ? pref->mute : m_core->mset.mute));

    bool muted = pref->global_volume ? pref->mute : m_core->mset.mute;
    m_bottomToolbar->onMutedChanged(muted, pref->volume);
}

void MainWindow::updateOnTopWidgets()
{
    // Stay on top
    onTopActionGroup->setChecked((int)pref->stay_on_top);
}

void MainWindow::updatePlayOrderWidgets()
{
    playOrderActionGroup->setChecked((int)pref->play_order);
}

void MainWindow::updateWidgets()
{
    m_centralWidget->setFocus();

    this->updateMuteWidgets();
    this->updateOnTopWidgets();

    m_bottomToolbar->setPreviewData(pref->preview_when_playing);
//	// Aspect ratio
    aspectGroup->setChecked(m_core->mset.aspect_ratio_id);
    // Rotate
    rotateGroup->setChecked(m_core->mset.rotate);
    // Flip
    flipAct->setChecked(m_core->mset.flip);
    // Mirror
    mirrorAct->setChecked(m_core->mset.mirror);
    // Audio menu
    stereoGroup->setChecked(m_core->mset.stereo_mode);
    channelsGroup->setChecked(m_core->mset.audio_use_channels);
    unloadAudioAct->setEnabled(!m_core->mset.external_audio.isEmpty());
    // Karaoke menu option
    karaokeAct->setChecked(m_core->mset.karaoke_filter);
    // Extrastereo menu option
    extrastereoAct->setChecked(m_core->mset.extrastereo_filter);
    // Volnorm menu option
    volnormAct->setChecked(m_core->mset.volnorm_filter);
    // Earwax menu option
    earwaxAct->setChecked(m_core->mset.earwax_filter);
    // Audio equalizer
    audioEqualizerAct->setChecked(audio_equalizer->isVisible());
    // Subtitle visibility
    subVisibilityAct->setChecked(pref->sub_visibility);
    // OSD
    osdGroup->setChecked(pref->osd);
    OSDFractionsAct->setChecked(pref->osd_fractions);

    if (Utils::player(pref->mplayer_bin) == Utils::MPLAYER) {
        //secondary_subtitles_track_menu->setEnabled(false);
        //frameBackStepAct->setEnabled(false);
        OSDFractionsAct->setEnabled(false);
        earwaxAct->setEnabled(false);
    }
    else {
        //karaokeAct->setEnabled(false);
    }
}

void MainWindow::updateAudioEqualizer()
{
    // Audio Equalizer
    AudioEqualizerList l = pref->global_audio_equalizer ? pref->audio_equalizer : m_core->mset.audio_equalizer;
    audio_equalizer->blockSignals(true);
    audio_equalizer->setEqualizer(l);
    audio_equalizer->blockSignals(false);
}

void MainWindow::slideEdgeWidget(QWidget *right, QRect start, QRect end, int delay, bool hide)
{
    right->show();
    QPropertyAnimation *animation = new QPropertyAnimation(right, "geometry");
    animation->setEasingCurve(QEasingCurve::InCurve);
    animation->setDuration(delay);
    animation->setStartValue(start);
    animation->setEndValue(end);
    animation->start();
    connect(animation, SIGNAL(finished()), animation, SLOT(deleteLater()));
    if (hide) {
        connect(animation, SIGNAL(finished()), right, SLOT(hide()));
    }
}

void MainWindow::hideCentralWidget()
{
    if (m_centralWidget->isVisible()) {
        if (isMaximized()) {
            m_topToolbar->updateMaxButtonStatus(false);
            this->onShowOrHideEscWidget(false);
            this->showNormal();
        }
        // Exit from fullscreen mode
        if (pref->fullscreen) { toggleFullscreen(false); update(); }

        int width = size().width();
        if (width > pref->default_size.width()) width = pref->default_size.width();
        resize( width, size().height() - m_centralWidget->size().height() );
        m_centralWidget->hide();
    }
}

void MainWindow::showAudioEqualizer(bool b)
{
    if (!b) {
        audio_equalizer->hide();
    }
    else {
        // Exit fullscreen, otherwise dialog is not visible
        exitFullscreenIfNeeded();
        audio_equalizer->show();
//        int w_x = this->frameGeometry().topLeft().x() + (audio_equalizer->width() / 2) - (400  / 2);
//        int w_y = this->frameGeometry().topLeft().y() + (audio_equalizer->height() /2) - (170  / 2);
//        audio_equalizer->move(w_x, w_y);
    }
    updateWidgets();
}

void MainWindow::showPreferencesDialog()
{
    exitFullscreenIfNeeded();
	
    if (!m_prefDialog) {
        createPreferencesDialog();
    }
    m_prefDialog->setData(pref);

    //从最新的配置文件读取快捷键并进行设置
    m_prefDialog->mod_shortcut_page()->actions_editor->clear();
    m_prefDialog->mod_shortcut_page()->actions_editor->addActions(this);

    m_prefDialog->move((width() - m_prefDialog->width()) / 2 +
                               mapToGlobal(QPoint(0, 0)).x(),
                               (window()->height() - m_prefDialog->height()) / 2 +
                               mapToGlobal(QPoint(0, 0)).y());
    m_prefDialog->show();
}

// The user has pressed OK in preferences dialog
void MainWindow::applyNewPreferences()
{
    Utils::PlayerId old_player_type = Utils::player(pref->mplayer_bin);
    m_prefDialog->getData(pref);
    m_bottomToolbar->setPreviewData(pref->preview_when_playing);
    m_mplayerWindow->activateMouseDragTracking(true/*pref->move_when_dragging*/);
//#ifndef MOUSE_GESTURES
//	m_mplayerWindow->activateMouseDragTracking(pref->drag_function == Preferences::MoveWindow);
//#endif
//	m_mplayerWindow->delayLeftClick(pref->delay_left_click);
	setJumpTexts(); // Update texts in menus
    updateWidgets(); // Update the screenshot action

    // Restart the video if needed
    if (m_prefDialog->requiresRestart())
        m_core->restart();

    // Update actions
    m_prefDialog->mod_shortcut_page()->actions_editor->applyChanges();
    saveActions();
	pref->save();

    if (old_player_type != Utils::player(pref->mplayer_bin)) {
        // Hack, simulate a change of GUI to restart the interface
        // FIXME: try to create a new Core::proc in the future
        m_lastPlayingSeek = 0;
        m_core->stop();
        if (m_prefDialog && m_prefDialog->isVisible()) {//add by kobe to hide the m_prefDialog
            m_prefDialog->accept();
        }
        emit requestGuiChanged();
    }
}


void MainWindow::showFilePropertiesDialog()
{
    exitFullscreenIfNeeded();

    if (!m_propertyDialog) {
        createFilePropertiesDialog();
    }

    setDataToFileProperties();

    m_propertyDialog->show();
}

void MainWindow::applyFileProperties()
{
	bool need_restart = false;

#undef TEST_AND_SET
#define TEST_AND_SET( Pref, Dialog ) \
    if ( Pref != Dialog ) { Pref = Dialog; need_restart = true; }

    bool demuxer_changed = false;

    QString prev_demuxer = m_core->mset.forced_demuxer;

    if (prev_demuxer != m_core->mset.forced_demuxer) {
        // Demuxer changed
        demuxer_changed = true;
        m_core->mset.current_audio_id = MediaSettings::NoneSelected;
        m_core->mset.current_subtitle_track = MediaSettings::NoneSelected;
    }

    // Restart the video to apply
    if (need_restart) {
        if (demuxer_changed) {
            m_core->reload();
        } else {
            m_core->restart();
        }
    }
}

void MainWindow::updateMediaInfo()
{
    m_mainTray->setToolTip(windowTitle());
    this->displayVideoInfo(m_core->mdat.video_width, m_core->mdat.video_height, m_core->mdat.video_fps.toDouble());
}

void MainWindow::displayVideoInfo(int width, int height, double fps)
{
    if ((width != 0) && (height != 0)) {
//		video_info_display->setText(tr("%1x%2 %3 fps", "width + height + fps").arg(width).arg(height).arg(fps));
    } else {
//		video_info_display->setText(" ");
    }

//	QString format = m_core->mdat.video_format;
//	if (!format.isEmpty() && !m_core->mdat.audio_format.isEmpty()) format += " / ";
//	format += m_core->mdat.audio_format;
//	format_info_display->setText(format.toUpper());
}

void MainWindow::displayBitrateInfo(int vbitrate, int arbitrate)
{
//	bitrate_info_display->setText(tr("V: %1 kbps A: %2 kbps").arg(vbitrate/1000).arg(arbitrate/1000));
}

void MainWindow::newMediaLoaded()
{
    QString stream_title = m_core->mdat.stream_title;
//	qDebug("MainWindow::newMediaLoaded: mdat.stream_title: %s", stream_title.toUtf8().constData());

	if (!stream_title.isEmpty()) {
        pref->history_recents->addItem( m_core->mdat.m_filename, stream_title );//20181201  m_filename
		//pref->history_recents->list();
	} else {
        pref->history_recents->addItem( m_core->mdat.m_filename );
	}
	updateRecents();

    QFileInfo fi(m_core->mdat.m_filename);//20181201  m_filename
    if (fi.exists()) {
        QString name = fi.fileName();
        m_topToolbar->onSetPlayingTitleName(name);
    }

	// Automatically add files to playlist
    if ((m_core->mdat.type == TYPE_FILE) /*&& (pref->auto_add_to_playlist)*/) {
        //qDebug("MainWindow::newMediaLoaded: playlist count: %d", playlist->count());
        QStringList files_to_add;
        if (m_playlistWidget->count() == 1) {
            files_to_add = Utils::filesForPlaylist(m_core->mdat.m_filename, pref->media_to_add_to_playlist);//20181201  m_filename
        }
        if (!files_to_add.empty()) m_playlistWidget->addFiles(files_to_add);
	}
}

void MainWindow::gotNoFileToPlay()
{
    m_playlistWidget->resumePlay();//当前播放的文件不存在时，去播放下一个
}

void MainWindow::clearMplayerLog()
{
    m_mplayerLogMsg.clear();
}

void MainWindow::recordMplayerLog(QString line)
{
    if ((line.indexOf("A:") == -1) && (line.indexOf("V:") == -1)) {
        line.append("\n");
        m_mplayerLogMsg.append(line);
    }
}

void MainWindow::clearRecentsList()
{
    MessageDialog msgDialog(this, tr("Confirm deletion - Kylin Video"), tr("Delete the list of recent files?"));
    if (msgDialog.exec() != -1) {
        if (msgDialog.standardButton(msgDialog.clickedButton()) == QMessageBox::Ok) {
            // Delete items in menu
            pref->history_recents->clear();
            updateRecents();
        }
    }
}

void MainWindow::openRecent()
{
    QAction *a = qobject_cast<QAction *> (sender());
    if (a) {
        int item = a->data().toInt();
        QString file = pref->history_recents->item(item);
        if (file.startsWith("file:")) {
            file = QUrl(file).toLocalFile();
        }

        QFileInfo fi(file);
        if (fi.exists() && (!fi.isDir())) {
            file = QFileInfo(file).absoluteFilePath();
            m_playlistWidget->addFile(file, Playlist::NoGetInfo);
            doOpen(file);
        }
        else if ((file.toLower().startsWith("http:")) || (file.toLower().startsWith("https:"))) {
            m_core->openStream(file);
        }
        else {
            this->showErrorFromPlayList(file);
        }
    }
}

void MainWindow::doOpen(QString file)
{
#ifdef PREVIEW_TEST
    if (m_previewMgr) {
        m_previewMgr->stop();
        delete m_previewMgr;
    }
#endif
	// If file is a playlist, open that playlist
	QString extension = QFileInfo(file).suffix().toLower();
    if (((extension == "m3u") || (extension== "m3u8")) && (QFile::exists(file))) {
        //playlist->load_m3u(file);
	} 
    else if (extension=="pls") {
        //playlist->load_pls(file);
	}
    else if (QFileInfo(file).isDir()) {
		openDirectory(file);
	} 
	else {
        // Let the m_core to open it, autodetecting the file type
        this->m_playlistWidget->setPlaying(file, 0);
        m_core->open(file/*, 0*/);
	}

    if (QFile::exists(file)) {
        pref->latest_dir = QFileInfo(file).absolutePath();
    }
}

void MainWindow::openFiles(QStringList files)
{
	if (files.empty()) return;

    if (files.count() == 1) {
        m_playlistWidget->addFile(files[0], Playlist::NoGetInfo);
        doOpen(files[0]);
	} else {
        m_playlistWidget->addFiles(files);
        doOpen(files[0]);
	}
}

void MainWindow::openFile()
{
	exitFullscreenIfNeeded();

	Extensions e;
    QString s = MyFileDialog::getOpenFileName(
                       this, tr("Choose a file"), pref->latest_dir, 
                       tr("Multimedia") + e.allPlayable().forFilter()+";;" +
                       tr("Video") + e.video().forFilter()+";;" +
                       tr("Audio") + e.audio().forFilter()+";;" +
                       tr("Playlists") + e.playlist().forFilter()+";;" +
                       tr("All files") +" (*.*)" );
    if (!s.isEmpty()) {
		openFile(s);
	}
}

void MainWindow::openFile(QString file)
{
   if (!file.isEmpty()) {
#ifdef PREVIEW_TEST
       if (m_previewMgr) {
           m_previewMgr->stop();
           delete m_previewMgr;
       }
#endif
		// If file is a playlist, open that playlist
		QString extension = QFileInfo(file).suffix().toLower();
        if ((extension == "m3u") || (extension == "m3u8")) {
            //playlist->load_m3u(file);
		} 
        else if (extension == "pls") {
            //playlist->load_pls(file);
		}
        else if (extension == "iso") {
            this->m_playlistWidget->setPlaying(file, 0);
            m_core->open(file/*, 0*/);//每次从头开始播放文件
		}
        else if (QFileInfo(file).isDir()) {
            openDirectory(file);
        }
        else {//打开一个本地视频文件
            this->m_playlistWidget->setPlaying(file, 0);
            m_core->openFile(file);//开始播放新打开的本地视频文件   m_core->open(file/*, 0*/);
            m_playlistWidget->addFile(file, Playlist::NoGetInfo);//将新打开的本地视频文件加入到播放列表中
		}
		if (QFile::exists(file)) pref->latest_dir = QFileInfo(file).absolutePath();
	}
}

void MainWindow::openDirectory()
{
	QString s = MyFileDialog::getExistingDirectory(
                    this, tr("Choose a directory"),
                    pref->latest_dir );

	if (!s.isEmpty()) {
		openDirectory(s);
	}
}

void MainWindow::openDirectory(QString directory)
{
    if (Utils::directoryContainsDVD(directory)) {
        m_core->open(directory);
	} 
	else {
		QFileInfo fi(directory);
        if ((fi.exists()) && (fi.isDir())) {
            m_playlistWidget->addDirectory(fi.absoluteFilePath());
            m_playlistWidget->startPlayPause();
        }
        else {
            qDebug("MainWindow::openDirectory: directory is not valid");
		}
	}
}

void MainWindow::openURL()
{
    exitFullscreenIfNeeded();

    InputURL d(this);

    // Get url from clipboard
    QString clipboard_text = QApplication::clipboard()->text();
    if ((!clipboard_text.isEmpty()) && (clipboard_text.contains("://")) /*&& (QUrl(clipboard_text).isValid())*/) {
        d.setURL(clipboard_text);
    }

    for (int n=0; n < pref->history_urls->count(); n++) {
        d.setURL(pref->history_urls->url(n) );
    }
//    int w_x = this->frameGeometry().topLeft().x() + (this->width() / 2) - (400  / 2);
//    int w_y = this->frameGeometry().topLeft().y() + (this->height() /2) - (170  / 2);
//    d.move(w_x, w_y);
    if (d.exec() == QDialog::Accepted ) {
        QString url = d.url();
        if (!url.isEmpty()) {
            pref->history_urls->addUrl(url);
            m_core->openStream(url);
        }
    }
}

void MainWindow::loadSub()
{
	exitFullscreenIfNeeded();

	Extensions e;
    QString s = MyFileDialog::getOpenFileName(
        this, tr("Choose a file"), 
	    pref->latest_dir, 
        tr("Subtitles") + e.subtitles().forFilter()+ ";;" +
        tr("All files") +" (*.*)" );

    if (!s.isEmpty()) m_core->loadSub(s);
}

void MainWindow::setInitialSubtitle(const QString & subtitle_file)
{
    m_core->setInitialSubtitle(subtitle_file);
}

void MainWindow::onSavePreviewImage(int time, QPoint pos)
{
//    QString state = m_core->stateToString().toUtf8().data();
//    if (state == "Playing" || state == "Paused") {

//    }
    if (m_core) {
        if (m_core->state() == Core::Playing || m_core->state() == Core::Paused) {

#ifdef PREVIEW_TEST
            if (!m_previewMgr) {
                //预览界面
                m_previewMgr = new PreviewManager(this);
                QObject::connect(m_previewMgr, SIGNAL(processFinished(int)),this,SLOT(onStopPreview(int)));
                QObject::connect(m_previewMgr, SIGNAL(requestPausePreview()),this,SLOT(onPausePreview()));
                m_previewMgr->setVideoWinid(m_previewDlg->getWindowID());
                m_previewMgr->play(m_core->mdat.m_filename, m_core->mdat.duration);
            }
            m_previewDlg->resize(128,105);
            m_previewDlg->move(pos.x()-64, this->m_bottomToolbar->y() -105);
            m_previewDlg->show();
            m_previewDlg->setPosition(Utils::formatTime(time));
            m_previewMgr->seek(time);
#else
            if (m_videoPreview == 0) {
                m_videoPreview = new VideoPreview(pref->mplayer_bin, 0);
            }

            if (!m_core->mdat.m_filename.isEmpty()) {//20181201  m_filename
                m_videoPreview->setVideoFile(m_core->mdat.m_filename);

                // DVD
                /*if (m_core->mdat.type==TYPE_DVD) {
                    QString file = m_core->mdat.filename;
                    DiscData disc_data = DiscName::split(file);
                    QString dvd_folder = disc_data.device;
                    if (dvd_folder.isEmpty()) dvd_folder = pref->dvd_device;
                    int dvd_title = disc_data.title;
                    file = disc_data.protocol + "://" + QString::number(dvd_title);

                    m_videoPreview->setVideoFile(file);
                    m_videoPreview->setDVDDevice(dvd_folder);
                } else {
                    m_videoPreview->setDVDDevice("");
                }*/
            }

            m_videoPreview->setMplayerPath(pref->mplayer_bin);
            bool res = m_videoPreview->createPreThumbnail(time);
            if (res) {
                if (m_bottomToolbar) {
                    m_bottomToolbar->savePreviewImageName(time, m_videoPreview->getCurrentPicture());
                }
            }
            else {
                if (m_bottomToolbar) {
                    m_bottomToolbar->savePreviewImageName(time, "");
                }
            }
#endif
        }
        else {
            if (m_bottomToolbar) {
                m_bottomToolbar->savePreviewImageName(time, "");
            }
        }
    }
}

#ifdef PREVIEW_TEST
void MainWindow::onStopPreview(int rc)
{
    Q_UNUSED(rc);

    if (m_previewMgr) {
        m_previewMgr->stop();
        m_previewMgr = NULL;
    }
}
void MainWindow::onPausePreview()
{
    if (m_previewMgr) {
        m_previewMgr->pause();
    }
}
#endif

void MainWindow::showAboutDialog()
{
//    if (!m_aboutDialog) {
//        createAboutDialog();
//    }
//    m_aboutDialog->setVersions();
//    int w_x = this->frameGeometry().topLeft().x() + (this->width() / 2) - (438  / 2);
//    int w_y = this->frameGeometry().topLeft().y() + (this->height() /2) - (320  / 2);
//    m_aboutDialog->move(w_x, w_y);
//    m_aboutDialog->show();

    AboutDialog d(this->m_snap, this);
    //d.setWindowModality(Qt::ApplicationModal);
    d.setVersions();
    d.exec();
}

void MainWindow::showHelpDialog()
{
    if (!m_helpDialog) {
        createHelpDialog();
    }
    m_helpDialog->setData(pref);
    m_helpDialog->move((width() - m_helpDialog->width()) / 2 +
                               mapToGlobal(QPoint(0, 0)).x(),
                               (window()->height() - m_helpDialog->height()) / 2 +
                               mapToGlobal(QPoint(0, 0)).y());
    m_helpDialog->show();
}

void MainWindow::showGotoDialog()
{
    TimeDialog d(this);
    d.setLabel(tr("&Jump to:"));
    d.setWindowTitle(tr("Kylin Video - Seek"));
    d.setMaximumTime((int) m_core->mdat.duration);
    d.setTime((int) m_core->mset.current_sec);
//    int w_x = this->frameGeometry().topLeft().x() + (this->width() / 2) - (380  / 2);
//    int w_y = this->frameGeometry().topLeft().y() + (this->height() /2) - (170  / 2);
//    d.move(w_x, w_y);
    if (d.exec() == QDialog::Accepted) {
        m_core->goToSec(d.time());
    }
}

void MainWindow::showAudioDelayDialog()
{
    AudioDelayDialog dlg(this);
    dlg.setDefaultValue(m_core->mset.audio_delay);
//    int w_x = this->frameGeometry().topLeft().x() + (this->width() / 2) - (380  / 2);
//    int w_y = this->frameGeometry().topLeft().y() + (this->height() /2) - (170  / 2);
//    dlg.move(w_x, w_y);
    if (dlg.exec() == QDialog::Accepted) {
        int delay = dlg.getCurrentValue();
        m_core->setAudioDelay(delay);
    }
}

void MainWindow::showSubDelayDialog()
{
	bool ok;
	#if QT_VERSION >= 0x050000
    int delay = QInputDialog::getInt(this, tr("Kylin Video - Subtitle delay"),
                                     tr("Subtitle delay (in milliseconds):"), m_core->mset.sub_delay,
                                     -3600000, 3600000, 1, &ok);
	#else
    int delay = QInputDialog::getInteger(this, tr("Kylin Video - Subtitle delay"),
                                         tr("Subtitle delay (in milliseconds):"), m_core->mset.sub_delay,
                                         -3600000, 3600000, 1, &ok);
	#endif
	if (ok) {
        m_core->setSubDelay(delay);
	}
}

void MainWindow::leftClickFunction()
{
    if (m_playlistWidget->isVisible()) {
        setPlaylistVisible(false);
    }

    QString state = m_core->stateToString().toUtf8().data();
    if (state == "Playing" || state == "Paused") {//Stopped Playing Paused
        this->onPlayPause();
    }
//	if (!pref->mouse_left_click_function.isEmpty()) {
//		processFunction(pref->mouse_left_click_function);
//	}
}

void MainWindow::rightClickFunction()
{
    showPopupMenu();
//	if (!pref->mouse_right_click_function.isEmpty()) {
//        processFunction(pref->mouse_right_click_function);
//	}
}

void MainWindow::doubleClickFunction()
{
    this->onFullScreen();
}

void MainWindow::middleClickFunction()
{
    processFunction("mute");
//	if (!pref->mouse_middle_click_function.isEmpty()) {
//		processFunction(pref->mouse_middle_click_function);
//	}
}

void MainWindow::xbutton1ClickFunction()
{
    qDebug("MainWindow::xbutton1ClickFunction");

//	if (!pref->mouse_xbutton1_click_function.isEmpty()) {
//		processFunction(pref->mouse_xbutton1_click_function);
//	}
}

void MainWindow::xbutton2ClickFunction()
{
    qDebug("MainWindow::xbutton2ClickFunction");

//	if (!pref->mouse_xbutton2_click_function.isEmpty()) {
//		processFunction(pref->mouse_xbutton2_click_function);
//	}
}

void MainWindow::processFunction(QString function)
{
    //parse args for checkable actions
    QRegExp func_rx("(.*) (true|false)");
    bool value = false;
    bool checkableFunction = false;

    if(func_rx.indexIn(function) > -1){
        function = func_rx.cap(1);
        value = (func_rx.cap(2) == "true");
        checkableFunction = true;
    } //end if


    //用QAction关联各个事件，比如全屏，创建全屏菜单项的时候设置了objectName为fullscreen，
    //然后如果双击屏幕，因为代码设置了mouse_double_click_function = "fullscreen"，所以在调用processFunction时，传递的是fullscreen，
    //后续会在ActionsEditor::findAction中寻找是否有fullscreen，如果有，则调用绑定fullscreen的菜单项的槽函数。

    QAction * action = ActionsEditor::findAction(this, function);
    if (!action) action = ActionsEditor::findAction(m_playlistWidget, function);

    if (action) {
        qDebug("MainWindow::processFunction: action found");

        if (!action->isEnabled()) {
            qDebug("MainWindow::processFunction: action is disabled, doing nothing");
            return;
        }

        if (action->isCheckable()){
            if(checkableFunction)
                action->setChecked(value);
            else
                //action->toggle();
                action->trigger();
        }else{
            action->trigger();
        }
    }
    else {
        qDebug("MainWindow::processFunction: action not found");
    }
}

void MainWindow::gotForbidden()
{
	static bool busy = false;

	if (busy) return;

	busy = true;

    QMessageBox::warning(this, tr("Error detected"),
        tr("Unfortunately this video can't be played.") +"<br>"+
        tr("The server returned '%1'").arg("403: Forbidden"));
	busy = false;
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }

    /*if (event->mimeData()->hasFormat("text/uri-list")) {
        event->setDropAction(Qt::CopyAction);
        event->acceptProposedAction();
        return;
    }

    QMainWindow::dragEnterEvent(event);*/
}

void MainWindow::dropEvent(QDropEvent *e)
{
	QStringList files;

	if (e->mimeData()->hasUrls()) {
		QList <QUrl> l = e->mimeData()->urls();
		QString s;
		for (int n=0; n < l.count(); n++) {
			if (l[n].isValid()) {
                qDebug("MainWindow::dropEvent: scheme: '%s'", l[n].scheme().toUtf8().data());
				if (l[n].scheme() == "file") 
					s = l[n].toLocalFile();
				else
					s = l[n].toString();
				/*
				qDebug(" * '%s'", l[n].toString().toUtf8().data());
				qDebug(" * '%s'", l[n].toLocalFile().toUtf8().data());
				*/
                //qDebug("MainWindow::dropEvent: file: '%s'", s.toUtf8().data());
                if (pref->m_videoMap.contains(s)) {
                    continue;
                }
				files.append(s);
			}
		}
	}

    QStringList file_list;
    QStringList dir_list;
    QString sub_file;

	if (files.count() > 0) {
		files.sort();

        Extensions ext;
        QRegExp ext_sub(ext.subtitles().forRegExp());
        ext_sub.setCaseSensitivity(Qt::CaseInsensitive);

        foreach (QString file, files) {
            QFileInfo fi(file);
            if (fi.isDir()) {
                // Folder
                dir_list << file;
            }
            else
            if (ext_sub.indexIn(fi.suffix()) > -1) {
                // Subtitle file
                if (sub_file.isEmpty()) sub_file = file;
            }
            else {
                // File (or something else)
                file_list << file;
            }
        }

        /* If only one file is dropped and it's not a folder nor a subtitle,
           open it with openFile so that it remembers the position */
        if (dir_list.isEmpty() && sub_file.isEmpty() && file_list.count() == 1 && QFile::exists(file_list[0])) {
            openFile(file_list[0]);
            return;
        }

        if (!sub_file.isEmpty()) {
            m_core->loadSub(sub_file);
            return;
        }

        if (file_list.isEmpty() && dir_list.isEmpty()) {
            return;
        }

        if (dir_list.count() == 1 && file_list.isEmpty()) {
            openDirectory(dir_list[0]);
            return;
        }

        if (pref->auto_add_to_playlist) {//true
            if (!dir_list.isEmpty()) {
                // Add directories to the playlist
                foreach(QString dir, dir_list) m_playlistWidget->addDirectory(dir);
            }

            if (!file_list.isEmpty()) {
                // Add files to the playlist
                m_playlistWidget->addFiles(files);
            }

            // All files are in the playlist, let's start to play
            m_playlistWidget->startPlayPause();
        }
        else {
            // It wasn't possible to add files to the list
            // Let's open the first directory or file
            if (!dir_list.isEmpty()) openDirectory(dir_list[0]); // Bug? This actually modifies the playlist...
            else
            if (!file_list.isEmpty()) doOpen(file_list[0]);
        }
    }



//		if (files.count() == 1) {
//			QFileInfo fi( files[0] );

//			Extensions e;
//			QRegExp ext_sub(e.subtitles().forRegExp());
//			ext_sub.setCaseSensitivity(Qt::CaseInsensitive);
//			if (ext_sub.indexIn(fi.suffix()) > -1) {
//				qDebug( "MainWindow::dropEvent: loading sub: '%s'", files[0].toUtf8().data());
//				m_core->loadSub( files[0] );
//			}
//			else
//			if (fi.isDir()) {
//				openDirectory( files[0] );
//			} else {
//                m_playlistWidget->addFile(files[0], Playlist::NoGetInfo);//20170712
//                doOpen(files[0]);
//			}
//		} else {
//			// More than one file
//            qDebug("MainWindow::dropEvent: adding files to playlist");
////            m_playlistWidget->clear();
//            m_playlistWidget->addFiles(files);
//            //openFile( files[0] );
//            m_playlistWidget->startPlayPause();
//		}
//	}
}

void MainWindow::showPopupMenu()
{
    showPopupMenu(QCursor::pos());
}

void MainWindow::showPopupMenu(QPoint p)
{
    m_mainMenu->move(p);
    m_mainMenu->show();
}

void MainWindow::playlistHasFinished()
{
    m_lastPlayingSeek = 0;
    m_core->stop();

    this->disableActionsOnStop();

    exitFullscreenOnStop();
}

void MainWindow::onCleanPlaylistFinished()
{
    //if (m_core->state() != Core::Stopped) {
        m_lastPlayingSeek = 0;
        m_core->stop();

        this->disableActionsOnStop();
    //}

    exitFullscreenOnStop();
}

void MainWindow::powerOffPC()
{
    PoweroffDialog d(this);
    if (d.exec() == QDialog::Accepted) {
        Utils::shutdown();
    }
}

void MainWindow::displayState(Core::State state)
{
    switch (state) {
        case Core::Playing://播放时开启自动隐藏标题栏和控制栏的定时器
            break;
        case Core::Paused://暂停时显示标题栏和控制栏
            if (m_bottomController)
                m_bottomController->permanentShow();
            break;
        case Core::Stopped:
        if (m_bottomController)
            m_bottomController->permanentShow();
            m_topToolbar->onSetPlayingTitleName("");
            break;
    }
}

void MainWindow::displayMessage(QString message) {
    this->showTipWidget(message);
}

void MainWindow::gotCurrentTime(double sec, bool flag)
{
	//qDebug( "DefaultGui::displayTime: %f", sec);
    QString time;
    QString all_time;
    if (flag) {
        time = "00:00:00";
        all_time = " / 00:00:00";
    }
    else {
        static int last_second = 0;
//        if (floor(sec)==last_second) return; // Update only once per second
//        last_second = (int) floor(sec);
        if (qFloor(sec) == last_second) return; // Update only once per second
        last_second = qFloor(sec);

//        time = Utils::formatTime( (int) sec ) + " / " + Utils::formatTime( (int) m_core->mdat.duration );
        time = Utils::formatTime((int) sec);
        all_time = " / " +  Utils::formatTime((int) m_core->mdat.duration);

        //qDebug( " duration: %f, current_sec: %f", m_core->mdat.duration, m_core->mset.current_sec);
    }
    if (m_bottomToolbar) {
        m_bottomToolbar->displayTime(time, all_time);
    }
}

void MainWindow::resizeMainWindow(int w, int h)
{
	// If fullscreen, don't resize!
	if (pref->fullscreen) return;

    if ((pref->resize_method==Preferences::Never) && (m_centralWidget->isVisible()) ) {
		return;
	}

    if (!m_centralWidget->isVisible()) {
        m_centralWidget->show();
    }

    QSize video_size(w,h);

    if (video_size == m_centralWidget->size()) {
        qDebug("MainWindow::resizeMainWindow: the m_centralWidget size is already the required size. Doing nothing.");
        return;
    }

    int diff_width = this->width() - m_centralWidget->width();
    int diff_height = this->height() - m_centralWidget->height();

    int new_width = w + diff_width;
    int new_height = h + diff_height;

    int minimum_width = minimumSizeHint().width();
    if (new_width < minimum_width) {
        qDebug("MainWindow::resizeMainWindow: width is too small, setting width to %d", minimum_width);
        new_width = minimum_width;
    }

    resize(new_width, new_height);
}

void MainWindow::displayGotoTime(int t)
{
    int jump_time = (int)m_core->mdat.duration * t / SEEKBAR_RESOLUTION;
    QString s = tr("Jump to %1").arg(Utils::formatTime(jump_time));
    if (pref->fullscreen) {
        m_core->displayTextOnOSD( s );
    }
}

void MainWindow::goToPosOnDragging(int t)
{
//		m_core->goToPosition(t);//m_core->goToPos(t);
}

void MainWindow::loadActions()
{
    ActionsEditor::loadFromConfig(this, settings);
//	actions_list = ActionsEditor::actionsNames(this);
}

void MainWindow::saveActions()
{
    ActionsEditor::saveToConfig(this, settings);
}

void MainWindow::moveWindow()
{
//    Display *display = QX11Info::display();
//    Atom netMoveResize = XInternAtom(display, "_NET_WM_MOVERESIZE", False);
//    XEvent xEvent;
//    const auto pos = QCursor::pos();

//    memset(&xEvent, 0, sizeof(XEvent));
//    xEvent.xclient.type = ClientMessage;
//    xEvent.xclient.message_type = netMoveResize;
//    xEvent.xclient.display = display;
//    xEvent.xclient.window = this->winId();
//    xEvent.xclient.format = 32;
//    xEvent.xclient.data.l[0] = pos.x();
//    xEvent.xclient.data.l[1] = pos.y();
//    xEvent.xclient.data.l[2] = 8;
//    xEvent.xclient.data.l[3] = Button1;
//    xEvent.xclient.data.l[4] = 0;

//    XUngrabPointer(display, CurrentTime);
//    XSendEvent(display, QX11Info::appRootWindow(QX11Info::appScreen()),
//               False, SubstructureNotifyMask | SubstructureRedirectMask,
//               &xEvent);
//    XFlush(display);
}

void MainWindow::moveWindowDiff(QPoint diff)
{
    if (pref->fullscreen || isMaximized()) {
        return;
    }

#if QT_VERSION >= 0x050000
    // Move the window with some delay.
    // Seems to work better with Qt 5

    static QPoint d;
    static int count = 0;

    d += diff;
    count++;

    if (count > 3) {
        QPoint new_pos = pos() + d;
        if (new_pos.y() < 0) new_pos.setY(0);
        if (new_pos.x() < 0) new_pos.setX(0);
        move(new_pos);
        count = 0;
        d = QPoint(0,0);

        //TODO：将会导致标题栏和播放控制栏失去焦点
        /*Display *display = QX11Info::display();
        Atom netMoveResize = XInternAtom(display, "_NET_WM_MOVERESIZE", False);
        XEvent xEvent;
        const auto pos = QCursor::pos();
        memset(&xEvent, 0, sizeof(XEvent));
        xEvent.xclient.type = ClientMessage;
        xEvent.xclient.message_type = netMoveResize;
        xEvent.xclient.display = display;
        xEvent.xclient.window = this->winId();
        xEvent.xclient.format = 32;
        xEvent.xclient.data.l[0] = pos.x();
        xEvent.xclient.data.l[1] = pos.y();
        xEvent.xclient.data.l[2] = 8;
        xEvent.xclient.data.l[3] = Button1;
        xEvent.xclient.data.l[4] = 0;

        XUngrabPointer(display, CurrentTime);
        XSendEvent(display, QX11Info::appRootWindow(QX11Info::appScreen()),
                   False, SubstructureNotifyMask | SubstructureRedirectMask,
                   &xEvent);
        XFlush(display);
        count = 0;
        d = QPoint(0,0);*/
    }
#else
    move(pos() + diff);
#endif
}

#if QT_VERSION < 0x050000
void MainWindow::showEvent( QShowEvent * ) {
    qDebug("MainWindow::showEvent");

    if (m_ignoreShowHideEvents) return;

    //qDebug("MainWindow::showEvent: pref->pause_when_hidden: %d", pref->pause_when_hidden);
    if ((pref->pause_when_hidden) && (m_core->state() == Core::Paused)) {
        qDebug("MainWindow::showEvent: unpausing");
        m_core->pause(); // Unpauses
	}
}

void MainWindow::hideEvent( QHideEvent * ) {
    qDebug("MainWindow::hideEvent");

    if (m_ignoreShowHideEvents) return;

    //qDebug("MainWindow::hideEvent: pref->pause_when_hidden: %d", pref->pause_when_hidden);
    if ((pref->pause_when_hidden) && (m_core->state() == Core::Playing)) {
        qDebug("MainWindow::hideEvent: pausing");
        m_core->pause();
	}
}
#else
// Qt 5 doesn't call showEvent / hideEvent when the window is minimized or unminimized
bool MainWindow::event(QEvent * e)
{
    bool result = QWidget::event(e);
    if ((m_ignoreShowHideEvents)/* || (!pref->pause_when_hidden)*/) {
        return result;
    }

    if (e->type() == QEvent::WindowStateChange) {//窗口的状态（最小化、最大化或全屏）发生改变（QWindowStateChangeEvent）
        if (isMinimized()) {
            was_minimized = true;
            if (m_core->state() == Core::Playing && pref->pause_when_hidden) {
                m_core->pause();
            }
        }
        else if (isMaximized()) {
            this->m_isMaximized = true;
        }
    }

    if ((e->type() == QEvent::ActivationChange) && (isActiveWindow())) {//Widget 的顶层窗口激活状态发生了变化
        m_isMaximized = isMaximized();
        if (m_oldIsMaxmized) {
            //QWindowStateChangeEvent *ce = static_cast<QWindowStateChangeEvent*>(e);
            //if (ce->oldState() & Qt::WindowMinimized) {
            //}
            if ((!isMinimized()) && (was_minimized)) {
                was_minimized = false;
                this->showMaximized();
                m_topToolbar->updateMaxButtonStatus(true);
            }
            if (m_core->state() == Core::Paused) {
                m_core->pause(); // Unpauses
            }
        }
        else {
            if ((!isMinimized()) && (was_minimized)) {
                was_minimized = false;
                if (this->m_isMaximized) {
                    m_topToolbar->updateMaxButtonStatus(true);
                }
                else {
                    m_topToolbar->updateMaxButtonStatus(false);
                }

                if (m_core->state() == Core::Paused) {
                    //qDebug("MainWindow::showEvent: unpausing");
                    m_core->pause(); // Unpauses
                }
            }
        }
    }

    return result;
}
#endif

void MainWindow::trayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch(reason) {
        case QSystemTrayIcon::Trigger:
            toggleShowOrHideMainWindow();
            break;
        case QSystemTrayIcon::DoubleClick:
            toggleShowOrHideMainWindow();
            break;
        case QSystemTrayIcon::MiddleClick:
            m_core->pause();
            break;
        default:
            break;
    }
}

void MainWindow::toggleShowOrHideMainWindow()
{
    if (m_mainTray->isVisible()) {
        if (this->isVisible()) {
            m_windowPos = pos();
            this->hide();
        } else {
            this->move(m_windowPos);
            this->show();
        }
    }
}

void MainWindow::showMainWindow()
{
    if (!this->isVisible()) {
        this->move(m_windowPos);
        this->show();
    }
}

void MainWindow::showErrorFromPlayList(QString errorStr)
{
    ErrorDialog d(this);
    d.setWindowTitle(tr("%1 Error").arg(PLAYER_NAME));
    d.setTitleText(tr("%1 Error").arg(PLAYER_NAME));
    d.setText(tr("'%1' was not found!").arg(errorStr));
    d.hideDetailBtn();
    d.exec();
}

void MainWindow::showExitCodeFromMplayer(int exit_code)
{
	if (exit_code != 255 ) {
        ErrorDialog d(this);
		d.setWindowTitle(tr("%1 Error").arg(PLAYER_NAME));
        d.setTitleText(tr("%1 Error").arg(PLAYER_NAME));
		d.setText(tr("%1 has finished unexpectedly.").arg(PLAYER_NAME) + " " + 
	              tr("Exit code: %1").arg(exit_code));
        d.setLog(m_mplayerLogMsg);
		d.exec();
    }
    this->clearMplayerLog();
}

void MainWindow::showErrorFromMplayer(QProcess::ProcessError e)
{
	if ((e == QProcess::FailedToStart) || (e == QProcess::Crashed)) {
        ErrorDialog d(this);
		d.setWindowTitle(tr("%1 Error").arg(PLAYER_NAME));
        d.setTitleText(tr("%1 Error").arg(PLAYER_NAME));
		if (e == QProcess::FailedToStart) {
			d.setText(tr("%1 failed to start.").arg(PLAYER_NAME) + " " + 
                         tr("Please check the %1 path in preferences.").arg(PLAYER_NAME));
		} else {
			d.setText(tr("%1 has crashed.").arg(PLAYER_NAME) + " " + 
                      tr("See the log for more info."));
		}
        d.setLog(m_mplayerLogMsg);
		d.exec();
	}
    this->clearMplayerLog();
}

void MainWindow::showTipWidget(const QString text)
{
    //注意：这里不能将m_tipWidget先hide，然后再show，这样会导致按快捷键时视频刷新会闪烁以下，如按快进键
    this->m_tipWidget->setText(text);
    this->m_tipWidget->show();
    if (m_tipTimer->isActive())
        m_tipTimer->stop();
    m_tipTimer->start();
}

void MainWindow::onShowOrHideEscWidget(bool b)
{
    if (m_escWidget) {
        if (b) {
            if (!m_escWidget->isVisible() && this->isFullScreen()) {
                this->m_escWidget->show();
                QTimer::singleShot(5000, this, [=] {
                    this->m_escWidget->hide();
                });
                m_maskWidget->showMask();
            }
        }
        else {
            if (m_escWidget->isVisible() && this->isFullScreen()) {
                this->m_escWidget->hide();
                m_maskWidget->hide();
            }
        }
    }
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == this->m_resizeCornerBtn) {
        if (!this->isMaximized()) {
            if (event->type() == QEvent::MouseButtonPress) {
                this->m_resizeFlag = true;
            }
            else if (event->type() == QEvent::MouseButtonRelease) {
                this->m_resizeFlag = false;
            }
        }
    }
    /*else {
        if (event->type() == QEvent::KeyPress) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
            if (keyEvent->key() == Qt::Key_Space) {
                this->leftClickFunction();
            }
        }
        return false;
    }*/

    return qApp->eventFilter(obj, event);
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        m_dragWindow = true;
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        m_dragWindow = false;
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    QMainWindow::mouseMoveEvent(event);

    if (this->isMaximized()) {
        return;
    }
    if (event->buttons() == Qt::LeftButton) {
        if (m_dragWindow) {
            moveWindow();
        }
        if (this->m_resizeFlag) {
            int targetWidth = event->globalX() - this->frameGeometry().topLeft().x();
            int targetHeight = event->globalY() - this->frameGeometry().topLeft().y();
            if(targetWidth < WINDOW_MIN_WIDTH) {
                if(targetHeight < WINDOW_MIN_HEIGHT)
                    resize(WINDOW_MIN_WIDTH, WINDOW_MIN_HEIGHT);
                else
                    resize(WINDOW_MIN_WIDTH, targetHeight);
            }
            else {
                if(targetHeight < WINDOW_MIN_HEIGHT) {
                    resize(targetWidth, WINDOW_MIN_HEIGHT);
                }
                else {
                    resize(targetWidth, targetHeight);
                }
            }
            event->accept();
        }
    }
}

void MainWindow::resizeEvent(QResizeEvent *e)
{
    QMainWindow::resizeEvent(e);
    QSize newSize = QMainWindow::size();

    int titleBarHeight = this->m_topToolbar->height();
    this->m_topToolbar->raise();
    this->m_topToolbar->move(0, 0);
    this->m_topToolbar->resize(newSize.width(), titleBarHeight);

    this->m_mplayerWindow->resize(newSize);

    this->m_playlistWidget->setFixedSize(220, newSize.height() - BOTTOM_TOOLBAR_HEIGHT - titleBarHeight);
    this->m_playlistWidget->setViewHeight();
    if (this->m_playlistWidget->isVisible()) {
        this->m_playlistWidget->hide();
        emit this->requestUpdatePlaylistBtnQssProperty(false);
    }

    this->m_bottomToolbar->raise();
    this->m_bottomToolbar->resize(newSize.width(), BOTTOM_TOOLBAR_HEIGHT);
    this->m_bottomToolbar->move(0, newSize.height() - BOTTOM_TOOLBAR_HEIGHT);
    this->m_bottomToolbar->onShowControlWidget();
    m_resizeCornerBtn->move(this->m_bottomToolbar->width()- 15, this->m_bottomToolbar->height() - 15);
}

void MainWindow::closeEvent(QCloseEvent * e)
{
    this->onCloseWindow();
    e->accept();
}

void MainWindow::paintEvent(QPaintEvent *e)
{
//    QPainter painter(this);
//    painter.drawPixmap(rect(), currentBackground);


    QBitmap bmp(this->size());
    bmp.fill();
    QPainter painter(&bmp);
    painter.setRenderHint(QPainter::Antialiasing, true);//设置反走样，避免锯齿
//    painter.setRenderHints(QPainter::Antialiasing | QPainter::HighQualityAntialiasing);
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::black);
    painter.drawRoundedRect(bmp.rect(), 6, 6);
    setMask(bmp);
}

void MainWindow::setBackgroudPixmap(QString pixmapDir)
{
    try {
        if (!pixmapDir.isEmpty()) {
            currentBackground = QPixmap(pixmapDir);
            this->update();
        }
    }
    catch (QString e) {

    }
}
