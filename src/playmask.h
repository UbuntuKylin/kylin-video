#ifndef PLAYMASK_H
#define PLAYMASK_H

#include <QObject>
#include <QMainWindow>
#include <QWidget>
#include <QPushButton>
#include <QGraphicsOpacityEffect>
#include <QPaintEvent>

class PlayMask : public QWidget
{
	Q_OBJECT

public:
    PlayMask(QWidget *parent = 0);
    ~PlayMask();

	void setTransparent(bool transparent);
	void set_widget_opacity(const float &opacity=0.8);

protected:
    void paintEvent(QPaintEvent *event);

signals:
    void signal_play_continue();

private:
    QPushButton *play_Btn;
};

#endif // PLAYMASK_H
