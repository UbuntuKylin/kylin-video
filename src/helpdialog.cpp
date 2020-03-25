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

#include "helpdialog.h"
#include <QDebug>
#include <QMouseEvent>
#include <QPoint>
#include <QVBoxLayout>
#include <QTextBrowser>
#include <QPushButton>

#include "supportformats.h"
//#include "supportshortcuts.h"
#include "titlebutton.h"
#include "../smplayer/preferences.h"

HelpDialog::HelpDialog(QWidget * parent, Qt::WindowFlags f)
	: QDialog(parent, f )
    , m_dragState(NOT_DRAGGING)
    , m_startDrag(QPoint(0,0))
{
	setupUi(this);
    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    this->setFixedSize(675, 425);
    this->setAutoFillBackground(true);
    this->setAttribute(Qt::WA_TranslucentBackground);//设置窗口背景透明
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setWindowIcon(QIcon::fromTheme("kylin-video", QIcon(":/res/kylin-video.png")));
//    this->setWindowIcon(QIcon::fromTheme("kylin-video", QIcon(":/res/kylin-video.png")).pixmap(QSize(64, 64)).scaled(64, 64, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));

    //TODO: 无效
    this->setObjectName("popDialog");
    //this->setStyleSheet("QDialog#prefrecesdialog{border: 1px solid #121212;border-radius:6px;background-color:#1f1f1f;}");

    this->setMouseTracking(true);
    installEventFilter(this);

    title_widget->setAutoFillBackground(true);
    title_widget->setObjectName("leftWidget");
    //TODO:这里如果不用setObjectName的方式，而是直接使用setStyleSheet，将影响各自窗口下的子控件的样式
//    title_widget->setStyleSheet("QWidget{border:none;border-top-left-radius:6px;border-bottom-left-radius:6px;background-color:#2e2e2e;}");//

    widget->setAutoFillBackground(true);
    widget->setObjectName("rightWidget");
//    widget->setStyleSheet("QWidget{border:none;border-top-right-radius:6px;border-bottom-right-radius:6px;background-color:#1f1f1f;}");//

    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));

    okButton->setFixedSize(91, 25);
    okButton->setFocusPolicy(Qt::NoFocus);
    okButton->setStyleSheet("QPushButton{font-size:12px;background:#0f0f0f;border:1px solid #0a9ff5;color:#999999;}QPushButton:hover{background-color:#0a9ff5;border:1px solid #2db0f6;color:#ffffff;} QPushButton:pressed{background-color:#0993e3;border:1px solid #0a9ff5;color:#ffffff;}");

    title_label->setStyleSheet("QLabel{background:transparent;font-family: 方正黑体_GBK;font-size:20px;color:#999999;}");

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 5, 0, 0);
    layout->setSpacing(0);

    m_pageFormats = new SupportFormats;
    addSection(m_pageFormats);
    pages->setCurrentWidget(m_pageFormats);

    TitleButton *btn = new TitleButton(0, false, tr("Supported formats"));
    connect(btn, SIGNAL(clicked(int)), this, SLOT(onButtonClicked(int)));
    layout->addWidget(btn);
    m_buttonList << btn;
    btn->setActived(true);

//    page_shortcuts = new SupportShortcuts;
//    addSection(page_shortcuts);
//    btn = new TitleButton(1, false, tr("Supported shortcuts"));
//    connect(btn, SIGNAL(clicked(int)), this, SLOT(onButtonClicked(int)));
//    layout->addWidget(btn);
//    m_buttonList << btn;

    layout->addStretch();
    sections->setLayout(layout);

    retranslateStrings();
}

HelpDialog::~HelpDialog()
{
    for(int i=0; i<m_buttonList.count(); i++)
    {
        TitleButton *btn = m_buttonList.at(i);
        delete btn;
        btn = NULL;
    }
    m_buttonList.clear();

    if (m_pageFormats != NULL) {
        delete m_pageFormats;
        m_pageFormats = NULL;
    }
//    if (page_shortcuts != NULL) {
//        delete page_shortcuts;
//        page_shortcuts = NULL;
//    }
}

void HelpDialog::switchCurrentIDPage(int id)
{

}

void HelpDialog::setCurrentID(int id)
{
    foreach (TitleButton *button, m_buttonList) {
        if (button->id() == id) {
            button->setActived(true);
        }
        else {
            button->setActived(false);
        }
    }
}

void HelpDialog::onButtonClicked(int id)
{
    setCurrentID(id);
    if (id == 0) {
        pages->setCurrentWidget(m_pageFormats);
    }
//    else if (id == 1) {
//        pages->setCurrentWidget(page_shortcuts);
//    }
}

void HelpDialog::showSection(Section s) {
    qDebug("HelpDialog::showSection: %d", s);
}

void HelpDialog::retranslateStrings() {
	retranslateUi(this);
    icon_label->setPixmap(QPixmap(":/res/help.png"));

    okButton->setText(tr("OK"));
}

void HelpDialog::accept() {
	hide();
	setResult( QDialog::Accepted );
	emit applied();
}

void HelpDialog::addSection(QWidget *w) {
    pages->addWidget(w);
}

void HelpDialog::setData(Preferences * pref) {
    m_pageFormats->setData();
//    page_shortcuts->setData(pref);
}

// Language change stuff
void HelpDialog::changeEvent(QEvent *e) {
	if (e->type() == QEvent::LanguageChange) {
		retranslateStrings();
	} else {
		QDialog::changeEvent(e);
	}
}

void HelpDialog::moveDialog(QPoint diff) {
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

bool HelpDialog::eventFilter( QObject * object, QEvent * event ) {
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

//#include "moc_helpdialog.cpp"
