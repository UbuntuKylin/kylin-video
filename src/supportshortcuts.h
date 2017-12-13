#ifndef _SUPPORTSHORTCUTS_H_
#define _SUPPORTSHORTCUTS_H_

#include "ui_supportshortcuts.h"
#include "preferences.h"

class SupportShortcuts : public QWidget, public Ui::SupportShortcuts
{
	Q_OBJECT

public:
    SupportShortcuts( QWidget * parent = 0, Qt::WindowFlags f = 0 );
    ~SupportShortcuts();

    void setData(Preferences * pref);

protected:
	virtual void retranslateStrings();
};

#endif
