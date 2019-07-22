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

#include "playlist.h"
#include "../smplayer/paths.h"
#include "../playlistmodel.h"
#include "../playlistview.h"
#include "../smplayer/myaction.h"
#include "../smplayer/filedialog.h"
#include "../smplayer/helper.h"
#include "../smplayer/preferences.h"
#include "../smplayer/version.h"
#include "../smplayer/global.h"
#include "../smplayer/core.h"
#include "../smplayer/extensions.h"
#include "../messagedialog.h"
#include "../smplayer/infoprovider.h"

#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QFileInfo>
#include <QPushButton>
#include <QLabel>
#include <QRegExp>
#include <QMenu>
#include <QSettings>
#include <QTimer>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QApplication>
#include <QMimeData>
#include <QUrl>
#include <QDebug>
#include <QResizeEvent>
#include <QModelIndex>
#include <QPainter>
#include <QGraphicsOpacityEffect>

using namespace Global;

Playlist::Playlist(Core *c, QWidget * parent, Qt::WindowFlags f)
    : QFrame(parent, Qt::SubWindow)
    , set(0)
    , modified(false)
    , recursive_add_directory(false)
    , automatically_get_info(false)
    , play_files_from_start(true)
    , start_play_on_load(true)
    , automatically_play_next(true)
    , ignore_player_errors(false)
    , allow_delete_from_disk(true)
{
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setAutoFillBackground(true);
    this->setFixedWidth(220);
    setObjectName("PlaylistWidget");

    setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred);
    setAcceptDrops(true);
    setAttribute(Qt::WA_NoMousePropagation);

	core = c;
    playlist_path = "";
    latest_dir = QDir::homePath();
    titleLabel = 0;
    btDel = 0;
    btAdd = 0;
    novideo_icon = 0;
    novideo_text = 0;
    add_Btn = 0;
    title_layout = NULL;

    setConfigPath(Paths::configPath());

    createNoVideo();
	createTable();
    createToolbar();

    QVBoxLayout *layout = new QVBoxLayout(this);
    setFocusPolicy(Qt::ClickFocus);
    layout->setContentsMargins(0, 0, 0, 15);
    layout->setSpacing(16);
    btAddFrame = new QFrame;
    btAddFrame->setFixedSize(120, 36);
    btAddFrame->setObjectName("PlaylistWidgetAddFrame");
    btAddFrame->setFocusPolicy(Qt::NoFocus);
    QHBoxLayout *btAddFameLayout = new QHBoxLayout(btAddFrame);
    btAddFameLayout->setMargin(0);
    btAddFameLayout->setSpacing(0);
    btAddFameLayout->addWidget(btAdd, 0, Qt::AlignVCenter);
    btAddFameLayout->addWidget(btDel, 0, Qt::AlignVCenter);
    title_layout = new QHBoxLayout();
    title_layout->setSpacing(0);
    title_layout->setMargin(0);
    title_layout->addWidget(titleLabel, 0, Qt::AlignHCenter);
    title_layout->addWidget(btAddFrame, 0, Qt::AlignHCenter);
    noVideoFrame = new QFrame();
    noVideoFrame->setFocusPolicy(Qt::NoFocus);
    QVBoxLayout *noVideoFameLayout = new QVBoxLayout(noVideoFrame);
    noVideoFameLayout->setMargin(0);
    noVideoFameLayout->setSpacing(30);
    noVideoFameLayout->addStretch();
    noVideoFameLayout->addWidget(novideo_icon, 0, Qt::AlignHCenter);
    noVideoFameLayout->addWidget(novideo_text, 0, Qt::AlignHCenter);
    noVideoFameLayout->addWidget(add_Btn, 0, Qt::AlignHCenter);
    noVideoFameLayout->addStretch();
    layout->addLayout(title_layout);

    layout->addWidget(m_playlistView, 0, Qt::AlignHCenter);
    layout->addWidget(noVideoFrame, 0, Qt::AlignHCenter);
    layout->addStretch();
    setLayout(layout);

    clear();

    connect(core, SIGNAL(mediaFinished()), this, SLOT(playNextAuto()), Qt::QueuedConnection);
    connect(core, SIGNAL(mplayerFailed(QProcess::ProcessError)), this, SLOT(playerFailed(QProcess::ProcessError)) );
    connect(core, SIGNAL(mplayerFinishedWithError(int)), this, SLOT(playerFinishedWithError(int)) );
    connect(core, SIGNAL(mediaDataReceived(const MediaData &)), this, SLOT(getMediaInfo(const MediaData &)));

	// Ugly hack to avoid to play next item automatically
    /*if (!automatically_play_next) {
		disconnect( core, SIGNAL(mediaFinished()), this, SLOT(playNext()) );
    }*/

	// Save config every 5 minutes.
	save_timer = new QTimer(this);
    connect(save_timer, SIGNAL(timeout()), this, SLOT(maybeSaveSettings()));
    save_timer->start(5 * 60000);

    if (this->count() > 0) {
        noVideoFrame->hide();
        m_playlistView->show();
    }
    else {
        noVideoFrame->show();
        m_playlistView->hide();
    }

    loadSettings();
}

Playlist::~Playlist()
{
    saveSettings();
    if (set) {
        delete set;
        set = 0;
    }

    if (titleLabel != NULL) {
        delete titleLabel;
        titleLabel = NULL;
    }
    if (btDel != NULL) {
        delete btDel;
        btDel = NULL;
    }
    if (btAdd != NULL) {
        delete btAdd;
        btAdd = NULL;
    }
    if (novideo_icon != NULL) {
        delete novideo_icon;
        novideo_icon = NULL;
    }
    if (novideo_text != NULL) {
        delete novideo_text;
        novideo_text = NULL;
    }
    if (add_Btn != NULL) {
        delete add_Btn;
        add_Btn = NULL;
    }

    if (noVideoFrame != NULL) {
        delete noVideoFrame;
        noVideoFrame = NULL;
    }
    if (btAddFrame != NULL) {
        delete btAddFrame;
        btAddFrame = NULL;
    }

    if (title_layout) {
        delete title_layout;
        title_layout = NULL;
    }

    if (m_playlistView != NULL) {
        delete m_playlistView;
        m_playlistView = NULL;
    }

    if (save_timer != NULL) {
        disconnect(save_timer, SIGNAL(timeout()), this, SLOT(maybeSaveSettings()));
        if(save_timer->isActive()) {
            save_timer->stop();
        }
        delete save_timer;
        save_timer = NULL;
    }
}

void Playlist::setConfigPath(const QString & config_path)
{
    if (set) {
        delete set;
        set = 0;
    }

    if (!config_path.isEmpty()) {
        QString inifile = config_path + "/kylin-video-playlist.ini";
        //qDebug() << "Playlist::setConfigPath: ini file:" << inifile;
        set = new QSettings(inifile, QSettings::IniFormat);
        set->setIniCodec("UTF-8");
    }
}

void Playlist::updateWindowTitle()
{
    /*
    QString title;

    title = playlist_filename;
    if (title.isEmpty()) title = tr("Untitled playlist");
    if (modified) title += " (*)";

    emit sig_playing_title(title);//emit windowTitleChanged(title);
*/
}

