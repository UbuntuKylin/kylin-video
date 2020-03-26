/*  smplayer, GUI front-end for mplayer.
    Copyright (C) 2006-2015 Ricardo Villalba <rvm@users.sourceforge.net>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "mpvprocess.h"
#include <QRegExp>
#include <QStringList>
#include <QApplication>
#include <QDebug>

#include "global.h"
#include "preferences.h"
#include "mplayerversion.h"
#include "colorutils.h"

using namespace Global;

#define CUSTOM_STATUS

#define TOO_CHAPTERS_WORKAROUND

static QRegExp mpv_screenshot("Screenshot: '(.*)'");

MPVProcess::MPVProcess(const QString &snap, QObject * parent)
	: PlayerProcess(parent)
	, notified_mplayer_is_running(false)
	, notified_pause(false)
	, received_end_of_file(false)
	, last_sub_id(-1)
	, mplayer_svn(-1) // Not found yet
	, verbose(false)
	, subtitle_info_received(false)
	, subtitle_info_changed(false)
    , selected_subtitle(-1)
	, audio_info_changed(false)
    , selected_audio(-1)
	, video_info_changed(false)
    , selected_video(-1)
    , chapter_info_changed(false)
	, dvd_current_title(-1)
	, br_current_title(-1)
{
    this->m_snap = snap;

    m_playerId = Utils::MPV;

	connect( this, SIGNAL(lineAvailable(QByteArray)),
			 this, SLOT(parseLine(QByteArray)) );

	connect( this, SIGNAL(finished(int,QProcess::ExitStatus)), 
             this, SLOT(processFinished(int,QProcess::ExitStatus)) );

	connect( this, SIGNAL(error(QProcess::ProcessError)),
             this, SLOT(gotError(QProcess::ProcessError)) );

	/* int svn = MplayerVersion::mplayerVersion("mpv unknown version (C)"); */

    initializeOptionVars();
    initializeRX();
}

MPVProcess::~MPVProcess() {
}

bool MPVProcess::start() {
	md.reset();
	notified_mplayer_is_running = false;
	notified_pause = false;
	last_sub_id = -1;
	mplayer_svn = -1; // Not found yet
	received_end_of_file = false;

	subs.clear();
	subtitle_info_received = false;
	subtitle_info_changed = false;
    selected_subtitle = -1;


	audios.clear();
	audio_info_changed = false;
    selected_audio = -1;

	videos.clear();
    video_info_changed = false;
    selected_video = -1;


//#if NOTIFY_CHAPTER_CHANGES
    chapters.clear();
    chapter_info_changed = false;
//#endif

	dvd_current_title = -1;
	br_current_title = -1;

	MyProcess::start();

	return waitForStarted();
}

