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

#include "filepropertiesdialog.h"
#include <QListWidget>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QPushButton>
#include "images.h"
#include "infofile.h"
#include "playerid.h"
#include <QDebug>
#include <QMouseEvent>
#include <QPoint>
#include <QScrollBar>
#include "titlebutton.h"

FilePropertiesDialog::FilePropertiesDialog( QWidget* parent, Qt::WindowFlags f )
	: QDialog(parent, f)
{
	setupUi(this);

    this->setFixedSize(650, 509);
    this->setStyleSheet("QDialog{border: 1px solid #121212;border-radius:1px;background-color:#1f1f1f;}");
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setAutoFillBackground(true);

    info_edit->setStyleSheet("QTextEdit {border: 1px solid #000000;color: #999999;background: #0f0f0f;font-family:方正黑体_GBK;font-size: 12px;}");
    info_edit->verticalScrollBar()->setStyleSheet("QScrollBar:vertical {width: 12px;background: #141414;margin:0px 0px 0px 0px;border:1px solid #141414;}QScrollBar::handle:vertical {width: 12px;min-height: 45px;background: #292929;margin-left: 0px;margin-right: 0px;}QScrollBar::handle:vertical:hover {background: #3e3e3e;}QScrollBar::handle:vertical:pressed {background: #272727;}QScrollBar::sub-line:vertical {height: 6px;background: transparent;subcontrol-position: top;}QScrollBar::add-line:vertical {height: 6px;background: transparent;subcontrol-position: bottom;}QScrollBar::sub-line:vertical:hover {background: #292929;}QScrollBar::add-line:vertical:hover {background: #292929;}QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {background: transparent;}");

    this->setMouseTracking(true);
    installEventFilter(this);

    title_widget->setAutoFillBackground(true);
    title_widget->setStyleSheet("QWidget{border:none;background-color:#2e2e2e;}");
//    QPalette palette;
//    palette.setColor(QPalette::Background, QColor("#2e2e2e"));
//    title_widget->setPalette(palette);

    demuxer_listbox->setStyleSheet("QListWidget{color: #999999;font-size: 12px;border: 0px solid rgba(255, 0, 0, 0.7);background-color: #171717;outline:none;}QListWidget::item{color: #999999;height: 30px;}QListWidget::item:hover{background: #242424;}QListWidget::item:selected:!active {color: #999999;background-color: #171717;}QListWidget::item:selected:active {color: white;background-color: #0f0f0f;}");
    vc_listbox->setStyleSheet("QListWidget{color: #999999;font-size: 12px;border: 0px solid rgba(255, 0, 0, 0.7);background-color: #171717;outline:none;}QListWidget::item{color: #999999;height: 30px;}QListWidget::item:hover{background: #242424;}QListWidget::item:selected:!active {color: #999999;background-color: #171717;}QListWidget::item:selected:active {color: white;background-color: #0f0f0f;}");
    ac_listbox->setStyleSheet("QListWidget{color: #999999;font-size: 12px;border: 0px solid rgba(255, 0, 0, 0.7);background-color: #171717;outline:none;}QListWidget::item{color: #999999;height: 30px;}QListWidget::item:hover{background: #242424;}QListWidget::item:selected:!active {color: #999999;background-color: #171717;}QListWidget::item:selected:active {color: white;background-color: #0f0f0f;}");

    demuxer_listbox->verticalScrollBar()->setStyleSheet("QScrollBar:vertical {width: 12px;background: #141414;margin:0px 0px 0px 0px;border:1px solid #141414;}QScrollBar::handle:vertical {width: 12px;min-height: 45px;background: #292929;margin-left: 0px;margin-right: 0px;}QScrollBar::handle:vertical:hover {background: #3e3e3e;}QScrollBar::handle:vertical:pressed {background: #272727;}QScrollBar::sub-line:vertical {height: 6px;background: transparent;subcontrol-position: top;}QScrollBar::add-line:vertical {height: 6px;background: transparent;subcontrol-position: bottom;}QScrollBar::sub-line:vertical:hover {background: #292929;}QScrollBar::add-line:vertical:hover {background: #292929;}QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {background: transparent;}");
    demuxer_listbox->horizontalScrollBar()->setStyleSheet("QScrollBar:horizontal {height: 8px;background: #141414;margin:0px 0px 0px 0px;border:1px solid #141414;}QScrollBar::handle:horizontal {height: 8px;min-width: 45px;background: #292929;margin-left: 0px;margin-right: 0px;}QScrollBar::handle:horizontal:hover {background: #3e3e3e;}QScrollBar::handle:horizontal:pressed {background: #272727;}QScrollBar::sub-line:horizontal {width: 6px;background: transparent;subcontrol-position: left;}QScrollBar::add-line:horizontal {width: 6px;background: transparent;subcontrol-position: right;}QScrollBar::sub-line:horizontal:hover {background: #292929;}QScrollBar::add-line:horizontal:hover {background: #292929;}QScrollBar::add-page:horizontal,QScrollBar::sub-page:horizontal {background: transparent;}");
    vc_listbox->verticalScrollBar()->setStyleSheet("QScrollBar:vertical {width: 12px;background: #141414;margin:0px 0px 0px 0px;border:1px solid #141414;}QScrollBar::handle:vertical {width: 12px;min-height: 45px;background: #292929;margin-left: 0px;margin-right: 0px;}QScrollBar::handle:vertical:hover {background: #3e3e3e;}QScrollBar::handle:vertical:pressed {background: #272727;}QScrollBar::sub-line:vertical {height: 6px;background: transparent;subcontrol-position: top;}QScrollBar::add-line:vertical {height: 6px;background: transparent;subcontrol-position: bottom;}QScrollBar::sub-line:vertical:hover {background: #292929;}QScrollBar::add-line:vertical:hover {background: #292929;}QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {background: transparent;}");
    vc_listbox->horizontalScrollBar()->setStyleSheet("QScrollBar:horizontal {height: 8px;background: #141414;margin:0px 0px 0px 0px;border:1px solid #141414;}QScrollBar::handle:horizontal {height: 8px;min-width: 45px;background: #292929;margin-left: 0px;margin-right: 0px;}QScrollBar::handle:horizontal:hover {background: #3e3e3e;}QScrollBar::handle:horizontal:pressed {background: #272727;}QScrollBar::sub-line:horizontal {width: 6px;background: transparent;subcontrol-position: left;}QScrollBar::add-line:horizontal {width: 6px;background: transparent;subcontrol-position: right;}QScrollBar::sub-line:horizontal:hover {background: #292929;}QScrollBar::add-line:horizontal:hover {background: #292929;}QScrollBar::add-page:horizontal,QScrollBar::sub-page:horizontal {background: transparent;}");
    ac_listbox->verticalScrollBar()->setStyleSheet("QScrollBar:vertical {width: 12px;background: #141414;margin:0px 0px 0px 0px;border:1px solid #141414;}QScrollBar::handle:vertical {width: 12px;min-height: 45px;background: #292929;margin-left: 0px;margin-right: 0px;}QScrollBar::handle:vertical:hover {background: #3e3e3e;}QScrollBar::handle:vertical:pressed {background: #272727;}QScrollBar::sub-line:vertical {height: 6px;background: transparent;subcontrol-position: top;}QScrollBar::add-line:vertical {height: 6px;background: transparent;subcontrol-position: bottom;}QScrollBar::sub-line:vertical:hover {background: #292929;}QScrollBar::add-line:vertical:hover {background: #292929;}QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {background: transparent;}");
    ac_listbox->horizontalScrollBar()->setStyleSheet("QScrollBar:horizontal {height: 8px;background: #141414;margin:0px 0px 0px 0px;border:1px solid #141414;}QScrollBar::handle:horizontal {height: 8px;min-width: 45px;background: #292929;margin-left: 0px;margin-right: 0px;}QScrollBar::handle:horizontal:hover {background: #3e3e3e;}QScrollBar::handle:horizontal:pressed {background: #272727;}QScrollBar::sub-line:horizontal {width: 6px;background: transparent;subcontrol-position: left;}QScrollBar::add-line:horizontal {width: 6px;background: transparent;subcontrol-position: right;}QScrollBar::sub-line:horizontal:hover {background: #292929;}QScrollBar::add-line:horizontal:hover {background: #292929;}QScrollBar::add-page:horizontal,QScrollBar::sub-page:horizontal {background: transparent;}");

    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
    connect(applyButton, SIGNAL(clicked()), this, SLOT(apply()));

    okButton->setFixedSize(91, 25);
    cancelButton->setFixedSize(91, 25);
    applyButton->setFixedSize(91, 25);
    resetDemuxerButton->setFixedSize(91, 25);
    resetACButton->setFixedSize(91, 25);
    resetVCButton->setFixedSize(91, 25);

    okButton->setFocusPolicy(Qt::NoFocus);
    cancelButton->setFocusPolicy(Qt::NoFocus);
    applyButton->setFocusPolicy(Qt::NoFocus);
    resetDemuxerButton->setFocusPolicy(Qt::NoFocus);
    resetACButton->setFocusPolicy(Qt::NoFocus);
    resetVCButton->setFocusPolicy(Qt::NoFocus);

    okButton->setStyleSheet("QPushButton{font-size:12px;background:#0f0f0f;border:1px solid #0a9ff5;color:#999999;}QPushButton:hover{background-color:#0a9ff5;border:1px solid #2db0f6;color:#ffffff;} QPushButton:pressed{background-color:#0993e3;border:1px solid #0a9ff5;color:#ffffff;}");
    cancelButton->setStyleSheet("QPushButton{font-size:12px;background:#0f0f0f;border:1px solid #000000;color:#999999;}QPushButton:hover{background-color:#1f1f1f;border:1px solid #0f0f0f;color:#ffffff;} QPushButton:pressed{background-color:#0d0d0d;border:1px solid #000000;color:#ffffff;}");
    applyButton->setStyleSheet("QPushButton{font-size:12px;background:#0f0f0f;border:1px solid #0a9ff5;color:#999999;}QPushButton:hover{background-color:#0a9ff5;border:1px solid #2db0f6;color:#ffffff;} QPushButton:pressed{background-color:#0993e3;border:1px solid #0a9ff5;color:#ffffff;}");
    resetDemuxerButton->setStyleSheet("QPushButton{font-size:12px;background:#0f0f0f;border:1px solid #0a9ff5;color:#999999;}QPushButton:hover{background-color:#0a9ff5;border:1px solid #2db0f6;color:#ffffff;} QPushButton:pressed{background-color:#0993e3;border:1px solid #0a9ff5;color:#ffffff;}");
    resetACButton->setStyleSheet("QPushButton{font-size:12px;background:#0f0f0f;border:1px solid #0a9ff5;color:#999999;}QPushButton:hover{background-color:#0a9ff5;border:1px solid #2db0f6;color:#ffffff;} QPushButton:pressed{background-color:#0993e3;border:1px solid #0a9ff5;color:#ffffff;}");
    resetVCButton->setStyleSheet("QPushButton{font-size:12px;background:#0f0f0f;border:1px solid #0a9ff5;color:#999999;}QPushButton:hover{background-color:#0a9ff5;border:1px solid #2db0f6;color:#ffffff;} QPushButton:pressed{background-color:#0993e3;border:1px solid #0a9ff5;color:#ffffff;}");

    title_label->setStyleSheet("QLabel{background:transparent;font-family:方正黑体_GBK;font-size:20px;color:#999999;}");

    demuxer_label->setStyleSheet("QLabel{background:transparent;font-size:12px;color:#999999;font-family:方正黑体_GBK;}");
    video_codec_label->setStyleSheet("QLabel{background:transparent;font-size:12px;color:#999999;font-family:方正黑体_GBK;}");
    audio_codec_label->setStyleSheet("QLabel{background:transparent;font-size:12px;color:#999999;font-family:方正黑体_GBK;}");

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 5, 0, 0);
    layout->setSpacing(0);
    TitleButton *btn = new TitleButton(0, false, tr("Information"));
    btn->setActived(true);
    connect(btn, SIGNAL(clicked(int)), this, SLOT(onButtonClicked(int)));
    layout->addWidget(btn);
    m_buttonList << btn;
    btn = new TitleButton(1, false, tr("Demuxer"));
    connect(btn, SIGNAL(clicked(int)), this, SLOT(onButtonClicked(int)));
    layout->addWidget(btn);
    m_buttonList << btn;
    btn = new TitleButton(2, false, tr("Video codec"));
    connect(btn, SIGNAL(clicked(int)), this, SLOT(onButtonClicked(int)));
    layout->addWidget(btn);
    m_buttonList << btn;
    btn = new TitleButton(3, false, tr("Audio codec"));
    connect(btn, SIGNAL(clicked(int)), this, SLOT(onButtonClicked(int)));
    layout->addWidget(btn);
    m_buttonList << btn;
    layout->addStretch();
    sections->setLayout(layout);

    pages->setCurrentWidget(info_page);

	codecs_set = false;

	retranslateStrings();
}

