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

#include "preferencesdialog.h"
#include <QDebug>
#include <QMouseEvent>
#include <QPoint>
#include <QPainter>
#include <QBitmap>

#include "prefgeneral.h"
#include "prefvideo.h"
#include "prefaudio.h"
#include "prefperformance.h"
#include "prefsubtitles.h"
#include "prefscreenshot.h"
#include "prefshortcut.h"
#include "../titlebutton.h"
#include "preferences.h"
#include <QVBoxLayout>
#include <QTextBrowser>
#include <QPushButton>

PreferencesDialog::PreferencesDialog(QString arch_type, QString snap, QWidget * parent, Qt::WindowFlags f)
	: QDialog(parent, f )
    , m_dragState(NOT_DRAGGING)
    , m_startDrag(QPoint(0,0))
{
	setupUi(this);
    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    this->setFixedSize(675, 425);
    this->setAutoFillBackground(true);
    this->setAttribute(Qt::WA_TranslucentBackground);//设置窗口背景透明

    //TODO: 无效
    this->setObjectName("popDialog");
    //this->setStyleSheet("QDialog#prefrecesdialog{border: 1px solid #121212;border-radius:6px;background-color:#1f1f1f;}");

    this->setWindowIcon(QIcon::fromTheme("kylin-video", QIcon(":/res/kylin-video.png")));
//    this->setWindowIcon(QIcon::fromTheme("kylin-video", QIcon(":/res/kylin-video.png")).pixmap(QSize(64, 64)).scaled(64, 64, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    this->setAttribute(Qt::WA_DeleteOnClose);
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
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
    connect(applyButton, SIGNAL(clicked()), this, SLOT(apply()));

    okButton->setFixedSize(91, 25);
    cancelButton->setFixedSize(91, 25);
    applyButton->setFixedSize(91, 25);

    okButton->setFocusPolicy(Qt::NoFocus);
    cancelButton->setFocusPolicy(Qt::NoFocus);
    applyButton->setFocusPolicy(Qt::NoFocus);

    okButton->setStyleSheet("QPushButton{font-size:12px;background:#0f0f0f;border:1px solid #0a9ff5;color:#999999;}QPushButton:hover{background-color:#0a9ff5;border:1px solid #2db0f6;color:#ffffff;} QPushButton:pressed{background-color:#0993e3;border:1px solid #0a9ff5;color:#ffffff;}");
    cancelButton->setStyleSheet("QPushButton{font-size:12px;background:#0f0f0f;border:1px solid #000000;color:#999999;}QPushButton:hover{background-color:#1f1f1f;border:1px solid #0f0f0f;color:#ffffff;} QPushButton:pressed{background-color:#0d0d0d;border:1px solid #000000;color:#ffffff;}");
    applyButton->setStyleSheet("QPushButton{font-size:12px;background:#0f0f0f;border:1px solid #0a9ff5;color:#999999;}QPushButton:hover{background-color:#0a9ff5;border:1px solid #2db0f6;color:#ffffff;} QPushButton:pressed{background-color:#0993e3;border:1px solid #0a9ff5;color:#ffffff;}");

    title_label->setStyleSheet("QLabel{background:transparent;font-family: 方正黑体_GBK;font-size:18px;color:#999999;}");

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 5, 0, 0);
    layout->setSpacing(0);
	
	page_general = new PrefGeneral;
    addSection(page_general);
    pages->setCurrentWidget(page_general);

    TitleButton *btn = new TitleButton(0, false, tr("General"));
    connect(btn, SIGNAL(clicked(int)), this, SLOT(onButtonClicked(int)));
    layout->addWidget(btn);
    m_buttonList << btn;
    btn->setActived(true);

    page_video = new PrefVideo(arch_type, snap);
    addSection(page_video);
    btn = new TitleButton(1, false, tr("Video"));
    connect(btn, SIGNAL(clicked(int)), this, SLOT(onButtonClicked(int)));
    layout->addWidget(btn);
    m_buttonList << btn;

    page_audio = new PrefAudio(snap);
    addSection(page_audio);
    btn = new TitleButton(2, false, tr("Audio"));
    connect(btn, SIGNAL(clicked(int)), this, SLOT(onButtonClicked(int)));
    layout->addWidget(btn);
    m_buttonList << btn;

    page_performance = new PrefPerformance;
    addSection( page_performance );
    btn = new TitleButton(3, false, tr("Performance"));
    connect(btn, SIGNAL(clicked(int)), this, SLOT(onButtonClicked(int)));
    layout->addWidget(btn);
    m_buttonList << btn;

    page_subtitles = new PrefSubtitles;
    addSection(page_subtitles);
    btn = new TitleButton(4, false, tr("Subtitles"));
    connect(btn, SIGNAL(clicked(int)), this, SLOT(onButtonClicked(int)));
    layout->addWidget(btn);
    m_buttonList << btn;

    page_screenshot = new PrefScreenShot;
    addSection(page_screenshot);
    btn = new TitleButton(5, false, tr("ScreenShot"));
    connect(btn, SIGNAL(clicked(int)), this, SLOT(onButtonClicked(int)));
    layout->addWidget(btn);
    m_buttonList << btn;

    page_shortcut = new PrefShortCut;
    addSection(page_shortcut);
    btn = new TitleButton(6, false, tr("Shortcut Key"));
    connect(btn, SIGNAL(clicked(int)), this, SLOT(onButtonClicked(int)));
    layout->addWidget(btn);
    m_buttonList << btn;


    layout->addStretch();
    sections->setLayout(layout);

    connect(page_general, SIGNAL(ready_to_update_driver()), page_video, SLOT(update_driver_combobox()));
    connect(page_general, SIGNAL(ready_to_update_driver()), page_audio, SLOT(update_driver_combobox()));

    retranslateStrings();
}

