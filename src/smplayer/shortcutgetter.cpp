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

    Note: The ShortcutGetter class is taken from the source code of Edyuk
    (http://www.edyuk.org/), from file 3rdparty/qcumber/qshortcutdialog.cpp

    Copyright (C) 2006 FullMetalCoder
    License: GPL

    I modified it to support multiple shortcuts and some other few changes.
*/


/****************************************************************************
**
** Copyright (C) 2006 FullMetalCoder
**
** This file is part of the Edyuk project (beta version)
** 
** This file may be used under the terms of the GNU General Public License
** version 2 as published by the Free Software Foundation and appearing in the
** file GPL.txt included in the packaging of this file.
**
** Notes :	Parts of the project are derivative work of Trolltech's QSA library
** or Trolltech's Qt4 framework but, unless notified, every single line of code
** is the work of the Edyuk team or a contributor. 
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/


#include "shortcutgetter.h"
#include "images.h"

#include <QLayout>
#include <QHash>
#include <QLabel>
#include <QString>
#include <QShortcut>
#include <QLineEdit>
#include <QKeyEvent>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QDebug>

#if 1

static QHash<int, const char*> keyMap;

static void initKeyMap()
{
	if ( !keyMap.isEmpty() )
		return;
	
	/*
		I'm a bit unsure about these one...
	*/
	keyMap[Qt::Key_Escape] = "Escape";
	keyMap[Qt::Key_Return] = "Return";
	keyMap[Qt::Key_Enter] = "Enter";
	keyMap[Qt::Key_Insert] = "Ins";
	keyMap[Qt::Key_Delete] = "Delete";
	keyMap[Qt::Key_Home] = "Home";
	keyMap[Qt::Key_End] = "End";
	keyMap[Qt::Key_Left] = "Left";
	keyMap[Qt::Key_Up] = "Up";
	keyMap[Qt::Key_Right] = "Right";
	keyMap[Qt::Key_Down] = "Down";
	keyMap[Qt::Key_PageUp] = "PgUp";
	keyMap[Qt::Key_PageDown] = "PgDown";
	keyMap[Qt::Key_CapsLock] = "CapsLock";
	keyMap[Qt::Key_NumLock] = "NumLock";
	keyMap[Qt::Key_ScrollLock] = "ScrollLock";
	
	/*
		These one are quite sure...
	*/
	keyMap[Qt::Key_F1] = "F1";
	keyMap[Qt::Key_F2] = "F2";
	keyMap[Qt::Key_F3] = "F3";
	keyMap[Qt::Key_F4] = "F4";
	keyMap[Qt::Key_F5] = "F5";
	keyMap[Qt::Key_F6] = "F6";
	keyMap[Qt::Key_F7] = "F7";
	keyMap[Qt::Key_F8] = "F8";
	keyMap[Qt::Key_F9] = "F9";
	keyMap[Qt::Key_F10] = "F10";
	keyMap[Qt::Key_F11] = "F11";
	keyMap[Qt::Key_F12] = "F12";
	keyMap[Qt::Key_F13] = "F13";
	keyMap[Qt::Key_F14] = "F14";
	keyMap[Qt::Key_F15] = "F15";
	keyMap[Qt::Key_F16] = "F16";
	keyMap[Qt::Key_F17] = "F17";
	keyMap[Qt::Key_F18] = "F18";
	keyMap[Qt::Key_F19] = "F19";
	keyMap[Qt::Key_F20] = "F20";
	keyMap[Qt::Key_F21] = "F21";
	keyMap[Qt::Key_F22] = "F22";
	keyMap[Qt::Key_F23] = "F23";
	keyMap[Qt::Key_F24] = "F24";
	keyMap[Qt::Key_F25] = "F25";
	keyMap[Qt::Key_F26] = "F26";
	keyMap[Qt::Key_F27] = "F27";
	keyMap[Qt::Key_F28] = "F28";
	keyMap[Qt::Key_F29] = "F29";
	keyMap[Qt::Key_F30] = "F30";
	keyMap[Qt::Key_F31] = "F31";
	keyMap[Qt::Key_F32] = "F32";
	keyMap[Qt::Key_F33] = "F33";
	keyMap[Qt::Key_F34] = "F34";
	keyMap[Qt::Key_F35] = "F35";
	
	keyMap[Qt::Key_Exclam] = "!";
	keyMap[Qt::Key_QuoteDbl] = "\"";
	keyMap[Qt::Key_NumberSign] = "-";
	keyMap[Qt::Key_Dollar] = "$";
	keyMap[Qt::Key_Percent] = "%";
	keyMap[Qt::Key_Ampersand] = "&amp;";
	keyMap[Qt::Key_Apostrophe] = "\'";
	keyMap[Qt::Key_ParenLeft] = "(";
	keyMap[Qt::Key_ParenRight] = ")";
	keyMap[Qt::Key_Asterisk] = "*";
	keyMap[Qt::Key_Plus] = "+";
	keyMap[Qt::Key_Comma] = ",";
	keyMap[Qt::Key_Minus] = "-";
	keyMap[Qt::Key_Period] = "Period";
	keyMap[Qt::Key_Slash] = "/";
	
	keyMap[Qt::Key_0] = "0";
	keyMap[Qt::Key_1] = "1";
	keyMap[Qt::Key_2] = "2";
	keyMap[Qt::Key_3] = "3";
	keyMap[Qt::Key_4] = "4";
	keyMap[Qt::Key_5] = "5";
	keyMap[Qt::Key_6] = "6";
	keyMap[Qt::Key_7] = "7";
	keyMap[Qt::Key_8] = "8";
	keyMap[Qt::Key_9] = "9";
	
	keyMap[Qt::Key_Colon] = ":";
	keyMap[Qt::Key_Semicolon] = ";";
	keyMap[Qt::Key_Less] = "<";
	keyMap[Qt::Key_Equal] = "=";
	keyMap[Qt::Key_Greater] = ">";
	keyMap[Qt::Key_Question] = "?";
	keyMap[Qt::Key_At] = "@";
	
	keyMap[Qt::Key_A] = "A";
	keyMap[Qt::Key_B] = "B";
	keyMap[Qt::Key_C] = "C";
	keyMap[Qt::Key_D] = "D";
	keyMap[Qt::Key_E] = "E";
	keyMap[Qt::Key_F] = "F";
	keyMap[Qt::Key_G] = "G";
	keyMap[Qt::Key_H] = "H";
	keyMap[Qt::Key_I] = "I";
	keyMap[Qt::Key_J] = "J";
	keyMap[Qt::Key_K] = "K";
	keyMap[Qt::Key_L] = "L";
	keyMap[Qt::Key_M] = "M";
	keyMap[Qt::Key_N] = "N";
	keyMap[Qt::Key_O] = "O";
	keyMap[Qt::Key_P] = "P";
	keyMap[Qt::Key_Q] = "Q";
	keyMap[Qt::Key_R] = "R";
	keyMap[Qt::Key_S] = "S";
	keyMap[Qt::Key_T] = "T";
	keyMap[Qt::Key_U] = "U";
	keyMap[Qt::Key_V] = "V";
	keyMap[Qt::Key_W] = "W";
	keyMap[Qt::Key_X] = "X";
	keyMap[Qt::Key_Y] = "Y";
	keyMap[Qt::Key_Z] = "Z";
	
	keyMap[Qt::Key_BracketLeft] = "[";
	keyMap[Qt::Key_Backslash] = "\\";
	keyMap[Qt::Key_BracketRight] = "]";
	
	keyMap[Qt::Key_Underscore] = "_";
	keyMap[Qt::Key_BraceLeft] = "{";
	keyMap[Qt::Key_Bar] = "|";
	keyMap[Qt::Key_BraceRight] = "}";
	keyMap[Qt::Key_AsciiTilde] = "~";

	// Added by rvm:
	keyMap[Qt::Key_Space] = "Space";
	keyMap[Qt::Key_Backspace] = "Backspace";
	keyMap[Qt::Key_MediaPlay] = "Media Play";
    keyMap[Qt::Key_MediaStop] = "Media Stop";
	keyMap[Qt::Key_MediaPrevious] = "Media Previous";
	keyMap[Qt::Key_MediaNext] = "Media Next";
	keyMap[Qt::Key_MediaRecord] = "Media Record";
	keyMap[Qt::Key_MediaLast] = "Media Last"; // doesn't work?
	keyMap[Qt::Key_VolumeUp] = "Volume Up";
	keyMap[Qt::Key_VolumeDown] = "Volume Down";
	keyMap[Qt::Key_VolumeMute] = "Volume Mute";	
	keyMap[Qt::Key_Back] = "Back";	
	keyMap[Qt::Key_Forward] = "Forward";	
	keyMap[Qt::Key_Stop] = "Stop";
}