FilePropertiesDialog::~FilePropertiesDialog() {
    for(int i=0; i<m_buttonList.count(); i++)
    {
        TitleButton *btn = m_buttonList.at(i);
        delete btn;
        btn = NULL;
    }
    m_buttonList.clear();
}

void FilePropertiesDialog::setMediaData(MediaData md) {
	media_data = md;
	showInfo();
}

void FilePropertiesDialog::showInfo() {
	InfoFile info;
    info_edit->setText(info.getInfo(media_data));
}

void FilePropertiesDialog::setCurrentID(int id)
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

void FilePropertiesDialog::onButtonClicked(int id)
{
    setCurrentID(id);

    if (id == 0) {
        pages->setCurrentWidget(info_page);
    }
    else if (id == 1) {
        pages->setCurrentWidget(demuxer_page);
    }
    else if (id == 2) {
        pages->setCurrentWidget(vc_page);
    }
    else if (id == 3) {
        pages->setCurrentWidget(ac_page);
    }
}

void FilePropertiesDialog::retranslateStrings() {
	retranslateUi(this);

//	setWindowIcon( Images::icon("logo") );
    this->setWindowIcon(QIcon(":/res/kylin-video.png"));
    icon_label->setPixmap(QPixmap(":/res/info.png"));

	showInfo();

	okButton->setText( tr("OK") );
	cancelButton->setText( tr("Cancel") );
	applyButton->setText( tr("Apply") );
}

