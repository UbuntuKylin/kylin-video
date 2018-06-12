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

#include "basegui.h"
#include "../smplayer/filedialog.h"
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

#include "../smplayer/mplayerwindow.h"
#include "../smplayer/desktopinfo.h"
#include "../smplayer/helper.h"
#include "../smplayer/paths.h"
#include "../smplayer/colorutils.h"
#include "../smplayer/global.h"
#include "../smplayer/translator.h"
#include "../smplayer/images.h"
#include "../smplayer/preferences.h"
#include "playlist.h"
#include "filepropertiesdialog.h"
#include "../smplayer/recents.h"
#include "../smplayer/urlhistory.h"
#include "errordialog.h"
#include "../smplayer/timedialog.h"
#include "../kylin/titlewidget.h"
#include "../kylin/bottomwidget.h"
#include "../kylin/playmask.h"
#include "../kylin/aboutdialog.h"
#include "../kylin/esctip.h"
#include "../kylin/tipwidget.h"
#include "audiodelaydialog.h"
#include "../kylin/messagedialog.h"
#include <QGraphicsOpacityEffect>
#include "../smplayer/mplayerversion.h"
#include "../smplayer/config.h"
#include "../smplayer/actionseditor.h"
#include "preferencesdialog.h"
#include "prefshortcut.h"
#include "../smplayer/myaction.h"
#include "../smplayer/myactiongroup.h"
#include "../smplayer/extensions.h"
#include "../smplayer/version.h"
#include "../smplayer/videopreview.h"
//#include "../kylin/shortcutswidget.h"
#include "../kylin/helpdialog.h"
#include "inputurl.h"

using namespace Global;

inline bool inRectCheck(QPoint point, QRect rect) {
    bool x = rect.x() <= point.x() && point.x() <= rect.x() + rect.width();
    bool y = rect.y() <= point.y() && point.y() <= rect.y() + rect.height();
    return x && y;
}

BaseGui::BaseGui(QString arch_type, QWidget* parent, Qt::WindowFlags flags)
    : QMainWindow( parent, flags )
#if QT_VERSION >= 0x050000
	, was_minimized(false)
