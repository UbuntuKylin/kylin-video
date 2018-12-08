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

#include "infofile.h"
#include <QFileInfo>
#include <QCoreApplication>
#include <QObject>
#include <QString>
#include "images.h"

InfoFile::InfoFile() {
	row = 0;
}

InfoFile::~InfoFile() {
}

QString InfoFile::getInfo(MediaData md) {
    QString s;

	// General
    QFileInfo fi(md.m_filename);//20181201  m_filename

	QString icon;
	switch (md.type) {
		case TYPE_FILE	:	if (md.novideo) 
								icon = "type_audio.png";
							else
								icon = "type_video.png"; 
							break;
		case TYPE_DVD	: 	icon = "type_dvd.png"; break;
		case TYPE_VCD	: 	icon = "type_vcd.png"; break;
		case TYPE_AUDIO_CD	: 	icon = "type_vcd.png"; break;
		case TYPE_TV	: 	icon = "type_tv.png"; break;
		case TYPE_STREAM : 	icon = "type_url.png"; break;
//#ifdef BLURAY_SUPPORT
//		case TYPE_BLURAY : 	icon = "type_bluray.png"; break;
//#endif
		default 		: 	icon = "type_unknown.png";
	}
	icon = icon.replace(".png", ""); // FIXME
	icon = "<img src=\"" + Images::file(icon) + "\"> ";

//#ifdef BLURAY_SUPPORT
//	if (md.type == TYPE_DVD || md.type == TYPE_BLURAY)
//#else
//	if (md.type == TYPE_DVD)
////#endif
//	{
//		DiscData disc_data = DiscName::split(md.filename);
//		s += title( icon + disc_data.protocol + "://" + QString::number(disc_data.title) );
//	} else {
//		s += title( icon + md.displayName() );
//	}

//    s += openPar(QObject::tr("General"));
    s += openPar(kylin_tr("General"));
    if (fi.exists()) {
        //s += addItem( kylin_tr("Path"), fi.dirPath());
        s += addItem(kylin_tr("File"), fi.absoluteFilePath());
        s += addItem(kylin_tr("Size"), kylin_tr("%1 KB (%2 MB)").arg(fi.size()/1024).arg(fi.size()/1048576));
    } else {
        QString url = md.m_filename;//20181201  m_filename
        s += addItem(kylin_tr("URL"), url);
    }
    s += addItem(kylin_tr("Length"), Helper::formatTime((int)md.duration));
    s += addItem(kylin_tr("Demuxer"), md.demuxer);
    s += closePar();

	// Clip info
	QString c;
    if (!md.clip_name.isEmpty()) c+= addItem( kylin_tr("Name"), md.clip_name );
//    if (!m.isEmpty()) c+= addItem( kylin_tr("Name"), md.clip_name );
    if (!md.clip_artist.isEmpty()) c+= addItem( kylin_tr("Artist"), md.clip_artist );
    if (!md.clip_author.isEmpty()) c+= addItem( kylin_tr("Author"), md.clip_author );
    if (!md.clip_album.isEmpty()) c+= addItem( kylin_tr("Album"), md.clip_album );
    if (!md.clip_genre.isEmpty()) c+= addItem( kylin_tr("Genre"), md.clip_genre );
    if (!md.clip_date.isEmpty()) c+= addItem( kylin_tr("Date"), md.clip_date );
    if (!md.clip_track.isEmpty()) c+= addItem( kylin_tr("Track"), md.clip_track );
    if (!md.clip_copyright.isEmpty()) c+= addItem( kylin_tr("Copyright"), md.clip_copyright );
    if (!md.clip_comment.isEmpty()) c+= addItem( kylin_tr("Comment"), md.clip_comment );
    if (!md.clip_software.isEmpty()) c+= addItem( kylin_tr("Software"), md.clip_software );
    if (!md.stream_title.isEmpty()) c+= addItem( kylin_tr("Stream title"), md.stream_title );
    if (!md.stream_url.isEmpty()) c+= addItem( kylin_tr("Stream URL"), md.stream_url );

	if (!c.isEmpty()) {
        s += openPar( kylin_tr("Clip info") );
		s += c;
		s += closePar();
    }

	// Video info
    if (!md.novideo) {
        s += openPar( kylin_tr("Video") );
        s += addItem( kylin_tr("Resolution"), QString("%1 x %2").arg(md.video_width).arg(md.video_height) );
        s += addItem( kylin_tr("Aspect ratio"), QString::number(md.video_aspect) );
        s += addItem( kylin_tr("Format"), md.video_format );
        s += addItem( kylin_tr("Bitrate"), kylin_tr("%1 kbps").arg(md.video_bitrate / 1000) );
        s += addItem( kylin_tr("Frames per second"), md.video_fps );
        s += addItem( kylin_tr("Selected codec"), md.video_codec );
		s += closePar();
	}

	// Audio info
    s += openPar( kylin_tr("Initial Audio Stream") );
    s += addItem( kylin_tr("Format"), md.audio_format );
    s += addItem( kylin_tr("Bitrate"), kylin_tr("%1 kbps").arg(md.audio_bitrate / 1000) );
    s += addItem( kylin_tr("Rate"), kylin_tr("%1 Hz").arg(md.audio_rate) );
    s += addItem( kylin_tr("Channels"), QString::number(md.audio_nch) );
    s += addItem( kylin_tr("Selected codec"), md.audio_codec );
    s += closePar();

    //TODO
    /*
	// Audio Tracks
    if (md.audios.numItems() > 0) {
        s += openPar( kylin_tr("Audio Streams") );
        row++;
//		s += openItem();//kobe 20170627
        s += "<td>" + kylin_tr("#", "Info for translators: this is a abbreviation for number") + "</td><td>" +
              kylin_tr("Language") + "</td><td>" + kylin_tr("Name") +"</td><td>" +
              kylin_tr("ID", "Info for translators: this is a identification code") + "</td>";
        s += closeItem();
        for (int n = 0; n < md.audios.numItems(); n++) {
            row++;
//			s += openItem();//kobe 20170627
            QString lang = md.audios.itemAt(n).lang();
            if (lang.isEmpty()) lang = "<i>&lt;"+kylin_tr("empty")+"&gt;</i>";
            QString name = md.audios.itemAt(n).name();
            if (name.isEmpty()) name = "<i>&lt;"+kylin_tr("empty")+"&gt;</i>";
            s += QString("<td>%1</td><td>%2</td><td>%3</td><td>%4</td>")
                 .arg(n).arg(lang).arg(name)
                 .arg(md.audios.itemAt(n).ID());
            s += closeItem();
        }
        s += closePar();
    }

	// Subtitles
	if (md.subs.numItems() > 0) {
        s += openPar( kylin_tr("Subtitles") );
		row++;
//		s += openItem();//kobe 20170627
        s += "<td>" + kylin_tr("#", "Info for translators: this is a abbreviation for number") + "</td><td>" +
              kylin_tr("Type") + "</td><td>" +
              kylin_tr("Language") + "</td><td>" + kylin_tr("Name") +"</td><td>" +
              kylin_tr("ID", "Info for translators: this is a identification code") + "</td>";
		s += closeItem();
		for (int n = 0; n < md.subs.numItems(); n++) {
			row++;
//			s += openItem();//kobe 20170627
			QString t;
			switch (md.subs.itemAt(n).type()) {
				case SubData::File: t = "FILE_SUB"; break;
				case SubData::Vob:	t = "VOB"; break;
				default:			t = "SUB";
			}
			QString lang = md.subs.itemAt(n).lang();
            if (lang.isEmpty()) lang = "<i>&lt;"+kylin_tr("empty")+"&gt;</i>";
			QString name = md.subs.itemAt(n).name();
            if (name.isEmpty()) name = "<i>&lt;"+kylin_tr("empty")+"&gt;</i>";
//			s += QString("<td>%1</td><td>%2</td><td>%3</td><td>%4</td><td>%5</td>")
//                 .arg(n).arg(t).arg(lang).arg(name)
//                 .arg(md.subs.itemAt(n).ID());
            s += "<td>" + QString::number(n) + "</td><td>" + t + 
                 "</td><td>" + lang + "</td><td>" + name + 
                 "</td><td>" + QString::number(md.subs.itemAt(n).ID()) + "</td>";
			s += closeItem();
		}
		s += closePar();
	}
    */

    return "<html><body><font font-size:12px;color=\"#999999\">"+ s + "</font></body></html>";
}

