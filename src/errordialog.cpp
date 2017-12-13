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

#include "errordialog.h"
#include "images.h"
#include <QLayout>
#include <QDesktopServices>
#include <QMouseEvent>
#include <QPoint>
#include <QScrollBar>

ErrorDialog::ErrorDialog( QWidget* parent, Qt::WindowFlags f )
	: QDialog(parent, f)
    , drag_state(NOT_EDRAGGING)
    , start_drag(QPoint(0,0))
{
	setupUi(this);

    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setFixedSize(514, 160);
    this->setStyleSheet("QDialog{border: 1px solid #121212;border-radius:1px;background-color:#1f1f1f;}");
    this->setWindowIcon(QIcon(":/res/kylin-video.png"));//setWindowIcon( Images::icon("logo", 64) );
    this->setAutoFillBackground(true);
    this->setMouseTracking(true);
    installEventFilter(this);

    log->setStyleSheet("QTextEdit {border: 1px solid #000000;color: #999999;background: #0f0f0f;font-family:方正黑体_GBK;font-size: 12px;}");
    log->verticalScrollBar()->setStyleSheet("QScrollBar:vertical {width: 12px;background: #141414;margin:0px 0px 0px 0px;border:1px solid #141414;}QScrollBar::handle:vertical {width: 12px;min-height: 45px;background: #292929;margin-left: 0px;margin-right: 0px;}QScrollBar::handle:vertical:hover {background: #3e3e3e;}QScrollBar::handle:vertical:pressed {background: #272727;}QScrollBar::sub-line:vertical {height: 6px;background: transparent;subcontrol-position: top;}QScrollBar::add-line:vertical {height: 6px;background: transparent;subcontrol-position: bottom;}QScrollBar::sub-line:vertical:hover {background: #292929;}QScrollBar::add-line:vertical:hover {background: #292929;}QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {background: transparent;}");

    icon->setScaledContents(true);
	icon->setText("");
    icon->setPixmap(Images::icon("warn"));

//	intro_label->setText("<html><head/><body><p align=\"left\"><span style=\"font-size:14pt; font-weight:600;\">" + tr("Oops, something wrong happened") +"</span></p></body></html>");
    intro_label->setStyleSheet("QLabel{background:transparent;font-size:20px;color:#999999;font-family:方正黑体_GBK;}");//font-weight:bold;
    title_label->setStyleSheet("QLabel{background:transparent;font-size:14px;color:#999999;font-family:方正黑体_GBK;}");//font-weight:bold;
    text_label->setStyleSheet("QLabel{background:transparent;font-size:12px;color:#999999;font-family:方正黑体_GBK;}");//font-weight:bold;
    text_label->setText("");
	toggleLog(false);

    connect(viewlog_button, SIGNAL(toggled(bool)), this, SLOT(toggleLog(bool)));

    closeBtn->setFocusPolicy(Qt::NoFocus);
    closeBtn->setStyleSheet("QPushButton{background-image:url(':/res/close_normal.png');border:0px;}QPushButton:hover{background:url(':/res/close_hover.png');}QPushButton:pressed{background:url(':/res/close_press.png');}");

    viewlog_button->setFixedSize(91, 25);
    viewlog_button->setFocusPolicy(Qt::NoFocus);
    viewlog_button->setStyleSheet("QPushButton{font-size:12px;background:#0f0f0f;border:1px solid #0a9ff5;color:#999999;}QPushButton:hover{background-color:#0a9ff5;border:1px solid #2db0f6;color:#ffffff;} QPushButton:pressed{background-color:#0993e3;border:1px solid #0a9ff5;color:#ffffff;}");

    okBtn->setFixedSize(91, 25);
    okBtn->setFocusPolicy(Qt::NoFocus);
    okBtn->setStyleSheet("QPushButton{font-size:12px;background:#0f0f0f;border:1px solid #0a9ff5;color:#999999;}QPushButton:hover{background-color:#0a9ff5;border:1px solid #2db0f6;color:#ffffff;} QPushButton:pressed{background-color:#0993e3;border:1px solid #0a9ff5;color:#ffffff;}");

    connect(closeBtn, SIGNAL(clicked()),this, SLOT(close()));
    connect(okBtn, SIGNAL(clicked()),this, SLOT(accept()));

//	layout()->setSizeConstraint(QLayout::SetFixedSize);
}

ErrorDialog::~ErrorDialog() {
}

void ErrorDialog::hideDetailBtn()
{
    viewlog_button->hide();
}

void ErrorDialog::setText(QString error) {
    text_label->setText(error);
}

void ErrorDialog::setTitleText(QString error) {
    title_label->setText(error);
}

void ErrorDialog::setLog(QString log_text) {
	log->setPlainText("");
	log->append(log_text); // To move cursor to the end
}

void ErrorDialog::toggleLog(bool checked) {
	log->setVisible(checked);

    if (checked) {
		viewlog_button->setText(tr("Hide log"));
        this->setFixedSize(514, 364);
    }
    else {
		viewlog_button->setText(tr("Show log"));
        this->setFixedSize(514, 160);
    }
}

void ErrorDialog::moveDialog(QPoint diff) {
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

bool ErrorDialog::eventFilter( QObject * object, QEvent * event ) {
    QEvent::Type type = event->type();
    if (type != QEvent::MouseButtonPress
        && type != QEvent::MouseButtonRelease
        && type != QEvent::MouseMove)
        return false;

    QMouseEvent *mouseEvent = dynamic_cast<QMouseEvent*>(event);
    if (!mouseEvent)
        return false;

    if (mouseEvent->modifiers() != Qt::NoModifier) {
        drag_state = NOT_EDRAGGING;
        return false;
    }

    if (type == QEvent::MouseButtonPress) {
        if (mouseEvent->button() != Qt::LeftButton) {
            drag_state = NOT_EDRAGGING;
            return false;
        }

        drag_state = START_EDRAGGING;
        start_drag = mouseEvent->globalPos();
        // Don't filter, so others can have a look at it too
        return false;
    }

    if (type == QEvent::MouseButtonRelease) {
        if (drag_state != EDRAGGING || mouseEvent->button() != Qt::LeftButton) {
            drag_state = NOT_EDRAGGING;
            return false;
        }

        // Stop dragging and eat event
        drag_state = NOT_EDRAGGING;
        event->accept();
        return true;
    }

    // type == QEvent::MouseMove
    if (drag_state == NOT_EDRAGGING)
        return false;

    // buttons() note the s
    if (mouseEvent->buttons() != Qt::LeftButton) {
        drag_state = NOT_EDRAGGING;
        return false;
    }

    QPoint pos = mouseEvent->globalPos();
    QPoint diff = pos - start_drag;
    if (drag_state == START_EDRAGGING) {
        // Don't start dragging before moving at least DRAG_THRESHOLD pixels
        if (abs(diff.x()) < 4 && abs(diff.y()) < 4)
            return false;

        drag_state = EDRAGGING;
    }
    this->moveDialog(diff);

    start_drag = pos;
    event->accept();
    return true;
}

#include "moc_errordialog.cpp"
