/*
 * Copyright (C) 2013 ~ 2019 National University of Defense Technology(NUDT) & Kylin Ltd.
 *
 * Authors:
 *  Kobe Lee    xiangli@ubuntukylin.com/kobe24_lixiang@126.com
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

#ifndef VIDEOWINDOW_H
#define VIDEOWINDOW_H

#include "autohidecursorwidget.h"
#include "utils.h"

class DisplayLayerComposer;

#define DRAG_THRESHOLD 4
#define ZOOM_STEP 0.05
#define ZOOM_MIN 0.5

class VideoWindow : public AutoHideCursorWidget
{
    Q_OBJECT

public:
    VideoWindow(QWidget* parent = 0, Qt::WindowFlags f = 0);
    ~VideoWindow();

    DisplayLayerComposer * displayLayer() { return m_displayLayer; };
    void moveDisplayLayer(int m_offsetX, int m_offsetY);

    void setResolution( int w, int h);
    void setAspect( double asp);
    void setMonitorAspect(double asp);
    void updateVideoWindow();

    void setColorKey(QColor c);

    void setOffsetX( int );
    int offsetX();

    void setOffsetY( int );
    int offsetY();

    void setZoom( double );
    double zoom();

    void allowVideoMovement(bool b) { m_allowVideoMovement = b; };
    bool isVideoMovementAllowed() { return m_allowVideoMovement; };

    void delayLeftClick(bool b) { m_delayLeftClick = b; };
    bool isLeftClickDelayed() { return m_delayLeftClick; };

    virtual QSize sizeHint () const;
    virtual QSize minimumSizeHint() const;

    virtual bool eventFilter(QObject *, QEvent *);

    bool animatedLogo() { return m_animatedLogo; }

    void setCornerWidget(QWidget * w);
    QWidget * cornerWidget() { return m_cornerWidget; };


public slots:
    void setLogoVisible(bool b);
    void showLogo() { setLogoVisible(true);};
    void hideLogo() { setLogoVisible(false); };
    void hideLogoForTemporary();
    void updateLogoPosition();
    void setAnimatedLogo(bool b) { m_animatedLogo = b; };
    void moveLeft();
    void moveRight();
    void moveUp();
    void moveDown();
    void incZoom();
    void decZoom();
    void activateMouseDragTracking(bool active) { m_mouseDragTracking = active; }

protected:
    virtual void resizeEvent(QResizeEvent * e);
    virtual void mouseReleaseEvent(QMouseEvent * e);
    virtual void mouseDoubleClickEvent(QMouseEvent *event);
    virtual void wheelEvent(QWheelEvent * e);

signals:
    void doubleClicked();
    void leftClicked();
    void rightClicked();
    void middleClicked();
    void xbutton1Clicked(); // first X button
    void xbutton2Clicked(); // second X button
    void keyPressed(QKeyEvent * e);
    void wheelUp();
    void wheelDown();
    void mouseMovedDiff(QPoint);

private:
    DragState m_dragState;
    QPoint m_startDrag;
    bool m_mouseDragTracking;
    int m_videoWidth, m_videoHeight;
    double m_aspect;
    double m_monitorAspect;
    DisplayLayerComposer *m_displayLayer = nullptr;
    QLabel *m_logoLabel = nullptr;
    bool m_stoped;
    // Zoom and moving
    int m_offsetX, m_offsetY;
    double m_zoomFactor;
    // Original pos and dimensions of the m_displayLayer before zooming or moving
    int m_origX, m_origY;
    int m_origWidth, m_origHeight;
    bool m_allowVideoMovement;
    // Delay left click event
    bool m_delayLeftClick;
    bool m_doubleClicked;
    bool m_animatedLogo;
    QTimer *m_leftClickTimer = nullptr;
    QWidget *m_cornerWidget = nullptr;
};

#endif // VIDEOWINDOW_H