void MPVProcess::initializeRX() {
//#ifdef CUSTOM_STATUS
    rx_av.setPattern("STATUS: ([0-9\\.-]+) / ([0-9\\.-]+) P: (yes|no) B: (yes|no) I: (yes|no) VB: ([0-9\\.-]+) AB: ([0-9\\.-]+)");// new for ubuntukylin 20.04 and 16.04
    //rx_av.setPattern("^STATUS: ([0-9\\.-]+) / ([0-9\\.-]+) P: (yes|no) B: (yes|no) I: (yes|no) VB: ([0-9\\.-]+) AB: ([0-9\\.-]+)"); //old, 在ubuntukylin 16.04上正常，但那是在ubuntukylin 20.04上无法获取视频的播放状态，即parseLine中解析会得不到想要的数据
//#else
//    rx_av.setPattern("(\\((.*)\\) |)(AV|V|A): ([0-9]+):([0-9]+):([0-9]+) / ([0-9]+):([0-9]+):([0-9]+)"); //AV: 00:02:15 / 00:09:56
//      //rx_av.setPattern("^(\\((.*)\\) |)(AV|V|A): ([0-9]+):([0-9]+):([0-9]+) / ([0-9]+):([0-9]+):([0-9]+)"); //AV: 00:02:15 / 00:09:56 //old
//#endif

    rx_dsize.setPattern("^INFO_VIDEO_DSIZE=(\\d+)x(\\d+)");
    rx_vo.setPattern("^VO: \\[(.*)\\]");
    rx_ao.setPattern("^AO: \\[(.*)\\]");
    rx_paused.setPattern("^\\(Paused\\)");
    rx_endoffile.setPattern("^Exiting... \\(End of file\\)");

    rx_audio.setPattern("^.* Audio\\s+--aid=(\\d+)( --alang=([a-zA-Z-]+)|)([ \\(\\)\\*]+)('(.*)'|)");
    rx_subs.setPattern("^.* Subs\\s+--sid=(\\d+)( --slang=([a-zA-Z-]+)|)([ \\(\\)\\*]+)('(.*)'|)");

    rx_videocodec.setPattern("^INFO_VIDEO_CODEC=(.*)\\s");
    rx_videocodec.setMinimal(true);

    rx_audiocodec.setPattern("^INFO_AUDIO_CODEC=(.*)\\s");
    rx_audiocodec.setMinimal(true);

//#if !NOTIFY_VIDEO_CHANGES
//    rx_video.setPattern("^.* Video\\s+--vid=(\\d+)([ \\(\\)\\*]+)('(.*)'|)");
//#endif
    rx_chaptername.setPattern("^INFO_CHAPTER_(\\d+)_NAME=(.*)");
    rx_trackinfo.setPattern("^INFO_TRACK_(\\d+): (audio|video|sub) (\\d+) '(.*)' '(.*)' (yes|no)");

    rx_forbidden.setPattern("HTTP error 403 Forbidden");

    rx_playing.setPattern("^Playing:.*|^\\[ytdl_hook\\].*");
    rx_generic.setPattern("^([A-Z_]+)=(.*)");
    rx_stream_title.setPattern("icy-title: (.*)");
    rx_debug.setPattern("^(INFO|METADATA)_.*=\\$.*");
}

