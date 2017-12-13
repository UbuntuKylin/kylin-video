#ifndef _TIPWIDGET_H_
#define _TIPWIDGET_H_

#include <QFrame>
class QLabel;

class TipWidget : public QFrame
{
    Q_OBJECT
    Q_PROPERTY(int radius READ radius WRITE setRadius)
    Q_PROPERTY(QBrush background READ background WRITE setBackground)
    Q_PROPERTY(QColor borderColor READ borderColor WRITE setBorderColor)
public:
    explicit TipWidget(const QString &text, QWidget *parent = 0);
    ~TipWidget();
    void setBackgroundImage(const QPixmap &srcPixmap);

    int radius() const;
    QColor borderColor() const;
    QBrush background() const;

    void setTransparent(bool transparent);
    void set_widget_opacity(const float &opacity);

public slots:
    void setText(const QString text);

    void setBackground(QBrush background);
    void setRadius(int radius);
    void setBorderColor(QColor borderColor);

    void aniFinished();

private:
    QLabel          *text_label;

    QBrush          m_background;
    int             m_radius;
    int             w_shadow;
    QMargins        shadow_margins;
    QColor          m_borderColor;
};

#endif
