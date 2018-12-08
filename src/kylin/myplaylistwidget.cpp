#include "myplaylistwidget.h"

#include <QAction>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QMimeData>
#include <QResizeEvent>
#include <QStandardItemModel>
#include <QDebug>
#include <QUuid>
#include <QModelIndex>

#include "medialibrary.h"
#include "playlistmodel.h"
#include "playlistview1.h"

QString newID()
{
    return QUuid::createUuid().toString().remove("{").remove("}").remove("-");
}


MyPlayListWidget::MyPlayListWidget(QWidget *parent) :
    QDialog(parent)
{
    qDebug() << "dalgjsalgjslkgjlskag";
//    setObjectName("MyPlayListWidget");
    this->setFixedSize(600, 500);
//    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setStyleSheet("QDialog{border: 1px solid #121212;border-radius:1px;background-color:#1f1f1f;}");

//    setAcceptDrops(true);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    actionBar = new QFrame;
    actionBar->setFixedHeight(40);

    QHBoxLayout *actionBarLayout = new QHBoxLayout(actionBar);
    actionBarLayout->setContentsMargins(10, 0, 8, 0);
    actionBarLayout->setSpacing(0);

    m_addBtn = new QPushButton;
    m_addBtn->setText(tr("Add"));
    m_addBtn->setFixedHeight(28);
    m_addBtn->setFocusPolicy(Qt::NoFocus);

    actionBarLayout->addWidget(m_addBtn, 0, Qt::AlignCenter);
    actionBarLayout->addStretch();
    musiclist = new PlayListView1();

    layout->addWidget(actionBar, 0, Qt::AlignTop);
    layout->addWidget(musiclist, 100, Qt::AlignTop);

    this->initConntion();


    library = MediaLibrary::Instance();
    library->initMediaLib();
    this->onLocate();
}

MyPlayListWidget::~MyPlayListWidget()
{
}

//void MyPlayListWidget::initData()
//{
//    musiclist->onMusiclistChanged();
//    qDebug() << "musiclist->getModelRowCount()=" << musiclist->getModelRowCount();
//}

void MyPlayListWidget::initConntion()
{
    connect(m_addBtn, SIGNAL(clicked(bool)), this, SLOT(onaddBtnClicked()));
    connect(musiclist, SIGNAL(requestCustomContextMenu(QPoint)), this, SIGNAL(requestCustomContextMenu(QPoint)));
    connect(musiclist, SIGNAL(removeMusicList(MetaPtrList)), this, SLOT(onRemoveMusicList(MetaPtrList)));
//    connect(musiclist, SIGNAL(deleteMusicList(MetaPtrList)), this, SLOT(onDeleteMusicList(MetaPtrList)));
//    connect(musiclist, SIGNAL(addToPlaylist(PlaylistPtr,MetaPtrList)), this, SIGNAL(addToPlaylist(PlaylistPtr,MetaPtrList)));
//    connect(musiclist, SIGNAL(playMedia(/*MetaPtr*/QString)), this, SIGNAL(playMedia(QString)));
//    connect(musiclist, SIGNAL(showInfoDialog(MetaPtr)), this, SIGNAL(showInfoDialog(MetaPtr)));
//    connect(musiclist, SIGNAL(updateMetaCodec(MetaPtr)), this, SIGNAL(updateMetaCodec(MetaPtr)));
}

void MyPlayListWidget::onaddBtnClicked()
{
    auto meta = MediaLibrary::Instance()->addOneMeta(newID());
    if (meta.isNull()) {
        qDebug() << "meta had exists.";
    }
    else {
        qDebug() << "new meta.data()->hash=" << meta.data()->hash;
        musiclist->addMedia(meta);
    }
}

void MyPlayListWidget::onRemoveMusicList(const MetaPtrList  & metalist)
{
    MediaLibrary::Instance()->removeMusicList(metalist);

    this->onMusicListRemoved(metalist);//remove item from ui's model
}

void MyPlayListWidget::setCustomSortType()
{

}

/*void MyPlayListWidget::dragEnterEvent(QDragEnterEvent *event)
{
    QFrame::dragEnterEvent(event);
    if (event->mimeData()->hasFormat("text/uri-list")) {
        qDebug() << "acceptProposedAction" << event;
        event->setDropAction(Qt::CopyAction);
        event->acceptProposedAction();
        return;
    }
}

void MyPlayListWidget::dropEvent(QDropEvent *event)
{
    QFrame::dropEvent(event);

    if (!event->mimeData()->hasFormat("text/uri-list")) {
        return;
    }

    auto urls = event->mimeData()->urls();
    QStringList localpaths;
    for (auto &url : urls) {
        localpaths << url.toLocalFile();
    }

//    if (!localpaths.isEmpty() && !this->musiclist->playlist().isNull()) {
//        emit importMusicListWidgetSelectFiles(this->musiclist->playlist(), localpaths);
//    }
}*/

void MyPlayListWidget::resizeEvent(QResizeEvent *event)
{
    QDialog::resizeEvent(event);
    QRect viewrect = QDialog::rect();
    QSize viewsize = viewrect.marginsRemoved(contentsMargins()).size();
    musiclist->setFixedSize(viewsize.width(), viewsize.height() - 40);
}

void MyPlayListWidget::onMusicListRemoved(const MetaPtrList metalist)
{
    this->musiclist->onMusicListRemoved(metalist);
    qDebug() << "musiclist->getModelRowCount()=" << musiclist->getModelRowCount();
}

void MyPlayListWidget::onMusicError(const MetaPtr meta, int error)
{
    this->musiclist->onMusicError(meta, error);
}

void MyPlayListWidget::onMusicListAdded(const MetaPtrList metalist)
{
    this->musiclist->onMusicListAdded(metalist);
}

void MyPlayListWidget::onLocate()
{
//    this->initData();
    musiclist->onMusiclistChanged();
    qDebug() << "musiclist->getModelRowCount()=" << musiclist->getModelRowCount();

    //set current index
    int row = musiclist->currentIndex().row();


    //model->insertRows(row, 1);
    QModelIndex index = this->musiclist->model->index(row, 0, QModelIndex());
//    QModelIndex index = this->musiclist->findIndex(meta);
    if (!index.isValid()) {
        return;
    }
    //model->setData(index, text);//TODO: kobe for data(hash)
    musiclist->setCurrentIndex(index);
//    this->musiclist->onLocate(/*meta*/);
}

void MyPlayListWidget::onMusiclistChanged()
{
//    this->initData();
}
