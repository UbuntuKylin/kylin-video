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

#include "aboutdialog.h"
#include "../smplayer/images.h"
#include "../smplayer/version.h"
#include "../smplayer/global.h"
#include "../smplayer/preferences.h"
#include "../smplayer/paths.h"
#include "../smplayer/inforeader.h"

#if QT_VERSION >= 0x050000
#include "../smplayer/scrollermodule.h"
#endif

#include <QFile>
#include <QDesktopServices>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QMouseEvent>
#include <QPoint>
#include <QDebug>
#include <QScrollBar>

#define URL_HOMEPAGE "https://github.com/ukui/kylin-video"

using namespace Global;

AboutDialog::AboutDialog(const QString &snap, QWidget * parent, Qt::WindowFlags f)
	: QDialog(parent, f)
    , m_dragState(NOT_DRAGGING)
    , tab_state(TAB_ABOUT)
    , m_startDrag(QPoint(0,0))
    , m_snap(snap)
{
    setupUi(this);
//    this->setModal(true);
    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    this->setAutoFillBackground(true);
    this->setAttribute(Qt::WA_TranslucentBackground);//设置窗口背景透明
    //this->setFixedSize(438, 320);
//    this->setStyleSheet("QDialog{border:none;background-color: #ffffff;}");//border: 1px solid #121212;border-radius:6px;
    this->setWindowIcon(QIcon::fromTheme("kylin-video", QIcon(":/res/kylin-video.png")));
    //this->setWindowIcon(QIcon::fromTheme("kylin-video", QIcon(":/res/kylin-video.png")).pixmap(QSize(64, 64)).scaled(64, 64, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));

    this->setMouseTracking(true);
    installEventFilter(this);

    //TODO: 无效
    this->setObjectName("popDialog");
    //this->setStyleSheet("QDialog#prefrecesdialog{border: 1px solid #121212;border-radius:6px;background-color:#1f1f1f;}");

    widget->setAutoFillBackground(true);
    widget->setObjectName("centerWidget");
//    widget->setStyleSheet("QWidget{border:none;border-top-right-radius:6px;border-bottom-right-radius:6px;background-color:#1f1f1f;}");//

#if QT_VERSION >= 0x050000
    ScrollerModule::setScroller(aboutText->viewport());
    ScrollerModule::setScroller(contributorText->viewport());
#endif

//	logo->setPixmap( QPixmap(":/default-theme/logo.png").scaledToHeight(64, Qt::SmoothTransformation) );//setPixmap( Images::icon("contributors" ) );
    m_aboutGroup = NULL;
    m_contributorGroup = NULL;

    baseWidget->setAutoFillBackground(true);
    baseWidget->setObjectName("aboutTopWidget");
//    baseWidget->setStyleSheet("QWidget{background-image:url(':/res/about_bg.png'); border:0px; border-radius:6px;}");
//    QPalette palette;
//    palette.setBrush(QPalette::Background, QBrush(QPixmap(":/res/about_bg.png")));
//    baseWidget->setPalette(palette);

    closeBtn->setFocusPolicy(Qt::NoFocus);
    aboutBtn->setFocusPolicy(Qt::NoFocus);
    contributorBtn->setFocusPolicy(Qt::NoFocus);

    closeBtn->setStyleSheet("QPushButton{background-image:url(':/res/close_normal.png');border:0px;}QPushButton:hover{background:url(':/res/close_hover.png');}QPushButton:pressed{background:url(':/res/close_press.png');}");
//    indicator->setStyleSheet("QLabel{background-image:url('://res/underline.png');background-position:center;}");
    indicator->setStyleSheet("QLabel{background:#0a9ff5;background-position:center;}");
    aboutBtn->setStyleSheet("QPushButton{background:transparent;border:none;text-align:center;font-family: 方正黑体_GBK;font-size:14px;color:#ffffff;}");
    contributorBtn->setStyleSheet("QPushButton{background:transparent;border:none;text-align:center;font-family: 方正黑体_GBK;font-size:14px;color:#ffffff;}");

    m_okBtn = buttonBox->button(QDialogButtonBox::Ok);
    m_okBtn->setFixedSize(91, 25);
    m_okBtn->setText(tr("OK"));
    m_okBtn->setFocusPolicy(Qt::NoFocus);
    m_okBtn->setStyleSheet("QPushButton{font-size:12px;background:#ffffff;border:1px solid #0a9ff5;color:#000000;}QPushButton:hover{background-color:#ffffff;border:1px solid #3f96e4;color:#000000;} QPushButton:pressed{background-color:#ffffff;border:1px solid #3f96e4;color:#000000;}");

    this->initConnect();
    this->initAnimation();

//	InfoReader * i = InfoReader::obj(pref->mplayer_bin, this->m_snap);
//	i->getInfo();

//    aboutText->setText(
//                "<br>" +
//                tr("Kylin Video is a graphical interface for MPlayer and MPV.") + "<br>" +
//                "<b>" + tr("Kylin Video") + tr("Version: %1").arg(Version::printable()) + "</b>" + "<br>" +
//                tr("Using Qt %1 (compiled with Qt %2)").arg(qVersion()).arg(QT_VERSION_STR) + "<br>" +
//                tr("Playback engine:") + i->playerVersion() + "<br><br>"
//                );



//    QString AboutDialog::link(const QString & url, QString name)
//    {
//            if (name.isEmpty()) name = url;
//            return QString("<a href=\"" + url + "\">" + name +"</a>");
//    }
//    <a href=\"http://www.smplayer.info\">SMPlayer</a>

    contributorText->setText("<br>&copy; 2006-2015 Ricardo Villalba &lt;rvm@users.sourceforge.net&gt;<br>&copy; 2017-2020 lixiang &lt;lixiang@kylinos.cn&gt;<br><br>");
    //contributorText->setText(QString("<br>&copy; 2006-2015 Ricardo Villalba &lt;rvm@users.sourceforge.net&gt;<br>&copy; 2017-2019 lixiang &lt;lixiang@kylinos.cn&gt; %1<br><br>").arg("<a href=\"https://eightplus.github.io\">https://eightplus.github.io</a>"));
    contributorText->hide();

	adjustSize();
    aboutText->setStyleSheet("QTextBrowser{background-color:transparent;border:none;font-family:方正黑体_GBK;font-size:12px;color:#999999;}");
    contributorText->setStyleSheet("QTextBrowser{background-color:transparent;border:none;font-family:方正黑体_GBK;font-size:12px;color:#999999;}");
    /*<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0//EN" "http://www.w3.org/TR/REC-html40/strict.dtd">
    <html><head><meta name="qrichtext" content="1" /><style type="text/css">
    p, li { white-space: pre-wrap; }
    </style></head><body style=" font-family:'Ubuntu'; font-size:12pt; font-weight:400; font-style:normal;">
    <p style="-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;"><br /></p></body></html>
    */


    //QString scrollstyle= "QScrollBar:vertical {width: 12px;background: #141414;margin:0px 0px 0px 0px;border:1px solid #141414;}QScrollBar::handle:vertical {width: 12px;min-height: 45px;background: #292929;margin-left: 0px;margin-right: 0px;}QScrollBar::handle:vertical:hover {background: #3e3e3e;}QScrollBar::handle:vertical:pressed {background: #272727;}QScrollBar::sub-line:vertical {height: 6px;background: transparent;subcontrol-position: top;}QScrollBar::add-line:vertical {height: 6px;background: transparent;subcontrol-position: bottom;}QScrollBar::sub-line:vertical:hover {background: #292929;}QScrollBar::add-line:vertical:hover {background: #292929;}QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {background: transparent;}";
    //aboutText->verticalScrollBar()->setStyleSheet(scrollstyle);

    aboutText->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    contributorText->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    aboutText->verticalScrollBar()->setStyleSheet("QScrollBar:vertical {width: 12px;background: #141414;margin:0px 0px 0px 0px;border:1px solid #141414;}QScrollBar::handle:vertical {width: 12px;min-height: 45px;background: #292929;margin-left: 0px;margin-right: 0px;}QScrollBar::handle:vertical:hover {background: #3e3e3e;}QScrollBar::handle:vertical:pressed {background: #272727;}QScrollBar::sub-line:vertical {height: 6px;background: transparent;subcontrol-position: top;}QScrollBar::add-line:vertical {height: 6px;background: transparent;subcontrol-position: bottom;}QScrollBar::sub-line:vertical:hover {background: #292929;}QScrollBar::add-line:vertical:hover {background: #292929;}QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {background: transparent;}");
    contributorText->verticalScrollBar()->setStyleSheet("QScrollBar:vertical {width: 12px;background: #141414;margin:0px 0px 0px 0px;border:1px solid #141414;}QScrollBar::handle:vertical {width: 12px;min-height: 45px;background: #292929;margin-left: 0px;margin-right: 0px;}QScrollBar::handle:vertical:hover {background: #3e3e3e;}QScrollBar::handle:vertical:pressed {background: #272727;}QScrollBar::sub-line:vertical {height: 6px;background: transparent;subcontrol-position: top;}QScrollBar::add-line:vertical {height: 6px;background: transparent;subcontrol-position: bottom;}QScrollBar::sub-line:vertical:hover {background: #292929;}QScrollBar::add-line:vertical:hover {background: #292929;}QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {background: transparent;}");

    this->adjustSize();
}

