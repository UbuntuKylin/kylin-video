#ifndef TITLEBUTTON_H
#define TITLEBUTTON_H

#include <QWidget>

class TitleButton : public QWidget
{
    Q_OBJECT
public:
    explicit TitleButton(int id, bool bigFont, const QString &title, QWidget *parent = 0);

    int id() const;
    void setId(const int &id);

    bool isActived() const;
    void setActived(bool isActived);

signals:
    void clicked(int id);

protected:
    void mousePressEvent(QMouseEvent *e);
    void paintEvent(QPaintEvent *e);

private:
    bool m_bigFont;
    bool m_isActived;
    int m_id;
    QString m_title;
};

#endif // TITLEBUTTON_H
