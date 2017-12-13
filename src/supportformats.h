#ifndef _SUPPORTFORMATS_H_
#define _SUPPORTFORMATS_H_

#include "ui_supportformats.h"

class SupportFormats : public QWidget, public Ui::SupportFormats
{
	Q_OBJECT

public:
    SupportFormats( QWidget * parent = 0, Qt::WindowFlags f = 0 );
    ~SupportFormats();

    void setData();

protected:
	virtual void retranslateStrings();
};

#endif
