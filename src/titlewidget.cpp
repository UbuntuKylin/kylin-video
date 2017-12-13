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

#include "titlewidget.h"
#include <QDebug>
#include <QHBoxLayout>
#include <QPainter>
#include <QProcess>
#include <QResizeEvent>
#include <QShortcut>
#include <QMenu>
#include <QStyleFactory>
#include <QToolButton>
#include <QTimer>
#include <QStyleOption>


TitleWidget::TitleWidget(QWidget *parent)
    : QWidget(parent)
    , turned_on(false)
    , spacing(0)
//    , activation_area(Anywhere)
    , internal_widget(0)
    , timer(0)
    , spreadAnimation(0)
    , gatherAnimation(0)
    , drag_state(NOT_TDRAGGING)
    , start_drag(QPoint(0,0))
{
    this->setMouseTracking(true);

    this->setAutoFillBackground(true);//20170615
    this->setStyleSheet("QWidget{background:transparent;}");//20170615   rgba(255, 255, 255, 20%);

    QPalette palette;
    palette.setColor(QPalette::Background, QColor("#040404"));
    this->setPalette(palette);

    initWidgets();

    parent->installEventFilter(this);
    installFilter(parent);

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(checkUnderMouse()));
    timer->setInterval(3000);
}

TitleWidget::~TitleWidget() {
    if (spreadAnimation) delete spreadAnimation;
    if (gatherAnimation) delete gatherAnimation;
    if (timer != NULL) {
        disconnect(timer,SIGNAL(timeout()),this,SLOT(checkUnderMouse()));
        if(timer->isActive()) {
            timer->stop();
        }
        delete timer;
        timer = NULL;
    }
    if (menu_button) {
        delete menu_button;
        menu_button = NULL;
    }
    if (min_button) {
        delete min_button;
        min_button = NULL;
    }
    if (max_button) {
        delete max_button;
        max_button = NULL;
    }
    if (close_button) {
        delete close_button;
        close_button = NULL;
    }
}

void TitleWidget::initWidgets() {
    m_layout = new QHBoxLayout(this);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(0);

    initLeftContent();
    initMiddleContent();
    initRightContent();
}

//20170810
void TitleWidget::mouseDoubleClickEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        if (window()->isMaximized())
            window()->showNormal();
        else if (! window()->isFullScreen())  // It would be normal state
            window()->showMaximized();
    }

    QWidget::mouseDoubleClickEvent(event);
}

void TitleWidget::initLeftContent() {
    QWidget *w = new QWidget;
    m_lLayout = new QHBoxLayout(w);
    m_lLayout->setContentsMargins(5, 0, 0, 0);
    m_lLayout->setSpacing(5);

    logo_label = new QLabel(this);
//    QImage image(":/res/logo.png");
//    image = image.scaled(QSize(32, 32), Qt::KeepAspectRatio, Qt::SmoothTransformation);
//    logo_label->setPixmap(QPixmap::fromImage(image));
    logo_label->setPixmap(QPixmap(":/res/logo.png"));
//    logo_label->setScaledContents(true);//自动缩放,显示图像大小自动调整为Qlabel大小

    soft_label = new QLabel(this);
    soft_label->setText(tr("Kylin Video"));
    soft_label->setStyleSheet("QLabel{font-size:14px;font-style:italic;color:#ffffff;}");//font-weight:bold;
    m_lLayout->addWidget(logo_label);
    m_lLayout->addWidget(soft_label);
    m_layout->addWidget(w, 1, Qt::AlignLeft);
}

void TitleWidget::initMiddleContent()
{
    QWidget *w = new QWidget;
    w->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_mLayout = new QHBoxLayout(w);
    m_mLayout->setContentsMargins(0, 0, 0, 0);
    m_mLayout->setSpacing(0);

    title_label = new QLabel(this);
    title_label->setMaximumWidth(300);
    title_label->setStyleSheet("QLabel{font-size:12px;color:#ffffff;}");
    m_mLayout->addWidget(title_label, 0 , Qt::AlignHCenter);
    m_layout->addWidget(w);
}

void TitleWidget::set_title_name(QString title){
    QFont ft;
    QFontMetrics fm(ft);
    QString elided_text = fm.elidedText(title, Qt::ElideRight, this->title_label->maximumWidth());
    this->title_label->setText(elided_text);
    if(elided_text.endsWith("…"))
        this->title_label->setToolTip(title);
}