void FilePropertiesDialog::accept() {
	qDebug("FilePropertiesDialog::accept");

	hide();
	setResult( QDialog::Accepted );
	emit applied();
}

void FilePropertiesDialog::apply() {
	qDebug("FilePropertiesDialog::apply");

	setResult( QDialog::Accepted );
	emit applied();
}

void FilePropertiesDialog::reject() {
    hide();
    setResult( QDialog::Rejected );

    setResult( QDialog::Accepted );
}

void FilePropertiesDialog::setCodecs(InfoList vc, InfoList ac, InfoList demuxer) 
{
	vclist = vc;
	aclist = ac;
	demuxerlist = demuxer;

	qSort(vclist);
	qSort(aclist);
	qSort(demuxerlist);

	vc_listbox->clear();
	ac_listbox->clear();
	demuxer_listbox->clear();

	InfoList::iterator it;

	for ( it = vclist.begin(); it != vclist.end(); ++it ) {
		vc_listbox->addItem( (*it).name() +" - "+ (*it).desc() );
	}

	for ( it = aclist.begin(); it != aclist.end(); ++it ) {
		ac_listbox->addItem( (*it).name() +" - "+ (*it).desc() );
	}

	for ( it = demuxerlist.begin(); it != demuxerlist.end(); ++it ) {
		demuxer_listbox->addItem( (*it).name() +" - "+ (*it).desc() );
	}

	codecs_set = true;
}