void Playlist::setPlaylistFilename(const QString & f)
{
    //playlist_filename = f;
    //updateWindowTitle();
}

void Playlist::setTransparent(bool transparent)
{
    if (transparent) {
        setAttribute(Qt::WA_TranslucentBackground);
        setWindowFlags(windowFlags() | Qt::FramelessWindowHint|Qt::X11BypassWindowManagerHint);
        setWidgetOpacity(0.5);
    }
    else {
        setAttribute(Qt::WA_TranslucentBackground,false);
        setWindowFlags(windowFlags() & ~Qt::FramelessWindowHint);
    }
}

void Playlist::setWidgetOpacity(const float &opacity)
{
    QGraphicsOpacityEffect* opacityEffect = new QGraphicsOpacityEffect;
    this->setGraphicsEffect(opacityEffect);
    opacityEffect->setOpacity(opacity);
}

void Playlist::setModified(bool mod)
{
    modified = mod;
    emit modifiedChanged(modified);
    updateWindowTitle();
}

void Playlist::createTable()
{
    m_playlistView = new PlayListView(set, this);
    //    m_playlistView->setFixedHeight(430);
    //    QSizePolicy sp(QSizePolicy::Preferred, QSizePolicy::Preferred);
    //    sp.setVerticalStretch(100);
    //    m_playlistView->setSizePolicy(sp);
    connect(m_playlistView, SIGNAL(activated(QModelIndex)), this, SLOT(itemActivated(QModelIndex)));
    connect(m_playlistView, SIGNAL(requestResortVideos(QStringList, int)), this, SLOT(onResortVideos(QStringList, int)));
    connect(m_playlistView, SIGNAL(requestPlayVideo(int, QString)), this, SLOT(onPlayListItemDoubleClicked(int,QString)));
    connect(m_playlistView, SIGNAL(requestRemoveFiles(QStringList)), this, SLOT(onPlayListItemDeleteBtnClicked(QStringList)));
    connect(m_playlistView, SIGNAL(requestDeleteVideos(QStringList)), this, SLOT(deleteSelectedFileFromDisk(QStringList)));
}

void Playlist::setViewHeight()
{
    m_playlistView->setFixedHeight(this->height()-36-16);//36为顶部按钮和label的高度，16为顶部和列表的间隔
}

void Playlist::createNoVideo()
{
    novideo_icon = new QLabel();
    novideo_icon->setFixedSize(70, 70);
    novideo_icon->setPixmap(QPixmap(":/res/no-video.png"));
    novideo_text = new QLabel();
    novideo_text->setObjectName("VideoText");
    novideo_text->setText(tr("Playlist is empty"));
    add_Btn = new QPushButton();
    add_Btn->setFocusPolicy(Qt::NoFocus);
    add_Btn->setFixedSize(140, 38);
    add_Btn->setObjectName("PlaylistAddButton");
    add_Btn->setText(tr("Add File"));
    connect(add_Btn, SIGNAL(clicked(bool)), SLOT(popupDialogtoSelectFiles()));
}

void Playlist::createToolbar()
{
    titleLabel = new QLabel();
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setFixedSize(220-120, 36);
    titleLabel->setStyleSheet("QLabel{font-size:12px;color:#ffffff;background:transparent;}");
    titleLabel->setText(tr("PlayList"));

    btDel = new QPushButton();
    btDel->setFocusPolicy(Qt::NoFocus);
    btDel->setFixedSize(60, 36);
    btDel->setText(tr("Clear"));
    btDel->setObjectName("PlaylistButton");
    connect(btDel, SIGNAL(clicked(bool)), SLOT(removeAll()));

    btAdd = new QPushButton();
    btAdd->setFocusPolicy(Qt::NoFocus);
    btAdd->setFixedSize(60, 36);
    btAdd->setObjectName("PlaylistButton");
    btAdd->setText(tr("Add"));
    connect(btAdd, SIGNAL(clicked(bool)), SLOT(popupDialogtoSelectFiles()));
}

void Playlist::clear()
{
    this->m_playlistView->reset();
    this->m_playlistView->clearSelection();
    this->m_playlistView->clearFocus();
    this->m_playlistView->updateScrollbarSize();

    emit this->update_playlist_count(0);
    this->setPlaying("", 0);
    noVideoFrame->show();
    m_playlistView->hide();
    setModified(false);
}

int Playlist::count()
{
    return m_playlistView->getModelRowCount();
}

bool Playlist::isEmpty()
{
    return (this->m_playlistView->getModelRowCount() == 0);
}

bool Playlist::rowIsEmpty()
{
    return pref->m_videoMap.isEmpty();
}

void Playlist::loadSingleItem(QString filename, QString name, double duration)
{
    //Attention: maybe name is empty when adding a new local file
    if (name.isEmpty()) {
        QFileInfo fi(filename);
        // Let's see if it looks like a file (no dvd://1 or something)
        if (filename.indexOf(QRegExp("^.*://.*")) == -1) {
            // Local file
            name = fi.fileName();
        } else {
            // Stream
            name = filename;
        }
    }

    auto video = pref->generateVedioData(filename, name, duration);
    bool exists = false;
    foreach (VideoPtr videoPtr, pref->m_videoMap) {//for (auto video : m_videoMap)
        if (videoPtr->m_localpath == filename.toUtf8().data()) {
            exists = true;
            break;
        }
    }

    if (!exists) {
        pref->m_videoMap.insert(video->m_localpath, video);
        setModified(true);
        m_playlistView->addPlayListItem(filename, name, duration);
        emit this->update_playlist_count(this->count());
    }
}

void Playlist::loadItemWithoutUI(QString filename, QString name, double duration)
{
    if (name.isEmpty()) {
        QFileInfo fi(filename);
        // Let's see if it looks like a file (no dvd://1 or something)
        if (filename.indexOf(QRegExp("^.*://.*")) == -1) {
            // Local file
            name = fi.fileName();
        } else {
            // Stream
            name = filename;
        }
    }

    auto video = pref->generateVedioData(filename, name, duration);
    bool exists = false;
    foreach (VideoPtr videoPtr, pref->m_videoMap) {//for (auto video : m_videoMap)
        if (videoPtr->m_localpath == filename.toUtf8().data()) {
            exists = true;
            break;
        }
    }

    if (!exists) {
        pref->m_videoMap.insert(video->m_localpath, video);
    }
}

