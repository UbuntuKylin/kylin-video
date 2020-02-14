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

#include "videopreview.h"
#include "../utils.h"
#include "../smplayer/inforeader.h"
#include "../smplayer/images.h"

#include <QProcess>
#include <QRegExp>
#include <QDir>
#include <QTime>
#include <QApplication>
#include <QPixmapCache>
#include <QImageWriter>
#include <QImageReader>
#include <QDebug>
#include <cmath>

#define RENAME_PICTURES 0

#define N_OUTPUT_FRAMES 1

// MPlayer2 doesn't support png outdir
/* #define VP_USE_PNG_OUTDIR */
VideoPreview::VideoPreview(QString mplayer_path, QObject * parent) : QObject(parent)
{
	setMplayerPath(mplayer_path);

    save_last_directory = true;

	prop.input_video.clear();
    prop.initial_step = 0;//20
	prop.max_width = 800;
	prop.aspect_ratio = 0;
	prop.display_osd = true;
    prop.extract_format = JPEG;//PNG;

    output_dir = "kylin_video_preview";
	full_output_dir = QDir::tempPath() +"/"+ output_dir;
}

VideoPreview::~VideoPreview() {
    cleanDir(full_output_dir/*, true*/);
}

void VideoPreview::setMplayerPath(QString mplayer_path) {
	mplayer_bin = mplayer_path;

	QFileInfo fi(mplayer_bin);
	if (fi.exists() && fi.isExecutable() && !fi.isDir()) {
		mplayer_bin = fi.absoluteFilePath();
    }
}

QString VideoPreview::framePicture() {
	return QString("0000000%1.%2").arg(N_OUTPUT_FRAMES == 1 ? 1 : N_OUTPUT_FRAMES-1).arg(prop.extract_format == PNG ? "png" : "jpg");
}

bool VideoPreview::createPreThumbnail(int seek) {
	error_message.clear();
    bool result = extractImages(seek);
	if ((result == false) && (!error_message.isEmpty())) {
//        qDebug("The following error has occurred while creating the thumbnails:\n %s", error_message);
        qDebug() << "The following error has occurred while creating the thumbnails:" << error_message;
	}
    //如果这里执行cleanDir，将会导致缩略图还没显示，图片就已经被删除了
    //cleanDir(full_output_dir);
	return result;
}

bool VideoPreview::extractImages(int seek) {
    //删除文件夹下原来的缩略图
    QDir dir(full_output_dir);
    if (dir.exists()) {
        dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
        QFileInfoList fileList = dir.entryInfoList();
        foreach (QFileInfo file, fileList){
            if (file.exists() && file.isFile()) {
                file.dir().remove(file.fileName());//QFile::remove(file.fileName());
            }
        }
    }

//    qDebug() << "VideoPreview::extractImages time=" << time;
	VideoInfo i = getInfo(mplayer_bin, prop.input_video);
	int length = i.length;

	if (length == 0) {
        if (error_message.isEmpty()) error_message = QString(tr("The length of the video is 0"));
		return false;
	}

	// Create a temporary directory
	QDir d(QDir::tempPath());
	if (!d.exists(output_dir)) {
		if (!d.mkpath(output_dir)) {
			qDebug("VideoPreview::extractImages: error: can't create '%s'", full_output_dir.toUtf8().constData());
            error_message = QString(tr("The temporary directory (%1) can't be created").arg(full_output_dir));
			return false;
		}
	}

    //在此处调用cleanDir，将导致在FT平台上预览视频时报错即QProcess会执行失败
    //cleanDir(full_output_dir/*, false*/);

    prop.initial_step = seek;
    int current_time = prop.initial_step;

	double aspect_ratio = i.aspect;
	if (prop.aspect_ratio != 0) aspect_ratio = prop.aspect_ratio;

    qApp->processEvents();

    if (!runPlayer(current_time, aspect_ratio)) return false;

    QString frame_picture = full_output_dir + "/" + framePicture();
    if (!QFile::exists(frame_picture)) {
        error_message = QString(tr("The file %1 doesn't exist").arg(frame_picture));
        return false;
    }

    QString extension = (extractFormat()==PNG) ? "png" : "jpg";
    QString output_file = output_dir + QString("/picture_%1.%2").arg(current_time, 8, 10, QLatin1Char('0')).arg(extension);

    d.rename(output_dir + "/" + framePicture(), output_file);
    current_picture = QDir::tempPath() + "/" + output_file;

	return true;
}


//#if defined(Q_OS_LINUX) && !defined(NO_SMPLAYER_SUPPORT)
bool VideoPreview::isOptionAvailableinMPV(const QString & option) {
    static QStringList option_list;
    static QString executable;

    if (option_list.isEmpty() || executable != mplayer_bin) {
        InfoReader * ir = InfoReader::obj(mplayer_bin);
        ir->getInfo();
        option_list = ir->optionList();
        executable = mplayer_bin;
    }
    return option_list.contains(option);
}
//#endif

