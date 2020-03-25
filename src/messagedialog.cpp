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

#include "messagedialog.h"
#include <QPushButton>
#include <QDesktopServices>
#include <QMouseEvent>
#include <QPoint>
#include <QLabel>

MessageDialog::MessageDialog(QWidget *parent, const QString &title, const QString &text, QMessageBox::StandardButtons buttons, QMessageBox::StandardButton defaultButton)
    : QDialog(parent)
    , m_dragState(NOT_DRAGGING)
    , m_startDrag(QPoint(0,0))
    , m_centerWidget(new QWidget(this))
{
    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
//    this->setStyleSheet("QDialog{border: 1px solid #121212;border-radius:1px;background-color:#1f1f1f;}");
    this->setAutoFillBackground(true);
    this->setAttribute(Qt::WA_TranslucentBackground);//设置窗口背景透明
    this->setWindowTitle(title);
    this->setWindowIcon(QIcon::fromTheme("kylin-video", QIcon(":/res/kylin-video.png")));
//    this->setWindowIcon(QIcon::fromTheme("kylin-video", QIcon(":/res/kylin-video.png")).pixmap(QSize(64, 64)).scaled(64, 64, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));

    this->setMouseTracking(true);
    installEventFilter(this);

    //TODO: 无效
    this->setObjectName("popDialog");
    //this->setStyleSheet("QDialog#prefrecesdialog{border: 1px solid #121212;border-radius:6px;background-color:#1f1f1f;}");

    m_centerWidget->setAutoFillBackground(true);
    m_centerWidget->setObjectName("centerWidget");
    m_centerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    title_label = new QLabel(this);
    title_label->setAlignment(Qt::AlignCenter);
    title_label->setStyleSheet("QLabel{background:transparent;font-size:14px;color:#999999;font-family:方正黑体_GBK;}");//font-weight:bold;
    title_label->setText(title);

    close_Btn = new QPushButton(this);
    close_Btn->setFixedSize(36,36);
    close_Btn->setFocusPolicy(Qt::NoFocus);
    close_Btn->setStyleSheet("QPushButton{background-image:url(':/res/close_normal.png');border:0px;}QPushButton:hover{background:url(':/res/close_hover.png');}QPushButton:pressed{background:url(':/res/close_press.png');}");

    QHBoxLayout *title_layout = new QHBoxLayout();
    title_layout->addWidget(title_label);
    title_layout->addStretch();
    title_layout->addWidget(close_Btn);
    title_layout->setMargin(0);
    title_layout->setContentsMargins(5,0,0,0);


    buttonBox = new QDialogButtonBox(this);
    buttonBox->setStandardButtons(QDialogButtonBox::StandardButtons(int(buttons)));

    QPushButton *okBtn = buttonBox->button(QDialogButtonBox::Ok);
    if (okBtn != NULL) {
        okBtn->setFixedSize(91, 25);
        okBtn->setText(tr("Ok"));
        okBtn->setFocusPolicy(Qt::NoFocus);
        okBtn->setStyleSheet("QPushButton{font-size:12px;background:#0f0f0f;border:1px solid #0a9ff5;color:#999999;}QPushButton:hover{background-color:#0a9ff5;border:1px solid #2db0f6;color:#ffffff;} QPushButton:pressed{background-color:#0993e3;border:1px solid #0a9ff5;color:#ffffff;}");
    }

    QPushButton *cancelBtn = buttonBox->button(QDialogButtonBox::Cancel);
    if (cancelBtn != NULL) {
        cancelBtn->setFixedSize(91, 25);
        cancelBtn->setText(tr("Cancel"));
        cancelBtn->setFocusPolicy(Qt::NoFocus);
        cancelBtn->setStyleSheet("QPushButton{font-size:12px;background:#0f0f0f;border:1px solid #000000;color:#999999;}QPushButton:hover{background-color:#1f1f1f;border:1px solid #0f0f0f;color:#ffffff;} QPushButton:pressed{background-color:#0d0d0d;border:1px solid #000000;color:#ffffff;}");
    }

    QPushButton *yesBtn = buttonBox->button(QDialogButtonBox::Yes);
    if (yesBtn != NULL) {
        yesBtn->setFixedSize(91, 25);
        yesBtn->setText(tr("Yes"));
        yesBtn->setFocusPolicy(Qt::NoFocus);
        yesBtn->setStyleSheet("QPushButton{font-size:12px;background:#0f0f0f;border:1px solid #0a9ff5;color:#999999;}QPushButton:hover{background-color:#0a9ff5;border:1px solid #2db0f6;color:#ffffff;} QPushButton:pressed{background-color:#0993e3;border:1px solid #0a9ff5;color:#ffffff;}");
    }

    QPushButton *noBtn = buttonBox->button(QDialogButtonBox::No);
    if (noBtn != NULL) {
        noBtn->setFixedSize(91, 25);
        noBtn->setText(tr("No"));
        noBtn->setFocusPolicy(Qt::NoFocus);
        noBtn->setStyleSheet("QPushButton{font-size:12px;background:#0f0f0f;border:1px solid #000000;color:#999999;}QPushButton:hover{background-color:#1f1f1f;border:1px solid #0f0f0f;color:#ffffff;} QPushButton:pressed{background-color:#0d0d0d;border:1px solid #000000;color:#ffffff;}");
    }

    icon_label = new QLabel(this);
    QPixmap pixmap(":/res/warn.png");
    icon_label->setPixmap(pixmap);
//    icon_label->setFixedSize(60, 58);
//    icon_label->setScaledContents(true);
    icon_label->adjustSize();
    icon_label->setWordWrap(true);

    msg_label = new QLabel(this);
    msg_label->setMinimumWidth(100);
    msg_label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    msg_label->setStyleSheet("QLabel{background:transparent;font-size:12px;color:#999999;font-family:方正黑体_GBK;}");//font-weight:bold;
    msg_label->setOpenExternalLinks(true);
    msg_label->setText(text);

    QHBoxLayout *hlayout = new QHBoxLayout();
    hlayout->setMargin(0);
    hlayout->setContentsMargins(30,0,30,0);
    hlayout->setSpacing(30);
    hlayout->addStretch();
    hlayout->addWidget(icon_label);
    hlayout->addWidget(msg_label);
    hlayout->addStretch();

    QHBoxLayout *blayout = new QHBoxLayout();
    blayout->addStretch();
    blayout->addWidget(buttonBox);
    blayout->setContentsMargins(10,0,10,0);

    QBoxLayout *layout = new QVBoxLayout(m_centerWidget);
    layout->addLayout(title_layout);
    layout->addLayout(hlayout);
    layout->addLayout(blayout);
    layout->setMargin(0);
    layout->setSpacing(10);
    layout->setContentsMargins(0, 0, 0, 10);


    main_layout = new QVBoxLayout(this);
    main_layout->setContentsMargins(0,0,0,0);
    main_layout->addWidget(m_centerWidget);

    this->layout()->setSizeConstraint(QLayout::SetFixedSize);

    this->initConnect();
}

