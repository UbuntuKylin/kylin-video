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

#ifndef PLAYLISTITEM_H
#define PLAYLISTITEM_H

#include <QFrame>

class QLineEdit;
class QLabel;
class QPushButton;

class PlayListItem : public QFrame
{
    Q_OBJECT
    Q_PROPERTY(QString animationPrefix READ animationPrefix WRITE setAnimationPrefix)
    Q_PROPERTY(QString highlightAnimationPrefix READ highlightAnimationPrefix WRITE setHighlightAnimationPrefix)
public:
    explicit PlayListItem(QString mtype, QString filename, QString name, double duration, QWidget *parent = 0);

    void setName(QString name) { m_name = name; };
    void setDuration(double duration) { m_duration = duration; };
    void setPlayed(bool b) { m_played = b; };
    void setMarkForDeletion(bool b) { m_deleted = b; };

    QString filename() { return m_filename; };
    QString name() { return m_name; };
    double duration() { return m_duration; };
    bool played() { return m_played; };
    bool markedForDeletion() { return m_deleted; };

    inline QString data() {return m_data;}

    void setActive(bool active);

    void mouseDoubleClickEvent(QMouseEvent *event);

    QString animationPrefix() const
    {
        return m_animationPrefix;
    }

    QString highlightAnimationPrefix() const
    {
        return m_highlightAnimationPrefix;
    }

    QString m_filename, m_name;
    double m_duration;
    bool m_played, m_deleted;

    void update_widget_qss_property(QWidget *w, const char *name, const QVariant &value);

signals:
    void rename(const QString &newNameA);
    void remove(QString filename);
    void sig_doubleclicked_resource(QString filename);

public slots:
    void onDelete();

    void setAnimationPrefix(QString animationPrefix)
    {
        m_animationPrefix = animationPrefix;
    }

    void setHighlightAnimationPrefix(QString highlightAnimationPrefix)
    {
        m_highlightAnimationPrefix = highlightAnimationPrefix;
    }

    void update_time(QString duration);

private:
    QLabel *icon_label;
    QLineEdit *m_titleedit;
    QLabel *time_label;
    QPushButton *delete_btn;
    QString m_data;
    QString m_animationPrefix;
    QString m_highlightAnimationPrefix;
};

#endif // PLAYLISTITEM_H