/*参数解释：-ss 指定开始时间
  -noframedrop 不跳过帧（即使解码速度跟不上）
  -nosound  没有声音输出（不对声音进行解码）
  -nolirc  禁用红外控制
  -nojoystick  禁用控制棒
  -vo  视频输出格式为jpeg
  -frames 从ss指定的时间开始截取多少帧
*/
bool VideoPreview::runPlayer(int seek, double aspect_ratio) {
	QStringList args;

    //edited by kobe 20180623
    if (Utils::player(mplayer_bin/*, this->m_snap*/) == Utils::MPV) {
		// MPV
        /*args << "--no-config" << "--no-audio" << "--no-cache";
		args << "--frames=" + QString::number(N_OUTPUT_FRAMES);
		args << "--framedrop=no" << "--start=" + QString::number(seek);
		if (aspect_ratio != 0) {
			args << "--video-aspect=" + QString::number(aspect_ratio);
		}
		QString format = (prop.extract_format == PNG) ? "png:png-compression=0" : "jpg";
        args << QString("--vo=image=format=%1:outdir=\"%2\"").arg(format).arg(full_output_dir);*/


        bool use_new_options = true;
        if (!isOptionAvailableinMPV("--vo-image-format")) use_new_options = false;
        // MPV
        args << "--no-config" << "--no-audio" << "--no-cache" << "--hr-seek=yes" << "--sid=no";
        args << "--frames=" + QString::number(N_OUTPUT_FRAMES);
        args << "--framedrop=no" << "--start=" + QString::number(seek);
        if (aspect_ratio != 0) {
            args << "--video-aspect=" + QString::number(aspect_ratio);
        }
        if (!prop.dvd_device.isEmpty()) args << "--dvd-device=" + prop.dvd_device;
        if (!use_new_options) {
            QString format = (prop.extract_format == PNG) ? "png:png-compression=0" : "jpg";
            args << QString("--vo=image=format=%1:outdir=\"%2\"").arg(format).arg(full_output_dir);
        } else {
            QString format = (prop.extract_format == PNG) ? "png" : "jpg";
            args << "--vo-image-format=" + format << "--vo-image-outdir=" + full_output_dir;
            args << "--vo=image";
            if (prop.extract_format == PNG) args << "--vo-image-png-compression=0";
        }
	}
	else {
		// MPlayer
		args << "-nosound" << "-nocache" << "-noframedrop";

		if (prop.extract_format == PNG) {
            args << "-vo" << "png";
		} else {
            args << "-vo" << "jpeg:outdir=\""+full_output_dir+"\"";
		}

		args << "-frames" << QString::number(N_OUTPUT_FRAMES) << "-ss" << QString::number(seek);

		if (aspect_ratio != 0) {
			args << "-aspect" << QString::number(aspect_ratio) << "-zoom";
		}

        if (!prop.dvd_device.isEmpty()) {
            args << "-dvd-device" << prop.dvd_device;
        }
	}

	args << prop.input_video;

    /*QString command = mplayer_bin + " ";
    for (int n = 0; n < args.count(); n++) command = command + args[n] + " ";*/
    //QString command = mplayer_bin + " " + args.join(" ");
    //qDebug("VideoPreview::runMplayer: command: %s", command.toUtf8().constData());///usr/bin/mplayer -nosound -nocache -noframedrop -vo jpeg:outdir="/tmp/kylin_video_preview" -frames 1 -ss 77 -aspect 1.7778 -zoom /home/lixiang/resources/1080.wmv
    //VideoPreview::runMplayer: command: /usr/bin/mplayer -nosound -nocache -noframedrop -vo jpeg:outdir="/tmp/kylin_video_preview" -frames 1 -ss 470 -aspect 1.33333 -zoom /home/lixiang/resources/Katy Perry Roar.swf
	QProcess p;
	p.setWorkingDirectory(full_output_dir);
	p.start(mplayer_bin, args);
    if (!p.waitForStarted()) {
        qDebug("VideoPreview::runMplayer: error: the process didn't start");
    }
	if (!p.waitForFinished()) {
        qDebug() << "VideoPreview::runMplayer: error running process";
        if (Utils::player(mplayer_bin/*, this->m_snap*/) == Utils::MPV) {
            error_message = QString(tr("The mpv process didn't run"));
        }
        else {
            error_message = QString(tr("The mplayer process didn't run"));
        }
		return false;
	}

	return true;
}

void VideoPreview::cleanDir(QString directory) {
    QStringList filter;
    if (prop.extract_format == PNG) {
        filter.append("*.png");
    } else {
        filter.append("*.jpg");
    }

    QDir d(directory);
    QStringList l = d.entryList( filter, QDir::Files, QDir::Unsorted);

    for (int n = 0; n < l.count(); n++) {
        //qDebug("VideoPreview::cleanDir: deleting '%s'", l[n].toUtf8().constData());
        d.remove(l[n]);
    }
    //qDebug("VideoPreview::cleanDir: removing directory '%s'", directory.toUtf8().constData());
    d.rmpath(directory);
}

