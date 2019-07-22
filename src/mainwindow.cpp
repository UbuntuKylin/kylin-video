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

#include "smplayer/mplayerwindow.h"
#include "smplayer/desktopinfo.h"
#include "smplayer/helper.h"
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
#include "smplayer/config.h"
#include "smplayer/actionseditor.h"
#include "smplayer/preferencesdialog.h"
#include "smplayer/prefshortcut.h"
#include "smplayer/myaction.h"
#include "smplayer/myactiongroup.h"
#include "smplayer/extensions.h"
#include "smplayer/version.h"
#include "smplayer/videopreview.h"
#include "smplayer/inputurl.h"


using namespace Global;


inline bool inRectCheck(QPoint point, QRect rect) {
    bool x = rect.x() <= point.x() && point.x() <= rect.x() + rect.width();
    bool y = rect.y() <= point.y() && point.y() <= rect.y() + rect.height();
    return x && y;
}

QDataStream &operator<<(QDataStream &dataStream, const VideoPtr &objectA)
{
    auto ptr = objectA.data();
    auto ptrval = reinterpret_cast<qulonglong>(ptr);//reinterpret_cast是C++里的强制类型转换符
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

MainWindow::MainWindow(QString arch_type, QString snap, QWidget* parent)
    : QMainWindow( parent)
#if QT_VERSION >= 0x050000
	, was_minimized(false)
#endif
    , m_bottomController(new BottomController(this))
    , m_mouseFilterHandler(new FilterHandler(*this, *qApp))
    , m_leftPressed(false)
    , m_maskWidget(new MaskWidget(this))
{
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);//设置窗体标题栏隐藏并设置位于顶层
    this->setMouseTracking(true);//可获取鼠标跟踪效果，界面拉伸需要这个属性
    this->setAutoFillBackground(true);
    this->setFocusPolicy(Qt::ClickFocus);//Qt::StrongFocus
    this->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    this->setMinimumSize(WINDOW_MIN_WIDTH, WINDOW_MIN_HEIGHT);
    this->resize(900, 600);
    this->setWindowTitle(tr("Kylin Video"));
    this->setWindowIcon(QIcon(":/res/kylin-video.png"));

    qRegisterMetaType<VideoPtr>();
    qRegisterMetaTypeStreamOperators<VideoPtr>();
    qRegisterMetaType<VideoPtrList>();
    qRegisterMetaType<QList<VideoData>>();

    arch = arch_type;
    m_snap = snap;
    ignore_show_hide_events = false;
    arg_close_on_finish = -1;
    arg_start_in_fullscreen = -1;
    isFinished = false;//kobe
    isPlaying = false;
    this->resizeFlag = false;

    m_currentWidget = NULL;

    popup = 0;
    main_popup = 0;
    pref_dialog = 0;
    file_dialog = 0;
    aboutDlg = 0;
    helpDlg = 0;
    video_preview = 0;
//    shortcuts_widget = 0;
    tray = 0;
    play_mask = 0;
    mplayerwindow = 0;
    m_playlistWidget = 0;
    panel = 0;
    resizeCorner = 0;
    m_topToolbar = 0;
    m_bottomToolbar = 0;
    escWidget = 0;
    tipWidget = 0;
    contentLayout = 0;

    //遮罩
//    shortcuts_widget = ShortcutsWidget::Instance();
//    shortcuts_widget->set_parent_widget(this);

	createPanel();

	createMplayerWindow();
	createCore();
	createPlaylist();

    connect(mplayerwindow, SIGNAL(wheelUp()), core, SLOT(wheelUp()));
    connect(mplayerwindow, SIGNAL(wheelDown()), core, SLOT(wheelDown()));

    createActionsAndMenus();
    createTrayActions();
    addTrayActions();
    createHiddenActions();

    setAcceptDrops(true);

    //top
    m_topToolbar = new TitleWidget(this/*mplayerwindow*/);
    m_topToolbar->setFixedHeight(TOP_TOOLBAR_HEIGHT);
    this->setMenuWidget(m_topToolbar);
    m_topToolbar->setMouseTracking(true);

    connect(m_playlistWidget, SIGNAL(sig_playing_title(QString)), m_topToolbar, SLOT(set_title_name(QString)));
    connect(this, SIGNAL(clear_playing_title()), m_topToolbar, SLOT(clear_title_name()));
    connect(m_topToolbar, SIGNAL(sig_menu()), this, SLOT(slot_menu()));
    connect(m_topToolbar, SIGNAL(sig_min()), this, SLOT(slot_min()));
    connect(m_topToolbar, SIGNAL(sig_close()), this, SLOT(slot_close()));
    connect(m_topToolbar, SIGNAL(requestMaxWindow(bool)), this, SLOT(onResponseMaxWindow(bool)));
    connect(m_topToolbar, SIGNAL(mouseMovedDiff(QPoint)), this, SLOT(moveWindowDiff(QPoint)), Qt::QueuedConnection );//kobe 0524

    //bottom
    m_bottomToolbar = new BottomWidget(this/*mplayerwindow*/);
    m_bottomToolbar->setFixedHeight(BOTTOM_TOOLBAR_HEIGHT);
    m_bottomToolbar->setLayoutDirection(Qt::LeftToRight);

    connect(m_bottomToolbar, SIGNAL(sig_resize_corner()), this, SLOT(slot_resize_corner()));
    connect(m_bottomToolbar, SIGNAL(volumeChanged(int)), core, SLOT(setVolume(int)));
    connect(core, SIGNAL(volumeChanged(int)), m_bottomToolbar, SIGNAL(valueChanged(int)));
    connect(m_bottomToolbar, SIGNAL(toggleFullScreen()), this, SLOT(slot_set_fullscreen()));//0621
    connect(m_bottomToolbar, SIGNAL(togglePlaylist()), this, SLOT(slot_playlist()));
    connect(this, SIGNAL(timeChanged(QString, QString)),m_bottomToolbar, SLOT(displayTime(QString, QString)));

    connect(m_bottomToolbar, &BottomWidget::signal_stop, this, [=] {
        core->stop();
        disconnect(m_mouseFilterHandler, SIGNAL(mouseMoved()), m_bottomController, SLOT(temporaryShow()));
        m_bottomController->permanentShow();
    });
    connect(m_bottomToolbar, SIGNAL(signal_prev()), m_playlistWidget, SLOT(playPrev()));

    connect(m_bottomToolbar, &BottomWidget::signal_play_pause_status, this, &MainWindow::startPlayPause);
    connect(m_bottomToolbar, &BottomWidget::requestTemporaryShow, this, [=] {
        m_bottomController->temporaryShow();
    });

    connect(m_bottomToolbar, SIGNAL(signal_next()), m_playlistWidget, SLOT(playNext()));
    connect(m_bottomToolbar, SIGNAL(signal_open_file()), this, SLOT(openFile()));
    connect(m_bottomToolbar, SIGNAL(signal_mute()), this, SLOT(slot_mute()));
    connect(this, SIGNAL(sigActionsEnabled(bool)), m_bottomToolbar, SLOT(setActionsEnabled(bool)));
    connect(this, SIGNAL(setPlayOrPauseEnabled(bool)), m_bottomToolbar, SLOT(setPlayOrPauseEnabled(bool)));
    connect(this, SIGNAL(setStopEnabled(bool)), m_bottomToolbar, SLOT(setStopEnabled(bool)));
    connect(this, SIGNAL(change_playlist_btn_status(bool)), m_bottomToolbar, SLOT(slot_playlist_btn_status(bool)));
    //progress
    connect(m_bottomToolbar, SIGNAL(posChanged(int)), core, SLOT(goToPosition(int)));
    connect(m_bottomToolbar, SIGNAL(delayedDraggingPos(int)), this, SLOT(goToPosOnDragging(int)));

    connect(core, SIGNAL(positionChanged(int)), m_bottomToolbar, SLOT(setPos(int)));
    connect(m_bottomToolbar, SIGNAL(draggingPos(int)), this, SLOT(displayGotoTime(int)));

    connect(m_bottomToolbar, SIGNAL(wheelUp()), core, SLOT(wheelUp()));
    connect(m_bottomToolbar, SIGNAL(wheelDown()), core, SLOT(wheelDown()));
    connect(m_bottomToolbar, SIGNAL(mouseMovedDiff(QPoint)), this, SLOT(moveWindowDiff(QPoint)), Qt::QueuedConnection );//kobe 0524
    connect(core, SIGNAL(newDuration(double)), m_bottomToolbar, SLOT(setDuration(double)));
    connect(m_bottomToolbar, SIGNAL(sig_show_or_hide_esc(bool)), this, SLOT(showOrHideEscWidget(bool)));
    connect(m_playlistWidget, SIGNAL(update_playlist_count(int)), m_bottomToolbar, SLOT(update_playlist_count_label(int)));
    connect(m_bottomToolbar, SIGNAL(need_to_save_pre_image(int)), this, SLOT(ready_save_pre_image(int)));
    connect(this, SIGNAL(send_save_preview_image_name(int,QString)), m_bottomToolbar, SIGNAL(send_save_preview_image_name(int,QString)));
    resizeCorner = new QPushButton(m_bottomToolbar);
    resizeCorner->setFocusPolicy(Qt::NoFocus);
    resizeCorner->setStyleSheet("QPushButton{background-image:url(':/res/dragbar_normal.png');border:0px;}QPushButton:hover{background-image:url(':/res/dragbar_normal.png')}QPushButton:pressed{background-image:url(':/res/dragbar_normal.png')}");
    resizeCorner->setFixedSize(15, 15);
    resizeCorner->setCursor(Qt::SizeFDiagCursor);
    resizeCorner->move(m_bottomToolbar->width()-15, m_bottomToolbar->height()-15);
    resizeCorner->installEventFilter(this);
    resizeCorner->raise();

    connect(m_bottomController, &BottomController::requestShow, this, [=] {
        m_bottomToolbar->show();
        m_topToolbar->show();
    });

    connect(m_bottomController, &BottomController::requestHide, this, [=] {
        m_bottomToolbar->hide();
        m_topToolbar->hide();
    });

//    m_coverWidget = new CoverWidget(this);
//    m_coverWidget->setContentsMargins(0, 0, 0, 0);

    mainwindow_pos = pos();

    tipWidget = new TipWidget("Hello, Kylin!", this);
    tipWidget->setFixedHeight(30);
    tipWidget->move(10, TOP_TOOLBAR_HEIGHT);
    tipWidget->hide();

    contentLayout = new QStackedLayout(panel);
    contentLayout->setContentsMargins(20, 20, 20, 20);
    contentLayout->setMargin(1);
    contentLayout->setSpacing(0);
    contentLayout->addWidget(m_topToolbar);
    contentLayout->addWidget(mplayerwindow);
//    contentLayout->addWidget(m_coverWidget);
    contentLayout->addWidget(m_bottomToolbar);

    m_topToolbar->show();
    mplayerwindow->show();
    m_bottomToolbar->show();
    m_bottomToolbar->setFocus();

    m_currentWidget = mplayerwindow;

    this->setActionsEnabled(false);
    if (m_playlistWidget->count() > 0) {
        emit this->setPlayOrPauseEnabled(true);
        m_bottomToolbar->update_playlist_count_label(m_playlistWidget->count());
    }
    else {
        m_bottomToolbar->update_playlist_count_label(0);
    }

    int windowWidth = QApplication::desktop()->screenGeometry(0).width();
    int windowHeight = QApplication::desktop()->screenGeometry(0).height();
    this->move((windowWidth - this->width()) / 2,(windowHeight - this->height()) / 2);

    this->reset_mute_button();

    escWidget = new EscTip(this);
    escWidget->setFixedSize(440, 64);
    escWidget->move((windowWidth - escWidget->width()) / 2,(windowHeight - escWidget->height()) / 2);
    escWidget->hide();

    play_mask = new PlayMask(mplayerwindow);
    mplayerwindow->setCornerWidget(play_mask);
    play_mask->hide();
    connect(play_mask, &PlayMask::signal_play_continue, this, &MainWindow::startPlayPause);

    tip_timer = new QTimer(this);
    connect(tip_timer, SIGNAL(timeout()), this, SLOT(hideTipWidget()));
    tip_timer->setInterval(2000);

    initializeGui();

    connect( this, SIGNAL(videoInfoChanged(int,int,double)), this, SLOT(displayVideoInfo(int,int,double)) );
    connect( core, SIGNAL(bitrateChanged(int,int)), this, SLOT(displayBitrateInfo(int,int)));


//    QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect(this);
//    effect->setBlurRadius(50);
//    effect->setColor(Qt::red);
//    effect->setOffset(0);
//    this->setGraphicsEffect(effect);
}

