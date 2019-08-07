/*
 * Copyright (C) 2013 ~ 2019 National University of Defense Technology(NUDT) & Tianjin Kylin Ltd.
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

#include "playlistdelegate.h"
#include "playlistview.h"
#include "playlistmodel.h"
#include "utils.h"

#include "../smplayer/preferences.h"
#include "../smplayer/global.h"
using namespace Global;

#include <QFont>
#include <QPainter>
#include <QDebug>

static inline QFlags<Qt::AlignmentFlag> cellAlignmentFlag(int column)
{
    PlaylistDelegate::PlayListColumn p_column = static_cast<PlaylistDelegate::PlayListColumn>(column);
    switch (p_column) {
    case PlaylistDelegate::Icon:
        return (Qt::AlignLeft | Qt::AlignVCenter);
    case PlaylistDelegate::Name:
        return (Qt::AlignLeft | Qt::AlignVCenter);
    case PlaylistDelegate::Length:
        return (Qt::AlignRight | Qt::AlignVCenter);
    case PlaylistDelegate::Invalid:
        break;
    }

    return (Qt::AlignLeft | Qt::AlignVCenter);
}

static inline QRect cellRect(int column, const QStyleOptionViewItem &option)
{
    const int leftRightMargin = 15;
    QFont font(option.font);
    QFontMetrics fm(font);
    static int iconWidth = 20;
    static int timeWidth = fm.width("00:00:00") + leftRightMargin;
    static int delWidth = 20;
    int w = option.rect.width() - timeWidth - delWidth - leftRightMargin - iconWidth;

    PlaylistDelegate::PlayListColumn p_column = static_cast<PlaylistDelegate::PlayListColumn>(column);
    switch (p_column) {
    case PlaylistDelegate::Icon:
        return QRect(leftRightMargin, option.rect.y(), iconWidth, option.rect.height());
    case PlaylistDelegate::Name:
        return QRect(leftRightMargin + iconWidth, option.rect.y(), w - 2, option.rect.height());
    case PlaylistDelegate::Length:
        return QRect(leftRightMargin + iconWidth + w, option.rect.y(), timeWidth - leftRightMargin, option.rect.height());
    case PlaylistDelegate::Invalid:
        break;
    }

    return option.rect.marginsRemoved(QMargins(0, 0, 0, 0));
}



PlaylistDelegate::PlaylistDelegate(QWidget *parent)
    : QStyledItemDelegate(parent)
{
    m_background = QColor("#2e2e2e");
    m_normalTextColor = QColor("#ffffff");
    m_activeTextColor = QColor("#009aff");
    m_hoverTextColor = QColor("#0da4f6");
    m_timeColor = QColor("#ffffff");
}

PlaylistDelegate::~PlaylistDelegate()
{
}

QColor PlaylistDelegate::foregroundColor(int column, const QStyleOptionViewItem &option) const
{
    if (option.state & QStyle::State_Selected) {
        return m_hoverTextColor;
    }

    PlaylistDelegate::PlayListColumn p_column = static_cast<PlaylistDelegate::PlayListColumn>(column);
    switch (p_column) {
    case PlaylistDelegate::Icon:
        break;
    case PlaylistDelegate::Name:
        return m_normalTextColor;
    case PlaylistDelegate::Length:
        return m_timeColor;
    case PlaylistDelegate::Invalid:
        break;
    }

    return m_normalTextColor;
}

void PlaylistDelegate::setItemHover(const QModelIndex &index)
{
//    m_hoverIndex = index;
}

void PlaylistDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (!index.isValid()) {
        return;
    }

    painter->save();

    QFont m_font = option.font;
    m_font.setPixelSize(11);

    painter->setRenderHint(QPainter::Antialiasing);
    painter->setRenderHint(QPainter::HighQualityAntialiasing);

    QColor background = m_background;
//    if (option.state & QStyle::State_Selected) {
//    }
//    else if (option.state & QStyle::State_HasFocus) {
//    }
    if(option.state.testFlag(QStyle::State_Selected)) {//选中状态
        QRect mRect = option.rect;
        QPoint start_point(mRect.topLeft());
        QPoint end_point(mRect.topRight());
        QLinearGradient gradient(start_point, end_point);
        gradient.setColorAt(0, QColor("#0da4f5"));
        gradient.setColorAt(0.015, QColor("#2aa6f8"));
        gradient.setColorAt(0.0155, QColor("rgba(42, 166, 248, 20%)"));
        gradient.setColorAt(1, QColor("rgba(42, 166, 248, 20%)"));
        painter->fillRect(mRect, QBrush(gradient));
//        painter->setCompositionMode(QPainter::CompositionMode_SourceIn);
    }
    else if(option.state.testFlag(QStyle::State_MouseOver)) {//鼠标划过状态
        background = QColor("rgba(42, 166, 248, 20%)");//QColor("#242424");//2aa6f8
        painter->fillRect(option.rect, background);
    }
    else {
        painter->fillRect(option.rect, background);
    }

    /*if (m_hoverIndex == index) {
        // draw hover background
        QRect mRect = option.rect;
        QPoint start_point(mRect.topLeft());
        QPoint end_point(mRect.topRight());
        QLinearGradient gradient(start_point, end_point);
        gradient.setColorAt(0, QColor("#0da4f5"));
        gradient.setColorAt(0.015, QColor("#2aa6f8"));
        gradient.setColorAt(0.0155, QColor("rgba(42, 166, 248, 20%)"));
        gradient.setColorAt(1, QColor("rgba(42, 166, 248, 20%)"));
        painter->fillRect(mRect, QBrush(gradient));
        painter->setCompositionMode(QPainter::CompositionMode_SourceIn);
    }*/

    QString filepath = index.data().toString();//TODO: kobe for data(filepath)
    auto video = pref->video(filepath);
    if (video.isNull()) {
        painter->restore();
        return;
    }

    for (int column = 0; column < Invalid; ++column) {
        auto textColor = this->foregroundColor(column, option);
        auto flag = cellAlignmentFlag(column);
        auto rect = cellRect(column, option);
        painter->setPen(textColor);
        switch (column) {
        case Icon: {
            auto listview = qobject_cast<PlayListView *>(const_cast<QWidget *>(option.widget));
            QString playingFile = listview->getPlayingFile();
            if (playingFile == video->m_localpath) {
                QString iconPath = ":/res/video_normal.png";
                if (option.state & QStyle::State_Selected) {
                    iconPath = ":/res/video_hover_press.png";
                }
                QPixmap icon = QPixmap(iconPath);
                auto centerF = QRectF(rect).center();
                auto iconRect = QRectF(centerF.x() - icon.width() / 2,
                                       centerF.y() - icon.height() / 2,
                                       icon.width(), icon.height());
                painter->drawPixmap(iconRect.toRect(), icon);
            }

            break;
        }
        case Name: {
            painter->setFont(m_font);
            QFontMetrics fm(m_font);
            QString text = fm.elidedText(video->m_name, Qt::ElideMiddle, rect.width());
            painter->drawText(rect, flag, text);
            break;
        }
        case Length:
            painter->setFont(m_font);
            painter->drawText(rect, flag, Utils::formatTime((int)video->m_duration));
            break;
        default:
            break;
        }
    }

    painter->restore();
}