VideoInfo VideoPreview::getInfo(const QString & mplayer_path, const QString & filename) {
	VideoInfo i;

	if (filename.isEmpty()) {
        error_message = QString(tr("No filename"));
		return i;
	}

	QFileInfo fi(filename);
	if (fi.exists()) {
		i.filename = fi.fileName();
		i.size = fi.size();
	}

	QRegExp rx("^ID_(.*)=(.*)");

	QProcess p;
	p.setProcessChannelMode( QProcess::MergedChannels );

	QStringList args;

    //edited by kobe 20180623
    if (Utils::player(mplayer_path/*, this->m_snap*/) == Utils::MPV) {
//		args << "--term-playing-msg="
//                "ID_LENGTH=${=length}\n"
//                "ID_VIDEO_WIDTH=${=width}\n"
//                "ID_VIDEO_HEIGHT=${=height}\n"
//                "ID_VIDEO_FPS=${=fps}\n"
//                "ID_VIDEO_ASPECT=${=video-aspect}\n"
//                "ID_VIDEO_BITRATE=${=video-bitrate}\n"
//                "ID_AUDIO_BITRATE=${=audio-bitrate}\n"
//                "ID_AUDIO_RATE=${=audio-samplerate}\n"
//                "ID_VIDEO_FORMAT=${=video-format}";

        args << "--term-playing-msg="
                "ID_LENGTH=${=duration:${=length}}\n"
                "ID_VIDEO_WIDTH=${=width}\n"
                "ID_VIDEO_HEIGHT=${=height}\n"
                "ID_VIDEO_FPS=${=container-fps:${=fps}}\n"
                "ID_VIDEO_ASPECT=${=video-aspect}\n"
                "ID_VIDEO_BITRATE=${=video-bitrate}\n"
                "ID_AUDIO_BITRATE=${=audio-bitrate}\n"
                "ID_AUDIO_RATE=${audio-params/samplerate:${=audio-samplerate}}\n"
                "ID_VIDEO_FORMAT=${video-format}\n"
                "ID_AUDIO_FORMAT=${audio-codec-name}\n";

		args << "--vo=null" << "-ao=null" << "--frames=1" << "--no-quiet" << "--no-cache" << "--no-config";
        if (!prop.dvd_device.isEmpty()) args << "--dvd-device=" + prop.dvd_device;
        args << filename;
	}
	else {
		// MPlayer
		args << "-vo" << "null" << "-ao" << "null" << "-frames" << "1" << "-identify" << "-nocache" << "-noquiet";
        if (!prop.dvd_device.isEmpty()) args << "-dvd-device" << prop.dvd_device;
        args << filename;
	}

//    QString command = mplayer_path + " " + args.join(" ").replace("\n", "\\n");//.replace("$", "\\$");
//	qDebug() << "VideoPreview::getInfo: command:" << command;
	p.start(mplayer_path, args);

	if (p.waitForFinished()) {
		QByteArray line;
		while (p.canReadLine()) {
			line = p.readLine().trimmed();
//			qDebug("VideoPreview::getInfo: '%s'", line.constData());
			if (rx.indexIn(line) > -1) {
				QString tag = rx.cap(1);
				QString value = rx.cap(2);
//				qDebug("VideoPreview::getInfo: tag: '%s', value: '%s'", tag.toUtf8().constData(), value.toUtf8().constData());

				if (tag == "LENGTH") i.length = (int) value.toDouble();
				else
				if (tag == "VIDEO_WIDTH") i.width = value.toInt();
				else
				if (tag == "VIDEO_HEIGHT") i.height = value.toInt();
				else
				if (tag == "VIDEO_FPS") i.fps = value.toDouble();
				else
				if (tag == "VIDEO_ASPECT") {
					i.aspect = value.toDouble();
					if ((i.aspect == 0) && (i.width != 0) && (i.height != 0)) {
						i.aspect = (double) i.width / i.height;
					}
				}
				else
				if (tag == "VIDEO_BITRATE") i.video_bitrate = value.toInt();
				else
				if (tag == "AUDIO_BITRATE") i.audio_bitrate = value.toInt();
				else
				if (tag == "AUDIO_RATE") i.audio_rate = value.toInt();
				else
				if (tag == "VIDEO_FORMAT") i.video_format = value;
                else
                if (tag == "AUDIO_FORMAT") i.audio_format = value;
			}
		}
	} else {
		qDebug("VideoPreview::getInfo: error: process didn't start");
        error_message = QString(tr("The mplayer process didn't start while trying to get info about the video"));
	}

	return i;
}

