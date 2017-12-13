#ifndef _MESSAGEDIALOG_H_
#define _MESSAGEDIALOG_H_

#include <QString>
#include <QMessageBox>
#include <QGridLayout>
#include <QDialogButtonBox>

class QPushButton;
class QPoint;

enum MDragState {NOT_MDRAGGING, START_MDRAGGING, MDRAGGING};

class MessageDialog : public QDialog
{
	Q_OBJECT

public:
    MessageDialog(QWidget *parent = 0, const QString &title = "", const QString &text = "", QMessageBox::StandardButtons buttons = QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::StandardButton defaultButton = QMessageBox::Ok);
    ~MessageDialog();

    QAbstractButton *clickedButton() const;
    QMessageBox::StandardButton standardButton(QAbstractButton *button) const;
//    void setDefaultButton(QPushButton *button);
//    void setDefaultButton(QMessageBox::StandardButton button);
    void setIcon(const QString &icon);
    void setDialogSize(int w, int h);

    void initConnect();
    virtual bool eventFilter(QObject *, QEvent *);
    void moveDialog(QPoint diff);

private slots:
    void onButtonClicked(QAbstractButton *button);

private:
    int returnCodeByRun(QAbstractButton *button);

private:
    QLabel *title_label;
    QLabel *icon_label;
    QLabel *msg_label;
    QPushButton *close_Btn;
//    QGridLayout *gridLayout;
    QVBoxLayout *main_layout;
    QDialogButtonBox *buttonBox;
    QAbstractButton *clickedBtn;
    QAbstractButton *defaultBtn;
    MDragState drag_state;
    QPoint start_drag;
};

#endif
