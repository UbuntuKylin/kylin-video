/*  smplayer, GUI front-end for mplayer.
    Copyright (C) 2006-2015 Ricardo Villalba <rvm@users.sourceforge.net>
    Copyright (C) 2013 ~ 2019 National University of Defense Technology(NUDT) & Tianjin Kylin Ltd.

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

#include "inputurl.h"
#include "../smplayer/images.h"
#include "../smplayer/mylineedit.h"
#include <QPushButton>
#include <QDesktopServices>
#include <QMouseEvent>
#include <QPoint>

//http://rbv01.ku6.com/WrTcR6Yik9QIW4XIcxYvDw.mp4

InputURL::InputURL( QWidget* parent, Qt::WindowFlags f ) 
	: QDialog(parent, f)
    , m_dragState(NOT_DRAGGING)
    , m_startDrag(QPoint(0,0))
{
	setupUi(this);
    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    this->setFixedSize(500, 170);
    this->setAutoFillBackground(true);
    this->setAttribute(Qt::WA_TranslucentBackground);//设置窗口背景透明
//    this->setStyleSheet("QDialog{border: 1px solid #121212;border-radius:1px;background-color:#1f1f1f;}");
    this->setWindowIcon(QIcon::fromTheme("kylin-video", QIcon(":/res/kylin-video.png")));
//    this->setWindowIcon(QIcon::fromTheme("kylin-video", QIcon(":/res/kylin-video.png")).pixmap(QSize(64, 64)).scaled(64, 64, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));

    this->setMouseTracking(true);
    installEventFilter(this);
//	setMinimumSize( QSize(500,140) );
//	setMaximumSize( QSize(600,170) );
    //layout()->setSizeConstraint(QLayout::SetFixedSize);

    //TODO: 无效
    this->setObjectName("popDialog");
    //this->setStyleSheet("QDialog#prefrecesdialog{border: 1px solid #121212;border-radius:6px;background-color:#1f1f1f;}");

    widget->setAutoFillBackground(true);
    widget->setObjectName("centerWidget");

    title_label->setStyleSheet("QLabel{background:transparent;font-size:14px;color:#999999;font-family:方正黑体_GBK;}");//font-weight:bold;
    label->setStyleSheet("QLabel{background:transparent;font-size:12px;color:#999999;font-family:方正黑体_GBK;}");//font-weight:bold;

//	url_icon->setPixmap( Images::icon("url_big", 48) );
	url_edit->setFocus();
    url_edit->setFixedHeight(27);
    url_edit->setStyleSheet("QComboBox{width:150px;height:27px;border:1px solid #000000;background:#0f0f0f;font-size:12px;font-family:方正黑体_GBK;background-position:center left;padding-left:5px;color:#999999;selection-color:#ffffff;selection-background-color:#1f1f1f;}QComboBox::hover{background-color:#0f0f0f;border:1px solid #0a9ff5;font-family:方正黑体_GBK;font-size:12px;color:#999999;}QComboBox:!enabled {background:#0f0f0f;color:#383838;}QComboBox::drop-down {width:17px;border:none;background:transparent;}QComboBox::drop-down:hover {background:transparent;}QComboBox::down-arrow{image:url(:/res/combobox_arrow_normal.png);}QComboBox::down-arrow:hover{image:url(:/res/combobox_arrow_hover.png);}QComboBox::down-arrow:pressed{image:url(:/res/combobox_arrow_press.png);}QComboBox QAbstractItemView{border:1px solid #0a9ff5;background:#262626;outline:none;}");
    MyLineEdit *edit = new MyLineEdit(this);
    edit->setFixedHeight(25);
    url_edit->setLineEdit(edit);

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

InputURL::~InputURL() {
}

void InputURL::initConnect()
{
    connect(okBtn, SIGNAL(clicked()), this, SLOT(accept()));
    connect(cancelBtn, SIGNAL(clicked()), this, SLOT(close()));
    connect(closeBtn, SIGNAL(clicked()), this, SLOT(close()));
}


void InputURL::setURL(QString url) {
	url_edit->addItem(url);
}

QString InputURL::url() {
	return url_edit->currentText().trimmed();
}

void InputURL::moveDialog(QPoint diff) {
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

bool InputURL::eventFilter( QObject * object, QEvent * event ) {
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

//#include "moc_inputurl.cpp"