#endif
{
    this->setWindowTitle(tr("Kylin Video"));
    this->setMouseTracking(true);
    this->setAutoFillBackground(true);
    QWidget::setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    this->setMinimumSize(WINDOW_MIN_WIDTH, WINDOW_MIN_HEIGHT);
    this->resize(900, 600);
    this->setWindowIcon(QIcon(":/res/kylin-video.png"));

    arch = arch_type;
    ignore_show_hide_events = false;
    arg_close_on_finish = -1;
    arg_start_in_fullscreen = -1;
    isFinished = false;//kobe
    isPlaying = false;
    this->resizeFlag = false;
    fullscreen = false;
    core = 0;
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
    playlistWidget = 0;
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
    setCentralWidget(panel);//kobe for QMainWindow 中心窗口部件，panel加载的是视频显示区域
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

    panel->setFocus();

    //top
    m_topToolbar = new TitleWidget(this);
    m_topToolbar->setFixedHeight(TOP_TOOLBAR_HEIGHT);
    this->setMenuWidget(m_topToolbar);

    connect(playlistWidget, SIGNAL(sig_playing_title(QString)), m_topToolbar, SLOT(set_title_name(QString)));
    connect(this, SIGNAL(clear_playing_title()), m_topToolbar, SLOT(clear_title_name()));
    connect(m_topToolbar, SIGNAL(sig_menu()), this, SLOT(slot_menu()));
    connect(m_topToolbar, SIGNAL(sig_min()), this, SLOT(slot_min()));
    connect(m_topToolbar, SIGNAL(sig_close()), this, SLOT(slot_close()));
    connect(m_topToolbar, SIGNAL(sig_max()), this, SLOT(slot_max()));
    connect(m_topToolbar, SIGNAL(mouseMovedDiff(QPoint)), this, SLOT(moveWindowDiff(QPoint)), Qt::QueuedConnection );//kobe 0524

    //bottom
    m_bottomToolbar = new BottomWidget(this);
    m_bottomToolbar->setFixedHeight(BOTTOM_TOOLBAR_HEIGHT);

    connect(m_bottomToolbar, SIGNAL(sig_resize_corner()), this, SLOT(slot_resize_corner()));
    connect(m_bottomToolbar, SIGNAL(volumeChanged(int)), core, SLOT(setVolume(int)));
    connect(core, SIGNAL(volumeChanged(int)), m_bottomToolbar, SIGNAL(valueChanged(int)));
    connect(m_bottomToolbar, SIGNAL(toggleFullScreen()), this, SLOT(slot_set_fullscreen()));//0621
    connect(m_bottomToolbar, SIGNAL(togglePlaylist()), this, SLOT(slot_playlist()));
    connect(this, SIGNAL(timeChanged(QString, QString)),m_bottomToolbar, SLOT(displayTime(QString, QString)));
    connect(m_bottomToolbar, SIGNAL(signal_stop()), core, SLOT(stop()));
    connect(m_bottomToolbar, SIGNAL(signal_prev()), playlistWidget, SLOT(playPrev()));
    connect(m_bottomToolbar, SIGNAL(signal_play_pause_status()), core, SLOT(play_or_pause()));
    connect(m_bottomToolbar, SIGNAL(signal_next()), playlistWidget, SLOT(playNext()));
    connect(m_bottomToolbar, SIGNAL(signal_open_file()), this, SLOT(openFile()));
    connect(m_bottomToolbar, SIGNAL(signal_mute(/*bool*/)), this, SLOT(slot_mute(/*bool*/)));
    connect(this, SIGNAL(sigActionsEnabled(bool)), m_bottomToolbar, SLOT(setActionsEnabled(bool)));
    connect(this, SIGNAL(setPlayOrPauseEnabled(bool)), m_bottomToolbar, SLOT(setPlayOrPauseEnabled(bool)));
    connect(this, SIGNAL(setStopEnabled(bool)), m_bottomToolbar, SLOT(setStopEnabled(bool)));
    connect(this, SIGNAL(change_playlist_btn_status(bool)), m_bottomToolbar, SLOT(slot_playlist_btn_status(bool)));
    //progress
    connect(m_bottomToolbar, SIGNAL(posChanged(int)), core, SLOT(goToPosition(int)));
    connect(core, SIGNAL(positionChanged(int)), m_bottomToolbar, SLOT(setPos(int)));
    connect(m_bottomToolbar, SIGNAL(draggingPos(int)), this, SLOT(displayGotoTime(int)));
    connect(m_bottomToolbar, SIGNAL(delayedDraggingPos(int)), this, SLOT(goToPosOnDragging(int)));
    connect(m_bottomToolbar, SIGNAL(wheelUp()), core, SLOT(wheelUp()));
    connect(m_bottomToolbar, SIGNAL(wheelDown()), core, SLOT(wheelDown()));
    connect(m_bottomToolbar, SIGNAL(mouseMovedDiff(QPoint)), this, SLOT(moveWindowDiff(QPoint)), Qt::QueuedConnection );//kobe 0524
    connect(core, SIGNAL(newDuration(double)), m_bottomToolbar, SLOT(setDuration(double)));
    connect(m_bottomToolbar, SIGNAL(sig_show_or_hide_esc(bool)), this, SLOT(showOrHideEscWidget(bool)));
    connect(playlistWidget, SIGNAL(update_playlist_count(int)), m_bottomToolbar, SLOT(update_playlist_count_label(int)));
    connect(m_bottomToolbar, SIGNAL(need_to_save_pre_image(int)), this, SLOT(ready_save_pre_image(int)));
    connect(this, SIGNAL(send_save_preview_image_name(int,QString)), m_bottomToolbar, SIGNAL(send_save_preview_image_name(int,QString)));
    resizeCorner = new QPushButton(m_bottomToolbar);
    resizeCorner->setFocusPolicy(Qt::NoFocus);
    resizeCorner->setStyleSheet("QPushButton{background-image:url(':/res/dragbar_normal.png');border:0px;}QPushButton:hover{background-image:url(':/res/dragbar_normal.png')}QPushButton:pressed{background-image:url(':/res/dragbar_normal.png')}");
    resizeCorner->setFixedSize(15, 15);
    resizeCorner->move(m_bottomToolbar->width()-15, m_bottomToolbar->height()-15);
    resizeCorner->installEventFilter(this);

    mainwindow_pos = pos();

    tipWidget = new TipWidget("Hello, Kylin!", this);
    tipWidget->setFixedHeight(30);
    tipWidget->move(10, TOP_TOOLBAR_HEIGHT);
    tipWidget->hide();

    contentLayout = new QStackedLayout(panel);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setMargin(0);
    contentLayout->setSpacing(0);
    contentLayout->addWidget(m_topToolbar);
    contentLayout->addWidget(mplayerwindow);
    contentLayout->addWidget(m_bottomToolbar);

    m_topToolbar->show();
    mplayerwindow->show();
    m_bottomToolbar->show();
    m_bottomToolbar->setFocus();

    this->setActionsEnabled(false);
    if (playlistWidget->count() > 0) {
        emit this->setPlayOrPauseEnabled(true);
        m_bottomToolbar->update_playlist_count_label(playlistWidget->count());
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
    connect(play_mask, SIGNAL(signal_play_continue()), core, SLOT(play_or_pause()));

    tip_timer = new QTimer(this);
    connect(tip_timer, SIGNAL(timeout()), this, SLOT(hideTipWidget()));
    tip_timer->setInterval(2000);

    initializeGui();
}

//0829
/*void BaseGui::onShowControls()
{
    if (turned_on) {
        if (!m_topToolbar->isVisible()) {
            m_topToolbar->showSpreadAnimated();
        }
        if (!m_bottomToolbar->getCtrlWidgetVisible()) {
            m_bottomToolbar->showSpreadAnimated();
        }
    }
}

bool BaseGui::mouseInControlsArea() {
    QPoint mousePos = QCursor::pos();//getCursorPos
    bool mouseInTitleBar = inRectCheck(QPoint(mousePos.x() - this->x(), mousePos.y() - this->y()),
                                        QRect(0, 0, this->width(), m_topToolbar->height()));
    bool mouseInControlBar = inRectCheck(QPoint(mousePos.x() - this->x(), mousePos.y() - this->y()),
                                        QRect(0, this->height() - m_bottomToolbar->height(),
                                                this->width(), m_bottomToolbar->height()));

    return mouseInTitleBar || mouseInControlBar;
}
void BaseGui::checkUnderMouse() {
//    if (!underMouse()) {
        if (m_topToolbar->isVisible()) {
            m_topToolbar->showGatherAnimated();
        }
        if (m_bottomToolbar->getCtrlWidgetVisible()) {
            this->showOrHideEscWidget(false);
            m_bottomToolbar->showGatherAnimated();
        }
//    }
}
void BaseGui::activate_timer() {
    turned_on = true;
    if (mouse_timer->isActive())
        mouse_timer->stop();
    mouse_timer->start();
}
//void BaseGui::deactivate_timer() {
//    turned_on = false;
//    if (mouse_timer->isActive())
//        mouse_timer->stop();
//    m_topToolbar->showSpreadAnimated();
//    m_bottomToolbar->showSpreadAnimated();
//}
void BaseGui::showAlways() {
    turned_on = false;
    if (mouse_timer->isActive())
        mouse_timer->stop();
    m_topToolbar->showAlways();
    m_bottomToolbar->showAlways();
}*/

/*void BaseGui::enterEvent(QEvent *event) {
    this->mouseIn = true;
//    if (turned_on) {
//    if (!m_topToolbar->isVisible()) {
//        qDebug() << "------------BaseGui::enterEvent------------111";
//        m_topToolbar->showWidget();
//    }
//    if (!m_bottomToolbar->getCtrlWidgetVisible()) {
//        qDebug() << "------------BaseGui::enterEvent------------222";
//        m_bottomToolbar->showWidget();
//    }
//    }
    QWidget::enterEvent(event);
}
void BaseGui::leaveEvent(QEvent *event) {
    this->mouseIn = false;
    qDebug() << "------------BaseGui::leaveEvent------------";
    QWidget::leaveEvent(event);
}*/


void BaseGui::setTransparent(bool transparent) {
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

void BaseGui::set_widget_opacity(const float &opacity) {
    QGraphicsOpacityEffect* opacityEffect = new QGraphicsOpacityEffect;
    this->setGraphicsEffect(opacityEffect);
    opacityEffect->setOpacity(opacity);
}

//void BaseGui::showShortcuts()
//{
//    shortcuts_widget->setPrefData(pref);
//    shortcuts_widget->show();
//    shortcuts_widget->restart_timer();
//}

void BaseGui::keyPressEvent(QKeyEvent *event) {
//    if (event->key() == Qt::Key_Up) {
//    }
//    if (event->key() == Qt::Key_Down) {
//    }
    if (event->key() == Qt::Key_Space) {
        this->leftClickFunction();
    }
    if (event->key() == Qt::Key_Escape) {
        if (this->fullscreen) {
            toggleFullscreen(false);
        }
    }
//    if (event->key() == Qt::Key_Question) {
//        this->showShortcuts();
//    }
    QMainWindow::keyPressEvent(event);
}

void BaseGui::slot_mute(/*bool b*/) {
    bool muted = core->getMute();
    core->mute(!muted);
    if (muted && core->getVolumn() <= 0) {//0620
        core->setVolume(50, true);
    }
    updateWidgets();
}

void BaseGui::reset_mute_button() {
    bool muted = core->getMute();
    m_bottomToolbar->onMutedChanged(muted, core->getVolumn());//0519
}

void BaseGui::slot_max() {
//    if (!this->isMaximized()) {
//            this->showMaximized();
//            m_topToolbar->show_max_or_normal_button(false);
//        }
    /*if(maxOrNormal){
        setWindowState( Qt::WindowMaximized);
    }else {
        setWindowState( Qt::WindowNoState);
    }
    maxOrNormal = !maxOrNormal;*/

    if (!this->isMaximized()) {
        m_topToolbar->update_max_status(true);
        this->showMaximized();
    }
    else {
        m_topToolbar->update_max_status(false);
        this->showNormal();
    }

    /*if (this->isMaximized()) {
//        if (window()->isMaximized())
//            window()->showNormal();
        m_topToolbar->update_max_status(false);
        this->showNormal();
    }
    else {
        m_topToolbar->update_max_status(true);
        this->showMaximized();
    }
    update();*/
}

void BaseGui::slot_min() {
    /*if( windowState() != Qt::WindowMinimized ){
        setWindowState( Qt::WindowMinimized );
    }*/
    this->showMinimized();
}

void BaseGui::slot_menu() {
    QPoint p = rect().topRight();
    p.setX(p.x() - 38*4);
    p.setY(p.y() + 36);
    main_popup->exec(this->mapToGlobal(p));
//    main_popup->move(QCursor::pos());
//    main_popup->show();
}

void BaseGui::slot_close() {
//    qApp->quit();
    this->quit();
}

void BaseGui::initializeGui() {
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
void BaseGui::handleMessageFromOtherInstances(const QString& message) {
//	qDebug("BaseGui::handleMessageFromOtherInstances: '%s'", message.toUtf8().constData());
	int pos = message.indexOf(' ');
	if (pos > -1) {
		QString command = message.left(pos);
		QString arg = message.mid(pos+1);
//		qDebug("command: '%s'", command.toUtf8().constData());
//		qDebug("arg: '%s'", arg.toUtf8().constData());

		if (command == "open_file") {
			emit openFileRequested();
			open(arg);
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

BaseGui::~BaseGui() {
    this->clearMplayerLog();//add by kobe
    if (core) {
        delete core; // delete before mplayerwindow, otherwise, segfault...
        core = 0;
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
    if (playlistWidget) {
        delete playlistWidget;
        playlistWidget = 0;
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

void BaseGui::createActionsAndMenus() {
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
    connect(playPrevAct, SIGNAL(triggered()), playlistWidget, SLOT(playPrev()));
    playNextAct = new MyAction(Qt::Key_Greater, this, "play_next");
    playNextAct->addShortcut(Qt::Key_MediaNext); // MCE remote key
    connect(playNextAct, SIGNAL(triggered()), playlistWidget, SLOT(playNext()));
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

    subtitlesMenu = new QMenu(this);
    subtitlesMenu->menuAction()->setText( tr("Subtitles") );
//    subtitlesMenuAct->setIcon(Images::icon("subtitles_menu"));
    loadSubsAct = new MyAction(this, "load_subs" );
    connect(loadSubsAct, SIGNAL(triggered()), this, SLOT(loadSub()));
    loadSubsAct->change(tr("Load..."));
    subVisibilityAct = new MyAction(Qt::Key_V, this, "subtitle_visibility");
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

void BaseGui::createTrayActions() {
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

void BaseGui::addTrayActions() {
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

void BaseGui::createHiddenActions() {
    playlist_action = new MyAction(QKeySequence("F3"), this, "playlist_open_close");
    playlist_action->change(tr("PlayList"));
    connect(playlist_action, SIGNAL(triggered()), this, SLOT(slot_playlist()));

    play_pause_aciton = new MyAction(QKeySequence(Qt::Key_Space), this, "play_pause");
    play_pause_aciton->change(tr("Play/Pause"));
    connect(playlist_action, SIGNAL(triggered()), core, SLOT(play_or_pause()));

    stopAct = new MyAction(Qt::Key_MediaStop, this, "stop");
    stopAct->change(tr("Stop"));
    connect(stopAct, SIGNAL(triggered()), core, SLOT(stop()));

    fullscreenAct = new MyAction(QKeySequence("Ctrl+Return"), this, "fullscreen");
    fullscreenAct->change(tr("Fullscreen"));
    connect(fullscreenAct, SIGNAL(triggered()), this, SLOT(slot_set_fullscreen()));
}

void BaseGui::setActionsEnabled(bool b) {
//    qDebug() << "BaseGui::setActionsEnabled  " << b;
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

void BaseGui::start_top_and_bottom_timer() {
//    this->activate();//0830
    m_topToolbar->activate();
    m_bottomToolbar->activate();
}

void BaseGui::enableActionsOnPlaying() {
    this->setActionsEnabled(true);

    isPlaying = true;
//    QTimer::singleShot(100, this, SLOT(start_top_and_bottom_timer()));
    start_top_and_bottom_timer();
//    this->activate_timer();

    // Screenshot option
    bool screenshots_enabled = ((pref->use_screenshot) &&
                                 (!pref->screenshot_directory.isEmpty()) &&
                                 (QFileInfo(pref->screenshot_directory).isDir()));
    screenshotAct->setEnabled(screenshots_enabled);

    // Disable audio actions if there's not audio track
    if ((core->mdat.audios.numItems()==0) && (core->mset.external_audio.isEmpty())) {
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

    if (pref->hwdec.startsWith("vdpau") && pref->mplayer_bin.contains("mpv")) {//kobe 20170706
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

void BaseGui::disableActionsOnStop() {
//    qDebug("BaseGui::disableActionsOnStop");
    this->setActionsEnabled(false);//kobe:此处会让一些按钮处于禁用状态
    emit this->setPlayOrPauseEnabled(true);

//    this->showAlways();//0830
    m_topToolbar->showAlways();
    m_bottomToolbar->showAlways();

    isPlaying = false;
    m_topToolbar->enable_turned_on();//0519
    isFinished = true;//kobe
//    turned_on = true;//0829
}

void BaseGui::togglePlayAction(Core::State state) {
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

void BaseGui::setJumpTexts() {
    rewind1Act->change( tr("-%1").arg(Helper::timeForJumps(pref->seeking1)) );
    rewind2Act->change( tr("-%1").arg(Helper::timeForJumps(pref->seeking2)) );
    rewind3Act->change( tr("-%1").arg(Helper::timeForJumps(pref->seeking3)) );

    forward1Act->change( tr("+%1").arg(Helper::timeForJumps(pref->seeking1)) );
    forward2Act->change( tr("+%1").arg(Helper::timeForJumps(pref->seeking2)) );
    forward3Act->change( tr("+%1").arg(Helper::timeForJumps(pref->seeking3)) );
}

void BaseGui::createCore() {
    core = new Core(mplayerwindow, this);
    connect(core, SIGNAL(widgetsNeedUpdate()), this, SLOT(updateWidgets()));
    connect(core, SIGNAL(showFrame(int)), this, SIGNAL(frameChanged(int)));
    connect(core, SIGNAL(ABMarkersChanged(int,int)), this, SIGNAL(ABMarkersChanged(int,int)));
    connect(core, SIGNAL(showTime(double, bool)), this, SLOT(gotCurrentTime(double, bool)));
    connect(core, SIGNAL(needResize(int, int)), this, SLOT(resizeWindow(int,int)));
    connect(core, SIGNAL(showMessage(QString)), this, SLOT(displayMessage(QString)));
    connect(core, SIGNAL(stateChanged(Core::State)), this, SLOT(displayState(Core::State)));
    connect(core, SIGNAL(stateChanged(Core::State)), this, SLOT(checkStayOnTop(Core::State)), Qt::QueuedConnection);
    connect(core, SIGNAL(mediaStartPlay()), this, SLOT(enterFullscreenOnPlay()), Qt::QueuedConnection);
    connect(core, SIGNAL(mediaStoppedByUser()), this, SLOT(exitFullscreenOnStop()));
    connect(core, SIGNAL(mediaStoppedByUser()), mplayerwindow, SLOT(showLogo()));
    connect(core, SIGNAL(show_logo_signal(bool)), mplayerwindow, SLOT(setLogoVisible(bool)));
    connect(core, SIGNAL(mediaLoaded()), this, SLOT(enableActionsOnPlaying()));
    connect(core, SIGNAL(noFileToPlay()), this, SLOT(gotNoFileToPlay()));
    connect(core, SIGNAL(audioTracksChanged()), this, SLOT(enableActionsOnPlaying()));
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
    connect(core, SIGNAL(noVideo()), mplayerwindow, SLOT(showLogo()));
	// Log mplayer output
    connect(core, SIGNAL(aboutToStartPlaying()), this, SLOT(clearMplayerLog()));
    connect(core, SIGNAL(logLineAvailable(QString)), this, SLOT(recordMplayerLog(QString)));
//    connect(core, SIGNAL(mediaLoaded()), this, SLOT(autosaveMplayerLog()));
	connect(core, SIGNAL(receivedForbidden()), this, SLOT(gotForbidden()));
}

void BaseGui::createMplayerWindow() {
    //20170720
    mplayerwindow = new MplayerWindow(this);
    mplayerwindow->setObjectName("mplayerwindow");
    mplayerwindow->installEventFilter(this);
    mplayerwindow->setColorKey("121212");//121212 mplayerwindow->setColorKey("20202");  0x020202 // pref->color_key kobe:视频显示区域背景色设置为黑色 0d87ca
    mplayerwindow->setContentsMargins(0, 0, 0, 0);
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

void BaseGui::createPlaylist() {
    playlistWidget = new Playlist(core, this, 0);
    playlistWidget->setFixedSize(220, this->height() - TOP_TOOLBAR_HEIGHT - BOTTOM_TOOLBAR_HEIGHT);
    playlistWidget->setViewHeight();
    playlistWidget->move(this->width()-220, TOP_TOOLBAR_HEIGHT);
    playlistWidget->hide();
    connect(playlistWidget, SIGNAL(playlistEnded()), this, SLOT(playlistHasFinished()));
    connect(playlistWidget, SIGNAL(playlistEnded()), mplayerwindow, SLOT(showLogo()));
    connect(playlistWidget, SIGNAL(closePlaylist()), this, SLOT(slot_playlist()));
    connect(playlistWidget, SIGNAL(playListFinishedWithError(QString)), this, SLOT(showErrorFromPlayList(QString)));
    connect(playlistWidget, SIGNAL(showMessage(QString)), this, SLOT(displayMessage(QString)));

//    connect(playlistWidget, SIGNAL(finish_list()),
}

void BaseGui::createPanel() {
    panel = new QWidget(this);
    panel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    panel->setMinimumSize(QSize(1,1));
    panel->setFocusPolicy(Qt::StrongFocus);
}

void BaseGui::createPreferencesDialog() {
    QApplication::setOverrideCursor(Qt::WaitCursor);
    pref_dialog = new PreferencesDialog(arch/*, this*/);
    pref_dialog->setModal(false);
    connect(pref_dialog, SIGNAL(applied()), this, SLOT(applyNewPreferences()));
    QApplication::restoreOverrideCursor();
}

void BaseGui::createFilePropertiesDialog() {
    QApplication::setOverrideCursor(Qt::WaitCursor);
    file_dialog = new FilePropertiesDialog(/*this*/);
    file_dialog->setModal(false);
    connect( file_dialog, SIGNAL(applied()), this, SLOT(applyFileProperties()) );
    QApplication::restoreOverrideCursor();
}

void BaseGui::createAboutDialog() {
    QApplication::setOverrideCursor(Qt::WaitCursor);
    aboutDlg = new AboutDialog();
    aboutDlg->setModal(false);
    QApplication::restoreOverrideCursor();
}

void BaseGui::createHelpDialog() {
    QApplication::setOverrideCursor(Qt::WaitCursor);
    helpDlg = new HelpDialog();
    helpDlg->setModal(false);
    QApplication::restoreOverrideCursor();
}

void BaseGui::slot_playlist() {
    setPlaylistVisible(!playlistWidget->isVisible());
}

void BaseGui::slideEdgeWidget(QWidget *right, QRect start, QRect end, int delay, bool hide) {
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

void BaseGui::disableControl(int delay) {
    QTimer::singleShot(delay, this, SLOT(disableSomeComponent()));
}

void BaseGui::disableSomeComponent() {
    playlistWidget->setEnabled(true);
}

void BaseGui::setPlaylistProperty() {
    playlistWidget->setProperty("moving", false);
}

void BaseGui::setPlaylistVisible(bool visible) {
    playlistWidget->setProperty("moving", true);
    int titleBarHeight = TOP_TOOLBAR_HEIGHT;

    double factor = 0.6;
    QRect start(this->width(), titleBarHeight, playlistWidget->width(), playlistWidget->height());
    QRect end(this->width() - playlistWidget->width(), titleBarHeight, playlistWidget->width(), playlistWidget->height());
    if (!visible) {
        this->slideEdgeWidget(playlistWidget, end, start, ANIMATIONDELAY * factor, true);
        emit this->change_playlist_btn_status(false);
    } else {
        playlistWidget->setFocus();
        this->slideEdgeWidget(playlistWidget, start, end, ANIMATIONDELAY * factor);
        emit this->change_playlist_btn_status(true);
    }
    disableControl(ANIMATIONDELAY * factor);
    m_topToolbar->raise();
    QTimer::singleShot(ANIMATIONDELAY * factor * 1, this, SLOT(setPlaylistProperty()));
}

void BaseGui::slot_resize_corner() {
    resizeCorner->move(m_bottomToolbar->width()- 16, m_bottomToolbar->height() - 16);//0519
}

void BaseGui::slot_set_fullscreen() {
    if (this->fullscreen) {
        toggleFullscreen(false);
    }
    else {
        toggleFullscreen(true);
    }
}

void BaseGui::showPreferencesDialog() {
    exitFullscreenIfNeeded();
	
    if (!pref_dialog) {
        createPreferencesDialog();
    }
    pref_dialog->setData(pref);

    //从最新的配置文件读取快捷键并进行设置
    pref_dialog->mod_shortcut_page()->actions_editor->clear();
    pref_dialog->mod_shortcut_page()->actions_editor->addCurrentActions(this);

    pref_dialog->move((width() - pref_dialog->width()) / 2 +
                               mapToGlobal(QPoint(0, 0)).x(),
                               (window()->height() - pref_dialog->height()) / 2 +
                               mapToGlobal(QPoint(0, 0)).y());
    pref_dialog->show();
}

// The user has pressed OK in preferences dialog
void BaseGui::applyNewPreferences() {
    PlayerID::Player old_player_type = PlayerID::player(pref->mplayer_bin);
    pref_dialog->getData(pref);
    m_bottomToolbar->setPreviewData(pref->preview_when_playing);
    mplayerwindow->activateMouseDragTracking(true/*pref->move_when_dragging*/);
	setJumpTexts(); // Update texts in menus
	updateWidgets(); // Update the screenshot action

    // Restart the video if needed
    if (pref_dialog->requiresRestart())
        core->restart();

    // Update actions
    pref_dialog->mod_shortcut_page()->actions_editor->applyChanges();
    saveActions();
	pref->save();

    if (old_player_type != PlayerID::player(pref->mplayer_bin)) {
//        qDebug("BaseGui::applyNewPreferences: player changed!");
        // Hack, simulate a change of GUI to restart the interface
        // FIXME: try to create a new Core::proc in the future
        core->stop();
        if (pref_dialog && pref_dialog->isVisible()) {//add by kobe to hide the pref_dialog
            pref_dialog->accept();
        }
        emit guiChanged();
    }
}


void BaseGui::showFilePropertiesDialog() {
//    qDebug("BaseGui::showFilePropertiesDialog");

    exitFullscreenIfNeeded();

    if (!file_dialog) {
        createFilePropertiesDialog();
    }

    setDataToFileProperties();

    file_dialog->show();
}

void BaseGui::setDataToFileProperties() {
    InfoReader *i = InfoReader::obj();
    i->getInfo();
    file_dialog->setCodecs( i->vcList(), i->acList(), i->demuxerList() );

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
    file_dialog->setMediaData(core->mdat);
}

void BaseGui::applyFileProperties() {
//    qDebug("BaseGui::applyFileProperties");

	bool need_restart = false;

#undef TEST_AND_SET
#define TEST_AND_SET( Pref, Dialog ) \
    if ( Pref != Dialog ) { Pref = Dialog; need_restart = true; qDebug("BaseGui::applyFileProperties====================================111");}

    bool demuxer_changed = false;

    QString prev_demuxer = core->mset.forced_demuxer;

    if (prev_demuxer != core->mset.forced_demuxer) {
        // Demuxer changed
        demuxer_changed = true;
        core->mset.current_audio_id = MediaSettings::NoneSelected;
        core->mset.current_sub_id = MediaSettings::NoneSelected;
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


void BaseGui::updateMediaInfo()
{
//	qDebug("BaseGui::updateMediaInfo");
    tray->setToolTip( windowTitle() );
	emit videoInfoChanged(core->mdat.video_width, core->mdat.video_height, core->mdat.video_fps.toDouble());
}

void BaseGui::newMediaLoaded()
{
	QString stream_title = core->mdat.stream_title;
//	qDebug("BaseGui::newMediaLoaded: mdat.stream_title: %s", stream_title.toUtf8().constData());

	if (!stream_title.isEmpty()) {
		pref->history_recents->addItem( core->mdat.filename, stream_title );
		//pref->history_recents->list();
	} else {
		pref->history_recents->addItem( core->mdat.filename );
	}
	updateRecents();

    QFileInfo fi(core->mdat.filename);
    if (fi.exists()) {
        QString name = fi.fileName();
        m_topToolbar->set_title_name(name);
    }

	// Automatically add files to playlist
    if ((core->mdat.type == TYPE_FILE) /*&& (pref->auto_add_to_playlist)*/) {
		//qDebug("BaseGui::newMediaLoaded: playlist count: %d", playlist->count());
        QStringList files_to_add;
        if (playlistWidget->count() == 1) {
            files_to_add = Helper::filesForPlaylist(core->mdat.filename, pref->media_to_add_to_playlist);
        }
        if (!files_to_add.empty()) playlistWidget->addFiles(files_to_add);
	}
}

void BaseGui::gotNoFileToPlay() {
    playlistWidget->resumePlay();//当前播放的文件不存在时，去播放下一个
}

void BaseGui::clearMplayerLog() {
    mplayer_log.clear();
}

void BaseGui::recordMplayerLog(QString line) {
    if (pref->log_mplayer) {
        if ( (line.indexOf("A:")==-1) && (line.indexOf("V:")==-1) ) {
            line.append("\n");
            mplayer_log.append(line);
        }
    }
}

void BaseGui::updateRecents() {
//	qDebug("BaseGui::updateRecents");
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

void BaseGui::clearRecentsList() {
    MessageDialog msgDialog(0, tr("Confirm deletion - Kylin Video"), tr("Delete the list of recent files?"));
    if (msgDialog.exec() != -1) {
        if (msgDialog.standardButton(msgDialog.clickedButton()) == QMessageBox::Ok) {
            // Delete items in menu
            pref->history_recents->clear();
            updateRecents();
        }
    }
}

void BaseGui::updateWidgets() {
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
}

void BaseGui::openRecent() {
	QAction *a = qobject_cast<QAction *> (sender());
	if (a) {
		int item = a->data().toInt();
//		qDebug("BaseGui::openRecent: %d", item);
		QString file = pref->history_recents->item(item);
        QFileInfo fi(file);
        if (fi.exists()) {
            playlistWidget->addFile(file, Playlist::NoGetInfo);//20170712
            open(file);
        }
        else {
            this->showErrorFromPlayList(file);
        }
	}
}

void BaseGui::open(QString file) {
//    qDebug("BaseGui::open: '%s'", file.toUtf8().data());
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
        core->open(file/*, 0*/);//每次从头开始播放文件
	}

	if (QFile::exists(file)) pref->latest_dir = QFileInfo(file).absolutePath();
}

void BaseGui::openFiles(QStringList files) {
//	qDebug("BaseGui::openFiles");
	if (files.empty()) return;

	if (files.count()==1) {
        playlistWidget->addFile(files[0], Playlist::NoGetInfo);//20170712
        open(files[0]);
	} else {
        playlistWidget->addFiles(files);
        open(files[0]);
	}
}

void BaseGui::openFile() {
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

void BaseGui::openFile(QString file) {
//    qDebug("BaseGui::openFile: '%s'", file.toUtf8().data());
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
            core->open(file/*, 0*/);//每次从头开始播放文件
		}
        else {//kobe:打开一个本地视频文件
            core->openFile(file);//开始播放新打开的本地视频文件
            playlistWidget->addFile(file, Playlist::NoGetInfo);//将新打开的本地视频文件加入到播放列表中
		}
		if (QFile::exists(file)) pref->latest_dir = QFileInfo(file).absolutePath();
	}
}

void BaseGui::openDirectory() {
	qDebug("BaseGui::openDirectory");

	QString s = MyFileDialog::getExistingDirectory(
                    this, tr("Choose a directory"),
                    pref->latest_dir );

	if (!s.isEmpty()) {
		openDirectory(s);
	}
}

void BaseGui::openDirectory(QString directory) {
//	qDebug("BaseGui::openDirectory: '%s'", directory.toUtf8().data());
	if (Helper::directoryContainsDVD(directory)) {
		core->open(directory);
	} 
	else {
		QFileInfo fi(directory);
		if ( (fi.exists()) && (fi.isDir()) ) {
            //playlistWidget->clear();
            //playlistWidget->addDirectory(directory);
            playlistWidget->addDirectory(fi.absoluteFilePath());
            playlistWidget->startPlay();
		} else {
			qDebug("BaseGui::openDirectory: directory is not valid");
		}
	}
}

void BaseGui::openURL() {
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

void BaseGui::openURL(QString url) {
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

void BaseGui::loadSub() {
	qDebug("BaseGui::loadSub");

	exitFullscreenIfNeeded();

	Extensions e;
    QString s = MyFileDialog::getOpenFileName(
        this, tr("Choose a file"), 
	    pref->latest_dir, 
        tr("Subtitles") + e.subtitles().forFilter()+ ";;" +
        tr("All files") +" (*.*)" );

	if (!s.isEmpty()) core->loadSub(s);
}

void BaseGui::setInitialSubtitle(const QString & subtitle_file) {
	qDebug("BaseGui::setInitialSubtitle: '%s'", subtitle_file.toUtf8().constData());

	core->setInitialSubtitle(subtitle_file);
}

void BaseGui::loadAudioFile() {
	qDebug("BaseGui::loadAudioFile");

	exitFullscreenIfNeeded();

	Extensions e;
	QString s = MyFileDialog::getOpenFileName(
        this, tr("Choose a file"), 
	    pref->latest_dir, 
        tr("Audio") + e.audio().forFilter()+";;" +
        tr("All files") +" (*.*)" );

	if (!s.isEmpty()) core->loadAudioFile(s);
}

void BaseGui::setDataToAboutDialog() {
    aboutDlg->setVersions();
}

void BaseGui::showAboutDialog() {
    if (!aboutDlg) {
        createAboutDialog();
    }
    setDataToAboutDialog();
    int w_x = this->frameGeometry().topLeft().x() + (this->width() / 2) - (438  / 2);
    int w_y = this->frameGeometry().topLeft().y() + (this->height() /2) - (320  / 2);
    aboutDlg->move(w_x, w_y);
    aboutDlg->show();
}

void BaseGui::showHelpDialog() {
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

void BaseGui::showGotoDialog() {
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

void BaseGui::showAudioDelayDialog() {
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

void BaseGui::showSubDelayDialog() {
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

void BaseGui::setStayOnTop(bool b) {
//    qDebug("BaseGui::setStayOnTop: %d", b);
    if ( (b && (windowFlags() & Qt::WindowStaysOnTopHint)) ||
         (!b && (!(windowFlags() & Qt::WindowStaysOnTopHint))) )
    {
        // identical do nothing
//        qDebug("BaseGui::setStayOnTop: nothing to do");
        return;
    }

    ignore_show_hide_events = true;

    bool visible = isVisible();

    QPoint old_pos = pos();

    if (b) {
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

void BaseGui::changeStayOnTop(int stay_on_top) {
    switch (stay_on_top) {
        case Preferences::AlwaysOnTop : setStayOnTop(true); break;
        case Preferences::NeverOnTop  : setStayOnTop(false); break;
        case Preferences::WhilePlayingOnTop : setStayOnTop((core->state() == Core::Playing)); break;
    }

    pref->stay_on_top = (Preferences::OnTop) stay_on_top;
//    updateWidgets();
    // Stay on top
    onTopActionGroup->setChecked((int)pref->stay_on_top);
}

void BaseGui::checkStayOnTop(Core::State state) {
//    qDebug("BaseGui::checkStayOnTop");
    if ((!pref->fullscreen) && (pref->stay_on_top == Preferences::WhilePlayingOnTop)) {
        setStayOnTop((state == Core::Playing));
    }
}

void BaseGui::changePlayOrder(int play_order) {
    pref->play_order = (Preferences::PlayOrder) play_order;
//    updateWidgets();
    playOrderActionGroup->setChecked((int)pref->play_order);
}

void BaseGui::exitFullscreen() {
	if (pref->fullscreen) {
		toggleFullscreen(false);
	}
}

void BaseGui::toggleFullscreen(bool b) {
    if (b==this->fullscreen) {//kobe:b==pref->fullscreen
        // Nothing to do
        qDebug("BaseGui::toggleFullscreen: nothing to do, returning");
        return;
    }

    this->fullscreen = b;

    mplayerwindow->hideLogoForTemporary();

    if (this->fullscreen) {
        m_topToolbar->showAlways();
        m_bottomToolbar->showAlways();
//        m_topToolbar->hide();
//        m_bottomToolbar->hide();
//        this->showAlways();
        was_maximized = isMaximized();
        showFullScreen();
        this->mplayerwindow->resize(QMainWindow::size());
        m_bottomToolbar->onFullScreen();//让全屏/取消全屏的按钮更换图片为取消全屏
        this->resizeCorner->hide();
//        if (core->state())//0811
//        QTimer::singleShot(100, this, SLOT(start_top_and_bottom_timer()));
        QString state = core->stateToString().toUtf8().data();
//        this->showAlways();
        if (state == "Playing" || state == "Paused") {//全屏的时候如果不是正在播放或暂停，则显示标题栏和控制栏
//            this->activate_timer();
            start_top_and_bottom_timer();
        }
        //kobe:全屏的时候让所有视频大小的设置菜单禁用
        // Make all not checkable
//        sizeGroup->setActionsEnabled(false);
//        doubleSizeAct->setEnabled(false);
    }
    else {
//        this->showAlways();
        m_topToolbar->showAlways();
        m_bottomToolbar->showAlways();
        showNormal();
        if (was_maximized) showMaximized(); // It has to be called after showNormal()
        this->mplayerwindow->resize(QMainWindow::size());
        m_bottomToolbar->onUnFullScreen();//让全屏/取消全屏的按钮更换图片为全屏//0519
        this->resizeCorner->show();//0519
        if (this->escWidget->isVisible())
            this->escWidget->hide();

        QString state = core->stateToString().toUtf8().data();
        if (state == "Playing" || state == "Paused") {//kobe: Stopped Playing Paused
    //        QTimer::singleShot(100, this, SLOT(start_top_and_bottom_timer()));
//            this->activate_timer();
            start_top_and_bottom_timer();
        }
        //kobe:退出全屏的时候如果视频仍然在播放，则让所有视频大小的设置菜单恢复使用
        // Make all checkable
//        if (isFinished == false) {
//            sizeGroup->setActionsEnabled(true);
//            doubleSizeAct->setEnabled(true);
//        }
    }
    updateWidgets();
    setFocus(); // Fixes bug #2493415
    QTimer::singleShot(100, mplayerwindow, SLOT(update_logo_pos()));
}

void BaseGui::leftClickFunction() {
    if (playlistWidget->isVisible()) {//20170713
        setPlaylistVisible(false);
    }
    //kobe
    QString state = core->stateToString().toUtf8().data();
    if (state == "Playing" || state == "Paused") {//kobe: Stopped Playing Paused
        core->play_or_pause();
    }
//	if (!pref->mouse_left_click_function.isEmpty()) {
//		processFunction(pref->mouse_left_click_function);
//	}
}

void BaseGui::rightClickFunction() {
    showPopupMenu();//kobe
//	if (!pref->mouse_right_click_function.isEmpty()) {
//        processFunction(pref->mouse_right_click_function);
//	}
}

void BaseGui::doubleClickFunction() {
    this->slot_set_fullscreen();//kobe
//	if (!pref->mouse_double_click_function.isEmpty()) {
//		processFunction(pref->mouse_double_click_function);
//	}
}

void BaseGui::middleClickFunction() {
    processFunction("mute");//kobe
//	if (!pref->mouse_middle_click_function.isEmpty()) {
//		processFunction(pref->mouse_middle_click_function);
//	}
}

void BaseGui::xbutton1ClickFunction() {
	qDebug("BaseGui::xbutton1ClickFunction");

//	if (!pref->mouse_xbutton1_click_function.isEmpty()) {
//		processFunction(pref->mouse_xbutton1_click_function);
//	}
}

void BaseGui::xbutton2ClickFunction() {
	qDebug("BaseGui::xbutton2ClickFunction");

//	if (!pref->mouse_xbutton2_click_function.isEmpty()) {
//		processFunction(pref->mouse_xbutton2_click_function);
//	}
}

void BaseGui::processFunction(QString function) {
//    qDebug("##########################BaseGui::processFunction: '%s'", function.toUtf8().data());

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
    if (!action) action = ActionsEditor::findAction(playlistWidget, function);

    if (action) {
        qDebug("BaseGui::processFunction: action found");

        if (!action->isEnabled()) {
            qDebug("BaseGui::processFunction: action is disabled, doing nothing");
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
        qDebug("BaseGui::processFunction: action not found");
    }
}

void BaseGui::gotForbidden() {
	qDebug("BaseGui::gotForbidden");

	static bool busy = false;

	if (busy) return;

	busy = true;

    QMessageBox::warning(this, tr("Error detected"),
        tr("Unfortunately this video can't be played.") +"<br>"+
        tr("The server returned '%1'").arg("403: Forbidden"));
	busy = false;
}

void BaseGui::dragEnterEvent( QDragEnterEvent *e ) {
//	qDebug("BaseGui::dragEnterEvent");

	if (e->mimeData()->hasUrls()) {
		e->acceptProposedAction();
	}
}

void BaseGui::dropEvent( QDropEvent *e ) {
//	qDebug("BaseGui::dropEvent");

	QStringList files;

	if (e->mimeData()->hasUrls()) {
		QList <QUrl> l = e->mimeData()->urls();
		QString s;
		for (int n=0; n < l.count(); n++) {
			if (l[n].isValid()) {
				qDebug("BaseGui::dropEvent: scheme: '%s'", l[n].scheme().toUtf8().data());
				if (l[n].scheme() == "file") 
					s = l[n].toLocalFile();
				else
					s = l[n].toString();
				/*
				qDebug(" * '%s'", l[n].toString().toUtf8().data());
				qDebug(" * '%s'", l[n].toLocalFile().toUtf8().data());
				*/
				qDebug("BaseGui::dropEvent: file: '%s'", s.toUtf8().data());
				files.append(s);
			}
		}
	}

	qDebug( "BaseGui::dropEvent: count: %d", files.count());
	if (files.count() > 0) {
		#ifdef Q_OS_WIN
		files = Helper::resolveSymlinks(files); // Check for Windows shortcuts
		#endif
		files.sort();

		if (files.count() == 1) {
			QFileInfo fi( files[0] );

			Extensions e;
			QRegExp ext_sub(e.subtitles().forRegExp());
			ext_sub.setCaseSensitivity(Qt::CaseInsensitive);
			if (ext_sub.indexIn(fi.suffix()) > -1) {
				qDebug( "BaseGui::dropEvent: loading sub: '%s'", files[0].toUtf8().data());
				core->loadSub( files[0] );
			}
			else
			if (fi.isDir()) {
				openDirectory( files[0] );
			} else {
				//openFile( files[0] );
//				if (pref->auto_add_to_playlist) {
//                    if (playlistWidget->maybeSave()) {
//                        playlistWidget->clear();
                        playlistWidget->addFile(files[0], Playlist::NoGetInfo);//20170712
						open( files[0] );
//                    }
//				} else {
//					open( files[0] );
//				}
			}
		} else {
			// More than one file
            qDebug("BaseGui::dropEvent: adding files to playlist");
//            playlistWidget->clear();
            playlistWidget->addFiles(files);
            //openFile( files[0] );
            playlistWidget->startPlay();
		}
	}
}

void BaseGui::showPopupMenu() {
    showPopupMenu(QCursor::pos());
}

void BaseGui::showPopupMenu( QPoint p ) {
//	//qDebug("BaseGui::showPopupMenu: %d, %d", p.x(), p.y());
    popup->move(p);
    popup->show();
}

// Called when a video has started to play
void BaseGui::enterFullscreenOnPlay() {
//	qDebug("BaseGui::enterFullscreenOnPlay: arg_start_in_fullscreen: %d, pref->start_in_fullscreen: %d", arg_start_in_fullscreen, pref->start_in_fullscreen);
	if (arg_start_in_fullscreen != 0) {
        if ( (arg_start_in_fullscreen == 1)/* || (pref->start_in_fullscreen) */) {
            if (!pref->fullscreen) {
                toggleFullscreen(true);
            }
		}
	}
}

// Called when the playlist has stopped
void BaseGui::exitFullscreenOnStop() {
//    qDebug("BaseGui::exitFullscreenOnStop");
    if (pref->fullscreen) {
		toggleFullscreen(false);
	}
    emit this->clear_playing_title();
}

void BaseGui::playlistHasFinished() {
//	qDebug("BaseGui::playlistHasFinished");
	core->stop();

	exitFullscreenOnStop();
}

void BaseGui::displayState(Core::State state) {
//    qDebug("BaseGui::displayState: %s", core->stateToString().toUtf8().data());
    switch (state) {//0830
        case Core::Playing://播放时开启自动隐藏标题栏和控制栏的定时器
            start_top_and_bottom_timer();
//            this->activate_timer();
            break;
        case Core::Paused://暂停时显示标题栏和控制栏
//            this->showAlways();
            m_bottomToolbar->showAlways();
            m_topToolbar->showAlways();
            break;
        case Core::Stopped:
//            this->showAlways();
            m_bottomToolbar->showAlways();
            m_topToolbar->set_title_name("");
            m_topToolbar->showAlways();
            break;
    }
}

void BaseGui::displayMessage(QString message) {
    this->showTipWidget(message);
}

//kobe 0606 flag=true时表示充值时间显示label的值为初始值
void BaseGui::gotCurrentTime(double sec, bool flag) {
	//qDebug( "DefaultGui::displayTime: %f", sec);
    QString time;
    QString all_time;
    if (flag) {
        time = "00:00:00";
        all_time = " / 00:00:00";
    }
    else {
        static int last_second = 0;
        if (floor(sec)==last_second) return; // Update only once per second
        last_second = (int) floor(sec);
//        time = Helper::formatTime( (int) sec ) + " / " +
//                               Helper::formatTime( (int) core->mdat.duration );
        time = Helper::formatTime((int) sec);
        all_time = " / " +  Helper::formatTime((int) core->mdat.duration);

        //qDebug( " duration: %f, current_sec: %f", core->mdat.duration, core->mset.current_sec);
    }

    emit timeChanged(time, all_time);
}

void BaseGui::loadConfig() {
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

//add by kobe, remove on 20170720 replace by resizeEvent
/*void BaseGui::slot_resize_mainwindow(int w, int h)
{
//    qDebug("==============================aaaBaseGui::slot_resize_mainwindow: %d, %d", w, h);

//    qDebug("BaseGui::slot_resize_mainwindow: done: panel->size: %d, %d",
//           panel->size().width(),
//           panel->size().height() );
//    qDebug("BaseGui::slot_resize_mainwindow: done: mplayerwindow->size: %d, %d",
//           mplayerwindow->size().width(),
//           mplayerwindow->size().height());
//    if (pref->fullscreen) return;
    this->playlistWidget->setFixedSize(220, panel->size().height() - TOP_TOOLBAR_HEIGHT - BOTTOM_TOOLBAR_HEIGHT);//0531
    if (this->playlistWidget->isVisible()) {
        this->playlistWidget->hide();
        emit this->change_playlist_btn_status(false);//0619
    }
    //test kobe
//    setPlaylistVisible(!playlistWidget->isVisible());
    if (m_topToolbar) {
        m_topToolbar->raise();
//        m_topToolbar->resize(width(), TOP_TOOLBAR_HEIGHT);
        m_topToolbar->resize(panel->size().width(), TOP_TOOLBAR_HEIGHT);
//        m_topToolbar->show_title_widget();
//        m_topSeparatorLine->setVisible(true);
    }
    if (m_bottomToolbar) {//0519
//        if (this->isFullScreen())
//            return;
        m_bottomToolbar->raise();
        m_bottomToolbar->resize(panel->size().width(), BOTTOM_TOOLBAR_HEIGHT);
//        qDebug() << "height()="<<height();
//        qDebug() << "m_bottomToolbar->pos().y()="<<m_bottomToolbar->pos().y();
        m_bottomToolbar->move(0, height() - BOTTOM_TOOLBAR_HEIGHT);
        m_bottomToolbar->show_control_widget();
        //kobe:设置一个临界值判断界面拉伸时底部工具栏是全显示模式还是只显示进度条模式,正常拉伸界面有效，但是点击最大化后再切换回正常尺寸显示会有异常，下方会显示不出来
//        int h_interval = qAbs(height() - m_bottomToolbar->pos().y());
//        if (h_interval < 30) {
//            m_bottomToolbar->move(0, height() - 6);
//        }
//        else {
//            m_bottomToolbar->move(0, height() - BOTTOM_TOOLBAR_HEIGHT);
//        }
        resizeCorner->move(m_bottomToolbar->width()- 16, m_bottomToolbar->height() - 16);
    }
    this->update();
//    if (m_topSeparatorLine) {//0519
//        m_topSeparatorLine->resize(window()->width(), 1);
//        m_topSeparatorLine->move(0, TOP_TOOLBAR_HEIGHT);
//    }

//    if (window()->isFullScreen()) {//0519
//        m_topSeparatorLine->setVisible(false);
//    } else {
//        m_topSeparatorLine->setVisible(true);
//    }

//    if (m_btmSeparatorLine) {//0519
////        m_btmSeparatorLine->resize(window()->width(), 1);
////        m_btmSeparatorLine->move(0, window()->height() - m_bottomToolbar->height() - 1);
//        m_btmSeparatorLine->resize(panel->size().width(), 1);
//        m_btmSeparatorLine->move(0, panel->size().height() - BOTTOM_TOOLBAR_HEIGHT - 1);
//    }
}*/

void BaseGui::resizeWindow(int w, int h) {
//    qDebug("BaseGui::resizeWindow: %d, %d", w, h);

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

void BaseGui::resizeMainWindow(int w, int h) {
//	qDebug("BaseGui::resizeWindow: size to scale: %d, %d", w, h);

	QSize video_size(w,h);

	if (video_size == panel->size()) {
		qDebug("BaseGui::resizeWindow: the panel size is already the required size. Doing nothing.");
		return;
	}

	int diff_width = this->width() - panel->width();
	int diff_height = this->height() - panel->height();

	int new_width = w + diff_width;
	int new_height = h + diff_height;

	int minimum_width = minimumSizeHint().width();
	if (new_width < minimum_width) {
		qDebug("BaseGui::resizeWindow: width is too small, setting width to %d", minimum_width);
		new_width = minimum_width;
	}

//	qDebug("BaseGui::resizeWindow: new_width: %d new_height: %d", new_width, new_height);
	resize(new_width, new_height);
}

void BaseGui::centerWindow() {//kobe:播放后界面调整至屏幕中央
    if (/*pref->center_window && */!pref->fullscreen && isVisible()) {
		QRect r = QApplication::desktop()->screenGeometry(this);
		// r.setX(500); r.setY(150); // Test
//		qDebug() << "BaseGui::centerWindow: desktop rect:" << r;
		int x = r.x() + ((r.width() - width()) / 2);
		int y = r.y() + ((r.height() - height()) / 2);
		move(x, y);
	}
}

void BaseGui::displayGotoTime(int t) {
    int jump_time = (int)core->mdat.duration * t / SEEKBAR_RESOLUTION;
    QString s = tr("Jump to %1").arg( Helper::formatTime(jump_time) );
    if (pref->fullscreen) {
        core->displayTextOnOSD( s );
    }
}

void BaseGui::goToPosOnDragging(int t) {
//		#ifdef SEEKBAR_RESOLUTION
//		core->goToPosition(t);
//		#else
//		core->goToPos(t);
//		#endif
}

// Called when a new window (equalizer, preferences..) is opened.
void BaseGui::exitFullscreenIfNeeded() {
	if (pref->fullscreen) {
		toggleFullscreen(false);
	}
}

void BaseGui::loadActions() {
//    qDebug("BaseGui::loadActions");
    ActionsEditor::loadFromConfig(this, settings);
//	actions_list = ActionsEditor::actionsNames(this);
}

void BaseGui::saveActions() {
//    qDebug("BaseGui::saveActions");
    ActionsEditor::saveToConfig(this, settings);
}

void BaseGui::moveWindowDiff(QPoint diff) {
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
//        qDebug() << "BaseGui::moveWindowDiff: new_pos:" << new_pos;
		move(new_pos);
		count = 0;
		d = QPoint(0,0);
	}
#else
	move(pos() + diff);
#endif
}

#if QT_VERSION < 0x050000
void BaseGui::showEvent( QShowEvent * ) {
	qDebug("BaseGui::showEvent");

	if (ignore_show_hide_events) return;

	//qDebug("BaseGui::showEvent: pref->pause_when_hidden: %d", pref->pause_when_hidden);
	if ((pref->pause_when_hidden) && (core->state() == Core::Paused)) {
		qDebug("BaseGui::showEvent: unpausing");
		core->pause(); // Unpauses
	}
}

void BaseGui::hideEvent( QHideEvent * ) {
	qDebug("BaseGui::hideEvent");

	if (ignore_show_hide_events) return;

	//qDebug("BaseGui::hideEvent: pref->pause_when_hidden: %d", pref->pause_when_hidden);
	if ((pref->pause_when_hidden) && (core->state() == Core::Playing)) {
		qDebug("BaseGui::hideEvent: pausing");
		core->pause();
	}
}
#else
// Qt 5 doesn't call showEvent / hideEvent when the window is minimized or unminimized
bool BaseGui::event(QEvent * e) {//kobe 0522 QWidget::paintEngine: Should no longer be called
//    qDebug("BaseGui::event: %d", e->type());

	bool result = QWidget::event(e);
//    if ((ignore_show_hide_events)/* || (!pref->pause_when_hidden)*/) return result;
    if ((ignore_show_hide_events) || (!pref->pause_when_hidden)) return result;//20170627 kobe

	if (e->type() == QEvent::WindowStateChange) {
//		qDebug("BaseGui::event: WindowStateChange");

		if (isMinimized()) {
			was_minimized = true;
			if (core->state() == Core::Playing) {
				qDebug("BaseGui::event: pausing");
				core->pause();
//                this->showAlways();
//                this->deactivate();//0829
//                m_topToolbar->show();
//                m_bottomToolbar->show();
                m_topToolbar->deactivate();
                m_bottomToolbar->deactivate();
			}
		}
	}

    if ((e->type() == QEvent::ActivationChange) && (isActiveWindow())) {
//        qDebug("BaseGui::event: ActivationChange: %d", was_minimized);

        if ((!isMinimized()) && (was_minimized)) {
            was_minimized = false;
            if (core->state() == Core::Paused) {
//                qDebug("BaseGui::showEvent: unpausing");
                core->pause(); // Unpauses

                //kobe:在函数displayState已经处理了
                /*m_topToolbar->deactivate();
                m_bottomToolbar->deactivate();
                QString state = core->stateToString().toUtf8().data();
                if (state == "Playing" || state == "Paused") {//kobe: Stopped Playing Paused
//                    QTimer::singleShot(100, this, SLOT(start_top_and_bottom_timer()));
                    start_top_and_bottom_timer();
                }*/
            }
        }
    }

	return result;
}
#endif

void BaseGui::quit() {
//    qDebug("BaseGui::quit");
    closeWindow();
}

void BaseGui::trayIconActivated(QSystemTrayIcon::ActivationReason reason) {
//    qDebug("DefaultGui::trayIconActivated: %d", reason);

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

void BaseGui::toggleShowAll() {
    // Ignore if tray is not visible
    if (tray->isVisible()) {
        showAll( !isVisible() );
    }
}

void BaseGui::showAll() {
    if (!isVisible()) showAll(true);
}

void BaseGui::showAll(bool b) {
    if (!b) {
        trayicon_playlist_was_visible = playlistWidget->isVisible();
        playlist_pos = playlistWidget->pos();
        playlistWidget->hide();
        mainwindow_pos = pos();
        hide();
    } else {
        // Show all
        move(mainwindow_pos);
        show();
        if (trayicon_playlist_was_visible) {
            playlistWidget->move(playlist_pos);
            playlistWidget->show();
        }
    }
}

void BaseGui::askForMplayerVersion(QString line) {
	qDebug("BaseGui::askForMplayerVersion: %s", line.toUtf8().data());
}

void BaseGui::showErrorFromPlayList(QString errorStr)
{
    ErrorDialog d;
    d.setWindowTitle(tr("%1 Error").arg(PLAYER_NAME));
    d.setTitleText(tr("%1 Error").arg(PLAYER_NAME));
    d.setText(tr("'%1' was not found!").arg(errorStr));
    d.hideDetailBtn();
    d.exec();
}

void BaseGui::showExitCodeFromMplayer(int exit_code) {
	qDebug("BaseGui::showExitCodeFromMplayer: %d", exit_code);

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

void BaseGui::showErrorFromMplayer(QProcess::ProcessError e) {
//	qDebug("BaseGui::showErrorFromMplayer");
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

void BaseGui::showTipWidget(const QString text) {
    if (this->tipWidget->isVisible())
        this->tipWidget->hide();
    if (tip_timer->isActive())
        tip_timer->stop();
    tip_timer->start();
//    this->tipWidget->move(this->pos().x() + 30, this->pos().y() + TOP_TOOLBAR_HEIGHT);
    this->tipWidget->setText(text);
    this->tipWidget->show();
}

void BaseGui::hideTipWidget() {
    this->tipWidget->hide();
}

void BaseGui::showOrHideEscWidget(bool b) {
    if (escWidget) {
        if (b) {
            if (!escWidget->isVisible() && this->isFullScreen()) {
                this->escWidget->show();
            }
        }
        else {
            if (escWidget->isVisible() && this->isFullScreen()) {
                this->escWidget->hide();
            }
        }
    }
}

bool BaseGui::eventFilter(QObject *obj, QEvent *event) {
//    qDebug() << "BaseGui::eventFilter" << obj->objectName();
    if (obj == this->resizeCorner) {
        if (!this->isMaximized()) {
            if (event->type() == QEvent::MouseButtonPress) {
                this->resizeFlag = true;
            }
            else if (event->type() == QEvent::MouseButtonRelease) {
                this->resizeFlag = false;
            }
        }
    }
    /*else if (obj == this->panel || obj == this->mplayerwindow || obj == this->m_bottomToolbar || obj == this->m_topToolbar) {
        QEvent::Type type = event->type();
        if (type != QEvent::MouseButtonPress
            && type != QEvent::MouseButtonRelease
            && type != QEvent::MouseMove) {
            return false;
        }
        QMouseEvent *mouseEvent = dynamic_cast<QMouseEvent*>(event);
        if (!mouseEvent) {
            return false;
        }
        if (mouseEvent->modifiers() != Qt::NoModifier) {
            return false;
        }

        if (event->type() == QEvent::MouseMove && turned_on) {
            if (!m_topToolbar->isVisible()) {
                m_topToolbar->showSpreadAnimated();
            }
            if (!m_bottomToolbar->getCtrlWidgetVisible()) {
                m_bottomToolbar->showSpreadAnimated();
            }
        }
    }*/

    return qApp->eventFilter(obj, event);
}

void BaseGui::mousePressEvent(QMouseEvent *event) {
    switch(event->button()) {
    case Qt::LeftButton:
//        dragPosition = event->globalPos() - this->frameGeometry().topLeft();
        break;
    default:
        QWidget::mousePressEvent(event);
    }
}

void BaseGui::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons() == Qt::LeftButton) {
        if (this->resizeFlag) {//resize
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
        //此处如果移动将会导致一些控件拖动时主界面乱动
        /*else {//# drag move
//            if(dragPosition != QPoint(-1, -1)) {
                this->move((event->globalPos().x() - this->dragPosition.x()), (event->globalPos().y() - this->dragPosition.y()));
                event->accept();
//            }
        }*/
    }
    QWidget::mouseMoveEvent(event);
}

void BaseGui::ready_save_pre_image(int time) {
//    QString state = core->stateToString().toUtf8().data();
//    if (state == "Playing" || state == "Paused") {

//    }
    if (core) {
        if (core->state() == Core::Playing || core->state() == Core::Paused) {
            if (video_preview == 0) {
                video_preview = new VideoPreview(pref->mplayer_bin, 0);
            }

            if (!core->mdat.filename.isEmpty()) {
                video_preview->setVideoFile(core->mdat.filename);
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
void BaseGui::resizeEvent(QResizeEvent *e) {
    QMainWindow::resizeEvent(e);
    QSize newSize = QMainWindow::size();
//    qDebug() << "new Size=" << newSize;

    int titleBarHeight = this->m_topToolbar->height();
    this->m_topToolbar->raise();
    this->m_topToolbar->move(0, 0);
    this->m_topToolbar->resize(newSize.width(), titleBarHeight);

//    this->mplayerwindow->setFixedSize(newSize);
    this->mplayerwindow->resize(newSize);

    this->playlistWidget->setFixedSize(220, newSize.height() - BOTTOM_TOOLBAR_HEIGHT - titleBarHeight);
    this->playlistWidget->setViewHeight();
    if (this->playlistWidget->isVisible()) {
        this->playlistWidget->hide();
        emit this->change_playlist_btn_status(false);
    }

    this->m_bottomToolbar->raise();
    this->m_bottomToolbar->resize(newSize.width(), BOTTOM_TOOLBAR_HEIGHT);
    this->m_bottomToolbar->move(0, newSize.height() - BOTTOM_TOOLBAR_HEIGHT);
    this->m_bottomToolbar->show_control_widget();
    resizeCorner->move(this->m_bottomToolbar->width()- 16, this->m_bottomToolbar->height() - 16);
}

void BaseGui::closeEvent( QCloseEvent * e )  {
    this->closeWindow();
    e->accept();
}

void BaseGui::closeWindow() {
    if (core->state() != Core::Stopped) {
        core->stop();
    }

    playlistWidget->close();//kobe

    //qApp->quit();
    emit quitSolicited();
}

void BaseGui::open_screenshot_directory() {
    bool open_enabled = ((!pref->screenshot_directory.isEmpty()) && (QFileInfo(pref->screenshot_directory).isDir()));
    if (open_enabled) {
        QDesktopServices::openUrl(QUrl(QString("file:%1").arg(pref->screenshot_directory), QUrl::TolerantMode));
    }
    else {
        tray->showMessage(tr("Information"), tr("The screenshot folder does not exist!"), QSystemTrayIcon::Information, 2000);//QSystemTrayIcon::Warning
    }
}

//#include "moc_basegui.cpp"
