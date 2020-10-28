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

#include "extensions.h"
#include <QDebug>

ExtensionList::ExtensionList() : QStringList()
{
}

QString ExtensionList::forFilter() {
	QString s;
	for (int n=0; n < count(); n++) {
		s = s + "*." + at(n) + " ";
	}
	if (!s.isEmpty()) s = " (" + s + ")";
	return s;
}

QStringList ExtensionList::forDirFilter() {
	QStringList l;
	for (int n=0; n < count(); n++) {
		QString s = "*." + at(n);
		l << s;
	}
	return l;
}

QString ExtensionList::forRegExp() {
	QString s;
	for (int n=0; n < count(); n++) {
		if (!s.isEmpty()) s = s + "|";
		s = s + "^" + at(n) + "$";
	}
	return s;
}

Extensions::Extensions()
{
    _video << "avi" << "vfw" << "divx"
       << "mpg" << "mpeg" << "m1v" << "m2v" << "mpv" << "dv" << "3gp"
       << "mov" << "mp4" << "m4v" << "mqv"
       << "dat" << "vcd"
       << "ogg" << "ogm" << "ogv" << "ogx"
       << "asf" << "wmv"
       << "bin" << "iso" << "vob"
       << "mkv" << "nsv" << "ram" << "flv"
       << "rm" << "swf"
       << "ts" << "rmvb" << "dvr-ms" << "m2t" << "m2ts" << "mts" << "rec" << "wtv"
       << "f4v" << "hdmov" << "webm" << "vp8"
       << "bik" << "smk" << "m4b" << "wtv"
       << "part"
       << "x-flv" << "ogg" << "3gpp" << "x-mng"
       << "mp2t" << "dv" << "mp4v-es" << "x-ms-wmx"
       << "vnd.mpegurl" << "fli" << "x-ms-wm" << "x-ms-wvx" << "vnd.vivo"
       << "x-fli" << "x-flc" << "x-m4v" << "3gpp2" << "x-ogm+ogg"
       << "x-avi" << "msvideo" << "x-theora+ogg" << "x-flic" << "x-mpeg"
       << "x-mpeg2" << "x-nsv" << "x-anim" << "3gp" << "divx"
       << "vnd.divx" << "x-ms-asx" << "mpeg-system" << "x-ms-afs"
       << "x-ms-asf-plugin" << "x-ms-wvxvideo"
       << "vivo" << "x-mpeg-system" << "x-totem-stream" << "mediaplayer"
       << "mkv" << "x-mjpeg" << "x-mpeg3" << "x-ms-wmp";

    _audio << "mp3" << "ogg" << "oga" << "wav" << "wma" <<  "aac" << "ac3"
       << "dts" << "ra" << "ape" << "flac" << "thd" << "mka" << "m4a" << "opus"
       << "amr" << "x-aiff" << "basic" << "midi" << "amr-wb" << "x-realaudio"
       << "3gpp" << "x-pn-realaudio-plugin" << "x-gsm" << "x-ms-wax" << "prs.sid"
       << "x-musepack" << "x-ape" << "x-m4a" << "x-mod" << "x-s3m" << "x-flac"
       << "x-vorbis+ogg" << "x-wavpack" << "mp2" << "x-ms-asx" << "x-tta" << "x-mpeg"
       << "m4a" << "mp1" << "mp3" << "mpg" << "scpls" << "wav" << "webm" << "x-aac" << "x-it"
       << "x-mp1" << "x-mpg" << "x-ms-asf" << "x-pn-au" << "x-pn-wav" << "x-shorten" << "x-speex"
       << "x-xm" << "3gpp2" << "dv" << "eac3" << "opus" << "x-pn-aiff" << "x-pn-windows-acm"
       << "x-real-audio" << "vnd.dolby.heaac.1" << "vnd.dolby.heaac.2" << "vnd.dts" << "vnd.dts.hd"
       << "x-m4b" << "x-stm" << "m3u" << "rn-mpeg" << "vnd.dolby.mlp" << "x-pls"
       << "x-pn-windows-pcm" << "x-sbc" << "x-voc" << "aiff" << "mpeg2"
       << "mpeg3" << "musepack" << "x-flac+ogg" << "x-m3u" << "x-oggflac" << "x-opus+ogg" ;

    _subtitles << "srt" << "sub" << "ssa" << "ass" << "idx" << "txt" << "smi"
           << "rt" << "utf" << "aqt" << "vtt";

    _playlist << "m3u" << "m3u8" << "pls" << "xspf";

    _multimedia = _video;
    for (int n = 0; n < _audio.count(); n++) {
            if (!_multimedia.contains(_audio[n])) _multimedia << _audio[n];
    }

    _all_playable << _multimedia << _playlist;
}

Extensions::~Extensions() {
}

QString Extensions::extensionFromUrl(const QString & url) {
    //qDebug() << "Extensions::extensionFromUrl:" << url;

    QString extension;
    int pos = url.lastIndexOf(".");
    if (pos != -1) {
        extension = url.mid(pos+1).toLower();
        // Check if extension contains a '?' and remove everything after it
        pos = extension.lastIndexOf("?");
        if (pos != -1) {
            extension = extension.left(pos);
        }
    }

    //qDebug() << "Extensions::extensionFromUrl: extension:" << extension;
    return extension;
}