static QString keyToString(int k)
{
	if (	k == Qt::Key_Shift || k == Qt::Key_Control || k == Qt::Key_Meta ||
			k == Qt::Key_Alt || k == Qt::Key_AltGr )
		return QString::null;

	initKeyMap();
    qDebug() << "========keyToString========";
	
	return keyMap[k];
}

#else

static QString keyToString(int k)
{
	if (	k == Qt::Key_Shift || k == Qt::Key_Control || k == Qt::Key_Meta ||
			k == Qt::Key_Alt || k == Qt::Key_AltGr )
		return QString::null;

	return QKeySequence(k).toString();
}

#endif

static QStringList modToString(Qt::KeyboardModifiers k)
{
	//qDebug("modToString: k: %x", (int) k);
	QStringList l;
	
	if ( k & Qt::ShiftModifier )
		l << "Shift";
	if ( k & Qt::ControlModifier )
		l << "Ctrl";
	if ( k & Qt::AltModifier )
		l << "Alt";
	if ( k & Qt::MetaModifier )
		l << "Meta";
	if ( k & Qt::GroupSwitchModifier )
		;
	if ( k & Qt::KeypadModifier )
		;
	
	return l;
}


ShortcutGetter::ShortcutGetter(/*bool isbtn, */QWidget *parent) : QDialog(parent)
  , m_dragState(NOT_DRAGGING)
  , m_startDrag(QPoint(0,0))
  , m_centerWidget(new QWidget(this))
{
    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    this->setFixedSize(438, 320);
    this->setAutoFillBackground(true);
    this->setAttribute(Qt::WA_TranslucentBackground);//设置窗口背景透明
//    this->setStyleSheet("QDialog{border: 1px solid #121212;border-radius:1px;background-color:#1f1f1f;}");
    this->setWindowIcon(QIcon::fromTheme("kylin-video", QIcon(":/res/kylin-video.png")));
//    this->setWindowIcon(QIcon::fromTheme("kylin-video", QIcon(":/res/kylin-video.png")).pixmap(QSize(64, 64)).scaled(64, 64, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));

    this->setMouseTracking(true);
	setWindowTitle(tr("Modify shortcut"));

    //TODO: 无效
    this->setObjectName("popDialog");
    //this->setStyleSheet("QDialog#prefrecesdialog{border: 1px solid #121212;border-radius:6px;background-color:#1f1f1f;}");

    m_centerWidget->setGeometry(0,0,this->width(),this->height());
    m_centerWidget->setAutoFillBackground(true);
    m_centerWidget->setObjectName("centerWidget");

    QVBoxLayout *vbox = new QVBoxLayout(m_centerWidget);
	vbox->setMargin(2);
    vbox->setSpacing(10);
    vbox->setContentsMargins(5,5,5,5);

    closeBtn = new QPushButton(this);
    closeBtn->setFixedSize(36, 36);
    closeBtn->setFocusPolicy(Qt::NoFocus);
    closeBtn->setStyleSheet("QPushButton{background-image:url(':/res/close_normal.png');border:0px;}QPushButton:hover{background:url(':/res/close_hover.png');}QPushButton:pressed{background:url(':/res/close_press.png');}");
    vbox->addWidget(closeBtn, 0, Qt::AlignRight);

	// List and buttons added by rvm
	list = new QListWidget(this);
    list->setObjectName("ShortCutList");
	connect(list, SIGNAL(currentRowChanged(int)), this, SLOT(rowChanged(int)));
	vbox->addWidget(list);

	QHBoxLayout *hbox = new QHBoxLayout;
    addItem = new QPushButton(/*Images::icon("plus"), "",*/ this);
    addItem->setStyleSheet("QPushButton{font-size:12px;background:#0f0f0f;border:1px solid #0a9ff5;color:#999999;}QPushButton:hover{background-color:#0a9ff5;border:1px solid #2db0f6;color:#ffffff;} QPushButton:pressed{background-color:#0993e3;border:1px solid #0a9ff5;color:#ffffff;}");
    addItem->setFixedSize(40, 25);
    addItem->setFocusPolicy(Qt::NoFocus);
    addItem->setText("+");
    addItem->setToolTip(tr("Add shortcut"));
	connect(addItem, SIGNAL(clicked()), this, SLOT(addItemClicked()));

    removeItem = new QPushButton(/*Images::icon("delete_normal"), "", */this);
    removeItem->setStyleSheet("QPushButton{font-size:12px;background:#0f0f0f;border:1px solid #0a9ff5;color:#999999;}QPushButton:hover{background-color:#0a9ff5;border:1px solid #2db0f6;color:#ffffff;} QPushButton:pressed{background-color:#0993e3;border:1px solid #0a9ff5;color:#ffffff;}");
    removeItem->setFixedSize(40, 25);
    removeItem->setFocusPolicy(Qt::NoFocus);
    removeItem->setText("-");
    removeItem->setToolTip(tr("Remove shortcut"));
	connect(removeItem, SIGNAL(clicked()), this, SLOT(removeItemClicked()));

	hbox->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Expanding));
	hbox->addWidget(addItem);
	hbox->addWidget(removeItem);
	vbox->addLayout(hbox);

