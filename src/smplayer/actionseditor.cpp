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

/* This is based on qq14-actioneditor-code.zip from Qt */


#include "actionseditor.h"

#include <QTableWidget>
#include <QHeaderView>

#include <QLayout>
#include <QObject>
#include <QPushButton>
#include <QString>
#include <QSettings>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QFileInfo>
#include <QRegExp>
#include <QApplication>
#include <QAction>
#include <QScrollBar>
#include <QDebug>

#include "images.h"
#include "filedialog.h"
#include "paths.h"

#include "shortcutgetter.h"
#include "global.h"

using namespace Global;


/*
#include <QLineEdit>
#include <QItemDelegate>

class MyDelegate : public QItemDelegate 
{
public:
	MyDelegate(QObject *parent = 0);

	QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                           const QModelIndex &index) const;
		virtual void setModelData(QWidget * editor, QAbstractItemModel * model, 
                              const QModelIndex & index ) const;
};

MyDelegate::MyDelegate(QObject *parent) : QItemDelegate(parent)
{
}

static QString old_accel_text;

QWidget * MyDelegate::createEditor(QWidget *parent, 
								   const QStyleOptionViewItem & option,
	                               const QModelIndex & index) const
{
	qDebug("MyDelegate::createEditor");

	old_accel_text = index.model()->data(index, Qt::DisplayRole).toString();
	//qDebug( "text: %s", old_accel_text.toUtf8().data());
	
	return QItemDelegate::createEditor(parent, option, index);
}

void MyDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                              const QModelIndex &index) const
{
	QLineEdit *line_edit = static_cast<QLineEdit*>(editor);

	QString accelText = QKeySequence(line_edit->text()).toString();
	if (accelText.isEmpty() && !line_edit->text().isEmpty()) {
		model->setData(index, old_accel_text);
	}
	else {
		model->setData(index, accelText);
	}
}
*/


QString ActionsEditor::shortcutsToString(QList <QKeySequence> shortcuts_list) {
	QString accelText = "";
//    qDebug() << "===========shortcutsToString=============";
	for (int n=0; n < shortcuts_list.count(); n++) {
		accelText += shortcuts_list[n].toString(QKeySequence::PortableText);
		if (n < (shortcuts_list.count()-1)) accelText += ", ";
	}

	//qDebug("ActionsEditor::shortcutsToString: accelText: '%s'", accelText.toUtf8().constData());

	return accelText;
}

QList <QKeySequence> ActionsEditor::stringToShortcuts(QString shortcuts) {
	QList <QKeySequence> shortcuts_list;
//    qDebug() << "===========stringToShortcuts=============";
	QStringList l = shortcuts.split(", ");

	for (int n=0; n < l.count(); n++) {
		//qDebug("%s", l[n].toUtf8().data());
#if QT_VERSION >= 0x040300
		// Qt 4.3 and 4.4 (at least on linux) seems to have a problem when using Traditional Chinese
		// QKeysequence deletes the arrow key names from the shortcut
		// so this is a work-around.
		QString s = l[n].simplified();
#else
		QString s = QKeySequence( l[n].simplified() );
#endif

		//Work-around for Simplified-Chinese
		s.replace( QString::fromUtf8("左"), "Left");
		s.replace( QString::fromUtf8("下"), "Down");
		s.replace( QString::fromUtf8("右"), "Right");
		s.replace( QString::fromUtf8("上"), "Up");

		shortcuts_list.append( s );
		//qDebug("ActionsEditor::stringToShortcuts: shortcut %d: '%s'", n, s.toUtf8().data());
	}

	return shortcuts_list;
}

#define COL_CONFLICTS 0
#define COL_SHORTCUT 1
#define COL_DESC 2
#define COL_NAME 3

