#ifndef OVERLAYMAINWINDOW_H
#define OVERLAYMAINWINDOW_H

#include <QObject>
#include <QMainWindow>
#include <QWidget>
#include <QPushButton>
#include <QGraphicsOpacityEffect>
#include <QPaintEvent>

//class OverlayMainWindow : public QMainWindow
class OverlayWidget : public QWidget
{
	Q_OBJECT

public: // member functions
	//OverlayMainWindow();
	OverlayWidget(QWidget *parent = 0);
	~OverlayWidget();

    void setBackgroundWidget(QWidget *backGroundWidget);
//	virtual void show();
	void setTransparent(bool transparent);
	void set_widget_opacity(const float &opacity=0.8);
protected: // member functions
    bool eventFilter(QObject *obj, QEvent *event);
//	virtual void changeEvent(QEvent *event);
//    void paintEvent(QPaintEvent *event);//0526

private: // member variables
    QWidget *m_pBackgroundWidget;
};

#endif // OVERLAYMAINWINDOW_H