AboutDialog::~AboutDialog()
{
    if(m_aboutGroup != NULL)
    {
        delete m_aboutGroup;
        m_aboutGroup = NULL;
    }
    if(m_contributorGroup != NULL)
    {
        delete m_contributorGroup;
        m_contributorGroup = NULL;
    }
}

void AboutDialog::initConnect()
{
    connect(closeBtn, SIGNAL(clicked()), this, SLOT(accept()));
    connect(m_okBtn, SIGNAL(clicked()), this, SLOT(accept()));
    connect(aboutBtn, SIGNAL(clicked()), this, SLOT(onAboutBtnClicked()));
    connect(contributorBtn, SIGNAL(clicked()), this, SLOT(onContributorBtnClicked()));
}

void AboutDialog::initAnimation()
{
    QRect mainAcitonRect(230, 70, 85, 2);
    QRect origAcitonRect(320, 70, 85, 2);

    QPropertyAnimation *aboutAnimation = new QPropertyAnimation(indicator, "geometry");
    aboutAnimation->setDuration(300);
    aboutAnimation->setStartValue(origAcitonRect);
    aboutAnimation->setEndValue(mainAcitonRect);

    m_aboutGroup = new QParallelAnimationGroup(this);
    m_aboutGroup->addAnimation(aboutAnimation);

    QPropertyAnimation *contributorAnimation = new QPropertyAnimation(indicator, "geometry");
    contributorAnimation->setDuration(300);
    contributorAnimation->setStartValue(mainAcitonRect);
    contributorAnimation->setEndValue(origAcitonRect);

    m_contributorGroup = new QParallelAnimationGroup(this);
    m_contributorGroup->addAnimation(contributorAnimation);
}