ActionsEditor::ActionsEditor(QWidget * parent, Qt::WindowFlags f)
	: QWidget(parent, f)
{
    latest_dir = Paths::shortcutsPath();

    actionsTable = new QTableWidget(0, COL_NAME +1, this);
    actionsTable->setSelectionMode(QAbstractItemView::SingleSelection );
	actionsTable->verticalHeader()->hide();

//    QPalette pal = actionsTable->palette();
//    pal.setBrush(this->backgroundRole(),QBrush(QPixmap("images/background.png")) );
//    actionsTable->setPalette(pal);

#if QT_VERSION >= 0x050000
	actionsTable->horizontalHeader()->setSectionResizeMode(COL_DESC, QHeaderView::Stretch);
	actionsTable->horizontalHeader()->setSectionResizeMode(COL_NAME, QHeaderView::Stretch);
#else
	actionsTable->horizontalHeader()->setResizeMode(COL_DESC, QHeaderView::Stretch);
	actionsTable->horizontalHeader()->setResizeMode(COL_NAME, QHeaderView::Stretch);
#endif

//    background-color:#0f0f0f;
//    border:1px solid #0a9ff5;

    actionsTable->horizontalHeader()->setHighlightSections(false);//点击表时不对表头行光亮（获取焦点）
	actionsTable->setAlternatingRowColors(true);
    actionsTable->setStyleSheet("font-family:方正黑体_GBK;font-size:12px;color:#999999;selection-background-color:red;alternate-background-color: #2e2e2e; background:transparent;");
    actionsTable->horizontalHeader()->setStyleSheet("QHeaderView::section{font-family:方正黑体_GBK;font-size:13px;color:#999999;background:transparent;}"); //设置表头背景色
    actionsTable->horizontalScrollBar()->setStyleSheet("QScrollBar{background:transparent; height:10px;}"
        "QScrollBar::handle{background:lightgray; border:2px solid transparent; border-radius:5px;}"
        "QScrollBar::handle:hover{background:gray;}"
        "QScrollBar::sub-line{background:transparent;}"
        "QScrollBar::add-line{background:transparent;}");

    actionsTable->verticalScrollBar()->setStyleSheet("QScrollBar{background:transparent; width: 10px;}"
        "QScrollBar::handle{background:lightgray; border:2px solid transparent; border-radius:5px;}"
        "QScrollBar::handle:hover{background:gray;}"
        "QScrollBar::sub-line{background:transparent;}"
        "QScrollBar::add-line{background:transparent;}");

	actionsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	actionsTable->setSelectionMode(QAbstractItemView::ExtendedSelection);


    connect(actionsTable, SIGNAL(itemActivated(QTableWidgetItem *)), this, SLOT(editShortcut()));

//	saveButton = new QPushButton(this);
//	loadButton = new QPushButton(this);

//	connect(saveButton, SIGNAL(clicked()), this, SLOT(saveActionsTable()));
//	connect(loadButton, SIGNAL(clicked()), this, SLOT(loadActionsTable()));

//	editButton = new QPushButton(this);
//	connect( editButton, SIGNAL(clicked()), this, SLOT(editShortcut()) );

//    QHBoxLayout *buttonLayout = new QHBoxLayout;
//    buttonLayout->setSpacing(8);
//	buttonLayout->addWidget(editButton);
//    buttonLayout->addStretch(1);
//	buttonLayout->addWidget(loadButton);
//	buttonLayout->addWidget(saveButton);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setMargin(8);
    mainLayout->setSpacing(8);
    mainLayout->addWidget(actionsTable);
//    mainLayout->addLayout(buttonLayout);




	retranslateStrings();
}

ActionsEditor::~ActionsEditor() {
}

void ActionsEditor::retranslateStrings() {
    actionsTable->setHorizontalHeaderLabels(QStringList() << "" << tr("Shortcut") << tr("Description") << tr("Name") );
//    QTableWidgetItem *columnHeaderItem = actionsTable->horizontalHeaderItem(1);
//    columnHeaderItem->setFont(QFont("Helvetica"));//设置字体
//    columnHeaderItem->setBackgroundColor(QColor(0,60,10));//设置单元格背景颜色
//    columnHeaderItem->setTextColor(QColor(200,111,30));//设置文字颜色

//	saveButton->setText(tr("&Save"));
//	saveButton->setIcon(Images::icon("save"));

//	loadButton->setText(tr("&Load"));
//	loadButton->setIcon(Images::icon("open"));

//    editButton->setText(tr("&Change shortcut..."));
}

