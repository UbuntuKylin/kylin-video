#include <QListWidget>
#include <QScopedPointer>


class PlayListView : public QListWidget
{
    Q_OBJECT
public:
    explicit PlayListView(QWidget *parent = 0);
    ~PlayListView();
    void checkScrollbarSize();

public slots:
    void updateScrollbar();
    void slot_scrollbar_value_changed(int value);

signals:
    void customResort(const QStringList &uuids);

protected:
    virtual void wheelEvent(QWheelEvent *event);
    virtual void resizeEvent(QResizeEvent *event);

private:
    QScrollBar *vscrollBar;
    int scrollBarWidth;
    int itemHeight;
};