void AboutDialog::setVersions()
{
    InfoReader * i = InfoReader::obj(this->m_snap, pref->mplayer_bin);//20181212
    i->getInfo();

    aboutText->setText(
                "<br>" +
                tr("Kylin Video is developed on the basis of %1, is a graphical interface for %2 and %3.").arg("<a href=\"http://www.smplayer.info\">SMPlayer</a>").arg("<a href=\"http://www.mplayerhq.hu/design7/info.html\">MPlayer</a>").arg("<a href=\"http://www.mpv.io\">mpv</a>") + "<br>" +
                "<b>" + tr("Kylin Video") + tr("Version: %1").arg(Version::printable()) + "</b>" + "<br>" +
                tr("Using Qt %1 (compiled with Qt %2)").arg(qVersion()).arg(QT_VERSION_STR) + "<br>" +
                tr("Playback engine:") + i->playerVersion() + "<br><br>" +
                "<b>"+ tr("Links:") + "</b><br>" +
                tr("Code website:") + " " +  link(URL_HOMEPAGE) + "<br>" +
                tr("Developer's personal home page:") + " " +  link("https://eightplus.github.io") + "<br>"
                );
}

QString AboutDialog::link(const QString & url, QString name)
{
        if (name.isEmpty()) name = url;
        return QString("<a href=\"" + url + "\">" + name +"</a>");
}

void AboutDialog::onAboutBtnClicked()
{
    if (tab_state != TAB_ABOUT) {
        tab_state = TAB_ABOUT;
        m_aboutGroup->start();
        aboutText->show();
        contributorText->hide();
    }
}

void AboutDialog::onContributorBtnClicked()
{
    if (tab_state != TAB_CONTRIBUTOR) {
        tab_state = TAB_CONTRIBUTOR;
        m_contributorGroup->start();
        contributorText->show();
        aboutText->hide();
    }
}

QSize AboutDialog::sizeHint () const
{
    return QSize(438, 320);
}

void AboutDialog::moveDialog(QPoint diff) {
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
    }
#else
    move(pos() + diff);
#endif
}

bool AboutDialog::eventFilter( QObject * object, QEvent * event ) {
    QEvent::Type type = event->type();
    if (type != QEvent::MouseButtonPress
        && type != QEvent::MouseButtonRelease
        && type != QEvent::MouseMove)
        return false;

    QMouseEvent *mouseEvent = dynamic_cast<QMouseEvent*>(event);
    if (!mouseEvent)
        return false;

    if (mouseEvent->modifiers() != Qt::NoModifier) {
        m_dragState = NOT_DRAGGING;
        return false;
    }

    if (type == QEvent::MouseButtonPress) {
        if (mouseEvent->button() != Qt::LeftButton) {
            m_dragState = NOT_DRAGGING;
            return false;
        }

        m_dragState = START_DRAGGING;
        m_startDrag = mouseEvent->globalPos();
        // Don't filter, so others can have a look at it too
        return false;
    }

    if (type == QEvent::MouseButtonRelease) {
        if (m_dragState != DRAGGING || mouseEvent->button() != Qt::LeftButton) {
            m_dragState = NOT_DRAGGING;
            return false;
        }

        // Stop dragging and eat event
        m_dragState = NOT_DRAGGING;
        event->accept();
        return true;
    }

    // type == QEvent::MouseMove
    if (m_dragState == NOT_DRAGGING)
        return false;

    // buttons() note the s
    if (mouseEvent->buttons() != Qt::LeftButton) {
        m_dragState = NOT_DRAGGING;
        return false;
    }

    QPoint pos = mouseEvent->globalPos();
    QPoint diff = pos - m_startDrag;
    if (m_dragState == START_DRAGGING) {
        // Don't start dragging before moving at least DRAG_THRESHOLD pixels
        if (abs(diff.x()) < 4 && abs(diff.y()) < 4)
            return false;

        m_dragState = DRAGGING;
    }
    this->moveDialog(diff);

    m_startDrag = pos;
    event->accept();
    return true;
}