bool ActionsEditor::isEmpty() {
    return actionsList.isEmpty()/* && btnActionsList.isEmpty()*/;
}

void ActionsEditor::clear() {
	actionsList.clear();
//    btnActionsList.clear();
}

void ActionsEditor::addCurrentActions(QWidget *widget) {
	QAction *action;

	QList<QAction *> actions = widget->findChildren<QAction *>();
	for (int n=0; n < actions.count(); n++) {
		action = static_cast<QAction*> (actions[n]);
		/*
		if (!action->objectName().isEmpty()) {
            qDebug("ActionsEditor::addCurrentActions: action # %d: '%s' menu: %d", n, action->objectName().toUtf8().constData(), action->menu()!=0);
		}
		*/

        if (!action->objectName().isEmpty() && action->objectName() != "pl_play" && action->objectName() != "pl_remove_selected" && action->objectName() != "pl_delete_from_disk" /*&& !action->inherits("QWidgetAction")*/ && (action->menu()==0) )
	        actionsList.append(action);
    }

//    QPushButton *btn_action;
//    QList<QPushButton *> btn_actions = widget->findChildren<QPushButton *>();
//    for (int m=0; m < btn_actions.count(); m++) {
//        btn_action = static_cast<QPushButton*> (btn_actions[m]);
//        QString actionName = btn_action->objectName();
//        if (!actionName.isEmpty()) {
//            if (actionName == "PlayListBtn" || actionName == "play_next") {
//                btnActionsList.append(btn_action);
//            }
//        }
//    }

	updateView();
}

void ActionsEditor::updateView() {
    actionsTable->setRowCount(actionsList.count()/* + btnActionsList.count()*/);

    QAction *action;
//    QPushButton *btn_action;
	QString accelText;

	//actionsTable->setSortingEnabled(false);
    int action_count = actionsList.count();
    for (int n=0; n < action_count; n++) {
		action = static_cast<QAction*> (actionsList[n]);

        accelText = shortcutsToString(action->shortcuts() );


		// Conflict column
		QTableWidgetItem * i_conf = new QTableWidgetItem();

		// Name column
		QTableWidgetItem * i_name = new QTableWidgetItem(action->objectName());

		// Desc column
		QTableWidgetItem * i_desc = new QTableWidgetItem(action->text().replace("&",""));
		i_desc->setIcon( action->icon() );

		// Shortcut column
		QTableWidgetItem * i_shortcut = new QTableWidgetItem(accelText);

		// Set flags
		i_conf->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
		i_name->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
		i_desc->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
		i_shortcut->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);

		// Add items to table
		actionsTable->setItem(n, COL_CONFLICTS, i_conf );
		actionsTable->setItem(n, COL_NAME, i_name );
		actionsTable->setItem(n, COL_DESC, i_desc );
		actionsTable->setItem(n, COL_SHORTCUT, i_shortcut );

	}

//    for (int m=0; m < btnActionsList.count(); m++) {
//        btn_action = static_cast<QPushButton*> (btnActionsList[m]);
//        accelText = btn_action->shortcut().toString();//shortcutsToString(btn_action->shortcut());

//        // Conflict column
//        QTableWidgetItem * i_conf = new QTableWidgetItem();

//        // Name column
//        QTableWidgetItem * i_name = new QTableWidgetItem(btn_action->objectName());

//        // Desc column
//        QTableWidgetItem * i_desc = new QTableWidgetItem(btn_action->text().replace("&",""));
//        i_desc->setIcon( action->icon() );

//        // Shortcut column
//        QTableWidgetItem * i_shortcut = new QTableWidgetItem(accelText);

//        // Set flags
//        i_conf->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
//        i_name->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
//        i_desc->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
//        i_shortcut->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);