void FilePropertiesDialog::setDemuxer(QString demuxer, QString original_demuxer) {
//	qDebug("FilePropertiesDialog::setDemuxer");
	if (!original_demuxer.isEmpty()) orig_demuxer = original_demuxer;
	int pos = find(demuxer, demuxerlist );
	if (pos != -1) demuxer_listbox->setCurrentRow(pos);

//	qDebug(" * demuxer: '%s', pos: %d", demuxer.toUtf8().data(), pos );
}

QString FilePropertiesDialog::demuxer() {
	int pos = demuxer_listbox->currentRow();
	if ( pos < 0 )
		return "";
	else
		return demuxerlist[pos].name();
}

void FilePropertiesDialog::setVideoCodec(QString vc, QString original_vc) {
	qDebug("FilePropertiesDialog::setVideoCodec");
	if (!original_vc.isEmpty()) orig_vc = original_vc;
	int pos = find(vc, vclist );
	if (pos != -1) vc_listbox->setCurrentRow(pos);

	qDebug(" * vc: '%s', pos: %d", vc.toUtf8().data(), pos );
}

QString FilePropertiesDialog::videoCodec() {
	int pos = vc_listbox->currentRow();
	if ( pos < 0 )
		return "";
	else
		return vclist[pos].name();
}

void FilePropertiesDialog::setAudioCodec(QString ac, QString original_ac) {
	qDebug("FilePropertiesDialog::setAudioCodec");
	if (!original_ac.isEmpty()) orig_ac = original_ac;
	int pos = find(ac, aclist );
	if (pos != -1) ac_listbox->setCurrentRow(pos);

	qDebug(" * ac: '%s', pos: %d", ac.toUtf8().data(), pos );
}