void Playlist::load_m3u(QString file, M3UFormat format)
{
    /*bool utf8 = false;
    if (format == DetectFormat) {
        utf8 = (QFileInfo(file).suffix().toLower() == "m3u8");
    } else {
        utf8 = (format == M3U8);
    }

    qDebug() << "Playlist::load_m3u: utf8:" << utf8;

    QRegExp m3u_id("^#EXTM3U|^#M3U");
    QRegExp rx_info("^#EXTINF:([.\\d]+).*tvg-logo=\"(.*)\",(.*)");

    QFile f( file );
    if ( f.open( QIODevice::ReadOnly ) ) {
        playlist_path = QFileInfo(file).path();

        clear();
        QString filename="";
        QString name="";
        double duration=0;
        QStringList extra_params;
        QString icon_url;

        QTextStream stream( &f );

        if (utf8)
            stream.setCodec("UTF-8");
        else
            stream.setCodec(QTextCodec::codecForLocale());

        QString line;
        while ( !stream.atEnd() ) {
            line = stream.readLine().trimmed();
            if (line.isEmpty()) continue; // Ignore empty lines

            qDebug() << "Playlist::load_m3u: line:" << line;
            if (m3u_id.indexIn(line)!=-1) {
                //#EXTM3U
                // Ignore line
            }
            else
            if (rx_info.indexIn(line) != -1) {
                duration = rx_info.cap(1).toDouble();
                name = rx_info.cap(3);
                icon_url = rx_info.cap(2);
                qDebug() << "Playlist::load_m3u: name:" << name << "duration:" << duration << "icon_url:" << icon_url;
            }
            else
            if (line.startsWith("#EXTINF:")) {
                QStringList fields = line.mid(8).split(",");
                //qDebug() << "Playlist::load_m3u: fields:" << fields;
                if (fields.count() >= 1) duration = fields[0].toDouble();
                if (fields.count() >= 2) name = fields[1];
            }
            else
            if (line.startsWith("#EXTVLCOPT:")) {
                QString par = line.mid(11);
                qDebug() << "Playlist::load_m3u: EXTVLCOPT:" << par;
                extra_params << par;
            }
            else
            if (line.startsWith("#")) {
                // Comment
                // Ignore
            } else {
                filename = line;
                QFileInfo fi(filename);
                if (fi.exists()) {
                    filename = fi.absoluteFilePath();
                }
                if (!fi.exists()) {
                    if (QFileInfo( playlist_path + "/" + filename).exists() ) {
                        filename = playlist_path + "/" + filename;
                    }
                }
                name.replace("&#44;", ",");
                //qDebug() << "Playlist::load_m3u: extra_params:" << extra_params;
                addItem( filename, name, duration, extra_params, "", icon_url );
                name = "";
                duration = 0;
                extra_params.clear();
                icon_url = "";
            }
        }
        f.close();
        //list();

        setPlaylistFilename(file);
        setModified(false);

        if (start_play_on_load) startPlayPause();
    }*/
}

void Playlist::load_pls(QString file)
{
    qDebug("Playlist::load_pls");

    /*if (!QFile::exists(file)) {
        qDebug("Playlist::load_pls: '%s' doesn't exist, doing nothing", file.toUtf8().constData());
        return;
    }

    playlist_path = QFileInfo(file).path();

    QSettings set(file, QSettings::IniFormat);
    set.beginGroup("playlist");

    if (set.status() == QSettings::NoError) {
        clear();
        QString filename;
        QString name;
        double duration;

        int num_items = set.value("NumberOfEntries", 0).toInt();

        #if QT_VERSION >= 0x050000
        // It seems Qt 5 is case sensitive
        if (num_items == 0) num_items = set.value("numberofentries", 0).toInt();
        #endif

        for (int n=0; n < num_items; n++) {
            filename = set.value("File"+QString::number(n+1), "").toString();
            name = set.value("Title"+QString::number(n+1), "").toString();
            duration = (double) set.value("Length"+QString::number(n+1), 0).toInt();

            QFileInfo fi(filename);
            if (fi.exists()) {
                filename = fi.absoluteFilePath();
            }
            if (!fi.exists()) {
                if (QFileInfo( playlist_path + "/" + filename).exists() ) {
                    filename = playlist_path + "/" + filename;
                }
            }
            addItem( filename, name, duration );
        }
    }

    set.endGroup();

    //list();

    setPlaylistFilename(file);
    setModified(false);

    if (set.status() == QSettings::NoError && start_play_on_load) startPlayPause();*/
}

void Playlist::loadXSPF(const QString & filename)
{
    qDebug() << "Playlist::loadXSPF:" << filename;

    /*QFile f(filename);
    if (!f.open(QIODevice::ReadOnly)) {
        return;
    }

    QDomDocument dom_document;
    bool ok = dom_document.setContent(f.readAll());
    qDebug() << "Playlist::loadXSPF: success:" << ok;
    if (!ok) return;

    QDomNode root = dom_document.documentElement();
    qDebug() << "Playlist::loadXSPF: tagname:" << root.toElement().tagName();

    QDomNode child = root.firstChildElement("trackList");
    if (!child.isNull()) {
        clear();

        qDebug() << "Playlist::loadXSPF: child:" << child.nodeName();
        QDomNode track = child.firstChildElement("track");
        while (!track.isNull()) {
            QString location = QUrl::fromPercentEncoding(track.firstChildElement("location").text().toLatin1());
            QString title = track.firstChildElement("title").text();
            int duration = track.firstChildElement("duration").text().toInt();

            qDebug() << "Playlist::loadXSPF: location:" << location;
            qDebug() << "Playlist::loadXSPF: title:" << title;
            qDebug() << "Playlist::loadXSPF: duration:" << duration;

            loadSingleItem( location, title, (double) duration / 1000 );

            track = track.nextSiblingElement("track");
        }

        //list();
        setPlaylistFilename(filename);
        setModified( false );
        if (start_play_on_load) startPlayPause();
    }*/

//    this->m_playlistView->updateScrollbarSize();
}

bool Playlist::save_m3u(QString file)
{
    qDebug() << "Playlist::save_m3u:" << file;

    return false;

    /*QString dir_path = QFileInfo(file).path();
    if (!dir_path.endsWith("/")) dir_path += "/";

//	#if defined(Q_OS_WIN) || defined(Q_OS_OS2)
//	dir_path = Helper::changeSlashes(dir_path);
//	#endif

    qDebug() << "Playlist::save_m3u: dir_path:" << dir_path;

    bool utf8 = (QFileInfo(file).suffix().toLower() == "m3u8");

    QFile f( file );
    if ( f.open( QIODevice::WriteOnly ) ) {
        QTextStream stream( &f );

        if (utf8)
            stream.setCodec("UTF-8");
        else
            stream.setCodec(QTextCodec::codecForLocale());

        QString filename;
        QString name;

        stream << "#EXTM3U" << "\n";
        stream << "# Playlist created by SMPlayer " << Version::printable() << " \n";

        for (int n = 0; n < count(); n++) {
            PlayListItem * i = itemData(n);
            filename = i->filename();
//			#if defined(Q_OS_WIN) || defined(Q_OS_OS2)
//			filename = Helper::changeSlashes(filename);
//			#endif
            name = i->name();
            name.replace(",", "&#44;");
            QString icon_url = i->iconURL();
            stream << "#EXTINF:";
            stream << i->duration();
            if (!icon_url.isEmpty()) stream << " tvg-logo=\"" + icon_url + "\"";
            stream << ",";
            stream << name << "\n";

            // Save extra params
            QStringList params = i->extraParams();
            foreach(QString par, params) {
                stream << "#EXTVLCOPT:" << par << "\n";
            }

            // Try to save the filename as relative instead of absolute
            if (filename.startsWith( dir_path )) {
                filename = filename.mid( dir_path.length() );
            }
            stream << filename << "\n";
        }
        f.close();

        setPlaylistFilename(file);
        setModified( false );
        return true;
    } else {
        return false;
    }*/
}