//        // Add items to table
//        actionsTable->setItem(m + action_count, COL_CONFLICTS, i_conf );
//        actionsTable->setItem(m + action_count, COL_NAME, i_name );
//        actionsTable->setItem(m + action_count, COL_DESC, i_desc );
//        actionsTable->setItem(m + action_count, COL_SHORTCUT, i_shortcut );

//    }

	hasConflicts(); // Check for conflicts
	actionsTable->resizeColumnsToContents();
	actionsTable->setCurrentCell(0, COL_SHORTCUT);

	//actionsTable->setSortingEnabled(true);
}


void ActionsEditor::applyChanges() {
	qDebug("ActionsEditor::applyChanges");
    int action_count = (int)actionsList.size();
    //QAction可以设置多个快捷键，QPushButton只能设置一个快捷键
    for (int row = 0; row < action_count; ++row) {
		QAction *action = actionsList[row];
		QTableWidgetItem *i = actionsTable->item(row, COL_SHORTCUT);
        action->setShortcuts(stringToShortcuts(i->text()) );//action->setShortcut( QKeySequence(i->text()) );
	}

//    for (int m=0; m < (int)btnActionsList.size(); m++) {
//        QPushButton *btn = btnActionsList[m];
//        QTableWidgetItem *i = actionsTable->item(m + action_count, COL_SHORTCUT);
//        btn->setShortcut(i->text());
//    }
}

void ActionsEditor::editShortcut() {
	QTableWidgetItem * i = actionsTable->item( actionsTable->currentRow(), COL_SHORTCUT );
	if (i) {
//        bool isbtn = false;
        QTableWidgetItem * j = actionsTable->item( actionsTable->currentRow(), COL_NAME );
//        if (j) {
//            if (j->text() == "PlayListBtn" || j->text() == "play_next") {
//                isbtn = true;
//            }
//        }
        ShortcutGetter d/*(this)*/;
		QString result = d.exec( i->text() );

		if (!result.isNull()) {
            qDebug("ActionsEditor::editShortcut: result: '%s'", result.toUtf8().constData());
			QString accelText = QKeySequence(result).toString(QKeySequence::PortableText);
			i->setText(accelText);
//            qDebug() << "accelText="<<accelText;
            if (hasConflicts()) qApp->beep();//使用默认的音量和铃声，发出铃声
		}
	}
}


int ActionsEditor::findActionName(const QString & name) {
	for (int row=0; row < actionsTable->rowCount(); row++) {
		if (actionsTable->item(row, COL_NAME)->text() == name) return row;
	}
	return -1;
}

bool ActionsEditor::containsShortcut(const QString & accel, const QString & shortcut) {
	QStringList shortcut_list = accel.split(", ");
	QString s;
	foreach(s, shortcut_list) {
		s = s.trimmed();
		//qDebug("ActionsEditor::containsShortcut: comparing '%s' with '%s'", s.toUtf8().constData(), shortcut.toUtf8().constData());
		if (s == shortcut) return true;
	}
	return false;
}

int ActionsEditor::findActionAccel(const QString & accel, int ignoreRow) {
	QStringList shortcuts = accel.split(", ");
	QString shortcut;

	for (int row = 0; row < actionsTable->rowCount(); row++) {
		QTableWidgetItem * i = actionsTable->item(row, COL_SHORTCUT);
		if (i && row != ignoreRow) {
			if (!i->text().isEmpty()) {
				foreach(shortcut, shortcuts) {
					if (containsShortcut(i->text(), shortcut.trimmed())) {
						return row;
					}
				}
			}
		}
	}
	return -1;
}