MessageDialog::~MessageDialog()
{
}

void MessageDialog::setDialogSize(int w, int h)
{
    this->setFixedSize(w, h);
}

void MessageDialog::setIcon(const QString &icon)
{
    icon_label->setPixmap(QPixmap(icon));
}

QMessageBox::StandardButton MessageDialog::standardButton(QAbstractButton *button) const
{
    return (QMessageBox::StandardButton)buttonBox->standardButton(button);
}

QAbstractButton *MessageDialog::clickedButton() const
{
    return clickedBtn;
}

int MessageDialog::returnCodeByRun(QAbstractButton *button)
{
    int ret = buttonBox->standardButton(button);
    return ret;
}

void MessageDialog::onButtonClicked(QAbstractButton *button)
{
    clickedBtn = button;
    done(returnCodeByRun(button));
}

//void MessageDialog::setDefaultButton(QPushButton *button)
//{
//    if (!buttonBox->buttons().contains(button))
//        return;
//    defaultBtn = button;
//    button->setDefault(true);
//    button->setFocus();
//}

//void MessageDialog::setDefaultButton(QMessageBox::StandardButton button)
//{
//    setDefaultButton(buttonBox->button(QDialogButtonBox::StandardButton(button)));
//}

void MessageDialog::initConnect()
{
    connect(close_Btn, SIGNAL(clicked()), this, SLOT(close()));
    connect(buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(onButtonClicked(QAbstractButton*)));
}

void MessageDialog::moveDialog(QPoint diff) {
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

bool MessageDialog::eventFilter( QObject * object, QEvent * event ) {
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
