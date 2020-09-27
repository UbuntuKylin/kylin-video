#ifndef RESTARTTIP_H
#define RESTARTTIP_H

#include <QDialog>

namespace Ui {
class RestartTip;
}

class RestartTip : public QDialog
{
    Q_OBJECT

public:
    static void showTip();
    ~RestartTip();

private slots:
    void on_pushButton_clicked();

private:
    explicit RestartTip(QWidget *parent = 0);
    Ui::RestartTip *ui;
    static RestartTip* instance;
};

#endif // RESTARTTIP_H
