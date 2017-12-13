#ifndef _ESCTIP_H_
#define _ESCTIP_H_

#include <QFrame>
class QLabel;


class EscTip : public QFrame
{
    Q_OBJECT
    Q_PROPERTY(int radius READ radius WRITE setRadius)
    Q_PROPERTY(QBrush background READ background WRITE setBackground)
    Q_PROPERTY(QColor borderColor READ borderColor WRITE setBorderColor)
public:
    explicit EscTip(QWidget *parent = 0);
    ~EscTip();
    void setBackgroundImage(const QPixmap &srcPixmap);

    int radius() const;
    QColor borderColor() const;
    QBrush background() const;

public slots:
    void setBackground(QBrush background);
    void setRadius(int radius);
    void setBorderColor(QColor borderColor);

    void aniFinished();

protected:
    virtual void paintEvent(QPaintEvent *);

private:
    QLabel          *text_label;

    QBrush          m_background;
    int             m_radius;
    int             w_shadow;
    QMargins        shadow_margins;
    QColor          m_borderColor;
};

#endif