bool ActionsEditor::hasConflicts() {
	int found;
	bool conflict = false;

	QString accelText;
	QTableWidgetItem *i;

	for (int n = 0; n < actionsTable->rowCount(); n++) {
		//actionsTable->setText( n, COL_CONFLICTS, " ");
		i = actionsTable->item( n, COL_CONFLICTS );
		if (i) i->setIcon( QPixmap() );

		i = actionsTable->item(n, COL_SHORTCUT );
		if (i) {
			accelText = i->text();
			if (!accelText.isEmpty()) {
				found = findActionAccel( accelText, n );
				if ( (found != -1) /*&& (found != n)*/ ) {
					conflict = true;
					//actionsTable->setText( n, COL_CONFLICTS, "!");
					actionsTable->item( n, COL_CONFLICTS )->setIcon( Images::icon("conflict") );
				}
			}
		}
	}
	//if (conflict) qApp->beep();
	return conflict;
}


/*void ActionsEditor::saveActionsTable() {
	QString s = MyFileDialog::getSaveFileName(
                    this, tr("Choose a filename"), 
                    latest_dir,
                    tr("Key files") +" (*.keys)" );

	if (!s.isEmpty()) {
		// If filename has no extension, add it
		if (QFileInfo(s).suffix().isEmpty()) {
			s = s + ".keys";
		}
		if (QFileInfo(s).exists()) {
			int res = QMessageBox::question( this,
					tr("Confirm overwrite?"),
                    tr("The file %1 already exists.\n"
                       "Do you want to overwrite?").arg(s),
                    QMessageBox::Yes,
                    QMessageBox::No,
                    Qt::NoButton);
			if (res == QMessageBox::No ) {
            	return;
			}
		}
		latest_dir = QFileInfo(s).absolutePath();
		bool r = saveActionsTable(s);
		if (!r) {
			QMessageBox::warning(this, tr("Error"), 
               tr("The file couldn't be saved"), 
               QMessageBox::Ok, Qt::NoButton);
		}
	} 
}

bool ActionsEditor::saveActionsTable(const QString & filename) {
	qDebug("ActionsEditor::saveActions: '%s'", filename.toUtf8().data());

	QFile f( filename );
	if ( f.open( QIODevice::WriteOnly ) ) {
		QTextStream stream( &f );
		stream.setCodec("UTF-8");

		for (int row=0; row < actionsTable->rowCount(); row++) {
			stream << actionsTable->item(row, COL_NAME)->text() << "\t" 
                   << actionsTable->item(row, COL_SHORTCUT)->text() << "\n";
		}
		f.close();
		return true;
	}
	return false;
}

void ActionsEditor::loadActionsTable() {
	QString s = MyFileDialog::getOpenFileName(
                    this, tr("Choose a file"),
                    latest_dir, tr("Key files") +" (*.keys)" );

	if (!s.isEmpty()) {
		latest_dir = QFileInfo(s).absolutePath();
		bool r = loadActionsTable(s);
		if (!r) {
			QMessageBox::warning(this, tr("Error"), 
               tr("The file couldn't be loaded"), 
               QMessageBox::Ok, Qt::NoButton);
		}
	}
}

bool ActionsEditor::loadActionsTable(const QString & filename) {
	qDebug("ActionsEditor::loadActions: '%s'", filename.toUtf8().data());

	QRegExp rx("^(.*)\\t(.*)");
	int row;

    QFile f( filename );
    if ( f.open( QIODevice::ReadOnly ) ) {

        QTextStream stream( &f );
		stream.setCodec("UTF-8");

        QString line;
        while ( !stream.atEnd() ) {
            line = stream.readLine();
			qDebug("line: '%s'", line.toUtf8().data());
			if (rx.indexIn(line) > -1) {
				QString name = rx.cap(1);
				QString accelText = rx.cap(2);
				qDebug(" name: '%s' accel: '%s'", name.toUtf8().data(), accelText.toUtf8().data());
				row = findActionName(name);
				if (row > -1) {
					qDebug("Action found!");
					actionsTable->item(row, COL_SHORTCUT)->setText(accelText);
				}				
			} else {
				qDebug(" wrong line");
			}
		}
		f.close();
		hasConflicts(); // Check for conflicts

		return true;
	} else {
		return false;
	}
}*/