//    if (isbtn) {
//        addItem->hide();
//        removeItem->hide();
//    }
//    else {
//        addItem->show();
//        removeItem->show();
//    }

	QLabel *l = new QLabel(this);
    l->setStyleSheet("QLabel{background:transparent;font-size:12px;color:#999999;font-family:方正黑体_GBK;}");
	l->setText(tr("Press the key combination you want to assign"));
	vbox->addWidget(l);

	leKey = new QLineEdit(this);
    leKey->setStyleSheet("QLineEdit {border:1px solid #000000;background: #0f0f0f;font-family:方正黑体_GBK;font-size:12px;color:#999999;}QLineEdit::hover{border: 1px solid #000000;background: #0a0a0a;font-family:方正黑体_GBK;font-size:12px;color:#ffffff;}QLineEdit:enabled {background: #0a0a0a;color:#999999;}QLineEdit:enabled:hover, QLineEdit:enabled:focus {background: #0a0a0a;color:#ffffff;}QLineEdit:!enabled {color: #383838;}");
//    leKey->setStyleSheet("QLineEdit {height: 25px;border: 1px solid #000000;background: #0f0f0f;font-family:方正黑体_GBK;font-size:12px;color:#999999;}QLineEdit::hover{border: 1px solid #000000;background: #0a0a0a;font-family:方正黑体_GBK;font-size:12px;color:#ffffff;}QLineEdit:enabled {background: #0a0a0a;color:#999999;}QLineEdit:enabled:hover, QLineEdit:enabled:focus {background: #0a0a0a;color:#ffffff;}QLineEdit:!enabled {color: #383838;}");
	connect(leKey, SIGNAL(textChanged(const QString &)), this, SLOT(textChanged(const QString &)));

	leKey->installEventFilter(this);
	vbox->addWidget(leKey);

	// Change by rvm: use a QDialogButtonBox instead of QPushButtons
	// and add a clear button
	setCaptureKeyboard(true);
	QDialogButtonBox * buttonbox = new QDialogButtonBox(QDialogButtonBox::Ok |
                                                     QDialogButtonBox::Cancel | 
                                                     QDialogButtonBox::Reset );
	QPushButton * clearbutton = buttonbox->button(QDialogButtonBox::Reset);
    clearbutton->setStyleSheet("QPushButton{font-size:12px;background:#0f0f0f;border:1px solid #0a9ff5;color:#999999;}QPushButton:hover{background-color:#0a9ff5;border:1px solid #2db0f6;color:#ffffff;} QPushButton:pressed{background-color:#0993e3;border:1px solid #0a9ff5;color:#ffffff;}");

    clearbutton->setFixedSize(91, 25);
	clearbutton->setText( tr("Clear") );
