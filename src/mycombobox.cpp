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

#include "mycombobox.h"
#include <QDir>
#include <QStringListModel>
#include <QDebug>

MyComboBox::MyComboBox( QWidget * parent ) : QComboBox(parent)
{
    this->setStyleSheet("QComboBox{width:150px;height:24px;border:1px solid #000000;background:#0f0f0f;font-size:12px;font-family:方正黑体_GBK;background-position:center left;padding-left:5px;color:#999999;selection-color:#ffffff;selection-background-color:#1f1f1f;}QComboBox::hover{background-color:#0f0f0f;border:1px solid #0a9ff5;font-family:方正黑体_GBK;font-size:12px;color:#999999;}QComboBox:!enabled {background:#0f0f0f;color:#383838;}QComboBox::drop-down {width:17px;border:none;background:transparent;}QComboBox::drop-down:hover {background:transparent;}QComboBox::down-arrow{image:url(:/res/combobox_arrow_normal.png);}QComboBox::down-arrow:hover{image:url(:/res/combobox_arrow_hover.png);}QComboBox::down-arrow:pressed{image:url(:/res/combobox_arrow_press.png);}QComboBox QAbstractItemView{border:1px solid #0a9ff5;background:#262626;outline:none;}");
//    this->setStyleSheet("QComboBox{width:150px;height:24px;border: 1px solid #000000;background: #0f0f0f;font-family:方正黑体_GBK;font-size:12px;color:#999999;}QComboBox::hover{background-color:#0f0f0f;border:1px solid #0a9ff5;font-family:方正黑体_GBK;font-size:12px;color:#999999;}QComboBox:enabled {background:#0f0f0f;color:#999999;}QComboBox:!enabled {background:#0f0f0f;color:#383838;}QComboBox:enabled:hover, QComboBox:enabled:focus {color: #1f1f1f;}QComboBox::drop-down {width: 17px;border: none;background: transparent;}QComboBox::drop-down:hover {background: transparent;}QComboBox::down-arrow{image:url(:/res/combobox_arrow_normal.png);}QComboBox::down-arrow:hover{image:url(:/res/combobox_arrow_hover.png);}QComboBox::down-arrow:pressed{image:url(:/res/combobox_arrow_press.png);}QComboBox QAbstractItemView {border: 1px solid #0a9ff5;background: #1f1f1f;outline: none;}");
}

MyComboBox::~MyComboBox() 
{
}

void MyComboBox::setCurrentText( const QString & text ) {
	int i = findText(text);
	if (i != -1)
		setCurrentIndex(i);
	else if (isEditable())
		setEditText(text);
	else
		setItemText(currentIndex(), text);
}

void MyComboBox::insertStringList( const QStringList & list, int index ) {
	insertItems((index < 0 ? count() : index), list);
}



MyFontComboBox::MyFontComboBox( QWidget * parent ) : QFontComboBox(parent)
{
}

MyFontComboBox::~MyFontComboBox() 
{
}

void MyFontComboBox::setCurrentText( const QString & text ) {
	int i = findText(text);
	if (i != -1)
		setCurrentIndex(i);
	else if (isEditable())
		setEditText(text);
	else
		setItemText(currentIndex(), text);
}

void MyFontComboBox::setFontsFromDir(const QString & fontdir) {
	QString current_text = currentText();

	if (fontdir.isEmpty()) {
		QFontDatabase::removeAllApplicationFonts();
		clear();
		setWritingSystem(QFontDatabase::Any);
	} else {
		QFontDatabase fdb;
		QStringList fontnames;
		QStringList fontfiles = QDir(fontdir).entryList(QStringList() << "*.ttf" << "*.otf", QDir::Files);
		for (int n=0; n < fontfiles.count(); n++) {
			qDebug() << "MyFontComboBox::setFontsFromDir: adding font:" << fontfiles[n];
			int id = fdb.addApplicationFont(fontdir +"/"+ fontfiles[n]);
			fontnames << fdb.applicationFontFamilies(id);
		}
		//fdb.removeAllApplicationFonts();
		fontnames.removeDuplicates();
		qDebug() << "MyFontComboBox::setFontsFromDir: fontnames:" << fontnames;
		clear();
		QStringListModel *m = qobject_cast<QStringListModel *>(model());
		if (m) m->setStringList(fontnames);
	}

	setCurrentText(current_text);
}