void MPVProcess::parseLine(QByteArray ba) {
    //qDebug() << "MPVProcess::parseLine:" << ba;

    if (ba.isEmpty()) return;

    QString tag;
    QString value;
//COLOR_OUTPUT_SUPPORT for windows
//#if COLOR_OUTPUT_SUPPORT
//    QString line = ColorUtils::stripColorsTags(QString::fromLocal8Bit(ba));
//#else
//    #ifdef Q_OS_WIN
//    QString line = QString::fromUtf8(ba);
//    #else
    QString line = QString::fromLocal8Bit(ba);
//    #endif
//#endif

    if (verbose) {
        line.replace("[statusline] ", "");
        line.replace("[cplayer] ", "");
        line.replace("[term-msg] ", "");

        if (rx_debug.indexIn(line) > -1) {
            line = "[debug] " + line;
        }
    }

    static double last_sec = -1;

    // Parse A: V: line
    //qDebug("MPVProcess::parseLine: %s", line.toUtf8().data());
    if (rx_av.indexIn(line) > -1) {
//        #ifdef CUSTOM_STATUS
        double sec = rx_av.cap(1).toDouble();
        double length = rx_av.cap(2).toDouble();
        bool paused = (rx_av.cap(3) == "yes");
        bool buffering = (rx_av.cap(4) == "yes");
        bool idle = (rx_av.cap(5) == "yes");
        int video_bitrate = rx_av.cap(6).toInt();
        int audio_bitrate = rx_av.cap(7).toInt();

        if (length != md.duration) {
            md.duration = length;
//            #if DVDNAV_SUPPORT
//            emit receivedDuration(length);
//            #endif
        }

        if (paused && notified_pause) {
            if (last_sec != sec) {
                last_sec = sec;
                emit receivedCurrentSec(sec);
                emit receivedPause();
            }
            return;
        }

        if (paused) {
            notified_pause = true;
            //qDebug("MPVProcess::parseLine: paused");
            emit receivedPause();
            return;
        }
        else
        if (buffering) {
            //qDebug("MPVProcess::parseLine: buffering");
            emit receivedBuffering();
            return;
        }
        else
        if (idle) {
            //qDebug("MPVProcess::parseLine: idle");
            emit receivedBuffering();
            return;
        }
        notified_pause = false;

        if (video_bitrate != md.video_bitrate) {
            md.video_bitrate = video_bitrate;
            emit receivedVideoBitrate(video_bitrate);
        }
        if (audio_bitrate != md.audio_bitrate) {
            md.audio_bitrate = audio_bitrate;
            emit receivedAudioBitrate(audio_bitrate);
        }

        /*#else

        //qDebug() << rx_av.cap(1);
        //qDebug() << rx_av.cap(2);

        QString status = rx_av.cap(2).trimmed();

        int i = 4;
        int h = rx_av.cap(i++).toInt();
        int m = rx_av.cap(i++).toInt();
        int s = rx_av.cap(i++).toInt();
        //qDebug("%d:%d:%d", h, m, s);
        double sec = h * 3600 + m * 60 + s;

        h = rx_av.cap(i++).toInt();
        m = rx_av.cap(i++).toInt();
        s = rx_av.cap(i++).toInt();
        double length = h * 3600 + m * 60 + s;
        if (length != md.duration) {
            md.duration = length;
            emit receivedDuration(length);
        }

        if (status == "Paused") {
            emit receivedPause();
            return;
        }
        else
        if ((status == "...") || (status == "Buffering")) {
            emit receivedBuffering();
            return;
        }

        if (!status.isEmpty()) {
            qDebug() << "MPVProcess::parseLine: status:" << status;
        }

        #endif*/

        if (notified_mplayer_is_running) {
            if (subtitle_info_changed && sec > 0.4) {
                //qDebug("MPVProcess::parseLine: subtitle_info_changed");
                subtitle_info_changed = false;
                subtitle_info_received = false;
                emit subtitleInfoChanged(subs, selected_subtitle);
            }
            if (subtitle_info_received) {
                //qDebug("MPVProcess::parseLine: subtitle_info_received");
                subtitle_info_received = false;
                emit subtitleInfoReceivedAgain(subs);
            }
        }

        if (notified_mplayer_is_running) {
            if (audio_info_changed && sec > 0.4) {
                //qDebug("MPVProcess::parseLine: audio_info_changed");
                audio_info_changed = false;
                emit audioInfoChanged(audios, selected_audio);
            }
        }

        if (notified_mplayer_is_running) {
            if (video_info_changed) {
                //qDebug("MPVProcess::parseLine: video_info_changed");
                video_info_changed = false;
                emit videoInfoChanged(videos, selected_video);
            }
        }

//#if NOTIFY_CHAPTER_CHANGES
        if (notified_mplayer_is_running) {
            if (chapter_info_changed) {
                //qDebug("MPVProcess::parseLine: chapter_info_changed");
                chapter_info_changed = false;
                emit chaptersChanged(chapters);
            }
        }
//#endif

        if (!notified_mplayer_is_running) {
            //qDebug() << "MPVProcess::parseLine: starting sec:" << sec;

            if (md.video_width == 0 || md.video_height == 0) {
                md.novideo = true;
                emit receivedNoVideo();
            }

            emit receivedStartingTime(sec);
            emit mplayerFullyLoaded();

            emit receivedCurrentFrame(0); // Ugly hack: set the frame counter to 0

            notified_mplayer_is_running = true;
        }

        emit receivedCurrentSec( sec );
    }
    else {
        emit lineAvailable(line);

        // Parse other things
        //qDebug() << "MPVProcess::parseLine:" << line;

        if (mpv_screenshot.indexIn(line) > -1) {//20170722
            QString shot = mpv_screenshot.cap(1);
//            qDebug() << "shot="<<shot;
            emit receivedScreenshot(shot);
        }

        // End of file
        if (rx_endoffile.indexIn(line) > -1)  {
            qDebug("MVPProcess::parseLine: detected end of file");
            if (!received_end_of_file) {
                // In case of playing VCDs or DVDs, maybe the first title
                // is not playable, so the GUI doesn't get the info about
                // available titles. So if we received the end of file
                // first let's pretend the file has started so the GUI can have
                // the data.
                if ( !notified_mplayer_is_running) {
                    emit mplayerFullyLoaded();
                }
                // Send signal once the process is finished, not now!
                received_end_of_file = true;
            }
        }
        else

        // Window resolution
        if (rx_dsize.indexIn(line) > -1) {
            int w = rx_dsize.cap(1).toInt();
            int h = rx_dsize.cap(2).toInt();
            emit receivedWindowResolution( w, h );
        }
        else

        // VO
        if (rx_vo.indexIn(line) > -1) {
            emit receivedVO( rx_vo.cap(1) );
            // Ask for window resolution
            writeToStdin("print_text INFO_VIDEO_DSIZE=${=dwidth}x${=dheight}");
        }
        else

        // AO
        if (rx_ao.indexIn(line) > -1) {
            emit receivedAO( rx_ao.cap(1) );
        }
        else


        // Audio
        if (rx_audio.indexIn(line) > -1) {
            int ID = rx_audio.cap(1).toInt();
            QString lang = rx_audio.cap(3);
            QString title = rx_audio.cap(6);
            updateAudioTrack(ID, title, lang, false);
        }
        else

        if (rx_stream_title.indexIn(line) > -1) {
            QString s = rx_stream_title.cap(1);
            //qDebug() << "MPVProcess::parseLine: stream_title:" << s;
            md.stream_title = s;
            emit receivedStreamTitle(s);
        }
        else

        // Subtitles
        if (rx_subs.indexIn(line) > -1) {
            int ID = rx_subs.cap(1).toInt();
            QString lang = rx_subs.cap(3);
            QString title = rx_subs.cap(6);
            //qDebug() << "MPVProcess::parseLine: sub id:" << ID << "lang:" << lang << "name:" << title;

            updateSubtitleTrack(ID, title, lang, false);
        }
        else
        // Chapters
        if (rx_chaptername.indexIn(line) > -1) {
            int ID = rx_chaptername.cap(1).toInt();
            QString title = rx_chaptername.cap(2);
            if (title.isEmpty()) title = QString::number(ID + 1);
            chapters.addName(ID, title);
            chapter_info_changed = true;
            //qDebug() << "MPVProcess::parseLine: chapter id:" << ID << "title:" << title;
        }
        else

        // Track info
        if (rx_trackinfo.indexIn(line) > -1) {
            int ID = rx_trackinfo.cap(3).toInt();
            QString type = rx_trackinfo.cap(2);
            QString name = rx_trackinfo.cap(5);
            QString lang = rx_trackinfo.cap(4);
            QString selected = rx_trackinfo.cap(6);
            //qDebug() << "MPVProcess::parseLine: ID:" << ID << "type:" << type << "name:" << name << "lang:" << lang << "selected:" << selected;
            /*
            if (lang == "(unavailable)") lang = "";
            if (name == "(unavailable)") name = "";
            */
            if (type == "video") {
                updateVideoTrack(ID, name, lang, (selected == "yes"));
            }
            else
            if (type == "audio") {
                updateAudioTrack(ID, name, lang, (selected == "yes"));
            }
            else
            if (type == "sub") {
                updateSubtitleTrack(ID, name, lang, (selected == "yes"));
            }
        }
        else

//#if DVDNAV_SUPPORT
//        if (rx_switch_title.indexIn(line) > -1) {
//            int title = rx_switch_title.cap(1).toInt();
//            qDebug("MPVProcess::parseLine: title changed to %d", title);
//            // Ask for track info
//            // Wait 10 secs. because it can take a while until the title start to play
//            QTimer::singleShot(10000, this, SLOT(requestChapterInfo()));
//        }
//        else
//#endif

        //Playing
        if (rx_playing.indexIn(line) > -1) {
            emit receivedPlaying();
        }
        else

        if (rx_videocodec.indexIn(line) > -1) {
            md.video_codec = rx_videocodec.cap(1);
            //qDebug() << "MPVProcess::parseLine: md.video_codec:" << md.video_codec;
        }
        else
        if (rx_audiocodec.indexIn(line) > -1) {
            md.audio_codec = rx_audiocodec.cap(1);
            //qDebug() << "MPVProcess::parseLine: md.audio_codec:" << md.audio_codec;
        }
        else

        if (rx_forbidden.indexIn(line) > -1) {
            //qDebug("MVPProcess::parseLine: 403 forbidden");
            emit receivedForbiddenText();
        }

        //Generic things
        if (rx_generic.indexIn(line) > -1) {
            tag = rx_generic.cap(1);
            value = rx_generic.cap(2);
            //qDebug("MPVProcess::parseLine: tag: %s", tag.toUtf8().constData());
            //qDebug("MPVProcess::parseLine: value: %s", value.toUtf8().constData());

            if (tag == "INFO_VIDEO_WIDTH") {
                md.video_width = value.toInt();
                //qDebug("MPVProcess::parseLine: md.video_width set to %d", md.video_width);
            }
            else
            if (tag == "INFO_VIDEO_HEIGHT") {
                md.video_height = value.toInt();
                //qDebug("MPVProcess::parseLine: md.video_height set to %d", md.video_height);
            }
            else
            if (tag == "INFO_VIDEO_ASPECT") {
                md.video_aspect = value.toDouble();
                if ( md.video_aspect == 0.0 ) {
                    // I hope width & height are already set.
                    md.video_aspect = (double) md.video_width / md.video_height;
                }
                //qDebug() << "MPVProcess::parseLine: md.video_aspect set to" << md.video_aspect;
            }
            if (tag == "INFO_VIDEO_BITRATE") {
                int bitrate = value.toInt();
                emit receivedVideoBitrate(bitrate);
            }
            else
            if (tag == "INFO_LENGTH") {
                md.duration = value.toDouble();
                //qDebug() << "MPVProcess::parseLine: md.duration set to" << md.duration;
            }
            else
            if (tag == "INFO_DEMUXER") {
                md.demuxer = value;
            }
            else
            if (tag == "INFO_VIDEO_FORMAT") {
                md.video_format = value;
            }
            if (tag == "INFO_VIDEO_FPS") {
                md.video_fps = value;
            }
            else
            /*
            if (tag == "INFO_VIDEO_CODEC") {
                md.video_codec = value;
            }
            else
            if (tag == "INFO_AUDIO_CODEC") {
                md.audio_codec = value;
            }
            else
            */
            if (tag == "INFO_AUDIO_BITRATE") {
                int bitrate = value.toInt();
                emit receivedAudioBitrate(bitrate);
            }
            else
            if (tag == "INFO_AUDIO_RATE") {
                md.audio_rate = value.toInt();
            }
            else
            if (tag == "INFO_AUDIO_NCH") {
                md.audio_nch = value.toInt();
            }
            else
            if (tag == "INFO_AUDIO_FORMAT") {
                md.audio_format = value;
            }
            else
            if (tag == "INFO_CHAPTERS") {
                md.n_chapters = value.toInt();
//                #ifdef TOO_CHAPTERS_WORKAROUND
                if (md.n_chapters > 1000) {
                    //qDebug("MPVProcess::parseLine: warning too many chapters: %d", md.n_chapters);
                    //qDebug("                       chapters will be ignored");
                    md.n_chapters = 0;
                }
//                #endif
                for (int n = 0; n < md.n_chapters; n++) {
                    writeToStdin(QString("print_text INFO_CHAPTER_%1_NAME=${chapter-list/%1/title}").arg(n));
                }
            }
            else
            if (tag == "INFO_TITLES") {
                int n_titles = value.toInt();
                for (int ID = 0; ID < n_titles; ID++) {
                    md.titles.addName(ID, QString::number(ID+1));
                }
            }
            else
            if (tag == "METADATA_TITLE") {
                if (!value.isEmpty()) md.clip_name = value;
            }
            else
            if (tag == "METADATA_ARTIST") {
                if (!value.isEmpty()) md.clip_artist = value;
            }
            else
            if (tag == "METADATA_DATE") {
                if (!value.isEmpty()) md.clip_date = value;
            }
            else
            if (tag == "METADATA_ALBUM") {
                if (!value.isEmpty()) md.clip_album = value;
            }
            else
            if (tag == "METADATA_COPYRIGHT") {
                if (!value.isEmpty()) md.clip_copyright = value;
            }
            else
            if (tag == "METADATA_TRACK") {
                if (!value.isEmpty()) md.clip_track = value;
            }
            else
            if (tag == "METADATA_GENRE") {
                if (!value.isEmpty()) md.clip_genre = value;
            }
            else
            if (tag == "INFO_MEDIA_TITLE") {
                if (!value.isEmpty() && value != "mp4" && !value.startsWith("mp4&") /*&& md.clip_name.isEmpty()*/) {
                    md.clip_name = value;
                }
            }
            else
            if (tag == "INFO_STREAM_PATH") {
                QRegExp rx("edl://%\\d+%(.*)");
                if (rx.indexIn(line) > -1) {
                    md.stream_path = rx.cap(1);
                } else {
                    md.stream_path = value;
                }
            }
            else
            if (tag == "MPV_VERSION") {
                mpv_version = value;
                if (mpv_version.startsWith("mpv ")) mpv_version = mpv_version.mid(4);
                //qDebug() << "MPVProcess::parseLine: mpv version:" << mpv_version;
                MplayerVersion::mplayerVersion("mpv " + mpv_version + " (C)");
            }
            else
            if (tag == "INFO_TRACKS_COUNT") {
                int tracks = value.toInt();
                for (int n = 0; n < tracks; n++) {
                    writeToStdin(QString("print_text \"INFO_TRACK_%1: "
                        "${track-list/%1/type} "
                        "${track-list/%1/id} "
                        "'${track-list/%1/lang:}' "
                        "'${track-list/%1/title:}' "
                        "${track-list/%1/selected}\"").arg(n));
                }
            }
        }
    }
}