// Static functions
void ActionsEditor::saveToConfig(QObject *o, QSettings *set)
{
    qDebug("ActionsEditor::saveToConfig");

    set->beginGroup("actions");

    QAction *action;
    QList<QAction *> actions = o->findChildren<QAction *>();
    for (int n=0; n < actions.count(); n++) {
        action = static_cast<QAction*> (actions[n]);
        if (!action->objectName().isEmpty()/* && !action->inherits("QWidgetAction")*/) {
            QString accelText = shortcutsToString(action->shortcuts());//QString accelText = action->shortcut().toString();
            set->setValue(action->objectName(), accelText);
        }
    }

//    QPushButton *btn_action;
//    QList<QPushButton *> btn_actions = o->findChildren<QPushButton *>();
//    for (int m=0; m < btn_actions.count(); m++) {
//        btn_action = static_cast<QPushButton*> (btn_actions[m]);
//        if (!btn_action->objectName().isEmpty()) {
//            if (btn_action->objectName() == "PlayListBtn" || btn_action->objectName() == "play_next") {
//                QString accelText = btn_action->shortcut().toString();
//                set->setValue(btn_action->objectName(), accelText);
//            }
//        }
//    }

    set->endGroup();
}


void ActionsEditor::loadFromConfig(QObject *o, QSettings *set)
{
	set->beginGroup("actions");

	QAction *action;
	QString accelText;

	QList<QAction *> actions = o->findChildren<QAction *>();
//    qDebug() << "ActionsEditor::loadFromConfig actions count="<<actions.count();
	for (int n=0; n < actions.count(); n++) {
		action = static_cast<QAction*> (actions[n]);
        if (!action->objectName().isEmpty()/* && !action->inherits("QWidgetAction")*/) {
			QString current = shortcutsToString(action->shortcuts());
			accelText = set->value(action->objectName(), current).toString();
            action->setShortcuts(stringToShortcuts(accelText));
//            qDebug() << "ActionsEditor::loadFromConfig action->objectName()="<<action->objectName()<<",accelText="<<accelText;
		}
    }

    QPushButton *btn_action;
    QList<QPushButton *> btn_actions = o->findChildren<QPushButton *>();
    for (int m=0; m < btn_actions.count(); m++) {
        btn_action = static_cast<QPushButton*> (btn_actions[m]);
        if (!btn_action->objectName().isEmpty()) {
            QList <QKeySequence> cur_shortcuts_list;
            cur_shortcuts_list.append(btn_action->shortcut());
            QString current = shortcutsToString(cur_shortcuts_list);
            accelText = set->value(btn_action->objectName(), current).toString();
            QList <QKeySequence> shortcuts_list = stringToShortcuts(accelText);
            if (shortcuts_list.count() > 0) {
                btn_action->setShortcut(shortcuts_list.at(0));
            }
        }
    }

	set->endGroup();
}

QAction * ActionsEditor::findAction(QObject *o, const QString & name) {
	QAction *action;

	QList<QAction *> actions = o->findChildren<QAction *>();
	for (int n=0; n < actions.count(); n++) {
		action = static_cast<QAction*> (actions[n]);
        qDebug("ActionsEditor::findAction: %s", action->objectName().toLatin1().constData());
		if (name == action->objectName()) return action;
    }

	return 0;
}

QStringList ActionsEditor::actionsNames(QObject *o) {
	QStringList l;

	QAction *action;

	QList<QAction *> actions = o->findChildren<QAction *>();
	for (int n=0; n < actions.count(); n++) {
		action = static_cast<QAction*> (actions[n]);
		//qDebug("action name: '%s'", action->objectName().toUtf8().data());
		//qDebug("action name: '%s'", action->text().toUtf8().data());
		if (!action->objectName().isEmpty())
			l.append( action->objectName() );
    }

	return l;
}


// Language change stuff
void ActionsEditor::changeEvent(QEvent *e) {
	if (e->type() == QEvent::LanguageChange) {
		retranslateStrings();
	} else {
		QWidget::changeEvent(e);
	}
}

//#include "moc_actionseditor.cpp"
