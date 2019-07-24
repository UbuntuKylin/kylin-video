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

#ifndef _FILEPROPERTIESDIALOG_H_
#define _FILEPROPERTIESDIALOG_H_

#include "ui_filepropertiesdialog.h"
#include "../smplayer/inforeader.h"
#include "../smplayer/mediadata.h"
#include "../utils.h"

class QPushButton;
class TitleButton;

class FilePropertiesDialog : public QDialog, public Ui::FilePropertiesDialog
{
	Q_OBJECT

public:
    enum Section { Info=0, Demuxer=1, AC=2, VC=3, Options=4};

    FilePropertiesDialog( QWidget* parent = 0, Qt::WindowFlags f = 0 );
    ~FilePropertiesDialog();

//	void setMediaData(MediaData md);
    void setMediaData(MediaData md, Tracks videos, Tracks audios, SubTracks subs);

	// Call it as soon as possible
	void setCodecs(InfoList vc, InfoList ac, InfoList demuxer);

	void setDemuxer(QString demuxer, QString original_demuxer="");
	QString demuxer();

	void setVideoCodec(QString vc, QString original_vc="");
	QString videoCodec();

	void setAudioCodec(QString ac, QString original_ac="");
	QString audioCodec();

    virtual bool eventFilter(QObject *, QEvent *);
    void moveDialog(QPoint diff);

public slots:
	void accept(); // Reimplemented to send a signal
    virtual void reject();
	void apply();

    void onButtonClicked(int id);
    void setCurrentID(int id);

signals:
	void applied();

protected slots:
	virtual void on_resetDemuxerButton_clicked();
	virtual void on_resetACButton_clicked();
	virtual void on_resetVCButton_clicked();

protected:
	bool hasCodecsList() { return codecs_set; };
	int find(QString s, InfoList &list);
	void showInfo();

protected:
	virtual void retranslateStrings();
	virtual void changeEvent ( QEvent * event ) ;

private:
	bool codecs_set;
	InfoList vclist, aclist, demuxerlist;
	QString orig_demuxer, orig_ac, orig_vc;
	MediaData media_data;

    Tracks video_tracks;
    Tracks audio_tracks;
    SubTracks sub_tracks;

    QList<TitleButton *> m_buttonList;
    DragState m_dragState;
    QPoint m_startDrag;
};

#endif