void MPVProcess::requestChapterInfo() {
	writeToStdin("print_text \"INFO_CHAPTERS=${=chapters}\"");
}

//void MPVProcess::requestBitrateInfo() {
//	writeToStdin("print_text INFO_VIDEO_BITRATE=${=video-bitrate}");
//	writeToStdin("print_text INFO_AUDIO_BITRATE=${=audio-bitrate}");
//}


void MPVProcess::updateVideoTrack(int ID, const QString & name, const QString & lang, bool selected) {
    int idx = videos.find(ID);
    if (idx == -1) {
        video_info_changed = true;
        videos.addName(ID, name);
        videos.addLang(ID, lang);
    } else {
        // Track already existed
        if (videos.itemAt(idx).name() != name) {
            video_info_changed = true;
            videos.addName(ID, name);
        }
        if (videos.itemAt(idx).lang() != lang) {
            video_info_changed = true;
            videos.addLang(ID, lang);
        }
    }

    if (selected && selected_video != ID) {
        selected_video = ID;
        video_info_changed = true;
    }
}

void MPVProcess::updateAudioTrack(int ID, const QString & name, const QString & lang, bool selected) {
    //qDebug("MPVProcess::updateAudioTrack: ID: %d", ID);

    int idx = audios.find(ID);
    if (idx == -1) {
        audio_info_changed = true;
        audios.addName(ID, name);
        audios.addLang(ID, lang);
    } else {
        // Track already existed
        if (audios.itemAt(idx).name() != name) {
            audio_info_changed = true;
            audios.addName(ID, name);
        }
        if (audios.itemAt(idx).lang() != lang) {
            audio_info_changed = true;
            audios.addLang(ID, lang);
        }
    }

    if (selected && selected_audio != ID) {
        selected_audio = ID;
        audio_info_changed = true;
    }
}

