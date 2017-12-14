#ifndef _ABOUTDIALOG_H_
#define _ABOUTDIALOG_H_

#include "ui_aboutdialog.h"

#include <QDialog>
#include <QPushButton>
class QParallelAnimationGroup;

enum ADragState {NOT_ADRAGGING, START_ADRAGGING, ADRAGGING};
enum TabState {TAB_ABOUT, TAB_CONTRIBUTOR};

class AboutDialog : public QDialog, public Ui::AboutDialog
{
	Q_OBJECT

public:
    AboutDialog( QWidget * parent = 0, Qt::WindowFlags f = 0 );
    ~AboutDialog();

    void initConnect();
    void initAnimation();

    virtual bool eventFilter(QObject *, QEvent *);
    void moveDialog(QPoint diff);

    void setVersions();

public slots:
    void onAboutBtnClicked();
    void onContributorBtnClicked();

private:
    QPushButton *okBtn;
    QParallelAnimationGroup *aboutGroup;
    QParallelAnimationGroup *contributorGroup;
    ADragState drag_state;
    TabState tab_state;
    QPoint start_drag;
};

#endif