//    clearbutton->setStyleSheet("QPushButton{font-size:12px;background:#ffffff;border:1px solid #0a9ff5;color:#000000;}QPushButton:hover{background-color:#ffffff;border:1px solid #3f96e4;color:#000000;} QPushButton:pressed{background-color:#ffffff;border:1px solid #3f96e4;color:#000000;}");

    QPushButton * okbutton = buttonbox->button(QDialogButtonBox::Ok);
    okbutton->setStyleSheet("QPushButton{font-size:12px;background:#0f0f0f;border:1px solid #0a9ff5;color:#999999;}QPushButton:hover{background-color:#0a9ff5;border:1px solid #2db0f6;color:#ffffff;} QPushButton:pressed{background-color:#0993e3;border:1px solid #0a9ff5;color:#ffffff;}");
    okbutton->setFixedSize(91, 25);
    okbutton->setText( tr("OK") );

    QPushButton * cancelbutton = buttonbox->button(QDialogButtonBox::Cancel);
    cancelbutton->setStyleSheet("QPushButton{font-size:12px;background:#0f0f0f;border:1px solid #000000;color:#999999;}QPushButton:hover{background-color:#1f1f1f;border:1px solid #0f0f0f;color:#ffffff;} QPushButton:pressed{background-color:#0d0d0d;border:1px solid #000000;color:#ffffff;}");
    cancelbutton->setFixedSize(91, 25);
    cancelbutton->setText(tr("Cancel"));

	QPushButton * captureButton = new QPushButton(tr("Capture"), this);
    captureButton->setStyleSheet("QPushButton{font-size:12px;background:#0f0f0f;border:1px solid #0a9ff5;color:#999999;}QPushButton:hover{background-color:#0a9ff5;border:1px solid #2db0f6;color:#ffffff;} QPushButton:pressed{background-color:#0993e3;border:1px solid #0a9ff5;color:#ffffff;}");
    captureButton->setFixedSize(91, 25);
    captureButton->setToolTip(tr("Capture keystrokes") );
	captureButton->setCheckable( captureKeyboard() );
	captureButton->setChecked( captureKeyboard() );
	connect(captureButton, SIGNAL(toggled(bool)), 
            this, SLOT(setCaptureKeyboard(bool)));
    connect(closeBtn, SIGNAL(clicked(bool)), this, SLOT(reject()));

	buttonbox->addButton(captureButton, QDialogButtonBox::ActionRole);

	connect( buttonbox, SIGNAL(accepted()), this, SLOT(accept()) );
	connect( buttonbox, SIGNAL(rejected()), this, SLOT(reject()) );
	connect( clearbutton, SIGNAL(clicked()), leKey, SLOT(clear()) );
	vbox->addWidget(buttonbox);
}