bool Playlist::save_pls(QString file)
{
    qDebug() << "Playlist::save_pls:" << file;

    return false;

    /*
    QString dir_path = QFileInfo(file).path();
    if (!dir_path.endsWith("/")) dir_path += "/";

//	#if defined(Q_OS_WIN) || defined(Q_OS_OS2)
//	dir_path = Helper::changeSlashes(dir_path);
//	#endif

    qDebug() << "Playlist::save_pls: dir_path:" << dir_path;

    QSettings set(file, QSettings::IniFormat);
    set.beginGroup( "playlist");

    QString filename;

    for (int n = 0; n < count(); n++) {
        PlayListItem * i = itemData(n);
        filename = i->filename();
//		#if defined(Q_OS_WIN) || defined(Q_OS_OS2)
//		filename = Helper::changeSlashes(filename);
//		#endif

        // Try to save the filename as relative instead of absolute
        if (filename.startsWith( dir_path )) {
            filename = filename.mid( dir_path.length() );
        }

        set.setValue("File"+QString::number(n+1), filename);
        set.setValue("Title"+QString::number(n+1), i->name());
        set.setValue("Length"+QString::number(n+1), (int) i->duration());
    }

    set.setValue("NumberOfEntries", count());
    set.setValue("Version", 2);

    set.endGroup();

    set.sync();

    bool ok = (set.status() == QSettings::NoError);
    if (ok) {
        setPlaylistFilename(file);
        setModified( false );
    }

    return ok;*/
}

bool Playlist::saveXSPF(const QString & filename)
{
    qDebug() << "Playlist::saveXSPF:" << filename;

    return false;

    /*QFile f(filename);
    if (f.open( QIODevice::WriteOnly)) {
        QTextStream stream(&f);
        stream.setCodec("UTF-8");

        stream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
        stream << "<playlist version=\"1\" xmlns=\"http://xspf.org/ns/0/\">\n";
        stream << "\t<trackList>\n";

        for (int n = 0; n < count(); n++) {
            PlayListItem * i = itemData(n);
            QString location = i->filename();
            qDebug() << "Playlist::saveXSPF:" << location;

            bool is_local = QFile::exists(location);

//			#ifdef Q_OS_WIN
//			if (is_local) {
//				location.replace("\\", "/");
//			}
//			#endif
            //qDebug() << "Playlist::saveXSPF:" << location;

            QUrl url(location);
            location = url.toEncoded();
            //qDebug() << "Playlist::saveXSPF:" << location;

            if (!location.startsWith("file:") && is_local) {
//				#ifdef Q_OS_WIN
//				location = "file:///" + location;
//				#else
                location = "file://" + location;
//				#endif
            }

            QString title = i->name();
            int duration = i->duration() * 1000;

            #if QT_VERSION >= 0x050000
            location = location.toHtmlEscaped();
            title = title.toHtmlEscaped();
            #else
            location = Qt::escape(location);
            title = Qt::escape(title);
            #endif

            stream << "\t\t<track>\n";
            stream << "\t\t\t<location>" << location << "</location>\n";
            stream << "\t\t\t<title>" << title << "</title>\n";
            stream << "\t\t\t<duration>" << duration << "</duration>\n";
            stream << "\t\t</track>\n";
        }

        stream << "\t</trackList>\n";
        stream << "</playlist>\n";

        setPlaylistFilename(filename);
        setModified(false);
        return true;
    } else {
        return false;
    }*/
}


void Playlist::load()
{
    /*if (maybeSave()) {
        Extensions e;
        QString s = MyFileDialog::getOpenFileName(
                    this, tr("Choose a file"),
                    lastDir(),
                    tr("Playlists") + e.playlist().forFilter() + ";;" + tr("All files") +" (*)");

        if (!s.isEmpty()) {
            latest_dir = QFileInfo(s).absolutePath();

            QString suffix = QFileInfo(s).suffix().toLower();
            if (suffix == "pls") {
                //load_pls(s);
            }
            else
            if (suffix == "xspf") {
                //loadXSPF(s);
            }
            else {
                //load_m3u(s);
            }
        }
    }*/
}

bool Playlist::saveCurrentPlaylist()
{
    return save(playlistFilename());
}

bool Playlist::save(const QString & filename)
{
    QString s = filename;

    if (s.isEmpty()) {
        Extensions e;
        s = MyFileDialog::getSaveFileName(
                    this, tr("Choose a filename"),
                    lastDir(),
                    tr("Playlists") + e.playlist().forFilter() + ";;" + tr("All files") +" (*)");
    }

    if (!s.isEmpty()) {
        // If filename has no extension, add it
        if (QFileInfo(s).suffix().isEmpty()) {
            s = s + ".m3u";
        }
        if (QFileInfo(s).exists()) {
            int res = QMessageBox::question( this,
                    tr("Confirm overwrite?"),
                    tr("The file %1 already exists.\n"
                       "Do you want to overwrite?").arg(s),
                    QMessageBox::Yes,
                    QMessageBox::No,
                    QMessageBox::NoButton);
            if (res == QMessageBox::No ) {
                return false;
            }
        }
        latest_dir = QFileInfo(s).absolutePath();

        QString suffix = QFileInfo(s).suffix().toLower();
        if (suffix  == "pls") {
            return save_pls(s);
        }
        else
        if (suffix  == "xspf") {
            return saveXSPF(s);
        }
        else {
            return save_m3u(s);
        }

    } else {
        return false;
    }
}

bool Playlist::maybeSave()
{
    return false;
    /*if (!isModified()) return true;

    int res = QMessageBox::question( this,
                tr("Playlist modified"),
                tr("There are unsaved changes, do you want to save the playlist?"),
                QMessageBox::Yes,
                QMessageBox::No,
                QMessageBox::Cancel);

    switch (res) {
        case QMessageBox::No : return true; // Discard changes
        case QMessageBox::Cancel : return false; // Cancel operation
        default : return save();
    }*/
}

//双击播放列表的一项时进行播放
void Playlist::onPlayListItemDoubleClicked(int row, const QString &filename)
{
    //playlist_filename = filename;

    QFileInfo fi(filename);
    if (fi.exists()) {
        // Local file
        if ((row >= this->m_playlistView->getModelRowCount()) || (row < 0)) {
            emit this->sig_playing_title("");
            return;
        }


        QString name = fi.fileName();
        emit this->sig_playing_title(name);

        //保存当前播放文件的索引和文件路径名
        this->setPlaying(filename, row);

        core->open(filename/*, 0*/);//每次从头开始播放文件
    }
    else {
        emit this->playListFinishedWithError(filename);
    }
}

void Playlist::setPlaying(const QString &filepath, int index)
{
    this->m_currentItemIndex = index;
    this->m_playlistView->setPlayingInfo(filepath, m_currentItemIndex);
}

void Playlist::itemActivated(const QModelIndex & index )
{
    // onPlayListItemDoubleClicked
    //qDebug() << "Playlist::itemActivated: row:" << index.row();
    //playItem(index.row());
}

