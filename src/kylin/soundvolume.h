/*
 * Copyright (C) 2013 ~ 2017 National University of Defense Technology(NUDT) & Tianjin Kylin Ltd.
 *
 * Authors:
 *  Kobe Lee    lixiang@kylinos.cn/kobe24_lixiang@126.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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