void MainWindow::bindThreadWorker(InfoWorker *worker)
{
    connect(m_playlistWidget, SIGNAL(requestGetMediaInfo(QStringList)), worker, SLOT(onGetMediaInfo(QStringList)));
    connect(worker, SIGNAL(meidaFilesAdded(VideoPtrList)), this, SLOT(onMeidaFilesAdded(VideoPtrList)));
}

void MainWindow::onMeidaFilesAdded(const VideoPtrList medialist)
{
    if (medialist.length() == 0) {
        QString message = QString(tr("Failed to add files!"));
        this->displayMessage(message);
    }
    else {
        this->m_playlistWidget->onPlayListChanged(medialist);
    }
}

void MainWindow::parseArguments()
{
    QStringList args = qApp->arguments();
    int index = args.indexOf("--snap");
    qDebug() << index << args;
}

void MainWindow::startPlayPause()
{
    m_topToolbar->show();
    m_bottomToolbar->show();
    connect(m_mouseFilterHandler, SIGNAL(mouseMoved()), m_bottomController, SLOT(temporaryShow()));
    core->play_or_pause();
}

void MainWindow::setTransparent(bool transparent) {
    if (transparent) {
        setAttribute(Qt::WA_TranslucentBackground);
        setWindowFlags(windowFlags() | Qt::FramelessWindowHint|Qt::X11BypassWindowManagerHint);
        set_widget_opacity(0.5);
    }
    else {
        setAttribute(Qt::WA_TranslucentBackground,false);
        setWindowFlags(windowFlags() & ~Qt::FramelessWindowHint);
    }
}

void MainWindow::set_widget_opacity(const float &opacity) {
    QGraphicsOpacityEffect* opacityEffect = new QGraphicsOpacityEffect;
    this->setGraphicsEffect(opacityEffect);
    opacityEffect->setOpacity(opacity);
}

//void MainWindow::showShortcuts()
//{
//    shortcuts_widget->setPrefData(pref);
//    shortcuts_widget->show();
//    shortcuts_widget->restart_timer();
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
//        if (isFullScreen()) {//201810

//        }
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

void MainWindow::slot_mute(/*bool b*/) {
    bool muted = core->getMute();
    core->mute(!muted);
    if (muted && core->getVolumn() <= 0) {
        core->setVolume(50, true);
    }
    updateWidgets();
}

void MainWindow::reset_mute_button() {
    bool muted = core->getMute();
    m_bottomToolbar->onMutedChanged(muted, core->getVolumn());//0519
}

void MainWindow::onResponseMaxWindow(bool b) {
//    this->toggleFullscreen(b);

    if (!this->isMaximized()) {
        m_topToolbar->updateMaxButtonStatus(true);
        this->showMaximized();
    }
    else {
        m_topToolbar->updateMaxButtonStatus(false);
        this->showNormal();
    }
}

void MainWindow::slot_min() {
    /*if( windowState() != Qt::WindowMinimized ){
        setWindowState( Qt::WindowMinimized );
    }*/
    this->showMinimized();
}

void MainWindow::slot_menu() {
    QPoint p = rect().topRight();
    p.setX(p.x() - 38*4);
    p.setY(p.y() + 36);
    main_popup->exec(this->mapToGlobal(p));
}

void MainWindow::slot_close() {
    this->quit();
}

void MainWindow::initializeGui() {
    changeStayOnTop(pref->stay_on_top);
    changePlayOrder(pref->play_order);
	updateRecents();
    updateWidgets();

    // Call loadActions() outside initialization of the class.
    // Otherwise DefaultGui (and other subclasses) doesn't exist, and
    // its actions are not loaded
    QTimer::singleShot(20, this, SLOT(loadActions()));
}

#ifdef SINGLE_INSTANCE
void MainWindow::handleMessageFromOtherInstances(const QString& message) {
	int pos = message.indexOf(' ');
	if (pos > -1) {
		QString command = message.left(pos);
		QString arg = message.mid(pos+1);

		if (command == "open_file") {
			emit openFileRequested();
            doOpen(arg);
		} 
		else
		if (command == "open_files") {
			QStringList file_list = arg.split(" <<sep>> ");
			emit openFileRequested();
			openFiles(file_list);
		}
		else
		if (command == "add_to_playlist") {
//			QStringList file_list = arg.split(" <<sep>> ");
			/* if (core->state() == Core::Stopped) { emit openFileRequested(); } */
//			playlist->addFiles(file_list);
		}
		else
		if (command == "media_title") {
			QStringList list = arg.split(" <<sep>> ");
			core->addForcedTitle(list[0], list[1]);
		}
        else
        if (command == "action") {
            processFunction(arg);
        }
		else
		if (command == "load_sub") {
			setInitialSubtitle(arg);
			if (core->state() != Core::Stopped) {
				core->loadSub(arg);
			}
		}
	}
}
#endif

MainWindow::~MainWindow()
{
    this->clearMplayerLog();
    if (core) {
        delete core; // delete before mplayerwindow, otherwise, segfault...
        core = nullptr;
    }
    if (play_mask) {
        delete play_mask;
        play_mask = 0;
    }
    if (escWidget) {
        delete escWidget;
        escWidget = 0;
    }
    if (tipWidget) {
        delete tipWidget;
        tipWidget = 0;
    }
    if (mplayerwindow) {
        delete mplayerwindow;
        mplayerwindow = 0;
    }
    if (m_playlistWidget) {
        delete m_playlistWidget;
        m_playlistWidget = 0;
    }
    if (video_preview) {
        delete video_preview;
        video_preview = 0;
    }
//    if (shortcuts_widget) {
//        delete shortcuts_widget;
//        shortcuts_widget = 0;
//    }
    if (pref_dialog) {
        delete pref_dialog;
        pref_dialog = 0;
    }
    if (file_dialog) {
        delete file_dialog;
        file_dialog = 0;
    }
    if (aboutDlg) {
        delete aboutDlg;
        aboutDlg = 0;
    }
    if (helpDlg) {
        delete helpDlg;
        helpDlg = 0;
    }
    if (tray) {
        delete tray;
        tray = 0;
    }
    if (popup) {
        delete popup;
        popup = 0;
    }
    if (main_popup) {
        delete main_popup;
        main_popup = 0;
    }
    if (resizeCorner) {
        delete resizeCorner;
        resizeCorner = 0;
    }
    if (m_topToolbar) {
        delete m_topToolbar;
        m_topToolbar = 0;
    }
    if (m_bottomToolbar) {
        delete m_bottomToolbar;
        m_bottomToolbar = 0;
    }
    if (contentLayout) {
        delete contentLayout;
        contentLayout = 0;
    }
    if (panel) {
        delete panel;
        panel = 0;
    }
    if (tip_timer != NULL) {
        disconnect(tip_timer,SIGNAL(timeout()),this,SLOT(hideTipWidget()));
        if(tip_timer->isActive()) {
            tip_timer->stop();
        }
        delete tip_timer;
        tip_timer = NULL;
    }
}

