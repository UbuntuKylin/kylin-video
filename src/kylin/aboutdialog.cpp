/*
 * Copyright (C) 2013 ~ 2017 National University of Defense Technology(NUDT) & Kylin Ltd.
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
#include "images.h"
#include "version.h"
#include "global.h"
#include "preferences.h"
#include "paths.h"
#include "inforeader.h"

#include <QFile>
#include <QDesktopServices>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QMouseEvent>
#include <QPoint>
#include <QDebug>

using namespace Global;

AboutDialog::AboutDialog(QWidget * parent, Qt::WindowFlags f)
	: QDialog(parent, f)
    , drag_state(NOT_ADRAGGING)
    , tab_state(TAB_ABOUT)
    , start_drag(QPoint(0,0))
{
	setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setFixedSize(438, 320);
    this->setStyleSheet("QDialog{border: 1px solid #121212;border-radius:1px;background-color: #ffffff;}");
    this->setWindowIcon(QIcon(":/res/kylin-video.png"));//setWindowIcon( Images::icon("logo", 64) );
    this->setAutoFillBackground(true);
    this->setMouseTracking(true);
    installEventFilter(this);

//	logo->setPixmap( QPixmap(":/default-theme/logo.png").scaledToHeight(64, Qt::SmoothTransformation) );//setPixmap( Images::icon("contributors" ) );
    aboutGroup = NULL;
    contributorGroup = NULL;

    baseWidget->setAutoFillBackground(true);
    QPalette palette;
    palette.setBrush(QPalette::Background, QBrush(QPixmap(":/res/about_bg.png")));
    baseWidget->setPalette(palette);

    closeBtn->setFocusPolicy(Qt::NoFocus);
    aboutBtn->setFocusPolicy(Qt::NoFocus);
    contributorBtn->setFocusPolicy(Qt::NoFocus);

    closeBtn->setStyleSheet("QPushButton{background-image:url(':/res/close_normal.png');border:0px;}QPushButton:hover{background:url(':/res/close_hover.png');}QPushButton:pressed{background:url(':/res/close_press.png');}");
//    indicator->setStyleSheet("QLabel{background-image:url('://res/underline.png');background-position:center;}");
    indicator->setStyleSheet("QLabel{background:#0a9ff5;background-position:center;}");
    aboutBtn->setStyleSheet("QPushButton{background:transparent;border:none;text-align:center;font-family: 方正黑体_GBK;font-size:14px;color:#ffffff;}");
    contributorBtn->setStyleSheet("QPushButton{background:transparent;border:none;text-align:center;font-family: 方正黑体_GBK;font-size:14px;color:#ffffff;}");

    okBtn = buttonBox->button(QDialogButtonBox::Ok);
    okBtn->setFixedSize(91, 25);
    okBtn->setText(tr("OK"));
    okBtn->setFocusPolicy(Qt::NoFocus);
    okBtn->setStyleSheet("QPushButton{font-size:12px;background:#ffffff;border:1px solid #0a9ff5;color:#000000;}QPushButton:hover{background-color:#ffffff;border:1px solid #3f96e4;color:#000000;} QPushButton:pressed{background-color:#ffffff;border:1px solid #3f96e4;color:#000000;}");

    this->initConnect();
    this->initAnimation();

//	InfoReader * i = InfoReader::obj(pref->mplayer_bin);
//	i->getInfo();

//    aboutText->setText(
//                "<br>" +
//                tr("Kylin Video is a graphical interface for MPlayer and MPV.") + "<br>" +
//                "<b>" + tr("Kylin Video") + tr("Version: %1").arg(Version::printable()) + "</b>" + "<br>" +
//                tr("Using Qt %1 (compiled with Qt %2)").arg(qVersion()).arg(QT_VERSION_STR) + "<br>" +
//                tr("Playback engine:") + i->playerVersion() + "<br><br>"
//                );
    contributorText->setText("<br>&copy; 2006-2015 Ricardo Villalba &lt;rvm@users.sourceforge.net&gt;<br>&copy; 2017 lixiang &lt;lixiang@kylinos.cn&gt;<br><br>");
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
}

AboutDialog::~AboutDialog()
{
    if(aboutGroup != NULL)
    {
        delete aboutGroup;
        aboutGroup = NULL;
    }
    if(contributorGroup != NULL)
    {
        delete contributorGroup;
        contributorGroup = NULL;
    }
}

void AboutDialog::initConnect()
{
    connect(closeBtn, SIGNAL(clicked()), this, SLOT(accept()));
    connect(okBtn, SIGNAL(clicked()), this, SLOT(accept()));
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

    aboutGroup = new QParallelAnimationGroup(this);
    aboutGroup->addAnimation(aboutAnimation);

    QPropertyAnimation *contributorAnimation = new QPropertyAnimation(indicator, "geometry");
    contributorAnimation->setDuration(300);
    contributorAnimation->setStartValue(mainAcitonRect);
    contributorAnimation->setEndValue(origAcitonRect);

    contributorGroup = new QParallelAnimationGroup(this);
    contributorGroup->addAnimation(contributorAnimation);
}

void AboutDialog::setVersions()
{
    InfoReader * i = InfoReader::obj(pref->mplayer_bin);
    i->getInfo();

    aboutText->setText(
                "<br>" +
                tr("Kylin Video is developed on the basis of SMPlayer, is a graphical interface for MPlayer and MPV.") + "<br>" +
                "<b>" + tr("Kylin Video") + tr("Version: %1").arg(Version::printable()) + "</b>" + "<br>" +
                tr("Using Qt %1 (compiled with Qt %2)").arg(qVersion()).arg(QT_VERSION_STR) + "<br>" +
                tr("Playback engine:") + i->playerVersion() + "<br><br>"
                );
}

void AboutDialog::onAboutBtnClicked()
{
    if (tab_state != TAB_ABOUT) {
        tab_state = TAB_ABOUT;
        aboutGroup->start();
        aboutText->show();
        contributorText->hide();
    }
}

void AboutDialog::onContributorBtnClicked()
{
    if (tab_state != TAB_CONTRIBUTOR) {
        tab_state = TAB_CONTRIBUTOR;
        contributorGroup->start();
        contributorText->show();
        aboutText->hide();
    }
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
        drag_state = NOT_ADRAGGING;
        return false;
    }

    if (type == QEvent::MouseButtonPress) {
        if (mouseEvent->button() != Qt::LeftButton) {
            drag_state = NOT_ADRAGGING;
            return false;
        }

        drag_state = START_ADRAGGING;
        start_drag = mouseEvent->globalPos();
        // Don't filter, so others can have a look at it too
        return false;
    }

    if (type == QEvent::MouseButtonRelease) {
        if (drag_state != ADRAGGING || mouseEvent->button() != Qt::LeftButton) {
            drag_state = NOT_ADRAGGING;
            return false;
        }

        // Stop dragging and eat event
        drag_state = NOT_ADRAGGING;
        event->accept();
        return true;
    }

    // type == QEvent::MouseMove
    if (drag_state == NOT_ADRAGGING)
        return false;

    // buttons() note the s
    if (mouseEvent->buttons() != Qt::LeftButton) {
        drag_state = NOT_ADRAGGING;
        return false;
    }

    QPoint pos = mouseEvent->globalPos();
    QPoint diff = pos - start_drag;
    if (drag_state == START_ADRAGGING) {
        // Don't start dragging before moving at least DRAG_THRESHOLD pixels
        if (abs(diff.x()) < 4 && abs(diff.y()) < 4)
            return false;

        drag_state = ADRAGGING;
    }
    this->moveDialog(diff);

    start_drag = pos;
    event->accept();
    return true;
}