PreferencesDialog::~PreferencesDialog()
{
    for(int i=0; i<m_buttonList.count(); i++)
    {
        TitleButton *btn = m_buttonList.at(i);
        delete btn;
        btn = NULL;
    }
    m_buttonList.clear();

    if (page_general != NULL) {
        delete page_general;
        page_general = NULL;
    }
    if (page_video != NULL) {
        delete page_video;
        page_video = NULL;
    }
    if (page_audio != NULL) {
        delete page_audio;
        page_audio = NULL;
    }
    if (page_performance != NULL) {
        delete page_performance;
        page_performance = NULL;
    }
    if (page_subtitles != NULL) {
        delete page_subtitles;
        page_subtitles = NULL;
    }
    if (page_screenshot != NULL) {
        delete page_screenshot;
        page_screenshot = NULL;
    }
    if (page_shortcut != NULL) {
        delete page_shortcut;
        page_shortcut = NULL;
    }
}

void PreferencesDialog::switchCurrentIDPage(int id)
{

}

void PreferencesDialog::setCurrentID(int id)
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

void PreferencesDialog::onButtonClicked(int id)
{
    setCurrentID(id);
    if (id == 0) {
        pages->setCurrentWidget(page_general);
    }
    else if (id == 1) {
        pages->setCurrentWidget(page_video);
    }
    else if (id == 2) {
        pages->setCurrentWidget(page_audio);
    }
    else if (id == 3) {
        pages->setCurrentWidget(page_performance);
    }
    else if (id == 4) {
        pages->setCurrentWidget(page_subtitles);
    }
    else if (id == 5) {
        pages->setCurrentWidget(page_screenshot);
    }
    else if (id == 6) {
        pages->setCurrentWidget(page_shortcut);
    }
}

void PreferencesDialog::showSection(Section s) {
	qDebug("PreferencesDialog::showSection: %d", s);
}

void PreferencesDialog::retranslateStrings() {
	retranslateUi(this);
    icon_label->setPixmap(QPixmap(":/res/settings.png"));

    okButton->setText(tr("OK"));
    cancelButton->setText(tr("Cancel"));
    applyButton->setText(tr("Apply"));
}

void PreferencesDialog::accept() {
	hide();
	setResult( QDialog::Accepted );
	emit applied();
}

void PreferencesDialog::apply() {
	setResult( QDialog::Accepted );
	emit applied();
}

void PreferencesDialog::reject() {
	hide();
	setResult( QDialog::Rejected );

	setResult( QDialog::Accepted );
}

void PreferencesDialog::addSection(PrefWidget *w) {
	pages->addWidget(w);
}

void PreferencesDialog::setData(Preferences * pref) {
	page_general->setData(pref);
    page_video->setData(pref);
    page_audio->setData(pref);
    page_performance->setData(pref);
    page_subtitles->setData(pref);
    page_screenshot->setData(pref);
    page_shortcut->setData(pref);
}

void PreferencesDialog::getData(Preferences * pref) {
	page_general->getData(pref);
    page_video->getData(pref);
    page_audio->getData(pref);
    page_performance->getData(pref);
    page_subtitles->getData(pref);
    page_screenshot->getData(pref);
    page_shortcut->getData(pref);
}

bool PreferencesDialog::requiresRestart() {
	bool need_restart = page_general->requiresRestart();
    if (!need_restart) need_restart = page_video->requiresRestart();
    if (!need_restart) need_restart = page_audio->requiresRestart();
    if (!need_restart) need_restart = page_performance->requiresRestart();
    if (!need_restart) need_restart = page_subtitles->requiresRestart();
    if (!need_restart) need_restart = page_screenshot->requiresRestart();
    if (!need_restart) need_restart = page_shortcut->requiresRestart();

	return need_restart;
}

// Language change stuff
void PreferencesDialog::changeEvent(QEvent *e) {
	if (e->type() == QEvent::LanguageChange) {
		retranslateStrings();
	} else {
		QDialog::changeEvent(e);
	}
}

void PreferencesDialog::moveDialog(QPoint diff) {
#if QT_VERSION >= 0x050000
    // Move the window with some delay.
    // Seems to work better with Qt 5

    static QPoint d;
    static int count = 0;

    d += diff;
    count++;

    if (count > 3) {
//        qDebug() << "> 3PreferencesDialog::moveWindowDiff:" << d;
        QPoint new_pos = pos() + d;
        if (new_pos.y() < 0) new_pos.setY(0);
        if (new_pos.x() < 0) new_pos.setX(0);
//        qDebug() << "PreferencesDialog::moveWindowDiff: new_pos:" << new_pos;
        move(new_pos);
        count = 0;
        d = QPoint(0,0);
    }
#else
    move(pos() + diff);
#endif
}

bool PreferencesDialog::eventFilter( QObject * object, QEvent * event ) {
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