void TitleWidget::clear_title_name() {
    title_label->clear();
}

void TitleWidget::initRightContent() {
    QWidget *w = new QWidget;
    m_rLayout = new QHBoxLayout(w);
    m_rLayout->setContentsMargins(0, 0, 0, 0);
    m_rLayout->setSpacing(0);

    m_layout->addWidget(w, 1, Qt::AlignRight);

//    menu_button = new QPushButton();
//    min_button = new QPushButton();
//    max_button = new QPushButton();
//    close_button = new QPushButton();
    menu_button = new SystemButton();
    menu_button->loadPixmap(":/res/option.png");
    menu_button->setObjectName("menu_button");
    min_button = new SystemButton();
    min_button->loadPixmap(":/res/min.png");
    min_button->setObjectName("min_button");
    max_button = new SystemButton();
    max_button->loadPixmap(":/res/max.png");
    max_button->setObjectName("max_button");
    close_button = new SystemButton();
    close_button->loadPixmap(":/res/close.png");
    close_button->setObjectName("close_button");
    menu_button->setFixedSize(36,36);
    min_button->setFixedSize(36,36);
    max_button->setFixedSize(36,36);
    close_button->setFixedSize(36,36);
    min_button->setObjectName("min_button");
    close_button->setObjectName("close_button");
    menu_button->setObjectName("menu_button");
    max_button->setObjectName("max_button");

    min_button->setFocusPolicy(Qt::NoFocus);
    close_button->setFocusPolicy(Qt::NoFocus);
    menu_button->setFocusPolicy(Qt::NoFocus);
    max_button->setFocusPolicy(Qt::NoFocus);

//    min_button->setStyleSheet("QPushButton{background-image:url(':/res/min_normal.png');border:0px;}QPushButton:hover{background:url(':/res/min_hover.png');}QPushButton:pressed{background:url(':/res/min_press.png');}");
//    close_button->setStyleSheet("QPushButton{background-image:url(':/res/close_normal.png');border:0px;}QPushButton:hover{background:url(':/res/close_hover.png');}QPushButton:pressed{background:url(':/res/close_press.png');}");
//    menu_button->setStyleSheet("QPushButton{background-image:url(':/res/option_normal.png');border:0px;}QPushButton:hover{background:url(':/res/option_hover.png');}QPushButton:pressed{background:url(':/res/option_press.png');}");
//    max_button->setStyleSheet("QPushButton{background-image:url(':/res/max_normal.png');border:0px;}QPushButton:hover{background:url(':/res/max_hover.png');}QPushButton:pressed{background:url(':/res/max_press.png');}");

    m_rLayout->addWidget(menu_button);
    m_rLayout->addWidget(min_button);
    m_rLayout->addWidget(max_button);
    m_rLayout->addWidget(close_button);

    connect(menu_button, SIGNAL(clicked()), this, SIGNAL(sig_menu()));
    connect(min_button, SIGNAL(clicked()), this, SLOT(onMinBtnClicked()));
    connect(close_button, SIGNAL(clicked()), this, SIGNAL(sig_close()));
    connect(max_button, SIGNAL(clicked()), this, SLOT(onMaxOrNormal()));
}

void TitleWidget::update_max_status(bool is_maxed) {
    if (is_maxed) {
        max_button->loadPixmap(":/res/unmax.png");
//        max_button->setStyleSheet("QPushButton{background-image:url(':/res/unmax_normal.png');border:0px;}QPushButton:hover{background:url(':/res/unmax_hover.png');}QPushButton:pressed{background:url(':/res/unmax_press.png');}");
    }
    else {
        max_button->loadPixmap(":/res/max.png");
//        max_button->setStyleSheet("QPushButton{background-image:url(':/res/max_normal.png');border:0px;}QPushButton:hover{background:url(':/res/max_hover.png');}QPushButton:pressed{background:url(':/res/max_press.png');}");
    }
}

void TitleWidget::onMaxOrNormal() {

    emit this->sig_max();
}

void TitleWidget::onMinBtnClicked() {
    emit this->sig_min();
    max_button->loadPixmap(":/res/max.png");
//    max_button->setStyleSheet("QPushButton{background-image:url(':/res/max_normal.png');border:0px;}QPushButton:hover{background:url(':/res/max_hover.png');}QPushButton:pressed{background:url(':/res/max_press.png');}");
}

