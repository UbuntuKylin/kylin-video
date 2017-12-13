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

#include "audiodelaydialog.h"
#include "images.h"
#include "version.h"
#include "global.h"
#include "paths.h"
#include <QDesktopServices>
#include <QMouseEvent>
#include <QPoint>

using namespace Global;

AudioDelayDialog::AudioDelayDialog(QWidget * parent, Qt::WindowFlags f)
	: QDialog(parent, f)
    , drag_state(NOT_AADRAGGING)
    , start_drag(QPoint(0,0))
{
	setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setFixedSize(380, 170);
    this->setStyleSheet("QDialog{border: 1px solid #121212;border-radius:1px;background-color:#1f1f1f;}");
    this->setWindowIcon(QIcon(":/res/kylin-video.png"));//setWindowIcon( Images::icon("logo", 64) );
    this->setAutoFillBackground(true);
    this->setMouseTracking(true);
    installEventFilter(this);

    title_label->setStyleSheet("QLabel{background:transparent;font-size:14px;color:#999999;font-family:方正黑体_GBK;}");//font-weight:bold;
    label->setStyleSheet("QLabel{background:transparent;font-size:12px;color:#999999;font-family:方正黑体_GBK;}");//font-weight:bold;

    spinBox->setStyleSheet("QSpinBox {height: 24px;min-width: 40px;border: 1px solid #000000;background: #0f0f0f;font-family:方正黑体_GBK;font-size:12px;color:#999999;}QSpinBox:hover {height: 24px;min-width: 40px;background-color:#0f0f0f;border:1px solid #0a9ff5;font-family:方正黑体_GBK;font-size:12px;color:#999999;}QSpinBox:enabled {color: #999999;}QSpinBox:enabled:hover, QSpinBox:enabled:focus {color: #999999;}QSpinBox:!enabled {color: #383838;background: transparent;}QSpinBox::up-button {border: none;width: 17px;height: 12px;image: url(:/res/spin_top_arrow_normal.png);}QSpinBox::up-button:hover {image: url(:/res/spin_top_arrow_hover.png);}QSpinBox::up-button:pressed {image: url(:/res/spin_top_arrow_press.png);}QSpinBox::up-button:!enabled {background: transparent;}QSpinBox::up-button:enabled:hover {background: rgb(255, 255, 255, 30);}QSpinBox::down-button {border: none;width: 17px;height: 12px;image: url(:/res/spin_bottom_arrow_normal.png);}QSpinBox::down-button:hover {image: url(:/res/spin_bottom_arrow_hover.png);}QSpinBox::down-button:pressed {image: url(:/res/spin_bottom_arrow_press.png);}QSpinBox::down-button:!enabled {background: transparent;}QSpinBox::down-button:hover{background: #0f0f0f;}");

//    baseWidget->setAutoFillBackground(true);
//    QPalette palette;
//    palette.setBrush(QPalette::Background, QBrush(QPixmap(":/res/about_bg.png")));
//    baseWidget->setPalette(palette);
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

AudioDelayDialog::~AudioDelayDialog()
{

}

void AudioDelayDialog::setDefaultValue(int audio_delay)
{
    spinBox->setValue(audio_delay);
}

int AudioDelayDialog::getCurrentValue()
{
    return spinBox->value();
}

void AudioDelayDialog::initConnect()
{
    connect(okBtn, SIGNAL(clicked()), this, SLOT(accept()));
    connect(cancelBtn, SIGNAL(clicked()), this, SLOT(close()));
    connect(closeBtn, SIGNAL(clicked()), this, SLOT(close()));
}

void AudioDelayDialog::moveDialog(QPoint diff) {
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

bool AudioDelayDialog::eventFilter( QObject * object, QEvent * event ) {
    QEvent::Type type = event->type();
    if (type != QEvent::MouseButtonPress
        && type != QEvent::MouseButtonRelease
        && type != QEvent::MouseMove)
        return false;

    QMouseEvent *mouseEvent = dynamic_cast<QMouseEvent*>(event);
    if (!mouseEvent)
        return false;

    if (mouseEvent->modifiers() != Qt::NoModifier) {
        drag_state = NOT_AADRAGGING;
        return false;
    }

    if (type == QEvent::MouseButtonPress) {
        if (mouseEvent->button() != Qt::LeftButton) {
            drag_state = NOT_AADRAGGING;
            return false;
        }

        drag_state = START_AADRAGGING;
        start_drag = mouseEvent->globalPos();
        // Don't filter, so others can have a look at it too
        return false;
    }

    if (type == QEvent::MouseButtonRelease) {
        if (drag_state != AADRAGGING || mouseEvent->button() != Qt::LeftButton) {
            drag_state = NOT_AADRAGGING;
            return false;
        }

        // Stop dragging and eat event
        drag_state = NOT_AADRAGGING;
        event->accept();
        return true;
    }

    // type == QEvent::MouseMove
    if (drag_state == NOT_AADRAGGING)
        return false;

    // buttons() note the s
    if (mouseEvent->buttons() != Qt::LeftButton) {
        drag_state = NOT_AADRAGGING;
        return false;
    }

    QPoint pos = mouseEvent->globalPos();
    QPoint diff = pos - start_drag;
    if (drag_state == START_AADRAGGING) {
        // Don't start dragging before moving at least DRAG_THRESHOLD pixels
        if (abs(diff.x()) < 4 && abs(diff.y()) < 4)
            return false;

        drag_state = AADRAGGING;
    }
    this->moveDialog(diff);

    start_drag = pos;
    event->accept();
    return true;
}
