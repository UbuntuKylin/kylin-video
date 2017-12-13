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
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "mediadata.h"
#include <QFileInfo>
#include <cmath>


MediaData::MediaData() {
	reset();
}

MediaData::~MediaData() {
}

void MediaData::reset() {
	filename="";
	dvd_id="";
	type = TYPE_UNKNOWN;
	duration=0;

	novideo = false;

	video_width=0;
    video_height=0;
    video_aspect= (double) 4/3;

//#if PROGRAM_SWITCH
//	programs.clear();
//#endif
	videos.clear();
	audios.clear();
	titles.clear();

	subs.clear();

//	chapters.clear();

	n_chapters = 0;

	initialized=false;

	// Clip info;
	clip_name = "";
    clip_artist = "";
    clip_author = "";
    clip_album = "";
    clip_genre = "";
    clip_date = "";
    clip_track = "";
    clip_copyright = "";
    clip_comment = "";
    clip_software = "";

	stream_title = "";
	stream_url = "";

	// Other data
	demuxer="";
	video_format="";
	audio_format="";
	video_bitrate=0;
	video_fps="";
	audio_bitrate=0;
	audio_rate=0;
	audio_nch=0;
	video_codec="";
	audio_codec="";
}

QString MediaData::displayName(bool show_tag) {
	if (show_tag) {
		if (!stream_title.isEmpty()) return stream_title;
//		else
//		if (!clip_name.isEmpty()) return clip_name;//0526
	}

    //kobe:有的rmvb视频的clip_name存在乱码 0526
    QString name;
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
    return name;

//	QFileInfo fi(filename);
//	if (fi.exists())
//		return fi.fileName(); // filename without path
//	else
//		return filename;
}


void MediaData::list() {
//	qDebug("MediaData::list");

//	qDebug("  filename: '%s'", filename.toUtf8().data());
//	qDebug("  duration: %f", duration);

//	qDebug("  video_width: %d", video_width);
//	qDebug("  video_height: %d", video_height);
//	qDebug("  video_aspect: %f", video_aspect);

//	qDebug("  type: %d", type);
//	qDebug("  novideo: %d", novideo);
//	qDebug("  dvd_id: '%s'", dvd_id.toUtf8().data());

//	qDebug("  initialized: %d", initialized);

//	qDebug("  chapters: %d", n_chapters);

//	qDebug("  Subs:");
	subs.list();

//#if PROGRAM_SWITCH
//	qDebug("  Programs:");
//	programs.list();
//#endif
//	qDebug("  Videos:");
	videos.list();

//	qDebug("  Audios:");
	audios.list();

//	qDebug("  Titles:");
	titles.list();

	//qDebug("  chapters: %d", chapters);
	//qDebug("  angles: %d", angles);

    //kobe
//    qDebug("  demuxer: '%s'", demuxer.toUtf8().data() );//avi   real
//    qDebug("  video_format: '%s'", video_format.toUtf8().data() );//XVID  RV40
//    qDebug("  audio_format: '%s'", audio_format.toUtf8().data() );//85   cook
//    qDebug("  video_bitrate: %d", video_bitrate );//1113640    0
//    qDebug("  video_fps: '%s'", video_fps.toUtf8().data() );//25.000    23.000
//    qDebug("  audio_bitrate: %d", audio_bitrate );//224000   64080
//    qDebug("  audio_rate: %d", audio_rate );//44100   44100
//    qDebug("  audio_nch: %d", audio_nch );//2   2
//    qDebug("  video_codec: '%s'", video_codec.toUtf8().data() );//ffodivx    ffrv40
//    qDebug("  audio_codec: '%s'", audio_codec.toUtf8().data() );//mpg123     ffcook
}