QSize PlaylistDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QSize totalSize = QStyledItemDelegate::sizeHint(option, index);
    totalSize.setHeight(48);//设置行高,也可以使用Model中通过Qt::SizeHintRole设置

    int iconWidth = 15 + 20;
    int delWidth = 15 + 20;
    const int timeWidth = 100;
    int w = option.widget->width() - iconWidth - delWidth - timeWidth;

    switch (index.column()) {
    case 0:
        return  QSize(iconWidth, totalSize.height());
    case 1:
        return  QSize(w, totalSize.height());
    case 2:
        return  QSize(timeWidth, totalSize.height());
    case 3:
        return  QSize(delWidth, totalSize.height());
    }

    return totalSize;
}

void PlaylistDelegate::initStyleOption(QStyleOptionViewItem *option, const QModelIndex &index) const
{
    QStyledItemDelegate::initStyleOption(option, index);
    option->state = option->state & ~QStyle::State_HasFocus;

}

QWidget *PlaylistDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)
    Q_UNUSED(index)

    QPushButton *m_removeBtn = new QPushButton(parent);
    m_removeBtn->setFixedSize(16,16);
    m_removeBtn->setFocusPolicy(Qt::NoFocus);
    m_removeBtn->setObjectName("PlayListDelete");
    connect(m_removeBtn, &QPushButton::clicked, this, &PlaylistDelegate::removeBtnClicked);

    return m_removeBtn;
}

void PlaylistDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index)

    QPushButton *m_removeBtn = static_cast<QPushButton *>(editor);
    QRect btnRect = option.rect;
    editor->setGeometry(btnRect.x() + btnRect.width() - m_removeBtn->width() - 10, btnRect.y() + (btnRect.height()-m_removeBtn->height())/2, m_removeBtn->width(), m_removeBtn->height());
}

void PlaylistDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QStyledItemDelegate::setEditorData(editor, index);
}

void PlaylistDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QStyledItemDelegate::setModelData(editor, model, index);
}