void MainWindow::createActionsAndMenus() {
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

    clearRecentsAct = new MyAction(this, "clear_recents");
    connect(clearRecentsAct, SIGNAL(triggered()), this, SLOT(clearRecentsList()));
    clearRecentsAct->change(QPixmap(":/res/delete_normal.png"), tr("&Clear"));

    recentfiles_menu = new QMenu(this);
    recentfiles_menu->addSeparator();
    recentfiles_menu->addAction(clearRecentsAct);
    recentfiles_menu->menuAction()->setText(tr("Recent files"));
//    recentfiles_menu->menuAction()->setIcon(QPixmap(":/res/delete.png"));

    playMenu = new QMenu(this);
    playMenu->menuAction()->setText(tr("Play control"));
    control_menu = new QMenu(this);
    control_menu->menuAction()->setText(tr("Forward and rewind"));//快进快退
//    control_menu->menuAction()->setIcon(QPixmap(":/res/speed.png"));
    control_menu->menuAction()->setObjectName("control_menu");
    rewind1Act = new MyAction( Qt::Key_Left, this, "rewind1");
    rewind1Act->addShortcut(QKeySequence("Shift+Ctrl+B")); // MCE remote key
    connect(rewind1Act, SIGNAL(triggered()), core, SLOT(srewind()));
    rewind2Act = new MyAction( Qt::Key_Down, this, "rewind2");
    connect(rewind2Act, SIGNAL(triggered()), core, SLOT(rewind()));
    rewind3Act = new MyAction( Qt::Key_PageDown, this, "rewind3");
    connect(rewind3Act, SIGNAL(triggered()), core, SLOT(fastrewind()));
    forward1Act = new MyAction( Qt::Key_Right, this, "forward1");
    forward1Act->addShortcut(QKeySequence("Shift+Ctrl+F")); // MCE remote key
    connect(forward1Act, SIGNAL(triggered()), core, SLOT(sforward()));
    forward2Act = new MyAction( Qt::Key_Up, this, "forward2");
    connect(forward2Act, SIGNAL(triggered()), core, SLOT(forward()));
    forward3Act = new MyAction( Qt::Key_PageUp, this, "forward3" );
    connect(forward3Act, SIGNAL(triggered()), core, SLOT(fastforward()));

//    setAMarkerAct = new MyAction( this, "set_a_marker" );
//	connect( setAMarkerAct, SIGNAL(triggered()),
//             core, SLOT(setAMarker()) );

//	setBMarkerAct = new MyAction( this, "set_b_marker" );
//	connect( setBMarkerAct, SIGNAL(triggered()),
//             core, SLOT(setBMarker()) );

//	clearABMarkersAct = new MyAction( this, "clear_ab_markers" );
//	connect( clearABMarkersAct, SIGNAL(triggered()),
//             core, SLOT(clearABMarkers()) );

//    repeatAct = new MyAction( this, "repeat" );
//	repeatAct->setCheckable( true );
//	connect( repeatAct, SIGNAL(toggled(bool)),
//             core, SLOT(toggleRepeat(bool)) );

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
    connect(normalSpeedAct, SIGNAL(triggered()), core, SLOT(normalSpeed()));
    halveSpeedAct = new MyAction(Qt::Key_BraceLeft, this, "halve_speed");
    connect(halveSpeedAct, SIGNAL(triggered()), core, SLOT(halveSpeed()));
    doubleSpeedAct = new MyAction(Qt::Key_BraceRight, this, "double_speed");
    connect(doubleSpeedAct, SIGNAL(triggered()), core, SLOT(doubleSpeed()));
    decSpeed10Act = new MyAction(Qt::Key_BracketLeft, this, "dec_speed");
    connect(decSpeed10Act, SIGNAL(triggered()), core, SLOT(decSpeed10()));
    incSpeed10Act = new MyAction(Qt::Key_BracketRight, this, "inc_speed");
    connect(incSpeed10Act, SIGNAL(triggered()), core, SLOT(incSpeed10()));
    decSpeed4Act = new MyAction(this, "dec_speed_4");
    connect(decSpeed4Act, SIGNAL(triggered()), core, SLOT(decSpeed4()));
    incSpeed4Act = new MyAction(this, "inc_speed_4");
    connect(incSpeed4Act, SIGNAL(triggered()), core, SLOT(incSpeed4()));
    decSpeed1Act = new MyAction(this, "dec_speed_1");
    connect(decSpeed1Act, SIGNAL(triggered()), core, SLOT(decSpeed1()));
    incSpeed1Act = new MyAction(this, "inc_speed_1");
    connect(incSpeed1Act, SIGNAL(triggered()), core, SLOT(incSpeed1()));
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
    aspect235Act = new MyActionGroupItem(this, aspectGroup, "aspect_2.35:1", MediaSettings::Aspect235 );
    {
        QAction * sep = new QAction(aspectGroup);
        sep->setSeparator(true);
    }
    aspectNoneAct = new MyActionGroupItem(this, aspectGroup, "aspect_none", MediaSettings::AspectNone);
    connect(aspectGroup, SIGNAL(activated(int)), core, SLOT(changeAspectRatio(int)));
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

    // Rotate
    rotateGroup = new MyActionGroup(this);
    rotateNoneAct = new MyActionGroupItem(this, rotateGroup, "rotate_none", MediaSettings::NoRotate);
    rotateClockwiseFlipAct = new MyActionGroupItem(this, rotateGroup, "rotate_clockwise_flip", MediaSettings::Clockwise_flip);
    rotateClockwiseAct = new MyActionGroupItem(this, rotateGroup, "rotate_clockwise", MediaSettings::Clockwise);
    rotateCounterclockwiseAct = new MyActionGroupItem(this, rotateGroup, "rotate_counterclockwise", MediaSettings::Counterclockwise);
    rotateCounterclockwiseFlipAct = new MyActionGroupItem(this, rotateGroup, "rotate_counterclockwise_flip", MediaSettings::Counterclockwise_flip);
    connect(rotateGroup, SIGNAL(activated(int)), core, SLOT(changeRotate(int)));
    rotateNoneAct->change(tr("&Off"));
    rotateClockwiseFlipAct->change(tr("&Rotate by 90 degrees clockwise and flip"));
    rotateClockwiseAct->change(tr("Rotate by 90 degrees &clockwise"));
    rotateCounterclockwiseAct->change(tr("Rotate by 90 degrees counterclock&wise"));
    rotateCounterclockwiseFlipAct->change(tr("Rotate by 90 degrees counterclockwise and &flip"));

    flipAct = new MyAction(this, "flip");
    flipAct->setCheckable(true);
    connect(flipAct, SIGNAL(toggled(bool)), core, SLOT(toggleFlip(bool)));
    flipAct->change(tr("Fli&p image"));

    mirrorAct = new MyAction(this, "mirror");
    mirrorAct->setCheckable(true);
    connect(mirrorAct, SIGNAL(toggled(bool)), core, SLOT(toggleMirror(bool)));
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

//    shortcutsAct = new MyAction(QKeySequence("Shift+?"), this, "Shortcuts");//快捷键  Qt::Key_Question
////    shortcutsAct->addShortcut(QKeySequence("Shift+Ctrl+?"));
//    connect(shortcutsAct, SIGNAL(triggered()), this, SLOT(showShortcuts()));
//    shortcutsAct->change(tr("Shortcuts"));

    // Single screenshot
    screenshotAct = new MyAction(Qt::Key_S, this, "screenshot");//屏幕截图
    connect(screenshotAct, SIGNAL(triggered()), core, SLOT(screenshot()));
    screenshotAct->change(Images::icon("screenshot_normal"), tr("&Screenshot"));

    /*screenshotWithSubsAct = new MyAction( QKeySequence("Ctrl+Shift+S"), this, "screenshot_with_subtitles" );
    connect( screenshotWithSubsAct, SIGNAL(triggered()),
             core, SLOT(screenshotWithSubtitles()) );

    screenshotWithNoSubsAct = new MyAction( QKeySequence("Ctrl+Alt+S"), this, "screenshot_without_subtitles" );
    connect( screenshotWithNoSubsAct, SIGNAL(triggered()),
             core, SLOT(screenshotWithoutSubtitles()) );

    // Multiple screenshots
    screenshotsAct = new MyAction( QKeySequence("Shift+D"), this, "multiple_screenshots" );
    connect( screenshotsAct, SIGNAL(triggered()),
             core, SLOT(screenshots()) );*/

    // On Top
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

    // Audio channels
//    channelsDefaultAct = new MyActionGroupItem(this, channelsGroup, "channels_default", MediaSettings::ChDefault);
    channelsGroup = new MyActionGroup(this);
    channelsStereoAct = new MyActionGroupItem(this, channelsGroup, "channels_stereo", MediaSettings::ChStereo);
    channelsSurroundAct = new MyActionGroupItem(this, channelsGroup, "channels_surround", MediaSettings::ChSurround);
    channelsFull51Act = new MyActionGroupItem(this, channelsGroup, "channels_ful51", MediaSettings::ChFull51);
    channelsFull61Act = new MyActionGroupItem(this, channelsGroup, "channels_ful61", MediaSettings::ChFull61);
    channelsFull71Act = new MyActionGroupItem(this, channelsGroup, "channels_ful71", MediaSettings::ChFull71);
    connect(channelsGroup, SIGNAL(activated(int)), core, SLOT(setAudioChannels(int)));
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
    connect(muteAct, SIGNAL(toggled(bool)), core, SLOT(mute(bool)));
    QIcon icset(Images::icon("volume_low_normal"));
    icset.addPixmap(Images::icon("volume_mute_normal"), QIcon::Normal, QIcon::On);
    muteAct->change(icset, tr("&Mute"));

    decVolumeAct = new MyAction(Qt::Key_9, this, "dec_volume");
    connect(decVolumeAct, SIGNAL(triggered()), core, SLOT(decVolume()));
    decVolumeAct->change(tr("Volume -"));
    incVolumeAct = new MyAction(Qt::Key_0, this, "inc_volume");
    connect(incVolumeAct, SIGNAL(triggered()), core, SLOT(incVolume()));
    incVolumeAct->change(tr("Volume +") );
    decAudioDelayAct = new MyAction(Qt::Key_Minus, this, "dec_audio_delay");
    connect( decAudioDelayAct, SIGNAL(triggered()), core, SLOT(decAudioDelay()));
    decAudioDelayAct->change(tr("Delay -"));
    incAudioDelayAct = new MyAction(Qt::Key_Plus, this, "inc_audio_delay");
    connect(incAudioDelayAct, SIGNAL(triggered()), core, SLOT(incAudioDelay()));
    incAudioDelayAct->change( tr("Delay +"));
    audioDelayAct = new MyAction(Qt::Key_Y, this, "audio_delay");
    connect(audioDelayAct, SIGNAL(triggered()), this, SLOT(showAudioDelayDialog()));
    audioDelayAct->change(tr("Set dela&y..."));

    // Stereo mode
    stereoGroup = new MyActionGroup(this);
    stereoAct = new MyActionGroupItem(this, stereoGroup, "stereo", MediaSettings::Stereo);
    leftChannelAct = new MyActionGroupItem(this, stereoGroup, "left_channel", MediaSettings::Left);
    rightChannelAct = new MyActionGroupItem(this, stereoGroup, "right_channel", MediaSettings::Right);
    monoAct = new MyActionGroupItem(this, stereoGroup, "mono", MediaSettings::Mono);
    reverseAct = new MyActionGroupItem(this, stereoGroup, "reverse_channels", MediaSettings::Reverse);
    connect(stereoGroup, SIGNAL(activated(int)), core, SLOT(setStereoMode(int)));
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


    // Submenu Filters
    /*extrastereoAct->change( tr("&Extrastereo") );
    karaokeAct->change( tr("&Karaoke") );
    volnormAct->change( tr("Volume &normalization") );
    earwaxAct->change( tr("&Headphone optimization") + " (earwax)" );*/


    subtitlesMenu = new QMenu(this);
    subtitlesMenu->menuAction()->setText( tr("Subtitles") );
//    subtitlesMenuAct->setIcon(Images::icon("subtitles_menu"));
    loadSubsAct = new MyAction(this, "load_subs" );
    connect(loadSubsAct, SIGNAL(triggered()), this, SLOT(loadSub()));
    loadSubsAct->change(tr("Load..."));
//    subVisibilityAct = new MyAction(Qt::Key_V, this, "subtitle_visibility");
    subVisibilityAct = new MyAction(this, "sub_visibility");
    subVisibilityAct->setCheckable(true);
    connect(subVisibilityAct, SIGNAL(toggled(bool)), core, SLOT(changeSubVisibility(bool)));
    subVisibilityAct->change(tr("Subtitle &visibility"));
    subtitlesMenu->addAction(loadSubsAct);
    subtitlesMenu->addAction(subVisibilityAct);

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
    connect(quitAct, SIGNAL(triggered()), this, SLOT(slot_close()));

    //20181120
    //showFilenameAct = new MyAction(Qt::SHIFT | Qt::Key_O, this, "show_filename_osd");
    //connect( showFilenameAct, SIGNAL(triggered()), core, SLOT(showFilenameOnOSD()) );
//    showFilenameAct->change( tr("Show filename on OSD") );//在OSD中显示文件名

    showMediaInfoAct = new MyAction(Qt::SHIFT | Qt::Key_I, this, "show_info_osd");
    connect( showMediaInfoAct, SIGNAL(triggered()), core, SLOT(showMediaInfoOnOSD()) );
    showMediaInfoAct->change( tr("Show &info on OSD") );

//    showTimeAct = new MyAction(Qt::Key_I, this, "show_time");
//    connect( showTimeAct, SIGNAL(triggered()), core, SLOT(showTimeOnOSD()) );
//    showTimeAct->change( tr("Show playback time on OSD") );//在 OSD 上显示播放时间

    // OSD
    incOSDScaleAct = new MyAction(Qt::SHIFT | Qt::Key_U, this, "inc_osd_scale");
    connect(incOSDScaleAct, SIGNAL(triggered()), core, SLOT(incOSDScale()));
    incOSDScaleAct->change(tr("Size &+"));

    decOSDScaleAct = new MyAction(Qt::SHIFT | Qt::Key_Y, this, "dec_osd_scale");
    connect(decOSDScaleAct, SIGNAL(triggered()), core, SLOT(decOSDScale()));
    decOSDScaleAct->change(tr("Size &-"));

//#ifdef MPV_SUPPORT
    OSDFractionsAct = new MyAction(this, "osd_fractions");
    OSDFractionsAct->change(tr("Show times with &milliseconds"));
    OSDFractionsAct->setCheckable(true);
    connect(OSDFractionsAct, SIGNAL(toggled(bool)), core, SLOT(setOSDFractions(bool)));
//#endif
    osdGroup = new MyActionGroup(this);
    osdNoneAct = new MyActionGroupItem(this, osdGroup, "osd_none", Preferences::None);
    osdSeekAct = new MyActionGroupItem(this, osdGroup, "osd_seek", Preferences::Seek);
    osdTimerAct = new MyActionGroupItem(this, osdGroup, "osd_timer", Preferences::SeekTimer);
    osdTotalAct = new MyActionGroupItem(this, osdGroup, "osd_total", Preferences::SeekTimerTotal);
    connect( osdGroup, SIGNAL(activated(int)), core, SLOT(changeOSD(int)) );
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
//#ifdef MPV_SUPPORT
    osd_menu->addSeparator();
    osd_menu->addAction(OSDFractionsAct);
//#endif
    // Menu Options
    osd_menu->menuAction()->setText( tr("&OSD") );
    osd_menu->menuAction()->setIcon( Images::icon("osd") );


    this->setJumpTexts();

    // POPUP MENU
    if (!popup)
        popup = new QMenu(this);
    else
        popup->clear();
    popup->addMenu(openMenu);
    popup->addMenu(recentfiles_menu);
    popup->addMenu(ontop_menu);
    popup->addMenu(playMenu);
    popup->addMenu(play_order_menu);
    popup->addMenu(aspect_menu);
    popup->addMenu(rotate_flip_menu);
    popup->addMenu(audioMenu);
    popup->addMenu(subtitlesMenu);
    popup->addMenu(osd_menu);
//    popup->addAction(shortcutsAct);
    popup->addAction(screenshotAct);
    popup->addAction(showPreferencesAct);
    popup->addAction(showPropertiesAct);
    popup->addAction(aboutAct);

    if (!main_popup)
        main_popup = new QMenu(this);
    else
        main_popup->clear();
    main_popup->addAction(openFileAct);
    main_popup->addAction(screenshotAct);
    main_popup->addAction(showPreferencesAct);
    main_popup->addAction(helpAct);
    main_popup->addAction(aboutAct);
    main_popup->addSeparator();
    main_popup->addAction(quitAct);
}

void MainWindow::createTrayActions() {
    tray = new QSystemTrayIcon(Images::icon("logo", 22), this);
    tray->setToolTip("Kylin Video");
    tray->show();
    connect(tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayIconActivated(QSystemTrayIcon::ActivationReason)));
    tray->setIcon(Images::icon("logo", 22));
    tray_menu = new QMenu(this);

    action_show = new MyAction(this, "open_window");
    action_show->change(Images::icon("open_window_normal"), tr("Open Homepage"));

    action_openshotsdir = new MyAction(this, "open_shots_dir");
    action_openshotsdir->change(Images::icon("open_screen"), tr("Open screenshots folder"));

    connect(action_show, SIGNAL(triggered()), this, SLOT(showAll()));
    connect(action_openshotsdir, SIGNAL(triggered()), this, SLOT(open_screenshot_directory()));
    tray_menu->setFixedWidth(250);

    tray->setContextMenu(tray_menu);
}

void MainWindow::addTrayActions() {
    //添加菜单项
    tray_menu->addAction(action_show);
    tray_menu->addAction(action_openshotsdir);
    tray_menu->addSeparator();
    tray_menu->addAction(aboutAct);
    tray_menu->addAction(helpAct);
    tray_menu->addSeparator();
    tray_menu->addAction(showPreferencesAct);
    tray_menu->addAction(quitAct);
}

