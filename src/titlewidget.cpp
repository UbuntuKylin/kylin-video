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
#include <QStyleOption>

TitleWidget::TitleWidget(QWidget *parent)
    : QWidget(parent)
    , m_spreadAnimation(0)
    , m_gatherAnimation(0)
    , m_dragState(NOT_DRAGGING)
    , m_startDrag(QPoint(0,0))
{
    this->setMouseTracking(true);
    this->setAutoFillBackground(true);
    this->setFocusPolicy(Qt::StrongFocus);
    this->setAttribute(Qt::WA_TranslucentBackground, true);//窗体标题栏不透明，背景透明

    initWidgets();

    this->installEventFilter(this);
}

TitleWidget::~TitleWidget()
{
    if (m_spreadAnimation) delete m_spreadAnimation;
    if (m_gatherAnimation) delete m_gatherAnimation;
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

void TitleWidget::initWidgets()
{
    m_layout = new QHBoxLayout(this);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(0);

    initLeftContent();
    initMiddleContent();
    initRightContent();
}

void TitleWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if (window()->isMaximized()) {
            this->updateMaxButtonStatus(false);
            window()->showNormal();
        }
        else if (! window()->isFullScreen()) {// It would be normal state
            this->updateMaxButtonStatus(true);
            window()->showMaximized();
        }
    }

    QWidget::mouseDoubleClickEvent(event);
}

void TitleWidget::initLeftContent()
{
    QWidget *w = new QWidget;
    m_lLayout = new QHBoxLayout(w);
    m_lLayout->setContentsMargins(5, 0, 0, 0);
    m_lLayout->setSpacing(5);

    m_logoLabel = new QLabel(this);
//    QImage image(":/res/logo.png");
//    image = image.scaled(QSize(32, 32), Qt::KeepAspectRatio, Qt::SmoothTransformation);
//    m_logoLabel->setPixmap(QPixmap::fromImage(image));
    m_logoLabel->setPixmap(QPixmap(":/res/logo.png"));
//    m_logoLabel->setScaledContents(true);//自动缩放,显示图像大小自动调整为Qlabel大小

    m_softLabel = new QLabel(this);
    m_softLabel->setText(tr("Kylin Video"));
    m_softLabel->setStyleSheet("QLabel{font-size:14px;font-style:italic;color:#ffffff;}");//font-weight:bold;
    m_lLayout->addWidget(m_logoLabel);
    m_lLayout->addWidget(m_softLabel);
    m_layout->addWidget(w, 1, Qt::AlignLeft);
}

void TitleWidget::initMiddleContent()
{
    QWidget *w = new QWidget;
    w->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_mLayout = new QHBoxLayout(w);
    m_mLayout->setContentsMargins(0, 0, 0, 0);
    m_mLayout->setSpacing(0);

    m_titleLabel = new QLabel(this);
    m_titleLabel->setMaximumWidth(300);
    m_titleLabel->setStyleSheet("QLabel{font-size:12px;color:#ffffff;}");
    m_mLayout->addWidget(m_titleLabel, 0 , Qt::AlignHCenter);
    m_layout->addWidget(w);
}

void TitleWidget::setTitleName(const QString &name)
{
    QFont ft;
    QFontMetrics fm(ft);
    QString elided_text = fm.elidedText(name, Qt::ElideRight, this->m_titleLabel->maximumWidth());
    this->m_titleLabel->setText(elided_text);
    if(elided_text.endsWith("…"))
        this->m_titleLabel->setToolTip(name);
}

void TitleWidget::cleaTitleName()
{
    m_titleLabel->clear();
}

void TitleWidget::initRightContent()
{
    QWidget *w = new QWidget;
    m_rLayout = new QHBoxLayout(w);
    m_rLayout->setContentsMargins(0, 0, 0, 0);
    m_rLayout->setSpacing(0);
    m_layout->addWidget(w, 1, Qt::AlignRight);

    menu_button = new SystemButton();
    menu_button->setMouseTracking(true);
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

    m_rLayout->addWidget(menu_button);
    m_rLayout->addWidget(min_button);
    m_rLayout->addWidget(max_button);
    m_rLayout->addWidget(close_button);

    connect(menu_button, SIGNAL(clicked()), this, SIGNAL(requestShowMenu()));
    connect(close_button, SIGNAL(clicked()), this, SIGNAL(requestCloseWindow()));
    connect(max_button, SIGNAL(clicked(bool)), this, SIGNAL(requestMaxWindow(bool)));
    connect(min_button, &SystemButton::clicked, this, [=] () {
        max_button->loadPixmap(":/res/max.png");
        emit this->requestMinWindow();
    });

}

void TitleWidget::updateMaxButtonStatus(bool is_maxed)
{
    if (is_maxed) {
        max_button->loadPixmap(":/res/unmax.png");
    }
    else {
        max_button->loadPixmap(":/res/max.png");
    }
}

void TitleWidget::checkUnderMouse()
{
    if ((isVisible()) && (!underMouse())) {
        this->showGatherAnimated();
    }
}

void TitleWidget::spreadAniFinished()
{
}

void TitleWidget::gatherAniFinished()
{
    QWidget::hide();
}

void TitleWidget::showSpreadAnimated()
{
    if (!m_spreadAnimation) {
        m_spreadAnimation = new QPropertyAnimation(this, "pos");
        connect(m_spreadAnimation, SIGNAL(finished()), this, SLOT(spreadAniFinished()));
    }

    QPoint initial_position = QPoint(pos().x(), -this->height());
    QPoint final_position = QPoint(0, 0);
    move(initial_position);

    QWidget::show();

    m_spreadAnimation->setDuration(300);
    m_spreadAnimation->setEndValue(final_position);
    m_spreadAnimation->setStartValue(initial_position);
    m_spreadAnimation->start();
}

void TitleWidget::showGatherAnimated()
{
    if (!m_gatherAnimation) {
        m_gatherAnimation = new QPropertyAnimation(this, "pos");
        connect(m_gatherAnimation, SIGNAL(finished()), this, SLOT(gatherAniFinished()));
    }

    QPoint initial_position = QPoint(0, 0);
    QPoint final_position = QPoint(pos().x(), -this->height());
    move(initial_position);

    m_gatherAnimation->setDuration(300);
    m_gatherAnimation->setStartValue(initial_position);
    m_gatherAnimation->setEndValue(final_position);
    m_gatherAnimation->start();
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

bool TitleWidget::eventFilter(QObject * obj, QEvent * event)
{
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

    if (event->type() == QEvent::MouseMove) {

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

    emit mouseMovedDiff(diff);
    m_startDrag = pos;

    event->accept();
    return true;
}