//kobe:添加多个文件文件夹或拖拽进多个文件文件夹时才会走这里，如果是支持乱序，则乱序选择一个开始播放
void Playlist::startPlayPause()
{
	// Start to play
    if (pref->play_order == Preferences::RandomPlay) {//随机播放
        playItem(chooseRandomItem());
    }
    else {//顺序播放 列表循环
        playItem(0);
    }
}

void Playlist::playItem( int n )
{
    //qDebug() << "Playlist::playItem n=" << n;

    if ( (n >= this->m_playlistView->getModelRowCount()) || (n < 0) ) {
        qDebug("Playlist::playItem: out of range");
        emit playlistEnded();
        emit this->sig_playing_title("");
        return;
    }

    QString filename = this->m_playlistView->getFileNameByRow(n);
    if (!filename.isEmpty()) {
        QFileInfo fi(filename);
        if (fi.exists()) {
            //this->playlist_filename = filename;

            // Local file
            QString name = fi.fileName();
            emit this->sig_playing_title(name);
            //0621 kobe:  core->open(filename, 0): 每次从头开始播放文件，    core->open(filename):每次从上次播放停止位置开始播放文件

            //保存当前播放文件的索引和文件路径名
            this->setPlaying(filename, n);

            if (play_files_from_start) {
                emit this->sig_playing_title(name);
                core->open(filename, 0);
            }
            else {
                emit this->sig_playing_title(name);
                core->open(filename, 0);
            }
        }
        else {
            emit this->playListFinishedWithError(filename);
        }
    }
}

void Playlist::playNext()
{
//    emit this->sig_playing_title("");
//    qDebug("Playlist::playNext  pl[m_currentItemIndex]->name()=%s", pl[m_currentItemIndex]->name());
    //qDebug() << "playNext m_currentItemIndex=" << m_currentItemIndex;
    if (pref->play_order == Preferences::RandomPlay) {//随机播放
        int chosen_item = chooseRandomItem();
        if (chosen_item == -1) {
            clearPlayedTag();
            chosen_item = chooseRandomItem();
            if (chosen_item == -1) chosen_item = 0;
        }
        playItem(chosen_item);
    }
    else if (pref->play_order == Preferences::ListLoopPlay) {//列表循环
        bool finished_list = (m_currentItemIndex + 1 >= this->m_playlistView->getModelRowCount());//pl.count()
        if (finished_list) {
            clearPlayedTag();
        }
        if (finished_list) {
            playItem(0);
        }
        else {
            playItem(m_currentItemIndex+1);
        }
    }
    else {//顺序播放
        bool finished_list = (m_currentItemIndex + 1 >= this->m_playlistView->getModelRowCount());//pl.count()
        if (finished_list) {
            clearPlayedTag();
        }
        if (finished_list) {
//            emit this->finish_list();
            emit this->showMessage(tr("Reached the end of the playlist"));
        }
        else {
            playItem(m_currentItemIndex + 1);
        }
    }
}

void Playlist::playPrev()
{
    if (m_currentItemIndex > 0) {
        playItem(m_currentItemIndex - 1);
    }
    else {
        if (this->m_playlistView->getModelRowCount() > 1) {
            playItem(this->m_playlistView->getModelRowCount() - 1);
        }
    }
}

void Playlist::playNextAuto()
{
    //qDebug("Playlist::playNextAuto");
    if (automatically_play_next) {
        playNext();
    }
    else {
        emit playlistEnded();
    }
}

void Playlist::resumePlay()
{
    if (this->m_playlistView->getModelRowCount() > 0) {
        if (m_currentItemIndex < 0) {
            this->setPlaying("", 0);
        }
        playItem(m_currentItemIndex);
    }
}

void Playlist::getMediaInfo(const MediaData & mdat)
{
    QString filename = mdat.m_filename;//20181201  m_filename
    double duration = mdat.duration;
    QString artist = mdat.clip_artist;
    QString video_url = mdat.stream_path;

    QString name;
    //name = mdat.clip_name;//有的rmvb视频的clip_name存在乱码
    if (name.isEmpty()) name = mdat.stream_title;

    if (name.isEmpty()) {
        QFileInfo fi(filename);
        if (fi.exists()) {
            // Local file
            name = fi.fileName();
        } else {
            // Stream
            name = filename;
        }
    }
    if (!artist.isEmpty())
        name = artist + " - " + name;
}

// Add current file to playlist
//void Playlist::addCurrentFile()
//{
//    emit requestToAddCurrentFile();
//}

void Playlist::addFiles()
{
    Extensions e;
    QStringList files = MyFileDialog::getOpenFileNames(
                            this, tr("Select one or more files to open"),
                            lastDir(),
                            tr("Multimedia") + e.multimedia().forFilter() + ";;" +
                            tr("All files") +" (*.*)" );

    if (files.count() != 0) {
        addFiles(files);
        setModified(true);
    }
}

void Playlist::onPlayListChanged(const VideoPtrList medialist)
{
    setCursor(Qt::WaitCursor);

    // update pref->m_videoMap
    for (auto &meta : medialist) {
        pref->m_videoMap.insert(meta->localpath(), meta);
        setModified(true);
    }

    // update settings
    set->remove("playlist_contents");
    set->beginGroup("playlist_contents");
    set->beginWriteArray("items");
    int index = 0;
    QMap<QString, VideoPtr>::iterator i;
    for (i = pref->m_videoMap.begin(); i != pref->m_videoMap.end(); ++i) {//for (auto video : pref->remainingMaps)
        set->setArrayIndex(index);
        set->setValue(QString("item_%1_filename").arg(index), i.value()->localpath());
        set->setValue(QString("item_%1_duration").arg(index), i.value()->duration());
        set->setValue(QString("item_%1_name").arg(index), i.value()->name());
        latest_dir = QFileInfo(i.value()->localpath()).absolutePath();
        index ++;
    }
    set->endArray();
    set->setValue("current_item", m_currentItemIndex);
    set->endGroup();

    // update ui
    this->m_playlistView->onPlayListChanged(/*medialist*/);
    emit this->update_playlist_count(this->count());//emit this->update_playlist_count(pref->m_videoMap.count());
    if (this->count() > 0) {
        noVideoFrame->setVisible(false);
        m_playlistView->setVisible(true);
    }
    else {
        m_currentItemIndex = 0;
        noVideoFrame->setVisible(true);
        m_playlistView->setVisible(false);
    }

    unsetCursor();
}

void Playlist::popupDialogtoSelectFiles()
{
    //打开一个或多个文件时，此时只是将选择的文件加入播放列表，并不会自动去播放选择的文件
    Extensions e;
    QStringList files = MyFileDialog::getOpenFileNames(
                            this, tr("Select one or more files to open"),
                            lastDir(),
                            tr("Multimedia") + e.multimedia().forFilter() + ";;" +
                            tr("All files") +" (*.*)" );

    if (files.count()!=0) addFiles(files);
}