void TitleWidget::setHideDelay(int ms) {
    timer->setInterval(ms);
}

int TitleWidget::hideDelay() {
    return timer->interval();
}

void TitleWidget::installFilter(QObject *o) {
    QObjectList children = o->children();
    for (int n=0; n < children.count(); n++) {
        if (children[n]->isWidgetType()) {
            if (children[n]->objectName() == "PlayListViewScrollBar") {//kobe:让滚动条可以鼠标拖动
                continue;
            }
            else if (children[n]->objectName() == "min_button" || children[n]->objectName() == "close_button" || children[n]->objectName() == "menu_button" || children[n]->objectName() == "max_button") {
                continue;
            }
            else if (children[n]->objectName() == "StopBtn" || children[n]->objectName() == "PrevBtn" || children[n]->objectName() == "PlayBtn" || children[n]->objectName() == "NextBtn") {
                continue;
            }
            else if (children[n]->objectName() == "SoundBtn" || children[n]->objectName() == "FullScreenBtn" || children[n]->objectName() == "PlayListBtn" || children[n]->objectName() == "VolumeSlider") {
                continue;
            }
            QWidget *w = static_cast<QWidget *>(children[n]);
            w->setMouseTracking(true);
            w->installEventFilter(this);
            installFilter(children[n]);
        }
    }
}

void TitleWidget::activate() {
    turned_on = true;
    if (timer->isActive())
        timer->stop();
    timer->start();
}

void TitleWidget::deactivate() {
    turned_on = false;
    timer->stop();
    this->showWidget();
}

void TitleWidget::showAlways() {
    timer->stop();
    QPoint dest_position = QPoint(0, 0);
    move(dest_position);
    QWidget::show();
}

void TitleWidget::enable_turned_on() {
    turned_on = true;
}

void TitleWidget::showWidget() {
    showSpreadAnimated();
    if (timer->isActive())
        timer->stop();
    timer->start();
}

void TitleWidget::checkUnderMouse() {
    if ((isVisible()) && (!underMouse())) {
        this->showGatherAnimated();
    }
}

bool TitleWidget::eventFilter(QObject * obj, QEvent * event) {
    QEvent::Type type = event->type();
    if (type != QEvent::MouseButtonPress
        && type != QEvent::MouseButtonRelease
        && type != QEvent::MouseMove)
        return false;
    QMouseEvent *mouseEvent = dynamic_cast<QMouseEvent*>(event);
    if (!mouseEvent)
        return false;
    if (mouseEvent->modifiers() != Qt::NoModifier) {
        drag_state = NOT_TDRAGGING;
        return false;
    }

    if (turned_on) {
        if (event->type() == QEvent::MouseMove) {
    //        qDebug() << "TitleWidget::eventFilter: mouse move" << obj;
            if (!isVisible()) {
                showWidget();
                /*if (activation_area == Anywhere) {
                    showWidget();
                } else {
                    QMouseEvent * mouse_event = dynamic_cast<QMouseEvent*>(event);
                    QWidget * parent = parentWidget();
                    QPoint p = parent->mapFromGlobal(mouse_event->globalPos());
                    if (p.y() > (parent->height() - height() - spacing)) {
                        showWidget();
                    }
                }*/
            }
        }

        if (type == QEvent::MouseButtonPress) {
            if (mouseEvent->button() != Qt::LeftButton) {
                drag_state = NOT_TDRAGGING;
                return false;
            }

            drag_state = START_TDRAGGING;
            start_drag = mouseEvent->globalPos();
            // Don't filter, so others can have a look at it too
            return false;
        }

        if (type == QEvent::MouseButtonRelease) {
            if (drag_state != TDRAGGING || mouseEvent->button() != Qt::LeftButton) {
                drag_state = NOT_TDRAGGING;
                return false;
            }

            // Stop dragging and eat event
            drag_state = NOT_TDRAGGING;
            event->accept();
            return true;
        }

        // type == QEvent::MouseMove
        if (drag_state == NOT_TDRAGGING)
            return false;

        // buttons() note the s
        if (mouseEvent->buttons() != Qt::LeftButton) {
            drag_state = NOT_TDRAGGING;
            return false;
        }

        QPoint pos = mouseEvent->globalPos();
        QPoint diff = pos - start_drag;
        if (drag_state == START_TDRAGGING) {
            // Don't start dragging before moving at least DRAG_THRESHOLD pixels
            if (abs(diff.x()) < 4 && abs(diff.y()) < 4)
                return false;

            drag_state = TDRAGGING;
        }

        emit mouseMovedDiff(diff);
        start_drag = pos;

        event->accept();
        return true;
    }
    else {
        if (type == QEvent::MouseButtonPress) {
            if (mouseEvent->button() != Qt::LeftButton) {
                drag_state = NOT_TDRAGGING;
                return false;
            }

            drag_state = START_TDRAGGING;
            start_drag = mouseEvent->globalPos();
            // Don't filter, so others can have a look at it too
            return false;
        }

        if (type == QEvent::MouseButtonRelease) {
            if (drag_state != TDRAGGING || mouseEvent->button() != Qt::LeftButton) {
                drag_state = NOT_TDRAGGING;
                return false;
            }

            // Stop dragging and eat event
            drag_state = NOT_TDRAGGING;
            event->accept();
            return true;
        }

        // type == QEvent::MouseMove
        if (drag_state == NOT_TDRAGGING)
            return false;

        // buttons() note the s
        if (mouseEvent->buttons() != Qt::LeftButton) {
            drag_state = NOT_TDRAGGING;
            return false;
        }

        QPoint pos = mouseEvent->globalPos();
        QPoint diff = pos - start_drag;
        if (drag_state == START_TDRAGGING) {
            // Don't start dragging before moving at least DRAG_THRESHOLD pixels
            if (abs(diff.x()) < 4 && abs(diff.y()) < 4)
                return false;

            drag_state = TDRAGGING;
        }

        emit mouseMovedDiff(diff);
        start_drag = pos;

        event->accept();
        return true;
    }
}