void ShortcutGetter::setCaptureKeyboard(bool b) { 
	capture = b; 
	leKey->setReadOnly(b);
	leKey->setFocus();
}

// Added by rvm
void ShortcutGetter::rowChanged(int row) {
	QString s = list->item(row)->text();
	leKey->setText(s);
	leKey->setFocus();
}

// Added by rvm
void ShortcutGetter::textChanged(const QString & text) {
	list->item(list->currentRow())->setText(text);
}

// Added by rvm
void ShortcutGetter::addItemClicked() {
	qDebug("ShortcutGetter::addItemClicked");
	list->addItem("");
	list->setCurrentRow( list->count()-1 ); // Select last item
}

// Added by rvm
void ShortcutGetter::removeItemClicked() {
	qDebug("ShortcutGetter::removeItemClicked");
	if (list->count() > 1) {
		QListWidgetItem * i = list->takeItem( list->currentRow() );
		if (i) delete i;
	} else {
		list->setCurrentRow(0);
		leKey->setText("");
	}
}

QString ShortcutGetter::exec(const QString& s)
{
	// Added by rvm
	QStringList shortcuts = s.split(", ");
	QString shortcut;
	foreach(shortcut, shortcuts) {
		list->addItem(shortcut.trimmed());
	}
	list->setCurrentRow(0);

	bStop = false;

	if (QDialog::exec() == QDialog::Accepted) {
		// Added by rvm
		QStringList l;
		for (int n = 0; n < list->count(); n++) {
			QString shortcut = list->item(n)->text();
			if (!shortcut.isEmpty()) {
				//qDebug("ShortcutGetter::exec: shortcut: '%s'", shortcut.toUtf8().constData());
				l << shortcut;
			}
		}
		QString res = l.join(", ");
		if (res.isNull()) res = "";
		return res;
	}

	return QString();
}

