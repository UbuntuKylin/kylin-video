#ifndef _AUDIODELAYDIALOG_H_
#define _AUDIODELAYDIALOG_H_

#include "ui_audiodelaydialog.h"

#include <QDialog>
#include <QPushButton>

enum AADragState {NOT_AADRAGGING, START_AADRAGGING, AADRAGGING};

class AudioDelayDialog : public QDialog, public Ui::AudioDelayDialog
{
	Q_OBJECT

public:
    AudioDelayDialog( QWidget * parent = 0, Qt::WindowFlags f = 0 );
    ~AudioDelayDialog();

    void setDefaultValue(int audio_delay);
    int getCurrentValue();

    void initConnect();

    virtual bool eventFilter(QObject *, QEvent *);
    void moveDialog(QPoint diff);

public slots:


private:
    AADragState drag_state;
    QPoint start_drag;
};

#endif
