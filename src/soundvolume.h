#include <QScopedPointer>
#include <QFrame>
class QSlider;
#include <QBrush>

class SoundVolume : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int radius READ radius WRITE setRadius)
    Q_PROPERTY(QBrush background READ background WRITE setBackground)
    Q_PROPERTY(QColor borderColor READ borderColor WRITE setBorderColor)
public:
    explicit SoundVolume(QWidget *parent = 0);
    ~SoundVolume();

    int volume() const;
    int radius() const;
    QColor borderColor() const;
    QBrush background() const;

signals:
    void volumeChanged(int vol);

public slots:
    void deleyHide();
    void onVolumeChanged(int vol);
    void setBackground(QBrush m_background);
    void setRadius(int m_radius);
    void setBorderColor(QColor m_borderColor);

    void slot_deley();

    virtual void setValue(int);

protected:
    virtual void showEvent(QShowEvent *event);
    virtual void enterEvent(QEvent *event);
    virtual void leaveEvent(QEvent *event);
    virtual void wheelEvent(QWheelEvent *event);

private:
    int         tradius;
    QBrush      tbackground;
    QColor      tborderColor;
    QSlider     *volSlider;
    bool        mouseIn;
};