void Playlist::addFiles(QStringList files, AutoGetInfo auto_get_info)
{
    emit requestGetMediaInfo(files);

//    bool get_info = (auto_get_info == GetInfo);
//    get_info = true;
    //get_info = automatically_get_info;

    /*MediaData data;
    setCursor(Qt::WaitCursor);

    for (int n = 0; n < files.count(); n++) {
        QString name = "";
        double duration = 0;

        if (pref->m_videoMap.contains(files[n])) {
            continue;
        }

        //TODO: thread
        //kobe 0606 如果选择多个文件，此时读取信息会耗时很长，导致界面卡顿，此处暂时不获取视频信息，在双击播放后再在函数updateView中更新视频的时长
        if ((QFile::exists(files[n])) ) {
            data = InfoProvider::getInfo(files[n]);
            name = data.displayName();
            duration = data.duration;
            this->loadSingleItem(files[n], name, duration);
            latest_dir = QFileInfo(files[n]).absolutePath();
        }
    }

    unsetCursor();

    this->m_playlistView->updateScrollbarSize();

    this->saveSettings();*/
}

void Playlist::addFile(QString file, AutoGetInfo auto_get_info)
{
    //打开一个文件时
	addFiles( QStringList() << file, auto_get_info );
}

void Playlist::addDirectory()
{
	QString s = MyFileDialog::getExistingDirectory(
                    this, tr("Choose a directory"),
                    lastDir() );

	if (!s.isEmpty()) {
		addDirectory(s);
        latest_dir = s;
	}
}


void Playlist::addUrls()
{
    /*MultilineInputDialog d(this);
    if (d.exec() == QDialog::Accepted) {
        QStringList urls = d.lines();
        foreach(QString u, urls) {
            if (!u.isEmpty()) addItem( u, "", 0 );
        }
        setModified(true);
    }*/
}

void Playlist::addOneDirectory(QString dir)
{
	QStringList filelist;

	Extensions e;
	QRegExp rx_ext(e.multimedia().forRegExp());
	rx_ext.setCaseSensitivity(Qt::CaseInsensitive);

	QStringList dir_list = QDir(dir).entryList();

	QString filename;
    QStringList::Iterator it = dir_list.begin();
    while( it != dir_list.end() ) {
		filename = dir;
		if (filename.right(1)!="/") filename += "/";
		filename += (*it);
		QFileInfo fi(filename);
		if (!fi.isDir()) {
			if (rx_ext.indexIn(fi.suffix()) > -1) {
				filelist << filename;
			}
		}
		++it;
	}
	addFiles(filelist);
}

void Playlist::addDirectory(QString dir)
{
	addOneDirectory(dir);

    if (recursive_add_directory) {//递归
        QFileInfoList dir_list = QDir(dir).entryInfoList(QStringList() << "*", QDir::AllDirs | QDir::NoDotAndDotDot);
        for (int n=0; n < dir_list.count(); n++) {
            if (dir_list[n].isDir()) {
                qDebug("Playlist::addDirectory: adding directory: %s", dir_list[n].filePath().toUtf8().data());
                addDirectory(dir_list[n].filePath());
            }
        }
    }

    setModified(true);
}

void Playlist::onPlayListItemDeleteBtnClicked(const QStringList &filepathlist)
{
    if (!filepathlist.isEmpty()) {
//    if (!filename.isEmpty()) {
//        MessageDialog msgDialog(0, tr("Confirm remove"),
//                                tr("You're about to remove the file '%1' from the playlist.").arg(filename) + "<br>"+
//                                tr("Are you sure you want to proceed?"), QMessageBox::Yes | QMessageBox::No);
        MessageDialog msgDialog(0, tr("Confirm remove"),
                                tr("You're about to remove the file from the playlist.") + "<br>"+
                                tr("Are you sure you want to proceed?"), QMessageBox::Yes | QMessageBox::No);
        if (msgDialog.exec() != -1) {
            if (msgDialog.standardButton(msgDialog.clickedButton()) == QMessageBox::Yes) {
                if (set) {
                    if (filepathlist.contains(this->m_playlistView->getPlayingFile())) {
                        this->setPlaying("", 0);
                    }

                    // Step 1: update pref->m_videoMap
                    for (QString filepath : filepathlist) {
                        foreach (VideoPtr videoPtr, pref->m_videoMap) {
                            if (videoPtr->m_localpath == filepath) {
                                pref->m_videoMap.remove(filepath);
                                break;
                            }
                        }
                    }

                    // Step 2: update settings
                    set->remove("playlist_contents");
                    set->beginGroup("playlist_contents");
                    set->beginWriteArray("items");
                    int index = 0;
                    QMap<QString, VideoPtr>::iterator i;
                    for (i = pref->m_videoMap.begin(); i != pref->m_videoMap.end(); ++i) {//for (auto video : pref->remainingMaps)
                        set->setArrayIndex(index);
                        set->setValue(QString("item_%1_filename").arg(index), i.value()->localpath());
                        set->setValue(QString("item_%1_duration").arg(index), i.value()->duration());
                        set->setValue(QString("item_%1_name").arg(index), i.value()->name());
                        index ++;
                    }
                    set->endArray();
                    set->setValue("current_item", m_currentItemIndex);
                    set->endGroup();

                    // Step3 : update ui
                    this->m_playlistView->removeFilesFromPlayList(filepathlist);
                    if (this->m_currentItemIndex > m_playlistView->getModelRowCount() -1) {
                        this->setPlaying("", 0);
                    }

                    // Step4: update count
                    if (!this->rowIsEmpty()) {//if (m_playlistView->getModelRowCount() > 0) {
                        noVideoFrame->hide();
                        this->m_playlistView->show();
                        this->playNext();//Fixed bug: #4915
                        emit this->update_playlist_count(pref->m_videoMap.count());
                    }
                    else {
                        this->setPlaying("", 0);
                        noVideoFrame->show();
                        this->m_playlistView->hide();
                        emit this->update_playlist_count(0);
                    }
//                        emit this->update_playlist_count(m_playlistView->getModelRowCount());
                }

                //TODO:如果playingFile不存在于pref->m_videoMap中了，则更新current_item
            }
        }
    }
}

