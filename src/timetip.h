#ifndef _TIMETIP_H_
#define _TIMETIP_H_

#include <QFrame>
class QLabel;


class TimeTip : public QFrame
{
    Q_OBJECT

public:
    explicit TimeTip(const QString &text, QWidget *parent = 0);
    ~TimeTip();

public slots:
    void setText(const QString text);
    void setPixMapAndTime(QPixmap pixmap, const QString time);

protected:
    virtual void paintEvent(QPaintEvent *event);

private:
    QFrame          *text_frame;
    QLabel          *text_label;
    QLabel          *split_line;
    bool            repaint_flag;
};

#endif
