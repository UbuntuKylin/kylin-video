#ifndef TOOlBUTTON_H
#define TOOlBUTTON_H

#include <QToolButton>
#include <QMouseEvent>
#include <QPainter>

class ToolButton : public QToolButton
{

public:

    explicit ToolButton(const QString &pic_name, const QString &text , QWidget *parent = 0);
    ~ToolButton();
    void setMousePress(bool mouse_press);
    void setMouseHover();

protected:
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);
    void painterInfo(int top_color, int middle_color, int bottom_color);

public:
    bool is_over;//鼠标是否移过
    bool is_press;//鼠标是否按下
};

#endif //TOOlBUTTON_H