QString InfoFile::title(QString text) {
	return "<h1>" + text + "</h1>";
}

QString InfoFile::openPar(QString text) {
    //kobe 20170627
    return "<h2><font color=#999999>" + text + "</font></h2>"
            "<table width=\"100%\" cellpadding=0 cellspacing=0>";
//    return "<h2>" + text + "</h2>"
//           "<table width=\"100%\">";
}

QString InfoFile::closePar() {
	row = 0;
	return "</table>";
}

QString InfoFile::openItem() {
    //kobe 20170627
    return "<tr bgcolor=\"#171717\">";//height="100" bgColor="red"
//	if (row % 2 == 1)
//		return "<tr bgcolor=\"lavender\">";
//	else
//		return "<tr bgcolor=\"powderblue\">";
}

QString InfoFile::closeItem() {
	return "</tr>";
}

QString InfoFile::addItem( QString tag, QString value ) {
	row++;
    return openItem() +
           "<td><b>" + tag + "</b></td>" +
           "<td>" + value + "</td>" +
           closeItem();// height=\"100px\"
}

/*
QString QCoreApplication::translate (const char * context, const char * sourceText, const char * disambiguation, Encoding encoding, int n )

其实，这个才是真正进行翻译操作的函数，前面我们提到的tr最终是通过调用该函数来实现翻译功能的(稍后我们会看tr是如何调用translate的)。
对tr和这个函数，manual中都有比较详尽的解释。我们这儿简单看一下它的这几个参数：
    context 上下文，一般就是需要翻译的字符串所在的类的名字
    sourceText 需要翻译的字符串。(我们关注的编码其实就是它的编码)
    disambiguation 消除歧义用的。(比如我们的类内出现两处"close"，一处含义是关闭，另一处含义是亲密的。显然需要让翻译人员知道这点区别)
    encoding 指定编码。它有两个值

        CodecForTr 使用setCodecForTr()设置的编码来解释 sourceText
        UnicodeUTF8 使用utf8编码来解释 sourceText
        其实这两个分别对应tr和trUtf8
    n 处理单复数(对中文来说，不存在这个问题)
*/
inline QString InfoFile::kylin_tr(const char * sourceText, const char * comment, int n)  {
#if QT_VERSION >= 0x050000
    return QCoreApplication::translate("InfoFile", sourceText, comment, n );
#else
    return QCoreApplication::translate("InfoFile", sourceText, comment, QCoreApplication::CodecForTr, n );
#endif
}

