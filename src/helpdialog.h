#ifndef _HELPDIALOG_H_
#define _HELPDIALOG_H_

#include "ui_helpdialog.h"

enum HDragState {NOT_HDRAGGING, START_HDRAGGING, HDRAGGING};

class QTextBrowser;
class QPushButton;
class SupportFormats;
class SupportShortcuts;
class TitleButton;
class Preferences;

class HelpDialog : public QDialog, public Ui::HelpDialog
{
	Q_OBJECT

public:
    enum Section { Formats=0, Other=1 };

    HelpDialog(QWidget * parent = 0, Qt::WindowFlags f = 0 );
    ~HelpDialog();

    void addSection(QWidget *w);

	// Pass data to the standard dialogs
    void setData(Preferences * pref);

    virtual bool eventFilter(QObject *, QEvent *);
    void moveDialog(QPoint diff);

public slots:
	void showSection(Section s);
	virtual void accept(); // Reimplemented to send a signal

signals:
	void applied();

protected:
	virtual void retranslateStrings();
	virtual void changeEvent ( QEvent * event ) ;

public slots:
    void onButtonClicked(int id);
    void setCurrentID(int id);
    void switchCurrentIDPage(int id);

private:
    QList<TitleButton *> m_buttonList;

protected:
    SupportFormats *page_formats;
    SupportShortcuts *page_shortcuts;

private:
    HDragState drag_state;
    QPoint start_drag;
};

#endif
