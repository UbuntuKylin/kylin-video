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
    explicit PlayListItem(/*PlaylistPtr playlist, */QString mtype, QString filename, QString name, double duration, QWidget *parent = 0);

    void setFilename(QString filename) { m_filename = filename; };
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
