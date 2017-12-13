#include "lineedit_with_icon.h"
#include <QToolButton>
#include <QStyle>
#include <QEvent>
#include <QDebug>

LineEditWithIcon::LineEditWithIcon(QWidget *parent) : QLineEdit(parent)
{
    //height: 27px;
    this->setStyleSheet("QLineEdit {border:1px solid #000000;background: #0f0f0f;font-family:方正黑体_GBK;font-size:12px;color:#999999;}QLineEdit::hover{border: 1px solid #000000;background: #0a0a0a;font-family:方正黑体_GBK;font-size:12px;color:#ffffff;}QLineEdit:enabled {background: #0a0a0a;color:#999999;}QLineEdit:enabled:hover, QLineEdit:enabled:focus {background: #0a0a0a;color:#ffffff;}QLineEdit:!enabled {color: #383838;}");

    button = new QToolButton(this);
    button->setObjectName("folderToolButton");
	button->setCursor(Qt::ArrowCursor);
	setupButton();
    button->setText(tr("Change"));
}

void LineEditWithIcon::setupButton() {
}

void LineEditWithIcon::setIcon(const QPixmap & pixmap) {
//    QPixmap p = pixmap;
//	//qDebug("height: %d, icon height: %d", height(), p.height());
//	int max_height = 16;
//	if (max_height > height()) max_height = height() - 4;
//	if (pixmap.height() > max_height) p = pixmap.scaledToHeight(max_height, Qt::SmoothTransformation);
//	button->setIcon(p);
//    button->setStyleSheet("QToolButton { border: none; padding: 0px; }");

//    int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
//	//qDebug("frameWidth: %d", frameWidth);
//    setStyleSheet(QString("QLineEdit { padding-right: %1px; } ").arg(button->sizeHint().width() + frameWidth + 1));
//    /*
//    QSize msz = minimumSizeHint();
//    setMinimumSize(qMax(msz.width(), button->sizeHint().height() + frameWidth * 2 + 2),
//                   qMax(msz.height(), button->sizeHint().height() + frameWidth * 2 + 2));
//    */
}

void LineEditWithIcon::resizeEvent(QResizeEvent *)
{
    QSize sz = button->sizeHint();
    button->setFixedHeight(this->height());
    button->move(rect().right() - sz.width() + 1, 0);
}

#include "moc_lineedit_with_icon.cpp"