void TitleWidget::spreadAniFinished() {
}

void TitleWidget::gatherAniFinished() {
    QWidget::hide();
}

void TitleWidget::show_title_widget() {
    QWidget::show();
}

void TitleWidget::showSpreadAnimated() {
    if (!spreadAnimation) {
        spreadAnimation = new QPropertyAnimation(this, "pos");
        connect(spreadAnimation, SIGNAL(finished()), this, SLOT(spreadAniFinished()));
    }

    QPoint initial_position = QPoint(pos().x(), -this->height());
    QPoint final_position = QPoint(0, 0);
    move(initial_position);

    QWidget::show();

    spreadAnimation->setDuration(300);
    spreadAnimation->setEndValue(final_position);
    spreadAnimation->setStartValue(initial_position);
    spreadAnimation->start();
}

void TitleWidget::showGatherAnimated() {
    if (!gatherAnimation) {
        gatherAnimation = new QPropertyAnimation(this, "pos");
        connect(gatherAnimation, SIGNAL(finished()), this, SLOT(gatherAniFinished()));
    }

    QPoint initial_position = QPoint(0, 0);
    QPoint final_position = QPoint(pos().x(), -this->height());
    move(initial_position);

    gatherAnimation->setDuration(300);
    gatherAnimation->setStartValue(initial_position);
    gatherAnimation->setEndValue(final_position);
    gatherAnimation->start();
}

void TitleWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

//    QStyleOption opt;
//    opt.init(this);
//    QPainter p(this);
//    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    QPainter p(this);
    p.setCompositionMode(QPainter::CompositionMode_Clear);
    p.fillRect(rect(), Qt::SolidPattern);//p.fillRect(0, 0, this->width(), this->height(), Qt::SolidPattern);
}



