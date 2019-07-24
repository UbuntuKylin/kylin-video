
#ifndef _POWEROFFDIALOG_H_
#define _POWEROFFDIALOG_H_

#include <QDialog>
#include <QPushButton>
#include <QLabel>
#include <QFrame>
#include <QDialogButtonBox>
#include <QLayout>

#include "utils.h"

class PoweroffDialog : public QDialog
{
	Q_OBJECT

public:
    PoweroffDialog(QWidget * parent = 0, Qt::WindowFlags f = 0 );
    ~PoweroffDialog();

    void moveDialog(QPoint diff);

protected:
    virtual bool eventFilter(QObject *, QEvent *);
    virtual QSize sizeHint () const;

private slots:
    void updateCountdown();

private:
    DragState m_dragState;
    QPoint m_startDrag;
    QWidget *m_topWidget;
    QFrame *m_centerFrame;
    QDialogButtonBox *m_buttonBox;
    QPushButton *m_closeBtn;

    int m_countdown;
    QTimer *m_timer = nullptr;
    QString m_text;
    QVBoxLayout *m_mainLayout;
    QLabel *icon_label;
    QLabel *m_textLabel;

};

#endif // _POWEROFFDIALOG_H_