void MainWindow::createHiddenActions() {
    playlist_action = new MyAction(QKeySequence("F3"), this, "playlist_open_close");
    playlist_action->change(tr("PlayList"));
    connect(playlist_action, SIGNAL(triggered()), this, SLOT(slot_playlist()));

    play_pause_aciton = new MyAction(QKeySequence(Qt::Key_Space), this, "play_pause");
    play_pause_aciton->change(tr("Play/Pause"));
    //201810
    //connect(playlist_action, SIGNAL(triggered()), core, SLOT(play_or_pause()));
    connect(playlist_action, &MyAction::triggered, this, &MainWindow::startPlayPause);

    stopAct = new MyAction(Qt::Key_MediaStop, this, "stop");
    stopAct->change(tr("Stop"));
    connect(stopAct, SIGNAL(triggered()), core, SLOT(stop()));

    fullscreenAct = new MyAction(QKeySequence("Ctrl+Return"), this, "fullscreen");
    fullscreenAct->change(tr("Fullscreen"));
//    connect(fullscreenAct, SIGNAL(triggered()), this, SLOT(slot_set_fullscreen()));
    connect(fullscreenAct, SIGNAL(toggled(bool)), this, SLOT(toggleFullscreen(bool)));
}

void MainWindow::setActionsEnabled(bool b) {
//    qDebug() << "MainWindow::setActionsEnabled  " << b;
    emit this->sigActionsEnabled(b);

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
    muteAct->setEnabled(b);
    decVolumeAct->setEnabled(b);
    incVolumeAct->setEnabled(b);
    decAudioDelayAct->setEnabled(b);
    incAudioDelayAct->setEnabled(b);
    audioDelayAct->setEnabled(b);

    flipAct->setEnabled(b);
    mirrorAct->setEnabled(b);

    // Menu Subtitles
    loadSubsAct->setEnabled(b);
    aspectGroup->setActionsEnabled(b);
    rotateGroup->setActionsEnabled(b);
    channelsGroup->setActionsEnabled(b);

    stereoGroup->setActionsEnabled(b);
}

void MainWindow::enableActionsOnPlaying() {
    this->setActionsEnabled(true);

    isPlaying = true;

    // Screenshot option
    bool screenshots_enabled = ((pref->use_screenshot) &&
                                 (!pref->screenshot_directory.isEmpty()) &&
                                 (QFileInfo(pref->screenshot_directory).isDir()));
    screenshotAct->setEnabled(screenshots_enabled);

    //TODO
    // Disable audio actions if there's not audio track
//    if ((core->mdat.audios.numItems()==0) && (core->mset.external_audio.isEmpty())) {
    if ((core->mset.audios.numItems()==0) && (core->mset.external_audio.isEmpty())) {
        muteAct->setEnabled(false);
        decVolumeAct->setEnabled(false);
        incVolumeAct->setEnabled(false);
        decAudioDelayAct->setEnabled(false);
        incAudioDelayAct->setEnabled(false);
        audioDelayAct->setEnabled(false);
        channelsGroup->setActionsEnabled(false);
        stereoGroup->setActionsEnabled(false);
    }

    // Disable video actions if it's an audio file
    if (core->mdat.novideo) {
        screenshotAct->setEnabled(false);
        flipAct->setEnabled(false);
        mirrorAct->setEnabled(false);
        aspectGroup->setActionsEnabled(false);
        rotateGroup->setActionsEnabled(false);
    }

    if (pref->hwdec.startsWith("vdpau") && pref->mplayer_bin.contains("/usr/bin/mpv")) {//kobe 20170706
        screenshotAct->setEnabled(false);
    }

    // Disable video filters if using vdpau
    if ((pref->vdpau.disable_video_filters) && (pref->vo.startsWith("vdpau"))) {
        screenshotAct->setEnabled(false);
        flipAct->setEnabled(false);
        mirrorAct->setEnabled(false);
        rotateGroup->setActionsEnabled(false);
        displayMessage(tr("Video filters are disabled when using vdpau"));//使用 VDPAU 时将禁用视频过滤器
    }
}

void MainWindow::disableActionsOnStop() {
//    qDebug("MainWindow::disableActionsOnStop");
    this->setActionsEnabled(false);//kobe:此处会让一些按钮处于禁用状态
    emit this->setPlayOrPauseEnabled(true);

//    if (m_bottomController)
//        m_bottomController->permanentShow();

    isPlaying = false;
    m_topToolbar->enable_turned_on();//0519
    isFinished = true;//kobe
//    turned_on = true;//0829
}

void MainWindow::togglePlayAction(Core::State state) {
    if (state == Core::Playing) {//Stopped = 0, Playing = 1, Paused = 2
        m_bottomToolbar->onMusicPlayed();
        play_mask->hide();//0620
    }
    else if (state == Core::Stopped) {//0621
        m_bottomToolbar->onMusicPause();
        play_mask->hide();//0620
    }
    else {
        m_bottomToolbar->onMusicPause();
        if (mplayerwindow) {
            mplayerwindow->hideLogo();
        }
        play_mask->show();//0620
    }
}

void MainWindow::setJumpTexts() {
    rewind1Act->change( tr("-%1").arg(Helper::timeForJumps(pref->seeking1)) );
    rewind2Act->change( tr("-%1").arg(Helper::timeForJumps(pref->seeking2)) );
    rewind3Act->change( tr("-%1").arg(Helper::timeForJumps(pref->seeking3)) );

    forward1Act->change( tr("+%1").arg(Helper::timeForJumps(pref->seeking1)) );
    forward2Act->change( tr("+%1").arg(Helper::timeForJumps(pref->seeking2)) );
    forward3Act->change( tr("+%1").arg(Helper::timeForJumps(pref->seeking3)) );
}

void MainWindow::createCore() {
    //edited by kobe 20180623
    core = new Core(mplayerwindow, this->m_snap, this);

    connect(core, SIGNAL(widgetsNeedUpdate()), this, SLOT(updateWidgets()));
    connect(core, SIGNAL(showFrame(int)), this, SIGNAL(frameChanged(int)));
    connect(core, SIGNAL(ABMarkersChanged(int,int)), this, SIGNAL(ABMarkersChanged(int,int)));
    connect(core, SIGNAL(showTime(double, bool)), this, SLOT(gotCurrentTime(double, bool)));
    connect(core, SIGNAL(needResize(int, int)), this, SLOT(resizeWindow(int,int)));
    connect(core, SIGNAL(showMessage(QString)), this, SLOT(displayMessage(QString)));
    connect(core, SIGNAL(stateChanged(Core::State)), this, SLOT(displayState(Core::State)));
    connect(core, SIGNAL(stateChanged(Core::State)), this, SLOT(checkStayOnTop(Core::State)), Qt::QueuedConnection);
    connect(core, SIGNAL(mediaStartPlay()), this, SLOT(enterFullscreenOnPlay()), Qt::QueuedConnection);
    connect(core, SIGNAL(mediaStartPlay()), this, SLOT(checkMplayerVersion()), Qt::QueuedConnection);
    connect(core, SIGNAL(mediaStoppedByUser()), this, SLOT(exitFullscreenOnStop()));
    connect(core, SIGNAL(mediaStoppedByUser()), mplayerwindow, SLOT(showLogo()));
    connect(core, SIGNAL(show_logo_signal(bool)), mplayerwindow, SLOT(setLogoVisible(bool)));
    connect(core, SIGNAL(mediaLoaded()), this, SLOT(enableActionsOnPlaying()));
    connect(core, SIGNAL(noFileToPlay()), this, SLOT(gotNoFileToPlay()));
    connect(core, SIGNAL(audioTracksInitialized()), this, SLOT(enableActionsOnPlaying()));
    connect(core, SIGNAL(mediaFinished()), this, SLOT(disableActionsOnStop()));
    connect(core, SIGNAL(mediaStoppedByUser()), this, SLOT(disableActionsOnStop()));
    connect(core, SIGNAL(stateChanged(Core::State)), this, SLOT(togglePlayAction(Core::State)));
    //kobe connect的第五个参数Qt::QueuedConnection表示槽函数由接受信号的线程所执行，如果不加表示槽函数由发出信号的次线程执行。当传递信号的参数类型不是QT的元类型时要用qRegisterMetaType先注册
    connect(core, SIGNAL(mediaStartPlay()), this, SLOT(newMediaLoaded()), Qt::QueuedConnection);
    connect(core, SIGNAL(mediaInfoChanged()), this, SLOT(updateMediaInfo()));
    connect(core, SIGNAL(failedToParseMplayerVersion(QString)), this, SLOT(askForMplayerVersion(QString)));
    connect(core, SIGNAL(mplayerFailed(QProcess::ProcessError)), this, SLOT(showErrorFromMplayer(QProcess::ProcessError)));
    connect(core, SIGNAL(mplayerFinishedWithError(int)), this, SLOT(showExitCodeFromMplayer(int)));
    //kobe:没有视频的时候进行相关隐藏设置 Hide mplayer window
    connect(core, SIGNAL(noVideo()), mplayerwindow, SLOT(showLogo()));//hidePanel():
	// Log mplayer output
    connect(core, SIGNAL(aboutToStartPlaying()), this, SLOT(clearMplayerLog()));
    connect(core, SIGNAL(logLineAvailable(QString)), this, SLOT(recordMplayerLog(QString)));
    //connect(core, SIGNAL(mediaLoaded()), this, SLOT(autosaveMplayerLog()));
    connect(core, SIGNAL(receivedForbidden()), this, SLOT(gotForbidden()));

//#ifdef MOUSE_GESTURES
//	mplayerwindow->activateMouseDragTracking(true);
//#else
//	mplayerwindow->activateMouseDragTracking(pref->drag_function == Preferences::MoveWindow);
//#endif
}

void MainWindow::createMplayerWindow() {
    mplayerwindow = new MplayerWindow(panel);
    mplayerwindow->setObjectName("mplayerwindow");
    //mplayerwindow->installEventFilter(this);
    mplayerwindow->setColorKey("121212");//121212 mplayerwindow->setColorKey("20202");  0x020202 // pref->color_key kobe:视频显示区域背景色设置为黑色 0d87ca
    mplayerwindow->setContentsMargins(0, 0, 0, 0);
    mplayerwindow->allowVideoMovement( pref->allow_video_movement );
    mplayerwindow->delayLeftClick(pref->delay_left_click);
    mplayerwindow->setAnimatedLogo( pref->animated_logo);

    /*
    mplayerwindow = new MplayerWindow(panel);
    mplayerwindow->setObjectName("mplayerwindow");
    mplayerwindow->setColorKey("121212");//mplayerwindow->setColorKey("20202");  0x020202 // pref->color_key kobe:视频显示区域背景色设置为黑色
	QVBoxLayout * layout = new QVBoxLayout;
	layout->setSpacing(0);
	layout->setMargin(0);
	layout->addWidget(mplayerwindow);
    panel->setLayout(layout);*/

	// mplayerwindow mouse events
    connect(mplayerwindow, SIGNAL(doubleClicked()), this, SLOT(doubleClickFunction()));
    connect(mplayerwindow, SIGNAL(leftClicked()), this, SLOT(leftClickFunction()));
    connect(mplayerwindow, SIGNAL(rightClicked()), this, SLOT(rightClickFunction()));
    connect(mplayerwindow, SIGNAL(middleClicked()), this, SLOT(middleClickFunction()));
    connect(mplayerwindow, SIGNAL(xbutton1Clicked()), this, SLOT(xbutton1ClickFunction()));
    connect(mplayerwindow, SIGNAL(xbutton2Clicked()), this, SLOT(xbutton2ClickFunction()));
    connect(mplayerwindow, SIGNAL(mouseMovedDiff(QPoint)), this, SLOT(moveWindowDiff(QPoint)), Qt::QueuedConnection);
//    connect(mplayerwindow->videoLayer(), SIGNAL(sigShowControls()), this, SLOT(onShowControls()));//0830
    mplayerwindow->activateMouseDragTracking(true/*pref->move_when_dragging*/);
//    connect(mplayerwindow, SIGNAL(resize_mainwindow(int,int)), this, SLOT(slot_resize_mainwindow(int,int)));//add by kobe. remove on 20170720 replace by resizeEvent
}