bool ShortcutGetter::event(QEvent *e)
{
	if (!capture) return QDialog::event(e);

	QString key;
	QStringList mods;
	QKeyEvent *k = static_cast<QKeyEvent*>(e);
			
	switch ( e->type() )
	{
		case QEvent::KeyPress :
					
		if ( bStop )
		{
			lKeys.clear();
			bStop = false;
		}

		key = keyToString(k->key());
		mods = modToString(k->modifiers());

		//qDebug("event: key.count: %d, mods.count: %d", key.count(), mods.count());

		if ( key.count() || mods.count() )
		{
						
			if ( key.count() && !lKeys.contains(key) )
				lKeys << key;
						
			foreach ( key, mods )
				if ( !lKeys.contains(key) )
					lKeys << key;
			
			} else {
				key = k->text();
						
				if ( !lKeys.contains(key) )
					lKeys << key;
			}
					
			setText();
			break;
					
		case QEvent::KeyRelease :
					
			bStop = true;
			break;
					
			/*
		case QEvent::ShortcutOverride :
			leKey->setText("Shortcut override");
			break;
			*/
					
		default:
			return QDialog::event(e);
			break;
	}
	
	return true;
}
		
bool ShortcutGetter::eventFilter(QObject *o, QEvent *e)
{
    if (!capture) return QDialog::eventFilter(o, e);

    if (	e->type() == QEvent::KeyPress ||
            e->type() ==QEvent::KeyRelease )
        return event(e);
    else
        return QDialog::eventFilter(o, e);
}
		
void ShortcutGetter::setText()
{
	QStringList seq;
			
	if ( lKeys.contains("Shift") )
		seq << "Shift";
			
	if ( lKeys.contains("Ctrl") )
		seq << "Ctrl";
				
	if ( lKeys.contains("Alt") )
		seq << "Alt";
			
	if ( lKeys.contains("Meta") )
		seq << "Meta";
			
	foreach ( QString s, lKeys ) {
		//qDebug("setText: s: '%s'", s.toUtf8().data());
		if ( s != "Shift" && s != "Ctrl"
			&& s != "Alt" && s != "Meta" )
			seq << s;
	}
			
	leKey->setText(seq.join("+"));
	//leKey->selectAll();
}
