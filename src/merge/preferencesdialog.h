/*  smplayer, GUI front-end for mplayer.
    Copyright (C) 2006-2015 Ricardo Villalba <rvm@users.sourceforge.net>
    Copyright (C) 2013 ~ 2017 National University of Defense Technology(NUDT) & Tianjin Kylin Ltd.

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

#ifndef _PREFERENCESDIALOG_H_
#define _PREFERENCESDIALOG_H_

#include "ui_preferencesdialog.h"

enum PDragState {NOT_PDRAGGING, START_PDRAGGING, PDRAGGING};

class QTextBrowser;
class QPushButton;

class PrefWidget;
class PrefGeneral;
class PrefVideo;
class PrefAudio;
class PrefPerformance;
class PrefSubtitles;
class PrefScreenShot;
class PrefShortCut;

//class PrefInterface;
//class PrefAdvanced;
//class PrefPlaylist;
//class PrefTV;
//class PrefUpdates;
//class PrefNetwork;
//class PrefAssociations;

class Preferences;

class TitleButton;


class PreferencesDialog : public QDialog, public Ui::PreferencesDialog
{
	Q_OBJECT

public:
	enum Section { General=0, Drives=1, Performance=2,
                   Subtitles=3, Gui=4, Mouse=5, Advanced=6, Associations=7 };

    PreferencesDialog(QString arch_type = "", QWidget * parent = 0, Qt::WindowFlags f = 0 );
	~PreferencesDialog();

    PrefShortCut *mod_shortcut_page() { return page_shortcut; }

	void addSection(PrefWidget *w);

	// Pass data to the standard dialogs
	void setData(Preferences * pref);

	// Apply changes
	void getData(Preferences * pref);

	// Return true if the mplayer process should be restarted.
	bool requiresRestart();

    virtual bool eventFilter(QObject *, QEvent *);
    void moveDialog(QPoint diff);

public slots:
	void showSection(Section s);

	virtual void accept(); // Reimplemented to send a signal
	virtual void reject();

signals:
	void applied();
    void generalclicked(int id);
    void performanceclicked(int id);
    void subtitlesclicked(int id);

protected:
	virtual void retranslateStrings();
	virtual void changeEvent ( QEvent * event ) ;

protected slots:
	void apply();

public slots:
    void onButtonClicked(int id);
    void setCurrentID(int id);
    void switchCurrentIDPage(int id);

private:
    QList<TitleButton *> m_buttonList;

protected:
	PrefGeneral * page_general;
    PrefVideo * page_video;
    PrefAudio * page_audio;
    PrefPerformance * page_performance;
    PrefSubtitles * page_subtitles;
    PrefScreenShot * page_screenshot;
    PrefShortCut *page_shortcut;

private:
    PDragState drag_state;
    QPoint start_drag;
};

#endif