void Playlist::deleteSelectedFileFromDisk(const QStringList &filepathlist)
{
    if (!allow_delete_from_disk) return;

    if (!filepathlist.isEmpty()) {
        // Ask the user for confirmation
        MessageDialog msgDialog(0, tr("Confirm deletion"),
                                tr("You're about to Delete the files from your drive.") + "<br>"+
                                tr("This action cannot be undone. Are you sure you want to proceed?"), QMessageBox::Yes | QMessageBox::No);
        if (msgDialog.exec() != -1) {
            if (msgDialog.standardButton(msgDialog.clickedButton()) == QMessageBox::Yes) {
                if (set) {
                    if (filepathlist.contains(this->m_playlistView->getPlayingFile())) {
                        this->setPlaying("", 0);
                    }


                    // Step 1: delete file from drive && update pref->m_videoMap
                    QStringList deletedList;
                    foreach(QString filepath, filepathlist) {
                        QFileInfo fi(filepath);
                        if (fi.exists() && fi.isFile() && fi.isWritable()) {
                            // Delete file
                            bool success = QFile::remove(filepath);
                            if (success) {
                                deletedList.append(filepath);
                                // Remove item from the playlist
                                if (pref->m_videoMap.contains(filepath)) {
                                    pref->m_videoMap.remove(filepath);
                                }
                            }
                        }
                    }

                    // Step 2: update settings
                    set->remove("playlist_contents");
                    set->beginGroup("playlist_contents");
                    set->beginWriteArray("items");
                    int index = 0;
                    QMap<QString, VideoPtr>::iterator i;
                    for (i = pref->m_videoMap.begin(); i != pref->m_videoMap.end(); ++i) {//for (auto video : pref->remainingMaps)
                        set->setArrayIndex(index);
                        set->setValue(QString("item_%1_filename").arg(index), i.value()->localpath());
                        set->setValue(QString("item_%1_duration").arg(index), i.value()->duration());
                        set->setValue(QString("item_%1_name").arg(index), i.value()->name());
                        index ++;
                    }
                    set->endArray();
                    set->setValue("current_item", m_currentItemIndex);
                    set->endGroup();

                    // Step3 : update ui
                    this->m_playlistView->removeFilesFromPlayList(deletedList);
                    if (this->m_currentItemIndex > m_playlistView->getModelRowCount() -1) {
                        this->setPlaying("", 0);
                    }

                    // Step4: update count
                    if (!this->rowIsEmpty()) {//if (m_playlistView->getModelRowCount() > 0) {
                        noVideoFrame->hide();
                        this->m_playlistView->show();
                        this->playNext();//Fixed bug: #4915
                        emit this->update_playlist_count(pref->m_videoMap.count());
                    }
                    else {
                        this->setPlaying("", 0);
                        noVideoFrame->show();
                        this->m_playlistView->hide();
                        emit this->update_playlist_count(0);
                    }
//                        emit this->update_playlist_count(m_playlistView->getModelRowCount());
                }
                //TODO:如果playingFile不存在于pref->m_videoMap中了，则更新current_item
            }
        }
    }
}

void Playlist::removeAll()
{
    MessageDialog msgDialog(0, tr("Confirm remove all"),
                            tr("You're about to empty the playlist.") + "<br>"+
                            tr("Are you sure you want to proceed?"), QMessageBox::Yes | QMessageBox::No);
    if (msgDialog.exec() != -1) {
        if (msgDialog.standardButton(msgDialog.clickedButton()) == QMessageBox::Yes) {
            if (set) {
                // Step 1: update pref->m_videoMap
                pref->m_videoMap.clear();

                // Step 2: update settings
                set->remove("playlist_contents");

                // Step3 : update ui && update count
                clear();
                setPlaylistFilename("");
            }
        }
    }
}

void Playlist::clearPlayedTag() {

}

int Playlist::chooseRandomItem()
{
    QList <int> fi; //List of not played items (free items)

    for (int row = 0; row < this->m_playlistView->getModelRowCount(); row++) {
        QString filepath = this->m_playlistView->getFileNameByRow(row);
        //Q_ASSERT(!filepath.isEmpty());
        if (filepath.isEmpty())
            continue;
        if (filepath != this->m_playlistView->getPlayingFile()) {//playlist_filename
            fi.append(row);
        }
    }

    if (fi.count() == 0) return -1; // none free

    int selected = (int) ((double) this->count() * rand()/(RAND_MAX+1.0));
    return fi[selected];
}

void Playlist::upItem()
{
//	qDebug("Playlist::upItem");

    /*int current = listView->currentRow();
    qDebug(" currentRow: %d", current );

    moveItemUp(current);*/




    /*QModelIndex index = listView->currentIndex();
    QModelIndex s_index = proxy->mapToSource(index);

    QModelIndex prev = listView->model()->index(index.row()-1, 0);
    QModelIndex s_prev = proxy->mapToSource(prev);

    qDebug() << "Playlist::upItem: row:" << index.row() << "source row:" << s_index.row();
    qDebug() << "Playlist::upItem: previous row:" << prev.row() << "previous source row:" << s_prev.row();

    if (s_index.isValid() && s_prev.isValid()) {
        int row = s_index.row();
        int prev_row = s_prev.row();

        int pos_num_current = itemData(row)->position();
        int pos_num_prev = itemData(prev_row)->position();

        qDebug() << "Playlist::upItem: pos_num_current:" << pos_num_current << "pos_num_prev:" << pos_num_prev;

        itemData(row)->setPosition(pos_num_prev);
        itemData(prev_row)->setPosition(pos_num_current);

        QList<QStandardItem*> cells = table->takeRow(row);
        table->insertRow(s_prev.row(), cells);
        listView->selectionModel()->setCurrentIndex(listView->model()->index(index.row()-1, 0), QItemSelectionModel::SelectCurrent | QItemSelectionModel::Rows);

        setModified(true);
    }*/

}

void Playlist::downItem() {
    /*qDebug("Playlist::downItem");

    int current = listView->currentRow();
    qDebug(" currentRow: %d", current );

    moveItemDown(current);*/




    /*QModelIndex index = listView->currentIndex();
    QModelIndex s_index = proxy->mapToSource(index);

    QModelIndex next = listView->model()->index(index.row()+1, 0);
    QModelIndex s_next = proxy->mapToSource(next);

    qDebug() << "Playlist::downItem: row:" << index.row() << "source row:" << s_index.row();
    qDebug() << "Playlist::downItem: next row:" << next.row() << "next source row:" << s_next.row();

    if (s_index.isValid() && s_next.isValid()) {
        int row = s_index.row();
        int next_row = s_next.row();

        int pos_num_current = itemData(row)->position();
        int pos_num_next = itemData(next_row)->position();

        qDebug() << "Playlist::downItem: pos_num_current:" << pos_num_current << "pos_num_next:" << pos_num_next;

        itemData(row)->setPosition(pos_num_next);
        itemData(next_row)->setPosition(pos_num_current);

        QList<QStandardItem*> cells = table->takeRow(row);
        table->insertRow(s_next.row(), cells);
        listView->selectionModel()->setCurrentIndex(listView->model()->index(index.row()+1, 0), QItemSelectionModel::SelectCurrent | QItemSelectionModel::Rows);

        setModified(true);
    }*/
}

void Playlist::moveItemUp(int current)
{

}
void Playlist::moveItemDown(int current	)
{

}

// Drag&drop
void Playlist::dragEnterEvent( QDragEnterEvent *e )
{
//	qDebug("Playlist::dragEnterEvent");

	if (e->mimeData()->hasUrls()) {
		e->acceptProposedAction();
	}
}
//#endif

void Playlist::copyURL()
{
    qDebug("Playlist::copyURL");

    /*QModelIndexList indexes = listView->selectionModel()->selectedRows();
    int count = indexes.count();

    QString text;

    for (int n = 0; n < count; n++) {
        QModelIndex s_index = proxy->mapToSource(indexes.at(n));
        int current = s_index.row();
        text += itemData(current)->filename();
        if (n < count-1) {
            #ifdef Q_OS_WIN
            text += "\r\n";
            #else
            text += "\n";
            #endif
        }
    }

    if (!text.isEmpty()) QApplication::clipboard()->setText(text);*/
}

void Playlist::openFolder()
{
    qDebug("Playlist::openFolder");

    /*QModelIndex index = listView->currentIndex();
    if (!index.isValid()) return;
    QModelIndex s_index = proxy->mapToSource(index);
    int current = s_index.row();
    PlayListItem * i = itemData(current);
    QString filename = i->filename();

    qDebug() << "Playlist::openFolder: filename:" << filename;

    QFileInfo fi(filename);
    if (fi.exists()) {
        QString src_folder = fi.absolutePath();
        QDesktopServices::openUrl(QUrl::fromLocalFile(src_folder));
    }*/
}