void MPVProcess::updateSubtitleTrack(int ID, const QString & name, const QString & lang, bool selected) {
    qDebug("MPVProcess::updateSubtitleTrack: ID: %d", ID);

    int idx = subs.find(SubData::Sub, ID);
    if (idx == -1) {
        subtitle_info_changed = true;
        subs.add(SubData::Sub, ID);
        subs.changeName(SubData::Sub, ID, name);
        subs.changeLang(SubData::Sub, ID, lang);
    }
    else {
        // Track already existed
        if (subs.itemAt(idx).name() != name) {
            subtitle_info_changed = true;
            subs.changeName(SubData::Sub, ID, name);
        }
        if (subs.itemAt(idx).lang() != lang) {
            subtitle_info_changed = true;
            subs.changeLang(SubData::Sub, ID, lang);
        }
    }

    if (selected && selected_subtitle != ID) {
        selected_subtitle = ID;
        subtitle_info_changed = true;
    }
}

// Called when the process is finished
void MPVProcess::processFinished(int exitCode, QProcess::ExitStatus exitStatus) {
	qDebug("MPVProcess::processFinished: exitCode: %d, status: %d", exitCode, (int) exitStatus);
	// Send this signal before the endoffile one, otherwise
	// the playlist will start to play next file before all
	// objects are notified that the process has exited.
	emit processExited();
	if (received_end_of_file) emit receivedEndOfFile();
}

void MPVProcess::gotError(QProcess::ProcessError error) {
	qDebug("MPVProcess::gotError: %d", (int) error);
}

#include "mpvoptions.cpp"
//#include "moc_mpvprocess.cpp"
