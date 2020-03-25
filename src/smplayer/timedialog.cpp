/*  smplayer, GUI front-end for mplayer.
    Copyright (C) 2006-2015 Ricardo Villalba <rvm@users.sourceforge.net>

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

#include "timedialog.h"
#include <QPushButton>
#include <QDesktopServices>
#include <QMouseEvent>
#include <QPoint>

TimeDialog::TimeDialog( QWidget* parent, Qt::WindowFlags f )
	: QDialog(parent, f)
    , m_dragState(NOT_DRAGGING)
    , m_startDrag(QPoint(0,0))
{
	setupUi(this);
    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    this->setFixedSize(380, 170);
    this->setAutoFillBackground(true);
    this->setAttribute(Qt::WA_TranslucentBackground);//设置窗口背景透明
//    this->setStyleSheet("QDialog{border: 1px solid #121212;border-radius:1px;background-color:#1f1f1f;}");
    this->setWindowIcon(QIcon::fromTheme("kylin-video", QIcon(":/res/kylin-video.png")));
//    this->setWindowIcon(QIcon::fromTheme("kylin-video", QIcon(":/res/kylin-video.png")).pixmap(QSize(64, 64)).scaled(64, 64, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
//    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setAutoFillBackground(true);
    this->setMouseTracking(true);
    installEventFilter(this);

    //TODO: 无效
    this->setObjectName("popDialog");
    //this->setStyleSheet("QDialog#prefrecesdialog{border: 1px solid #121212;border-radius:6px;background-color:#1f1f1f;}");

    widget->setAutoFillBackground(true);
    widget->setObjectName("centerWidget");

    time_edit->setObjectName("seekTime");
	time_edit->setDisplayFormat("H:mm:ss");

    title_label->setStyleSheet("QLabel{background:transparent;font-size:14px;color:#999999;font-family:方正黑体_GBK;}");//font-weight:bold;
    time_label->setStyleSheet("QLabel{background:transparent;font-size:12px;color:#999999;font-family:方正黑体_GBK;}");//font-weight:bold;

    closeBtn->setFocusPolicy(Qt::NoFocus);
    closeBtn->setStyleSheet("QPushButton{background-image:url(':/res/close_normal.png');border:0px;}QPushButton:hover{background:url(':/res/close_hover.png');}QPushButton:pressed{background:url(':/res/close_press.png');}");

    okBtn->setFixedSize(91, 25);
    okBtn->setText(tr("OK"));
    okBtn->setFocusPolicy(Qt::NoFocus);
    okBtn->setStyleSheet("QPushButton{font-size:12px;background:#0f0f0f;border:1px solid #0a9ff5;color:#999999;}QPushButton:hover{background-color:#0a9ff5;border:1px solid #2db0f6;color:#ffffff;} QPushButton:pressed{background-color:#0993e3;border:1px solid #0a9ff5;color:#ffffff;}");

    cancelBtn->setFixedSize(91, 25);
    cancelBtn->setText(tr("Cancel"));
    cancelBtn->setFocusPolicy(Qt::NoFocus);
    cancelBtn->setStyleSheet("QPushButton{font-size:12px;background:#0f0f0f;border:1px solid #000000;color:#999999;}QPushButton:hover{background-color:#1f1f1f;border:1px solid #0f0f0f;color:#ffffff;} QPushButton:pressed{background-color:#0d0d0d;border:1px solid #000000;color:#ffffff;}");

    this->initConnect();
}

TimeDialog::~TimeDialog() {
}

void TimeDialog::setTime(int seconds) {
	QTime t(0,0);
	time_edit->setTime(t.addSecs(seconds));
}

int TimeDialog::time() {
	QTime t(0,0);
	return t.secsTo(time_edit->time());
}

void TimeDialog::setMaximumTime( int seconds ) {
	QTime t(0,0);
	time_edit->setMaximumTime(t.addSecs(seconds));
}

int TimeDialog::maximumTime() {
	QTime t(0,0);
	return t.secsTo(time_edit->maximumTime());
}

void TimeDialog::setLabel(const QString & label) {
	time_label->setText(label);
}

QString TimeDialog::label() {
	return time_label->text();
}

void TimeDialog::initConnect()
{
    connect(okBtn, SIGNAL(clicked()), this, SLOT(accept()));
    connect(cancelBtn, SIGNAL(clicked()), this, SLOT(close()));
    connect(closeBtn, SIGNAL(clicked()), this, SLOT(close()));
}

void TimeDialog::moveDialog(QPoint diff) {
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

bool TimeDialog::eventFilter( QObject * object, QEvent * event ) {
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

//#include "moc_timedialog.cpp"