void Playlist::dropEvent( QDropEvent *e )
{
//	qDebug("Playlist::dropEvent");

	QStringList files;

	if (e->mimeData()->hasUrls()) {
		QList <QUrl> l = e->mimeData()->urls();
		QString s;
		for (int n=0; n < l.count(); n++) {
			if (l[n].isValid()) {
                //qDebug("Playlist::dropEvent: scheme: '%s'", l[n].scheme().toUtf8().data());
				if (l[n].scheme() == "file") 
					s = l[n].toLocalFile();
				else
					s = l[n].toString();
				/*
				qDebug(" * '%s'", l[n].toString().toUtf8().data());
				qDebug(" * '%s'", l[n].toLocalFile().toUtf8().data());
				*/
                //qDebug("Playlist::dropEvent: file: '%s'", s.toUtf8().data());
                if (pref->m_videoMap.contains(s)) {
                    continue;
                }
				files.append(s);
			}
		}
	}

	files.sort();

	QStringList only_files;
	for (int n = 0; n < files.count(); n++) {
        if (QFileInfo(files[n] ).isDir()) {
            addDirectory(files[n]);
		} else {
            only_files.append(files[n]);
		}
	}

    if (only_files.count() == 1) {
        // Check if the file is a playlist
        QString filename = only_files[0];
        QFileInfo fi(filename);
        QString extension = fi.suffix().toLower();
        if (extension == "m3u8" || extension == "m3u") { load_m3u(filename); return; }
        else
        if (extension == "pls") { load_pls(filename); return; }
        else
        if (extension == "xspf") { loadXSPF(filename); return; }
    }

    addFiles(only_files);
    setModified(true);
}

void Playlist::hideEvent( QHideEvent * )
{
    //emit change_playlist_btn_status(false);
}

void Playlist::showEvent( QShowEvent * )
{
    //emit change_playlist_btn_status(true);
}

void Playlist::playerFailed(QProcess::ProcessError e)
{
    emit this->sig_playing_title("");
    if (ignore_player_errors) {
        if (e != QProcess::FailedToStart) {
            playNext();
        }
    }
}

void Playlist::playerFinishedWithError(int e)
{
    emit this->sig_playing_title("");
    if (ignore_player_errors) {
        playNext();
    }
}

void Playlist::maybeSaveSettings()
{
    if (isModified())  {
        saveSettings();
    }
}

void Playlist::onResortVideos(const QStringList &sortList, int index)
{
    if (!set) return;

    if (sortList.isEmpty())
        return;

    if (index > -1) {
        m_currentItemIndex = index;
    }

    //rewrite ini file
    set->remove("playlist_contents");
    set->beginGroup("playlist_contents");
    set->beginWriteArray("items");
    for(int n=0; n < sortList.size(); n++) {
        QMap<QString, VideoPtr>::iterator k;
        for (k = pref->m_videoMap.begin(); k != pref->m_videoMap.end(); ++k) {
            if (sortList.at(n) == k.key()) {
                set->setArrayIndex(n);
                set->setValue(QString("item_%1_filename").arg(n), k.value()->localpath());
                set->setValue(QString("item_%1_duration").arg(n), k.value()->duration());
                set->setValue(QString("item_%1_name").arg(n), k.value()->name());
                break;
            }
        }
    }
    set->endArray();
    set->setValue("current_item", m_currentItemIndex);
    set->endGroup();

    //read form ini file
    pref->m_videoMap.clear();
    set->beginGroup("playlist_contents");
    int itemIndex = set->value("current_item", -1).toInt();
    this->m_currentItemIndex = itemIndex;
    int count = set->beginReadArray("items");
    QString filename, name;
    double duration;
    for (int n = 0; n < count; n++) {
        set->setArrayIndex(n);
        filename = set->value( QString("item_%1_filename").arg(n), "" ).toString();
        duration = set->value( QString("item_%1_duration").arg(n), -1 ).toDouble();
        name = set->value( QString("item_%1_name").arg(n), "" ).toString();
        this->loadItemWithoutUI(filename, name, duration);//kobe add playlist contents
        if (index == n) {
            this->setPlaying(filename, itemIndex);
        }
    }
    set->endArray();
    set->endGroup();
}

void Playlist::saveSettings()
{
    if (!set) return;

    set->beginGroup("directories");
    set->setValue("latest_dir", latest_dir);
    set->endGroup();

    //Save current list
    set->remove("playlist_contents");
    set->beginGroup("playlist_contents");
    set->beginWriteArray("items");
    int index = 0;
    for (int row = 0; row < this->m_playlistView->getModelRowCount(); row++ ) {
        QString filepath = this->m_playlistView->getFileNameByRow(row);
        //Q_ASSERT(!filepath.isEmpty());
        if (filepath.isEmpty())
            continue;
        QMap<QString, VideoPtr>::iterator i;
        for (i = pref->m_videoMap.begin(); i != pref->m_videoMap.end(); ++i) {
            if (filepath == i.value()->localpath()) {
                set->setArrayIndex(index);
                set->setValue(QString("item_%1_filename").arg(index), i.value()->localpath());
                set->setValue(QString("item_%1_duration").arg(index), i.value()->duration());
                set->setValue(QString("item_%1_name").arg(index), i.value()->name());
                index ++;
                break;
            }
        }
    }
    set->endArray();
    set->setValue("current_item", m_currentItemIndex);
    set->endGroup();
}

void Playlist::loadSettings()
{
    if (!set) return;

    //Load latest list
    set->beginGroup("playlist_contents");
    int index = set->value("current_item", -1).toInt();
    int count = set->beginReadArray("items");
    QString filename, name;
    double duration;
    for (int n = 0; n < count; n++) {
        set->setArrayIndex(n);
        filename = set->value( QString("item_%1_filename").arg(n), "" ).toString();
        duration = set->value( QString("item_%1_duration").arg(n), -1 ).toDouble();
        name = set->value(QString("item_%1_name").arg(n), "").toString();
        this->loadSingleItem(filename, name, duration);//kobe add playlist contents
        if (index == n) {
            this->setPlaying(filename, index);
        }
    }
    set->endArray();
    set->endGroup();
    this->m_playlistView->updateScrollbarSize();

    set->beginGroup("directories");
    latest_dir = set->value("latest_dir", latest_dir).toString();
    set->endGroup();

    int currentCount = this->count();
    emit this->update_playlist_count(currentCount);
    if (currentCount > 0) {
        noVideoFrame->hide();
        m_playlistView->show();
    }
    else {
        this->setPlaying("", 0);
        noVideoFrame->show();
        m_playlistView->hide();
    }
}

QString Playlist::lastDir()
{
    QString last_dir = latest_dir;
	return last_dir;
}

void Playlist::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

//    QPainter p(this);
//    p.setCompositionMode(QPainter::CompositionMode_Clear);
//    p.fillRect(rect(), Qt::SolidPattern);//p.fillRect(0, 0, this->width(), this->height(), Qt::SolidPattern);
}
