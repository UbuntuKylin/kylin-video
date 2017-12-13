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
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/


#include "prefshortcut.h"
#include "images.h"
#include "config.h"
//#include "actionseditor.h"
//#include "guiconfig.h"
//#include <QLabel>
//#include <QHBoxLayout>
//#include <QVBoxLayout>


PrefShortCut::PrefShortCut(QWidget * parent, Qt::WindowFlags f)
	: PrefWidget(parent, f )
{
    setupUi(this);

//    keyboard_icon = new QLabel(this);
//    actioneditor_desc = new QLabel(this);
//    actions_editor = new ActionsEditor(this);

//    QHBoxLayout *layout = new QHBoxLayout;
//    layout->addWidget(keyboard_icon, 0, Qt::AlignLeft | Qt::AlignVCenter);
//    layout->addWidget(actioneditor_desc, 0, Qt::AlignLeft | Qt::AlignVCenter);

//    QVBoxLayout *main_layout = new QVBoxLayout(this);
//    main_layout->addLayout(layout);
//    main_layout->addWidget(actions_editor);

	retranslateStrings();
}

PrefShortCut::~PrefShortCut()
{
}

QString PrefShortCut::sectionName() {
    return tr("ShortCut");
}

QPixmap PrefShortCut::sectionIcon() {
    return Images::icon("input_devices", 22);
}

void PrefShortCut::retranslateStrings() {
    retranslateUi(this);

//    keyboard_icon->setPixmap(Images::icon("keyboard"));
    actioneditor_desc->setStyleSheet("QLabel{background:transparent;font-size:12px;color:#999999;font-family:方正黑体_GBK;}");
	actioneditor_desc->setText( 
		tr("Here you can change any key shortcut. To do it double click or "
           "start typing over a shortcut cell.") );

	createHelp();
}

void PrefShortCut::setData(Preferences * pref) {

}

void PrefShortCut::getData(Preferences * pref) {
	requires_restart = false;

}

void PrefShortCut::createHelp() {
	clearHelp();

    addSectionTitle(tr("Shortcut Key"));

	setWhatsThis(actions_editor, tr("Shortcut editor"),
        tr("This table allows you to change the key shortcuts of most "
           "available actions. Double click or press enter on a item, or "
           "press the <b>Change shortcut</b> button to enter in the "
           "<i>Modify shortcut</i> dialog. There are two ways to change a "
           "shortcut: if the <b>Capture</b> button is on then just "
           "press the new key or combination of keys that you want to "
           "assign for the action (unfortunately this doesn't work for all "
           "keys). If the <b>Capture</b> button is off "
           "then you could enter the full name of the key.") );
}

#include "moc_prefshortcut.cpp"





/*
 *QPropertyAnimation *m_slidePosAni;
 * void BaseWidget::show()
{
    QWidget::show();

    m_slidePosAni->setStartValue(QPoint(width(), 0));
    m_slidePosAni->setEndValue(QPoint());
    m_slidePosAni->start();
}

void BaseWidget::hide()
{
    m_slidePosAni->setStartValue(QPoint());
    m_slidePosAni->setEndValue(QPoint(-width(), 0));
    m_slidePosAni->start();

    setAttribute(Qt::WA_TransparentForMouseEvents);
    QTimer::singleShot(m_slidePosAni->duration(), [=] { setAttribute(Qt::WA_TransparentForMouseEvents, false); QWidget::hide(); });
}

void BaseWidget::showBack()
{
    QWidget::show();

    m_slidePosAni->setStartValue(QPoint(-width(), 0));
    m_slidePosAni->setEndValue(QPoint());
    m_slidePosAni->start();
}

void BaseWidget::hideBack()
{
    m_slidePosAni->setStartValue(QPoint());
    m_slidePosAni->setEndValue(QPoint(width(), 0));
    m_slidePosAni->start();

    QTimer::singleShot(m_slidePosAni->duration(), this, &QWidget::hide);
}

void BaseWidget::destroy()
{
    m_slidePosAni->setStartValue(QPoint());
    m_slidePosAni->setEndValue(QPoint(width(), 0));
    m_slidePosAni->start();
    QTimer::singleShot(m_slidePosAni->duration(), this, &BaseWidget::destroySelf);
}*/