void MainWindow::createPlaylist() {
    m_playlistWidget = new Playlist(core, this, 0);
    m_playlistWidget->setFixedSize(220, this->height() - TOP_TOOLBAR_HEIGHT - BOTTOM_TOOLBAR_HEIGHT);
    m_playlistWidget->setViewHeight();
    m_playlistWidget->move(this->width()-220, TOP_TOOLBAR_HEIGHT);
    m_playlistWidget->hide();
    connect(m_playlistWidget, SIGNAL(playlistEnded()), this, SLOT(playlistHasFinished()));
    connect(m_playlistWidget, SIGNAL(playlistEnded()), mplayerwindow, SLOT(showLogo()));
    connect(m_playlistWidget, SIGNAL(closePlaylist()), this, SLOT(slot_playlist()));
    connect(m_playlistWidget, SIGNAL(playListFinishedWithError(QString)), this, SLOT(showErrorFromPlayList(QString)));
    connect(m_playlistWidget, SIGNAL(showMessage(QString)), this, SLOT(displayMessage(QString)));
}

void MainWindow::createPanel() {
    panel = new QWidget(this);
    panel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    panel->setMinimumSize(QSize(1,1));
    panel->setFocusPolicy(Qt::StrongFocus);
    this->setCentralWidget(panel);//kobe for QMainWindow 中心窗口部件，panel加载的是视频显示区域
    panel->setFocus();
}

void MainWindow::createPreferencesDialog() {
    QApplication::setOverrideCursor(Qt::WaitCursor);
    pref_dialog = new PreferencesDialog(arch, this->m_snap/*, this*/);
    pref_dialog->setModal(false);
    connect(pref_dialog, SIGNAL(applied()), this, SLOT(applyNewPreferences()));
    QApplication::restoreOverrideCursor();
}

void MainWindow::createFilePropertiesDialog() {
    QApplication::setOverrideCursor(Qt::WaitCursor);
    file_dialog = new FilePropertiesDialog(/*this*/);
    file_dialog->setModal(false);
    connect( file_dialog, SIGNAL(applied()), this, SLOT(applyFileProperties()) );
    QApplication::restoreOverrideCursor();
}

void MainWindow::createAboutDialog() {
    QApplication::setOverrideCursor(Qt::WaitCursor);
    aboutDlg = new AboutDialog(this->m_snap);
    aboutDlg->setModal(false);
    QApplication::restoreOverrideCursor();
}

void MainWindow::createHelpDialog() {
    QApplication::setOverrideCursor(Qt::WaitCursor);
    helpDlg = new HelpDialog();
    helpDlg->setModal(false);
    QApplication::restoreOverrideCursor();
}

void MainWindow::slot_playlist() {
    setPlaylistVisible(!m_playlistWidget->isVisible());
}