QString FilePropertiesDialog::audioCodec() {
	int pos = ac_listbox->currentRow();
	if ( pos < 0 )
		return "";
	else
		return aclist[pos].name();
}

void FilePropertiesDialog::on_resetDemuxerButton_clicked() {
	setDemuxer( orig_demuxer );
}

void FilePropertiesDialog::on_resetACButton_clicked() {
	setAudioCodec( orig_ac );
}

void FilePropertiesDialog::on_resetVCButton_clicked() {
	setVideoCodec( orig_vc );
}

int FilePropertiesDialog::find(QString s, InfoList &list) {
	int n=0;
	InfoList::iterator it;

	for ( it = list.begin(); it != list.end(); ++it ) {
		//qDebug(" * item: '%s', s: '%s'", (*it).name().toUtf8().data(), s.toUtf8().data());
		if ((*it).name() == s) return n;
		n++;
	}
	return -1;
}

// Language change stuff
void FilePropertiesDialog::changeEvent(QEvent *e) {
	if (e->type() == QEvent::LanguageChange) {
		retranslateStrings();
	} else {
		QDialog::changeEvent(e);
	}
}

void FilePropertiesDialog::moveDialog(QPoint diff) {
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

bool FilePropertiesDialog::eventFilter( QObject * object, QEvent * event )
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
        drag_state = NOT_FDRAGGING;
        return false;
    }

    if (type == QEvent::MouseButtonPress) {
        if (mouseEvent->button() != Qt::LeftButton) {
            drag_state = NOT_FDRAGGING;
            return false;
        }

        drag_state = START_FDRAGGING;
        start_drag = mouseEvent->globalPos();
        // Don't filter, so others can have a look at it too
        return false;
    }

    if (type == QEvent::MouseButtonRelease) {
        if (drag_state != FDRAGGING || mouseEvent->button() != Qt::LeftButton) {
            drag_state = NOT_FDRAGGING;
            return false;
        }

        // Stop dragging and eat event
        drag_state = NOT_FDRAGGING;
        event->accept();
        return true;
    }

    // type == QEvent::MouseMove
    if (drag_state == NOT_FDRAGGING)
        return false;

    // buttons() note the s
    if (mouseEvent->buttons() != Qt::LeftButton) {
        drag_state = NOT_FDRAGGING;
        return false;
    }

    QPoint pos = mouseEvent->globalPos();
    QPoint diff = pos - start_drag;
    if (drag_state == START_FDRAGGING) {
        // Don't start dragging before moving at least DRAG_THRESHOLD pixels
        if (abs(diff.x()) < 4 && abs(diff.y()) < 4)
            return false;

        drag_state = FDRAGGING;
    }
    this->moveDialog(diff);

    start_drag = pos;
    event->accept();
    return true;
}

#include "moc_filepropertiesdialog.cpp"