/*TitleWidget::TitleWidget(QWidget *parent)
    : QWidget(parent)
    , turned_on(false)
    , spacing(0)
    , activation_area(Anywhere)
    , internal_widget(0)
    , spreadAnimation(0)
    , gatherAnimation(0)
    , drag_state(NOT_TDRAGGING)
    , start_drag(QPoint(0,0))
{
    this->setAutoFillBackground(true);//20170615
    this->setStyleSheet("QWidget{background:transparent;}");//20170615   rgba(255, 255, 255, 20%);

    QPalette palette;
    palette.setColor(QPalette::Background, QColor("#040404"));
    this->setPalette(palette);

    initWidgets();
}

TitleWidget::~TitleWidget() {
    if (spreadAnimation) delete spreadAnimation;
    if (gatherAnimation) delete gatherAnimation;
    if (menu_button) {
        delete menu_button;
        menu_button = NULL;
    }
    if (min_button) {
        delete min_button;
        min_button = NULL;
    }
    if (max_button) {
        delete max_button;
        max_button = NULL;
    }
    if (close_button) {
        delete close_button;
        close_button = NULL;
    }
}

void TitleWidget::initWidgets() {
    m_layout = new QHBoxLayout(this);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(0);

    initLeftContent();
    initMiddleContent();
    initRightContent();
}

void TitleWidget::mouseDoubleClickEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        if (window()->isMaximized())
            window()->showNormal();
        else if (! window()->isFullScreen())  // It would be normal state
            window()->showMaximized();
    }

    QWidget::mouseDoubleClickEvent(event);
}

void TitleWidget::initLeftContent() {
    QWidget *w = new QWidget;
    m_lLayout = new QHBoxLayout(w);
    m_lLayout->setContentsMargins(5, 0, 0, 0);
    m_lLayout->setSpacing(5);

    logo_label = new QLabel(this);
//    QImage image(":/res/logo.png");
//    image = image.scaled(QSize(32, 32), Qt::KeepAspectRatio, Qt::SmoothTransformation);
//    logo_label->setPixmap(QPixmap::fromImage(image));
    logo_label->setPixmap(QPixmap(":/res/logo.png"));
//    logo_label->setScaledContents(true);//自动缩放,显示图像大小自动调整为Qlabel大小

    soft_label = new QLabel(this);
    soft_label->setText(tr("Kylin Video"));
    soft_label->setStyleSheet("QLabel{font-size:14px;font-style:italic;color:#ffffff;}");//font-weight:bold;
    m_lLayout->addWidget(logo_label);
    m_lLayout->addWidget(soft_label);
    m_layout->addWidget(w, 1, Qt::AlignLeft);
}

void TitleWidget::initMiddleContent()
{
    QWidget *w = new QWidget;
    w->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_mLayout = new QHBoxLayout(w);
    m_mLayout->setContentsMargins(0, 0, 0, 0);
    m_mLayout->setSpacing(0);

    title_label = new QLabel(this);
    title_label->setMaximumWidth(300);
    title_label->setStyleSheet("QLabel{font-size:12px;color:#ffffff;}");
    m_mLayout->addWidget(title_label, 0 , Qt::AlignHCenter);
    m_layout->addWidget(w);
}

void TitleWidget::set_title_name(QString title){
    QFont ft;
    QFontMetrics fm(ft);
    QString elided_text = fm.elidedText(title, Qt::ElideRight, this->title_label->maximumWidth());
    this->title_label->setText(elided_text);
    if(elided_text.endsWith("…"))
        this->title_label->setToolTip(title);
}

void TitleWidget::clear_title_name() {
    title_label->clear();
}

void TitleWidget::initRightContent() {
    QWidget *w = new QWidget;
    m_rLayout = new QHBoxLayout(w);
    m_rLayout->setContentsMargins(0, 0, 0, 0);
    m_rLayout->setSpacing(0);

    m_layout->addWidget(w, 1, Qt::AlignRight);

//    menu_button = new QPushButton();
//    min_button = new QPushButton();
//    max_button = new QPushButton();
//    close_button = new QPushButton();
    menu_button = new SystemButton();
    menu_button->loadPixmap(":/res/option.png");
    menu_button->setObjectName("menu_button");
    min_button = new SystemButton();
    min_button->loadPixmap(":/res/min.png");
    min_button->setObjectName("min_button");
    max_button = new SystemButton();
    max_button->loadPixmap(":/res/max.png");
    max_button->setObjectName("max_button");
    close_button = new SystemButton();
    close_button->loadPixmap(":/res/close.png");
    close_button->setObjectName("close_button");

    menu_button->setFixedSize(36,36);
    min_button->setFixedSize(36,36);
    max_button->setFixedSize(36,36);
    close_button->setFixedSize(36,36);

    min_button->setFocusPolicy(Qt::NoFocus);
    close_button->setFocusPolicy(Qt::NoFocus);
    menu_button->setFocusPolicy(Qt::NoFocus);
    max_button->setFocusPolicy(Qt::NoFocus);

//    min_button->setStyleSheet("QPushButton{background-image:url(':/res/min_normal.png');border:0px;}QPushButton:hover{background:url(':/res/min_hover.png');}QPushButton:pressed{background:url(':/res/min_press.png');}");
//    close_button->setStyleSheet("QPushButton{background-image:url(':/res/close_normal.png');border:0px;}QPushButton:hover{background:url(':/res/close_hover.png');}QPushButton:pressed{background:url(':/res/close_press.png');}");
//    menu_button->setStyleSheet("QPushButton{background-image:url(':/res/option_normal.png');border:0px;}QPushButton:hover{background:url(':/res/option_hover.png');}QPushButton:pressed{background:url(':/res/option_press.png');}");
//    max_button->setStyleSheet("QPushButton{background-image:url(':/res/max_normal.png');border:0px;}QPushButton:hover{background:url(':/res/max_hover.png');}QPushButton:pressed{background:url(':/res/max_press.png');}");

    m_rLayout->addWidget(menu_button);
    m_rLayout->addWidget(min_button);
    m_rLayout->addWidget(max_button);
    m_rLayout->addWidget(close_button);

    connect(menu_button, SIGNAL(clicked()), this, SIGNAL(sig_menu()));
    connect(min_button, SIGNAL(clicked()), this, SLOT(onMinBtnClicked()));
    connect(close_button, SIGNAL(clicked()), this, SIGNAL(sig_close()));
    connect(max_button, SIGNAL(clicked()), this, SLOT(onMaxOrNormal()));
}

void TitleWidget::update_max_status(bool is_maxed) {
    if (is_maxed) {
        max_button->loadPixmap(":/res/unmax.png");
//        max_button->setStyleSheet("QPushButton{background-image:url(':/res/unmax_normal.png');border:0px;}QPushButton:hover{background:url(':/res/unmax_hover.png');}QPushButton:pressed{background:url(':/res/unmax_press.png');}");
    }
    else {
        max_button->loadPixmap(":/res/max.png");
//        max_button->setStyleSheet("QPushButton{background-image:url(':/res/max_normal.png');border:0px;}QPushButton:hover{background:url(':/res/max_hover.png');}QPushButton:pressed{background:url(':/res/max_press.png');}");
    }}

void TitleWidget::onMaxOrNormal() {

    emit this->sig_max();
}

void TitleWidget::onMinBtnClicked() {
    emit this->sig_min();
    max_button->loadPixmap(":/res/max.png");
//    max_button->setStyleSheet("QPushButton{background-image:url(':/res/max_normal.png');border:0px;}QPushButton:hover{background:url(':/res/max_hover.png');}QPushButton:pressed{background:url(':/res/max_press.png');}");
}

void TitleWidget::showAlways() {
    QPoint dest_position = QPoint(0, 0);
    move(dest_position);
    QWidget::show();
}

void TitleWidget::enable_turned_on() {
    turned_on = true;
}

void TitleWidget::spreadAniFinished() {
}

void TitleWidget::gatherAniFinished() {
    QWidget::hide();
}

void TitleWidget::show_title_widget() {
    QWidget::show();
}

void TitleWidget::showSpreadAnimated() {
    if (!spreadAnimation) {
        spreadAnimation = new QPropertyAnimation(this, "pos");
        connect(spreadAnimation, SIGNAL(finished()), this, SLOT(spreadAniFinished()));
    }

    QPoint initial_position = QPoint(pos().x(), -this->height());
    QPoint final_position = QPoint(0, 0);
    move(initial_position);

    QWidget::show();

    spreadAnimation->setDuration(300);
    spreadAnimation->setEndValue(final_position);
    spreadAnimation->setStartValue(initial_position);
    spreadAnimation->start();
}

void TitleWidget::showGatherAnimated() {
    if (!gatherAnimation) {
        gatherAnimation = new QPropertyAnimation(this, "pos");
        connect(gatherAnimation, SIGNAL(finished()), this, SLOT(gatherAniFinished()));
    }

    QPoint initial_position = QPoint(0, 0);
    QPoint final_position = QPoint(pos().x(), -this->height());
    move(initial_position);

    gatherAnimation->setDuration(300);
    gatherAnimation->setStartValue(initial_position);
    gatherAnimation->setEndValue(final_position);
    gatherAnimation->start();
}

void TitleWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

//    QStyleOption opt;
//    opt.init(this);
//    QPainter p(this);
//    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    QPainter p(this);
    p.setCompositionMode(QPainter::CompositionMode_Clear);
    p.fillRect(rect(), Qt::SolidPattern);//p.fillRect(0, 0, this->width(), this->height(), Qt::SolidPattern);
}*/