void MainWindow::slideEdgeWidget(QWidget *right, QRect start, QRect end, int delay, bool hide) {
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

void MainWindow::disableControl(int delay) {
    QTimer::singleShot(delay, this, SLOT(disableSomeComponent()));
}

void MainWindow::disableSomeComponent() {
    m_playlistWidget->setEnabled(true);
}

void MainWindow::setPlaylistProperty() {
    m_playlistWidget->setProperty("moving", false);
}

void MainWindow::setPlaylistVisible(bool visible) {
    m_playlistWidget->setProperty("moving", true);
    int titleBarHeight = TOP_TOOLBAR_HEIGHT;

    double factor = 0.6;
    QRect start(this->width(), titleBarHeight, m_playlistWidget->width(), m_playlistWidget->height());
    QRect end(this->width() - m_playlistWidget->width(), titleBarHeight, m_playlistWidget->width(), m_playlistWidget->height());
    if (!visible) {
        this->slideEdgeWidget(m_playlistWidget, end, start, ANIMATIONDELAY * factor, true);
        emit this->change_playlist_btn_status(false);
    } else {
        m_playlistWidget->setFocus();
        this->slideEdgeWidget(m_playlistWidget, start, end, ANIMATIONDELAY * factor);
        emit this->change_playlist_btn_status(true);
    }
    disableControl(ANIMATIONDELAY * factor);
    m_topToolbar->raise();
    QTimer::singleShot(ANIMATIONDELAY * factor * 1, this, SLOT(setPlaylistProperty()));
}

void MainWindow::slot_resize_corner() {
    resizeCorner->move(m_bottomToolbar->width()- 15, m_bottomToolbar->height() - 15);
}

void MainWindow::slot_set_fullscreen() {
    if (pref->fullscreen) {
        toggleFullscreen(false);
    }
    else {
        toggleFullscreen(true);
    }
}

void MainWindow::hidePanel() {
    if (panel->isVisible()) {
        if (isMaximized()) {
            m_topToolbar->updateMaxButtonStatus(false);
            showNormal();
        }
        // Exit from fullscreen mode
        if (pref->fullscreen) { toggleFullscreen(false); update(); }

        int width = size().width();
        if (width > pref->default_size.width()) width = pref->default_size.width();
        resize( width, size().height() - panel->size().height() );
        panel->hide();
    }
}

void MainWindow::showPreferencesDialog() {
    exitFullscreenIfNeeded();
	
    if (!pref_dialog) {
        createPreferencesDialog();
    }
    pref_dialog->setData(pref);

    //从最新的配置文件读取快捷键并进行设置
    pref_dialog->mod_shortcut_page()->actions_editor->clear();
    pref_dialog->mod_shortcut_page()->actions_editor->addActions(this);

    pref_dialog->move((width() - pref_dialog->width()) / 2 +
                               mapToGlobal(QPoint(0, 0)).x(),
                               (window()->height() - pref_dialog->height()) / 2 +
                               mapToGlobal(QPoint(0, 0)).y());
    pref_dialog->show();
}

// The user has pressed OK in preferences dialog
void MainWindow::applyNewPreferences() {
    //edited by kobe 20180623
    PlayerID::Player old_player_type = PlayerID::player(pref->mplayer_bin/*, this->m_snap*/);
    pref_dialog->getData(pref);
    m_bottomToolbar->setPreviewData(pref->preview_when_playing);
    mplayerwindow->activateMouseDragTracking(true/*pref->move_when_dragging*/);
//#ifndef MOUSE_GESTURES
//	mplayerwindow->activateMouseDragTracking(pref->drag_function == Preferences::MoveWindow);
//#endif
//	mplayerwindow->delayLeftClick(pref->delay_left_click);
	setJumpTexts(); // Update texts in menus
	updateWidgets(); // Update the screenshot action

    // Restart the video if needed
    if (pref_dialog->requiresRestart())
        core->restart();

    // Update actions
    pref_dialog->mod_shortcut_page()->actions_editor->applyChanges();
    saveActions();
	pref->save();

    //edited by kobe 20180623
    if (old_player_type != PlayerID::player(pref->mplayer_bin/*, this->m_snap*/)) {
//        qDebug("MainWindow::applyNewPreferences: player changed!");
        // Hack, simulate a change of GUI to restart the interface
        // FIXME: try to create a new Core::proc in the future
        core->stop();
        if (pref_dialog && pref_dialog->isVisible()) {//add by kobe to hide the pref_dialog
            pref_dialog->accept();
        }
        emit guiChanged();
    }
}


void MainWindow::showFilePropertiesDialog() {
//    qDebug("MainWindow::showFilePropertiesDialog");

    exitFullscreenIfNeeded();

    if (!file_dialog) {
        createFilePropertiesDialog();
    }

    setDataToFileProperties();

    file_dialog->show();
}

void MainWindow::setDataToFileProperties() {
    InfoReader *i = InfoReader::obj(this->m_snap);//20181212
    i->getInfo();
    file_dialog->setCodecs(i->vcList(), i->acList(), i->demuxerList());

    // Save a copy of the original values
    if (core->mset.original_demuxer.isEmpty())
        core->mset.original_demuxer = core->mdat.demuxer;

    if (core->mset.original_video_codec.isEmpty())
        core->mset.original_video_codec = core->mdat.video_codec;

    if (core->mset.original_audio_codec.isEmpty())
        core->mset.original_audio_codec = core->mdat.audio_codec;

    QString demuxer = core->mset.forced_demuxer;
    if (demuxer.isEmpty()) demuxer = core->mdat.demuxer;

    QString ac = core->mset.forced_audio_codec;
    if (ac.isEmpty()) ac = core->mdat.audio_codec;

    QString vc = core->mset.forced_video_codec;
    if (vc.isEmpty()) vc = core->mdat.video_codec;

    file_dialog->setDemuxer(demuxer, core->mset.original_demuxer);
    file_dialog->setAudioCodec(ac, core->mset.original_audio_codec);
    file_dialog->setVideoCodec(vc, core->mset.original_video_codec);
    //file_dialog->setMediaData(core->mdat);
    file_dialog->setMediaData(core->mdat, core->mset.videos, core->mset.audios, core->mset.subs);
}

void MainWindow::applyFileProperties() {
	bool need_restart = false;

#undef TEST_AND_SET
#define TEST_AND_SET( Pref, Dialog ) \
    if ( Pref != Dialog ) { Pref = Dialog; need_restart = true; qDebug("MainWindow::applyFileProperties====================================111");}

    bool demuxer_changed = false;

    QString prev_demuxer = core->mset.forced_demuxer;

    if (prev_demuxer != core->mset.forced_demuxer) {
        // Demuxer changed
        demuxer_changed = true;
        core->mset.current_audio_id = MediaSettings::NoneSelected;
        core->mset.current_subtitle_track = MediaSettings::NoneSelected;
    }

    // Restart the video to apply
    if (need_restart) {
        if (demuxer_changed) {
            core->reload();
        } else {
            core->restart();
        }
    }
}


void MainWindow::updateMediaInfo()
{
    tray->setToolTip( windowTitle() );
	emit videoInfoChanged(core->mdat.video_width, core->mdat.video_height, core->mdat.video_fps.toDouble());
}

void MainWindow::displayVideoInfo(int width, int height, double fps) {
    if ((width != 0) && (height != 0)) {
//		video_info_display->setText(tr("%1x%2 %3 fps", "width + height + fps").arg(width).arg(height).arg(fps));
    } else {
//		video_info_display->setText(" ");
    }

//	QString format = core->mdat.video_format;
//	if (!format.isEmpty() && !core->mdat.audio_format.isEmpty()) format += " / ";
//	format += core->mdat.audio_format;
//	format_info_display->setText(format.toUpper());
}

void MainWindow::displayBitrateInfo(int vbitrate, int abitrate) {
//	bitrate_info_display->setText(tr("V: %1 kbps A: %2 kbps").arg(vbitrate/1000).arg(abitrate/1000));
}

void MainWindow::newMediaLoaded()
{
	QString stream_title = core->mdat.stream_title;
//	qDebug("MainWindow::newMediaLoaded: mdat.stream_title: %s", stream_title.toUtf8().constData());

	if (!stream_title.isEmpty()) {
        pref->history_recents->addItem( core->mdat.m_filename, stream_title );//20181201  m_filename
		//pref->history_recents->list();
	} else {
        pref->history_recents->addItem( core->mdat.m_filename );
	}
	updateRecents();

    QFileInfo fi(core->mdat.m_filename);//20181201  m_filename
    if (fi.exists()) {
        QString name = fi.fileName();
        m_topToolbar->set_title_name(name);
    }

	// Automatically add files to playlist
    if ((core->mdat.type == TYPE_FILE) /*&& (pref->auto_add_to_playlist)*/) {
        //qDebug("MainWindow::newMediaLoaded: playlist count: %d", playlist->count());
        QStringList files_to_add;
        if (m_playlistWidget->count() == 1) {
            files_to_add = Helper::filesForPlaylist(core->mdat.m_filename, pref->media_to_add_to_playlist);//20181201  m_filename
        }
        if (!files_to_add.empty()) m_playlistWidget->addFiles(files_to_add);
	}
}

void MainWindow::gotNoFileToPlay() {
    m_playlistWidget->resumePlay();//当前播放的文件不存在时，去播放下一个
}

void MainWindow::clearMplayerLog() {
    mplayer_log.clear();
}

void MainWindow::recordMplayerLog(QString line) {
    if (pref->log_mplayer) {
        if ( (line.indexOf("A:")==-1) && (line.indexOf("V:")==-1) ) {
            line.append("\n");
            mplayer_log.append(line);
        }
    }
}

/*!
    Save the mplayer log to a file, so it can be used by external
    applications.
*/
void MainWindow::autosaveMplayerLog() {
    qDebug("MainWindow::autosaveMplayerLog");

    if (pref->autosave_mplayer_log) {
        if (!pref->mplayer_log_saveto.isEmpty()) {
            QFile file( pref->mplayer_log_saveto );
            if ( file.open( QIODevice::WriteOnly ) ) {
                QTextStream strm( &file );
                strm << mplayer_log;
                file.close();
            }
        }
    }
}

void MainWindow::updateRecents() {
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

void MainWindow::clearRecentsList() {
    MessageDialog msgDialog(0, tr("Confirm deletion - Kylin Video"), tr("Delete the list of recent files?"));
    if (msgDialog.exec() != -1) {
        if (msgDialog.standardButton(msgDialog.clickedButton()) == QMessageBox::Ok) {
            // Delete items in menu
            pref->history_recents->clear();
            updateRecents();
        }
    }
}

void MainWindow::updateWidgets() {
//    panel->setFocus();//20190707
    m_bottomToolbar->setPreviewData(pref->preview_when_playing);
    muteAct->setChecked((pref->global_volume ? pref->mute : core->mset.mute));//kobe 0606

    bool muted = core->getMute();
    m_bottomToolbar->onMutedChanged(muted, core->getVolumn());//0519

    if (core->getVolumn() <= 0) {
        reset_mute_button();
    }
    channelsGroup->setChecked(core->mset.audio_use_channels);

//	// Aspect ratio
    aspectGroup->setChecked(core->mset.aspect_ratio_id);

    // Rotate
    rotateGroup->setChecked(core->mset.rotate);

    // Flip
    flipAct->setChecked(core->mset.flip);

    // Mirror
    mirrorAct->setChecked(core->mset.mirror);

    // Audio menu
    stereoGroup->setChecked(core->mset.stereo_mode);

    // Subtitle visibility
    subVisibilityAct->setChecked(pref->sub_visibility);

    // Stay on top
    onTopActionGroup->setChecked((int)pref->stay_on_top);

    // OSD
    osdGroup->setChecked( pref->osd );

//#ifdef MPV_SUPPORT
    OSDFractionsAct->setChecked(pref->osd_fractions);
//#endif

//#if defined(MPV_SUPPORT) && defined(MPLAYER_SUPPORT)
    if (PlayerID::player(pref->mplayer_bin) == PlayerID::MPLAYER) {
        //secondary_subtitles_track_menu->setEnabled(false);
        //frameBackStepAct->setEnabled(false);
        OSDFractionsAct->setEnabled(false);
        //earwaxAct->setEnabled(false);
    } else {
        //karaokeAct->setEnabled(false);
    }
//#endif
}

void MainWindow::openRecent() {
	QAction *a = qobject_cast<QAction *> (sender());
	if (a) {
		int item = a->data().toInt();
//		qDebug("MainWindow::openRecent: %d", item);
		QString file = pref->history_recents->item(item);
        QFileInfo fi(file);
        if (fi.exists()) {
            m_playlistWidget->addFile(file, Playlist::NoGetInfo);//20170712
            doOpen(file);
        }
        else {
            this->showErrorFromPlayList(file);
        }
	}
}

void MainWindow::doOpen(QString file) {
//    qDebug("MainWindow::open: '%s'", file.toUtf8().data());
	// If file is a playlist, open that playlist
	QString extension = QFileInfo(file).suffix().toLower();
	if ( ((extension=="m3u") || (extension=="m3u8")) && (QFile::exists(file)) ) {
//		playlist->load_m3u(file);
	} 
	else
	if (extension=="pls") {
//		playlist->load_pls(file);
	}
	else 
	if (QFileInfo(file).isDir()) {
		openDirectory(file);
	} 
	else {
		// Let the core to open it, autodetecting the file type
        this->m_playlistWidget->setPlaying(file, 0);
        core->open(file/*, 0*/);//每次从头开始播放文件
	}

	if (QFile::exists(file)) pref->latest_dir = QFileInfo(file).absolutePath();
}

void MainWindow::openFiles(QStringList files) {
//	qDebug("MainWindow::openFiles");
	if (files.empty()) return;

	if (files.count()==1) {
        m_playlistWidget->addFile(files[0], Playlist::NoGetInfo);//20170712
        doOpen(files[0]);
	} else {
        m_playlistWidget->addFiles(files);
        doOpen(files[0]);
	}
}

void MainWindow::openFile() {
	exitFullscreenIfNeeded();

	Extensions e;
    QString s = MyFileDialog::getOpenFileName(
                       this, tr("Choose a file"), pref->latest_dir, 
                       tr("Multimedia") + e.allPlayable().forFilter()+";;" +
                       tr("Video") + e.video().forFilter()+";;" +
                       tr("Audio") + e.audio().forFilter()+";;" +
                       tr("Playlists") + e.playlist().forFilter()+";;" +
                       tr("All files") +" (*.*)" );
    if ( !s.isEmpty() ) {
		openFile(s);
	}
}

void MainWindow::openFile(QString file) {
   if ( !file.isEmpty() ) {
		// If file is a playlist, open that playlist
		QString extension = QFileInfo(file).suffix().toLower();
		if ( (extension=="m3u") || (extension=="m3u8") ) {
//			playlist->load_m3u(file);
		} 
		else
		if (extension=="pls") {
//			playlist->load_pls(file);
		}
		else
		if (extension=="iso") {
            this->m_playlistWidget->setPlaying(file, 0);
            core->open(file/*, 0*/);//每次从头开始播放文件
		}
        else {//kobe:打开一个本地视频文件
            this->m_playlistWidget->setPlaying(file, 0);
            core->openFile(file);//开始播放新打开的本地视频文件
            m_playlistWidget->addFile(file, Playlist::NoGetInfo);//将新打开的本地视频文件加入到播放列表中
		}
		if (QFile::exists(file)) pref->latest_dir = QFileInfo(file).absolutePath();
	}
}

void MainWindow::openDirectory() {
    qDebug("MainWindow::openDirectory");

	QString s = MyFileDialog::getExistingDirectory(
                    this, tr("Choose a directory"),
                    pref->latest_dir );

	if (!s.isEmpty()) {
		openDirectory(s);
	}
}

void MainWindow::openDirectory(QString directory) {
//	qDebug("MainWindow::openDirectory: '%s'", directory.toUtf8().data());
	if (Helper::directoryContainsDVD(directory)) {
		core->open(directory);
	} 
	else {
		QFileInfo fi(directory);
		if ( (fi.exists()) && (fi.isDir()) ) {
            //m_playlistWidget->clear();
            //m_playlistWidget->addDirectory(directory);
            m_playlistWidget->addDirectory(fi.absoluteFilePath());
            m_playlistWidget->startPlayPause();
		} else {
            qDebug("MainWindow::openDirectory: directory is not valid");
		}
	}
}

void MainWindow::openURL() {
    exitFullscreenIfNeeded();

    /*
    bool ok;
    QString s = QInputDialog::getText(this,
            tr("Kylin Video - Enter URL"), tr("URL:"), QLineEdit::Normal,
            pref->last_url, &ok );

    if ( ok && !s.isEmpty() ) {

        //playlist->clear();
        //playlistdock->hide();

        openURL(s);
    } else {
        // user entered nothing or pressed Cancel
    }
    */



    //kobe 20170627
//	InputURL d(this);
    InputURL d;//kobe 20170627

    // Get url from clipboard
    QString clipboard_text = QApplication::clipboard()->text();
    if ((!clipboard_text.isEmpty()) && (clipboard_text.contains("://")) /*&& (QUrl(clipboard_text).isValid())*/) {
        d.setURL(clipboard_text);
    }

    for (int n=0; n < pref->history_urls->count(); n++) {
        d.setURL(pref->history_urls->url(n) );
    }
    int w_x = this->frameGeometry().topLeft().x() + (this->width() / 2) - (400  / 2);
    int w_y = this->frameGeometry().topLeft().y() + (this->height() /2) - (170  / 2);
    d.move(w_x, w_y);
    if (d.exec() == QDialog::Accepted ) {
        QString url = d.url();
        if (!url.isEmpty()) {
            pref->history_urls->addUrl(url);
            openURL(url);
        }
    }
}

void MainWindow::openURL(QString url) {
    if (!url.isEmpty()) {
        pref->history_urls->addUrl(url);
        core->openStream(url);

//		if (pref->auto_add_to_playlist) {
//			if (playlist->maybeSave()) {
//				core->openStream(url);

//				playlist->clear();
//				playlist->addFile(url, Playlist::NoGetInfo);
//			}
//		} else {
//			core->openStream(url);
//		}
    }
}

void MainWindow::loadSub() {
    qDebug("MainWindow::loadSub");

	exitFullscreenIfNeeded();

	Extensions e;
    QString s = MyFileDialog::getOpenFileName(
        this, tr("Choose a file"), 
	    pref->latest_dir, 
        tr("Subtitles") + e.subtitles().forFilter()+ ";;" +
        tr("All files") +" (*.*)" );

	if (!s.isEmpty()) core->loadSub(s);
}

void MainWindow::setInitialSubtitle(const QString & subtitle_file) {
    qDebug("MainWindow::setInitialSubtitle: '%s'", subtitle_file.toUtf8().constData());

	core->setInitialSubtitle(subtitle_file);
}

void MainWindow::loadAudioFile() {
    qDebug("MainWindow::loadAudioFile");

	exitFullscreenIfNeeded();

	Extensions e;
	QString s = MyFileDialog::getOpenFileName(
        this, tr("Choose a file"), 
	    pref->latest_dir, 
        tr("Audio") + e.audio().forFilter()+";;" +
        tr("All files") +" (*.*)" );

	if (!s.isEmpty()) core->loadAudioFile(s);
}

void MainWindow::setDataToAboutDialog() {
    aboutDlg->setVersions();
}

void MainWindow::showAboutDialog() {
    if (!aboutDlg) {
        createAboutDialog();
    }
    setDataToAboutDialog();
    int w_x = this->frameGeometry().topLeft().x() + (this->width() / 2) - (438  / 2);
    int w_y = this->frameGeometry().topLeft().y() + (this->height() /2) - (320  / 2);
    aboutDlg->move(w_x, w_y);
    aboutDlg->show();
}

void MainWindow::showHelpDialog() {
    if (!helpDlg) {
        createHelpDialog();
    }
    helpDlg->setData(pref);
    helpDlg->move((width() - helpDlg->width()) / 2 +
                               mapToGlobal(QPoint(0, 0)).x(),
                               (window()->height() - helpDlg->height()) / 2 +
                               mapToGlobal(QPoint(0, 0)).y());
    helpDlg->show();
}

void MainWindow::showGotoDialog() {
    TimeDialog d;
	d.setLabel(tr("&Jump to:"));
    d.setWindowTitle(tr("Kylin Video - Seek"));
    d.setMaximumTime((int) core->mdat.duration);
    d.setTime((int) core->mset.current_sec);
    int w_x = this->frameGeometry().topLeft().x() + (this->width() / 2) - (380  / 2);
    int w_y = this->frameGeometry().topLeft().y() + (this->height() /2) - (170  / 2);
    d.move(w_x, w_y);
	if (d.exec() == QDialog::Accepted) {
        core->goToSec(d.time());
	}
}

void MainWindow::showAudioDelayDialog() {
    AudioDelayDialog dlg;
    dlg.setDefaultValue(core->mset.audio_delay);
    int w_x = this->frameGeometry().topLeft().x() + (this->width() / 2) - (380  / 2);
    int w_y = this->frameGeometry().topLeft().y() + (this->height() /2) - (170  / 2);
    dlg.move(w_x, w_y);
    if (dlg.exec() == QDialog::Accepted) {
        int delay = dlg.getCurrentValue();
        core->setAudioDelay(delay);
    }
}

void MainWindow::showSubDelayDialog() {
	bool ok;
	#if QT_VERSION >= 0x050000
    int delay = QInputDialog::getInt(this, tr("Kylin Video - Subtitle delay"),
                                     tr("Subtitle delay (in milliseconds):"), core->mset.sub_delay, 
                                     -3600000, 3600000, 1, &ok);
	#else
    int delay = QInputDialog::getInteger(this, tr("Kylin Video - Subtitle delay"),
                                         tr("Subtitle delay (in milliseconds):"), core->mset.sub_delay, 
                                         -3600000, 3600000, 1, &ok);
	#endif
	if (ok) {
		core->setSubDelay(delay);
	}
}

void MainWindow::setStayOnTop(bool b) {
    if ( (b && (windowFlags() & Qt::WindowStaysOnTopHint)) ||
         (!b && (!(windowFlags() & Qt::WindowStaysOnTopHint))) )
    {
        // identical do nothing
//        qDebug("MainWindow::setStayOnTop: nothing to do");
        return;
    }

    ignore_show_hide_events = true;

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

    ignore_show_hide_events = false;
}

void MainWindow::changeStayOnTop(int stay_on_top) {
    switch (stay_on_top) {
        case Preferences::AlwaysOnTop : setStayOnTop(true); break;
        case Preferences::NeverOnTop  : setStayOnTop(false); break;
        case Preferences::WhilePlayingOnTop : setStayOnTop((core->state() == Core::Playing)); break;
    }

    pref->stay_on_top = (Preferences::OnTop) stay_on_top;
    updateWidgets();
    // Stay on top
    onTopActionGroup->setChecked((int)pref->stay_on_top);
}

void MainWindow::checkStayOnTop(Core::State state) {
    if ((!pref->fullscreen) && (pref->stay_on_top == Preferences::WhilePlayingOnTop)) {
        setStayOnTop((state == Core::Playing));
    }
}

void MainWindow::changePlayOrder(int play_order) {
    pref->play_order = (Preferences::PlayOrder) play_order;
    updateWidgets();
    playOrderActionGroup->setChecked((int)pref->play_order);
}

void MainWindow::exitFullscreen() {
	if (pref->fullscreen) {
		toggleFullscreen(false);
	}
}

void MainWindow::toggleFullscreen(bool b) {
    if (b == pref->fullscreen) {
        // Nothing to do
        qDebug("MainWindow::toggleFullscreen: nothing to do, returning");
        return;
    }

    pref->fullscreen = b;

    if (!panel->isVisible()) {
        qDebug() << "panel is not visible";
        return;
    }

    mplayerwindow->hideLogoForTemporary();

    if (pref->fullscreen) {
        if (m_bottomController)
            m_bottomController->permanentShow();
        was_maximized = isMaximized();

        if (pref->stay_on_top == Preferences::WhilePlayingOnTop && core->state() == Core::Playing) {
            setStayOnTop(false);
        }

        showFullScreen();
        this->mplayerwindow->resize(QMainWindow::size());
        m_bottomToolbar->onFullScreen();//让全屏/取消全屏的按钮更换图片为取消全屏
        this->resizeCorner->hide();
        m_topToolbar->updateMaxButtonStatus(true);

        QString state = core->stateToString().toUtf8().data();
        if (state == "Playing" || state == "Paused") {//全屏的时候如果不是正在播放或暂停，则显示标题栏和控制栏
        }
    }
    else {
        if (m_bottomController)
            m_bottomController->permanentShow();
        m_topToolbar->updateMaxButtonStatus(false);
        showNormal();
        if (was_maximized) {
            m_topToolbar->updateMaxButtonStatus(true);
            showMaximized(); // It has to be called after showNormal()
        }
        this->mplayerwindow->resize(QMainWindow::size());
        m_bottomToolbar->onUnFullScreen();//让全屏/取消全屏的按钮更换图片为全屏
        this->resizeCorner->show();
        if (this->escWidget->isVisible())
            this->escWidget->hide();

        QString state = core->stateToString().toUtf8().data();
        if (state == "Playing" || state == "Paused") {//kobe: Stopped Playing Paused

        }
    }
    updateWidgets();
    setFocus();
    QTimer::singleShot(100, mplayerwindow, SLOT(update_logo_pos()));
}

void MainWindow::leftClickFunction() {
    if (m_playlistWidget->isVisible()) {//20170713
        setPlaylistVisible(false);
    }

    QString state = core->stateToString().toUtf8().data();
    if (state == "Playing" || state == "Paused") {//kobe: Stopped Playing Paused
        this->startPlayPause();
    }
//	if (!pref->mouse_left_click_function.isEmpty()) {
//		processFunction(pref->mouse_left_click_function);
//	}
}

void MainWindow::rightClickFunction() {
    showPopupMenu();//kobe
//	if (!pref->mouse_right_click_function.isEmpty()) {
//        processFunction(pref->mouse_right_click_function);
//	}
}

void MainWindow::doubleClickFunction() {
    this->slot_set_fullscreen();//kobe
//	if (!pref->mouse_double_click_function.isEmpty()) {
//		processFunction(pref->mouse_double_click_function);
//	}
}

void MainWindow::middleClickFunction() {
    processFunction("mute");//kobe
//	if (!pref->mouse_middle_click_function.isEmpty()) {
//		processFunction(pref->mouse_middle_click_function);
//	}
}

void MainWindow::xbutton1ClickFunction() {
    qDebug("MainWindow::xbutton1ClickFunction");

//	if (!pref->mouse_xbutton1_click_function.isEmpty()) {
//		processFunction(pref->mouse_xbutton1_click_function);
//	}
}

void MainWindow::xbutton2ClickFunction() {
    qDebug("MainWindow::xbutton2ClickFunction");

//	if (!pref->mouse_xbutton2_click_function.isEmpty()) {
//		processFunction(pref->mouse_xbutton2_click_function);
//	}
}

void MainWindow::processFunction(QString function) {
//    qDebug("##########################MainWindow::processFunction: '%s'", function.toUtf8().data());

    //parse args for checkable actions
    QRegExp func_rx("(.*) (true|false)");
    bool value = false;
    bool checkableFunction = false;

    if(func_rx.indexIn(function) > -1){
        function = func_rx.cap(1);
        value = (func_rx.cap(2) == "true");
        checkableFunction = true;
    } //end if

    //kobe
    //smplayer源码是用QAction关联各个事件，比如全屏，创建全屏菜单项的时候设置了objectName为fullscreen，
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

void MainWindow::gotForbidden() {
    qDebug("MainWindow::gotForbidden");

	static bool busy = false;

	if (busy) return;

	busy = true;

    QMessageBox::warning(this, tr("Error detected"),
        tr("Unfortunately this video can't be played.") +"<br>"+
        tr("The server returned '%1'").arg("403: Forbidden"));
	busy = false;
}

void MainWindow::dragEnterEvent( QDragEnterEvent *event) {
//	qDebug("MainWindow::dragEnterEvent");

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

void MainWindow::dropEvent( QDropEvent *e )
{
	QStringList files;

//    if (e->mimeData()->hasFormat("text/uri-list")) {
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

    qDebug( "MainWindow::dropEvent: count: %d", files.count());
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
            core->loadSub(sub_file);
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
//				core->loadSub( files[0] );
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

void MainWindow::showPopupMenu() {
    showPopupMenu(QCursor::pos());
}

void MainWindow::showPopupMenu( QPoint p ) {
//	//qDebug("MainWindow::showPopupMenu: %d, %d", p.x(), p.y());
    popup->move(p);
    popup->show();
}

// Called when a video has started to play
void MainWindow::enterFullscreenOnPlay() {
//	qDebug("MainWindow::enterFullscreenOnPlay: arg_start_in_fullscreen: %d, pref->start_in_fullscreen: %d", arg_start_in_fullscreen, pref->start_in_fullscreen);
	if (arg_start_in_fullscreen != 0) {
        if ( (arg_start_in_fullscreen == 1)/* || (pref->start_in_fullscreen) */) {
            if (!pref->fullscreen) {
                toggleFullscreen(true);
            }
		}
	}
}

// Called when the playlist has stopped
void MainWindow::exitFullscreenOnStop() {
//    qDebug("MainWindow::exitFullscreenOnStop");
    if (pref->fullscreen) {
		toggleFullscreen(false);
	}
    emit this->clear_playing_title();
}

void MainWindow::playlistHasFinished() {
//	qDebug("MainWindow::playlistHasFinished");
	core->stop();

	exitFullscreenOnStop();
}

void MainWindow::displayState(Core::State state) {
    switch (state) {//0830
        case Core::Playing://播放时开启自动隐藏标题栏和控制栏的定时器
            break;
        case Core::Paused://暂停时显示标题栏和控制栏
            if (m_bottomController)
                m_bottomController->permanentShow();
            break;
        case Core::Stopped:
        if (m_bottomController)
            m_bottomController->permanentShow();
            m_topToolbar->set_title_name("");
            break;
    }
}

void MainWindow::displayMessage(QString message) {
    this->showTipWidget(message);
}

//kobe 0606 flag=true时表示充值时间显示label的值为初始值
void MainWindow::gotCurrentTime(double sec, bool flag) {
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

//        time = Helper::formatTime( (int) sec ) + " / " +
//                               Helper::formatTime( (int) core->mdat.duration );
        time = Helper::formatTime((int) sec);
        all_time = " / " +  Helper::formatTime((int) core->mdat.duration);

        //qDebug( " duration: %f, current_sec: %f", core->mdat.duration, core->mset.current_sec);
    }

    emit timeChanged(time, all_time);
}

void MainWindow::loadConfig() {
//    qDebug("DefaultGui::loadConfig");
    QSettings * set = settings;

    set->beginGroup("default_gui");

    QPoint p = set->value("pos", pos()).toPoint();
//    QSize s = set->value("size", size()).toSize();

    move(p);

    setWindowState( (Qt::WindowStates) set->value("state", 0).toInt() );

    if (!DesktopInfo::isInsideScreen(this)) {
        move(0,0);
        qWarning("DefaultGui::loadConfig: window is outside of the screen, moved to 0x0");
    }

    set->endGroup();

    updateWidgets();
}

void MainWindow::resizeWindow(int w, int h) {
	// If fullscreen, don't resize!
	if (pref->fullscreen) return;

    if ( (pref->resize_method==Preferences::Never) && (panel->isVisible()) ) {
		return;
	}

	if (!panel->isVisible()) {
		panel->show();
    }

    resizeMainWindow(w, h);
}

void MainWindow::resizeMainWindow(int w, int h) {
	QSize video_size(w,h);

    //20190717
    if (video_size == panel->size()) {
        qDebug("MainWindow::resizeWindow: the panel size is already the required size. Doing nothing.");
		return;
    }

	int diff_width = this->width() - panel->width();
	int diff_height = this->height() - panel->height();

	int new_width = w + diff_width;
	int new_height = h + diff_height;

	int minimum_width = minimumSizeHint().width();
	if (new_width < minimum_width) {
        qDebug("MainWindow::resizeWindow: width is too small, setting width to %d", minimum_width);
		new_width = minimum_width;
	}

//	qDebug("MainWindow::resizeWindow: new_width: %d new_height: %d", new_width, new_height);
    resize(new_width, new_height);
}

void MainWindow::centerWindow() {//kobe:播放后界面调整至屏幕中央
    if (/*pref->center_window && */!pref->fullscreen && isVisible()) {
		QRect r = QApplication::desktop()->screenGeometry(this);
		// r.setX(500); r.setY(150); // Test
//		qDebug() << "MainWindow::centerWindow: desktop rect:" << r;
		int x = r.x() + ((r.width() - width()) / 2);
		int y = r.y() + ((r.height() - height()) / 2);
		move(x, y);
	}
}

void MainWindow::displayGotoTime(int t) {
    int jump_time = (int)core->mdat.duration * t / SEEKBAR_RESOLUTION;
    QString s = tr("Jump to %1").arg( Helper::formatTime(jump_time) );
    if (pref->fullscreen) {
        core->displayTextOnOSD( s );
    }
}

void MainWindow::goToPosOnDragging(int t) {
//		core->goToPosition(t);//core->goToPos(t);
}

// Called when a new window (equalizer, preferences..) is opened.
void MainWindow::exitFullscreenIfNeeded() {
	if (pref->fullscreen) {
		toggleFullscreen(false);
	}
}

void MainWindow::loadActions() {
//    qDebug("MainWindow::loadActions");
    ActionsEditor::loadFromConfig(this, settings);
//	actions_list = ActionsEditor::actionsNames(this);
}

void MainWindow::saveActions() {
//    qDebug("MainWindow::saveActions");
    ActionsEditor::saveToConfig(this, settings);
}

/*void MainWindow::moveEvent(QMoveEvent *event)
{
    if (!isMaximized() && !isFullScreen() && !pref->fullscreen) {
        //const QPoint gPos = pos();
        //move(gPos);
    }

    QMainWindow::moveEvent(event);
}*/

void MainWindow::moveWindowDiff(QPoint diff) {
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
//        qDebug() << "MainWindow::moveWindowDiff: new_pos:" << new_pos;
		move(new_pos);
		count = 0;
		d = QPoint(0,0);
	}
#else
	move(pos() + diff);
#endif
}

#if QT_VERSION < 0x050000
void MainWindow::showEvent( QShowEvent * ) {
    qDebug("MainWindow::showEvent");

	if (ignore_show_hide_events) return;

    //qDebug("MainWindow::showEvent: pref->pause_when_hidden: %d", pref->pause_when_hidden);
	if ((pref->pause_when_hidden) && (core->state() == Core::Paused)) {
        qDebug("MainWindow::showEvent: unpausing");
		core->pause(); // Unpauses
	}
}

void MainWindow::hideEvent( QHideEvent * ) {
    qDebug("MainWindow::hideEvent");

	if (ignore_show_hide_events) return;

    //qDebug("MainWindow::hideEvent: pref->pause_when_hidden: %d", pref->pause_when_hidden);
	if ((pref->pause_when_hidden) && (core->state() == Core::Playing)) {
        qDebug("MainWindow::hideEvent: pausing");
		core->pause();
	}
}
#else
// Qt 5 doesn't call showEvent / hideEvent when the window is minimized or unminimized
bool MainWindow::event(QEvent * e) {//kobe 0522 QWidget::paintEngine: Should no longer be called
//    qDebug("MainWindow::event: %d", e->type());

	bool result = QWidget::event(e);
//    if ((ignore_show_hide_events)/* || (!pref->pause_when_hidden)*/) return result;
    if ((ignore_show_hide_events) || (!pref->pause_when_hidden)) return result;//20170627 kobe

    if (e->type() == QEvent::WindowStateChange) {//窗口的状态（最小化、最大化或全屏）发生改变（QWindowStateChangeEvent）
        //qDebug() << "MainWindow::event: WindowStateChange";

		if (isMinimized()) {
			was_minimized = true;
			if (core->state() == Core::Playing) {
                qDebug("MainWindow::event: pausing......");
				core->pause();
			}
		}
	}

    if ((e->type() == QEvent::ActivationChange) && (isActiveWindow())) {//Widget 的顶层窗口激活状态发生了变化
//        qDebug("MainWindow::event: ActivationChange: %d", was_minimized);

        if ((!isMinimized()) && (was_minimized)) {
            was_minimized = false;
            if (core->state() == Core::Paused) {
                qDebug("MainWindow::showEvent: unpausing");
                core->pause(); // Unpauses
            }
        }
    }


	return result;
}
#endif

void MainWindow::quit()
{
    closeWindow();
}

void MainWindow::trayIconActivated(QSystemTrayIcon::ActivationReason reason) {
    switch(reason) {
        case QSystemTrayIcon::Trigger:
            toggleShowAll();
            break;
        case QSystemTrayIcon::DoubleClick:
            toggleShowAll();
            break;
        case QSystemTrayIcon::MiddleClick:
            core->pause();
            break;
        default:
            break;
    }
}

void MainWindow::toggleShowAll() {
    // Ignore if tray is not visible
    if (tray->isVisible()) {
        showAll( !isVisible() );
    }
}

void MainWindow::showAll() {
    if (!isVisible()) showAll(true);
}

void MainWindow::showAll(bool b) {
    if (!b) {
        trayicon_playlist_was_visible = m_playlistWidget->isVisible();
        playlist_pos = m_playlistWidget->pos();
        m_playlistWidget->hide();
        mainwindow_pos = pos();
        hide();
    } else {
        // Show all
        move(mainwindow_pos);
        show();
        if (trayicon_playlist_was_visible) {
            m_playlistWidget->move(playlist_pos);
            m_playlistWidget->show();
        }
    }
}

void MainWindow::askForMplayerVersion(QString line) {
    qDebug("MainWindow::askForMplayerVersion: %s", line.toUtf8().data());
}

void MainWindow::showErrorFromPlayList(QString errorStr)
{
    ErrorDialog d;
    d.setWindowTitle(tr("%1 Error").arg(PLAYER_NAME));
    d.setTitleText(tr("%1 Error").arg(PLAYER_NAME));
    d.setText(tr("'%1' was not found!").arg(errorStr));
    d.hideDetailBtn();
    d.exec();
}

void MainWindow::showExitCodeFromMplayer(int exit_code) {
    qDebug("MainWindow::showExitCodeFromMplayer: %d", exit_code);

	if (exit_code != 255 ) {
        ErrorDialog d;//kobe
		d.setWindowTitle(tr("%1 Error").arg(PLAYER_NAME));
        d.setTitleText(tr("%1 Error").arg(PLAYER_NAME));
		d.setText(tr("%1 has finished unexpectedly.").arg(PLAYER_NAME) + " " + 
	              tr("Exit code: %1").arg(exit_code));
        d.setLog(mplayer_log);
		d.exec();
    }
    this->clearMplayerLog();//add by kobe
}

void MainWindow::showErrorFromMplayer(QProcess::ProcessError e) {
//	qDebug("MainWindow::showErrorFromMplayer");
    /*QProcess::FailedToStart        0        进程启动失败
    QProcess::Crashed                1        进程成功启动后崩溃
    QProcess::Timedout               2        最后一次调用waitFor...()函数超时.此时QProcess状态不变,并可以再次             调用waitFor()类型的函数
    QProcess::WriteError              3        向进程写入时出错.如进程尚未启动,或者输入通道被关闭时
    QProcess::ReadError              4        从进程中读取数据时出错.如进程尚未启动时
    QProcess::UnknownError       5        未知错误.这也是error()函数返回的默认值。*/

	if ((e == QProcess::FailedToStart) || (e == QProcess::Crashed)) {
        ErrorDialog d;//kobe 20170627
		d.setWindowTitle(tr("%1 Error").arg(PLAYER_NAME));
        d.setTitleText(tr("%1 Error").arg(PLAYER_NAME));
		if (e == QProcess::FailedToStart) {
			d.setText(tr("%1 failed to start.").arg(PLAYER_NAME) + " " + 
                         tr("Please check the %1 path in preferences.").arg(PLAYER_NAME));
		} else {
			d.setText(tr("%1 has crashed.").arg(PLAYER_NAME) + " " + 
                      tr("See the log for more info."));
		}
        d.setLog(mplayer_log);
		d.exec();
	}
    this->clearMplayerLog();//add by kobe
//    else if ((e == QProcess::ReadError) || (e == QProcess::WriteError)) {//add by kobe
//        ErrorDialog d;//kobe 20170627
//        d.setWindowTitle(tr("%1 Error").arg(PLAYER_NAME));
//        d.setTitleText(tr("%1 Error").arg(PLAYER_NAME));
//        if (e == QProcess::ReadError) {
//            d.setText(tr("%1 failed to read.").arg(PLAYER_NAME) + " " +
//                         tr("Please check the file %1 still exists."));
//        } else {
//            d.setText(tr("%1 failed to write.").arg(PLAYER_NAME) + " " +
//                      tr("Please check if the file still exists."));
//        }
//        d.setLog(mplayer_log);
//        d.exec();
//    }
}

void MainWindow::showTipWidget(const QString text) {
    if (this->tipWidget->isVisible())
        this->tipWidget->hide();
    if (tip_timer->isActive())
        tip_timer->stop();
    tip_timer->start();
//    this->tipWidget->move(this->pos().x() + 30, this->pos().y() + TOP_TOOLBAR_HEIGHT);
    this->tipWidget->setText(text);
    this->tipWidget->show();
}

void MainWindow::hideTipWidget() {
    this->tipWidget->hide();
}

void MainWindow::showOrHideEscWidget(bool b) {
    if (escWidget) {
        if (b) {
            if (!escWidget->isVisible() && this->isFullScreen()) {
                this->escWidget->show();
                m_maskWidget->showMask();
            }
        }
        else {
            if (escWidget->isVisible() && this->isFullScreen()) {
                this->escWidget->hide();
                m_maskWidget->hide();
            }
        }
    }
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event) {
    if (obj == this->resizeCorner) {
        if (!this->isMaximized()) {
            if (event->type() == QEvent::MouseButtonPress) {
                this->resizeFlag = true;
            }
            else if (event->type() == QEvent::MouseButtonRelease) {
                this->resizeFlag = false;
            }
        }
//        return false;
    }
//    else
//        return false;

    return qApp->eventFilter(obj, event);
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    QMainWindow::mouseMoveEvent(event);

    if (this->isMaximized()) {
        return;
    }
    if (event->buttons() == Qt::LeftButton) {
        if (this->resizeFlag) {
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

void MainWindow::ready_save_pre_image(int time) {
//    QString state = core->stateToString().toUtf8().data();
//    if (state == "Playing" || state == "Paused") {

//    }
    if (core) {
        if (core->state() == Core::Playing || core->state() == Core::Paused) {
            if (video_preview == 0) {
                video_preview = new VideoPreview(pref->mplayer_bin, 0);
            }

            if (!core->mdat.m_filename.isEmpty()) {//20181201  m_filename
                video_preview->setVideoFile(core->mdat.m_filename);

                // DVD
                /*if (core->mdat.type==TYPE_DVD) {
                    QString file = core->mdat.filename;
                    DiscData disc_data = DiscName::split(file);
                    QString dvd_folder = disc_data.device;
                    if (dvd_folder.isEmpty()) dvd_folder = pref->dvd_device;
                    int dvd_title = disc_data.title;
                    file = disc_data.protocol + "://" + QString::number(dvd_title);

                    video_preview->setVideoFile(file);
                    video_preview->setDVDDevice(dvd_folder);
                } else {
                    video_preview->setDVDDevice("");
                }*/
            }

            video_preview->setMplayerPath(pref->mplayer_bin);
            bool res = video_preview->createPreThumbnail(time);
            if (res) {
                emit this->send_save_preview_image_name(time, video_preview->getCurrentPicture());
            }
            else {
                emit this->send_save_preview_image_name(time, "");
            }
        }
        else {
            emit this->send_save_preview_image_name(time, "");
        }
    }
}

//20170720
void MainWindow::resizeEvent(QResizeEvent *e) {
//    QWidget::resizeEvent(e);
//    QSize newSize = QWidget::size();
    QMainWindow::resizeEvent(e);
    QSize newSize = QMainWindow::size();

    int titleBarHeight = this->m_topToolbar->height();
    this->m_topToolbar->raise();
    this->m_topToolbar->move(0, 0);
    this->m_topToolbar->resize(newSize.width(), titleBarHeight);

//    this->mplayerwindow->setFixedSize(newSize);
    this->mplayerwindow->resize(newSize);

    this->m_playlistWidget->setFixedSize(220, newSize.height() - BOTTOM_TOOLBAR_HEIGHT - titleBarHeight);
    this->m_playlistWidget->setViewHeight();
    if (this->m_playlistWidget->isVisible()) {
        this->m_playlistWidget->hide();
        emit this->change_playlist_btn_status(false);
    }

    this->m_bottomToolbar->raise();
    this->m_bottomToolbar->resize(newSize.width(), BOTTOM_TOOLBAR_HEIGHT);
    this->m_bottomToolbar->move(0, newSize.height() - BOTTOM_TOOLBAR_HEIGHT);
    this->m_bottomToolbar->show_control_widget();
    resizeCorner->move(this->m_bottomToolbar->width()- 15, this->m_bottomToolbar->height() - 15);
}

void MainWindow::closeEvent( QCloseEvent * e )  {
    this->closeWindow();
    e->accept();
}

void MainWindow::closeWindow() {
    if (core->state() != Core::Stopped) {
        core->stop();
    }

    m_playlistWidget->close();//kobe

    //qApp->quit();
    emit quitSolicited();
}

void MainWindow::open_screenshot_directory() {
    bool open_enabled = ((!pref->screenshot_directory.isEmpty()) && (QFileInfo(pref->screenshot_directory).isDir()));
    if (open_enabled) {
        QDesktopServices::openUrl(QUrl(QString("file:%1").arg(pref->screenshot_directory), QUrl::TolerantMode));
    }
    else {
        tray->showMessage(tr("Information"), tr("The screenshot folder does not exist!"), QSystemTrayIcon::Information, 2000);//QSystemTrayIcon::Warning
    }
}

void MainWindow::checkMplayerVersion() {
    // Qt 4.3.5 is crazy, I can't popup a messagebox here, it calls
    // this function once and again when the messagebox is shown

    if ( (pref->mplayer_detected_version > 0) && (!MplayerVersion::isMplayerAtLeast(25158)) ) {
        QTimer::singleShot(1000, this, SLOT(displayWarningAboutOldMplayer()));
    }
}

void MainWindow::displayWarningAboutOldMplayer() {
    if (!pref->reported_mplayer_is_old) {
        QMessageBox::warning(this, tr("Warning - Using old MPlayer"),
            tr("The version of MPlayer (%1) installed on your system "
               "is obsolete. SMPlayer can't work well with it: some "
               "options won't work, subtitle selection may fail...")
               .arg(MplayerVersion::toString(pref->mplayer_detected_version)) +
            "<br><br>" +
            tr("Please, update your MPlayer.") +
            "<br><br>" +
            tr("(This warning won't be displayed anymore)") );

        pref->reported_mplayer_is_old = true;
    }
}

//void MainWindow::paintEvent(QPaintEvent *)
//{
//    QPainter painter(this);
//    painter.drawPixmap(rect(), currentBackground);

////QPalette palette;
////palette.setBrush(QPalette::Background, image);
////setPalette(palette);
//}

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
