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

#include "preferences.h"
#include "global.h"
#include "paths.h"
#include "mediasettings.h"
#include "recents.h"
#include "urlhistory.h"
#include "filters.h"
#include "helper.h"

#include <QSettings>
#include <QFileInfo>
#include <QRegExp>
#include <QDir>
#include <QLocale>
#include <QNetworkProxy>

#if QT_VERSION >= 0x050000
#include <QStandardPaths>
#endif

#if QT_VERSION >= 0x040400
#include <QDesktopServices>
#endif

using namespace Global;

Preferences::Preferences(const QString &arch) {
    arch_type = arch;
    history_recents = new Recents;
    history_urls = new URLHistory;
    filters = new Filters;
	reset();
	load();
}

Preferences::~Preferences() {
	save();
    delete history_recents;
    delete history_urls;
    delete filters;
}

void Preferences::reset() {
    //edited by kobe 20180623
    mplayer_bin = "/usr/bin/mpv";//mplayer_bin = QString("%1/mpv").arg(Paths::appPath());
    if (arch_type == "aarch64") {//kobe 20180612
        vo = "vdpau";
    }
    else {
        vo = "xv";
    }
    ao = "pulse";//ao = "";
	use_screenshot = true;
	screenshot_template = "cap_%F_%p_%02n";
	screenshot_format = "jpg";
	screenshot_directory="";

	#if QT_VERSION < 0x040400
	QString default_screenshot_path = Paths::configPath() + "/screenshots";
	if (QFile::exists(default_screenshot_path)) {
		screenshot_directory = default_screenshot_path;
	}
	#endif

    remember_media_settings = true;
    remember_time_pos = true;
    remember_stream_settings = true;

    alang = "";
    slang = "";

	use_direct_rendering = false;
	use_double_buffer = true;

	use_soft_video_eq = false;
	use_slices = false;

    vdpau.ffh264vdpau = true;
    vdpau.ffmpeg12vdpau = true;
    vdpau.ffwmv3vdpau = true;
    vdpau.ffvc1vdpau = true;
    vdpau.ffodivxvdpau = false;
    vdpau.ffhevcvdpau = false;
    vdpau.disable_video_filters = true;//true;//kobe 20180612

	use_soft_vol = true;
	softvol_max = 110; // 110 = default value in mplayer
    use_scaletempo = Detect;
    use_hwac3 = false;
    use_audio_equalizer = false;

	global_volume = true;
	volume = 50;
	mute = false;

    global_audio_equalizer = true;
    audio_equalizer << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0; // FIXME: use initial_audio_equalizer (but it's set later)

    autosync = false;
    autosync_factor = 30;//调整音视频同步,帮助文件指出 置为30为最佳

    autoload_m4a = true;
	min_step = 4;

    osd = Seek;
    osd_scale = 1;
    subfont_osd_scale = 3;
    osd_delay = 5000;
//#ifdef MPV_SUPPORT
    osd_fractions = false;
//#endif
    osd_bar_pos = 80;
    osd_show_filename_duration = 2000;

    //kobe:pref->file_settings_method 记住时间位置的配置设置在一个ini文件时为normal，在多个ini文件时为hash
//	file_settings_method = "hash"; // Possible values: normal & hash

    /* ***********
       Performance
       *********** */
//    frame_drop = false;
//	hard_frame_drop = false;
    coreavc = false;
    h264_skip_loop_filter = LoopEnabled;
	HD_height = 720;
    threads = 4;
    hwdec = "auto";//hwdec = "no";
	cache_for_files = 2048;
	cache_for_streams = 2048;


    cache_auto = true;
//    cache_for_dvds = 0; // not recommended to use cache for dvds
//    cache_for_vcds = 1024;
//    cache_for_audiocds = 1024;

    /* *********
       Subtitles
       ********* */
    use_ass_subtitles = true;
    enable_ass_styles = true;
    ass_line_spacing = 0;
    use_forced_subs_only = false;


    subcp = "UTF-8";//"ISO-8859-1";
	use_enca = false;
	enca_lang = QString(QLocale::system().name()).section("_",0,0);
	sub_visibility = true;
    subfuzziness = 1;
    autoload_sub = true;

    subtitles_on_screenshots = false;
    force_ass_styles = false;
    user_forced_ass_style.clear();
    freetype_support = true;

    /* ********
       Advanced
       ******** */
//#if USE_ADAPTER
    adapter = -1;
//#endif
	use_idx = false;
	use_lavf_demuxer = false;

    mplayer_additional_options="";
//	#if defined(PORTABLE_APP) && defined(FONTS_HACK)
//	mplayer_additional_options="-nofontconfig";
//	#endif
    mplayer_additional_video_filters="";
    mplayer_additional_audio_filters="";

    log_mplayer = true;
    verbose_log = false;
//    autosave_mplayer_log = false;
//    mplayer_log_saveto = "";
    log_smplayer = true;
    log_filter = ".*";
    save_smplayer_log = false;
	use_edl_files = true;

//#ifdef MPLAYER_SUPPORT
    use_playlist_option = false;
//#endif

	prefer_ipv4 = true;
	use_short_pathnames = false;
	change_video_equalizer_on_startup = true;
	use_pausing_keep_force = true;
	use_correct_pts = Detect;
	actions_to_run = "";
	show_tag_in_window_title = true;
	time_to_kill_mplayer = 1000;

//#ifdef MPV_SUPPORT
    mpv_osd_media_info = "";
//#endif
//#ifdef MPLAYER_SUPPORT
    mplayer_osd_media_info = "";
//#endif
//#ifdef MPV_SUPPORT
    emulate_mplayer_ab_section = false;
//#endif

    /* *********
       GUI stuff
       ********* */
	fullscreen = false;
    stay_on_top = NeverOnTop;
    play_order = OrderPlay;
//    resize_method = Afterload;//kobe: Never;
    resize_method = Never;//0620 kobe

	wheel_function = Seeking;
	wheel_function_cycle = Seeking | Volume | Zoom | ChangeSpeed;
	wheel_function_seeking_reverse = false;

	seeking1 = 10;
	seeking2 = 60;
	seeking3 = 10*60;
	seeking4 = 30;

	time_slider_drag_delay = 100;

    pause_when_hidden = true;
    preview_when_playing = true;

	media_to_add_to_playlist = NoFiles;

    playlist_key = "F3";
    prev_key = "<, Media Previous";
    next_key = ">, Media Next";

    /* ***********
       Directories
       *********** */
	latest_dir = QDir::homePath();

    /* **************
       Initial values
       ************** */
    initial_sub_scale = 5;
    initial_sub_scale_ass = 1;
    initial_volume = 40;
    initial_contrast = 0;
    initial_brightness = 0;
    initial_hue = 0;
    initial_saturation = 0;
    initial_gamma = 0;

    initial_audio_equalizer << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0;

    initial_zoom_factor = 1.0;
    initial_sub_pos = 100; // 100%

//#ifdef INITIAL_BLACKBORDERS
    initial_blackborders = false;
//#endif

    initial_postprocessing = false;
    initial_volnorm = false;

    initial_deinterlace = MediaSettings::NoDeinterlace;

    initial_audio_channels = MediaSettings::ChStereo;//MediaSettings::ChDefault;
    initial_stereo_mode = MediaSettings::Stereo;

//#if SELECT_TRACKS_ON_STARTUP
    initial_audio_track = 0;
    initial_subtitle_track = 0;
//#endif

    /* ************
       MPlayer info
       ************ */
	mplayer_detected_version = -1; //None version parsed yet
	mplayer_user_supplied_version = -1;

    /* *******
       History
       ******* */
	history_recents->clear();
	history_urls->clear();

    /* *******
       Filters
       ******* */

    filters->init();
}

void Preferences::save() {
//    qDebug("Preferences::save");
	QSettings * set = settings;


    /* *******
       General
       ******* */

	set->beginGroup("General");
	set->setValue("mplayer_bin", mplayer_bin);
	set->setValue("driver/vo", vo);
	set->setValue("driver/audio_output", ao);
	set->setValue("use_screenshot", use_screenshot);
	set->setValue("screenshot_template", screenshot_template);
	set->setValue("screenshot_format", screenshot_format);
    //qDebug() << "save screenshot_directory="<< screenshot_directory;
	#if QT_VERSION >= 0x040400
	set->setValue("screenshot_folder", screenshot_directory);
	#else
	set->setValue("screenshot_directory", screenshot_directory);
	#endif

	set->setValue("use_direct_rendering", use_direct_rendering);
	set->setValue("use_double_buffer", use_double_buffer);
	set->setValue("use_soft_video_eq", use_soft_video_eq);
	set->setValue("use_slices", use_slices );
	set->setValue("use_soft_vol", use_soft_vol);
	set->setValue("softvol_max", softvol_max);
    set->setValue("use_scaletempo", use_scaletempo);
    set->setValue("use_hwac3", use_hwac3 );
    set->setValue("use_audio_equalizer", use_audio_equalizer );

	set->setValue("global_volume", global_volume);
	set->setValue("volume", volume);
    set->setValue("mute", mute);
    set->setValue("global_audio_equalizer", global_audio_equalizer);
    set->setValue("audio_equalizer", audio_equalizer);
    set->setValue("autosync", autosync);
    set->setValue("autosync_factor", autosync_factor);
    set->setValue("min_step", min_step);

    set->setValue("osd", osd);
    set->setValue("osd_scale", osd_scale);
    set->setValue("subfont_osd_scale", subfont_osd_scale);
    set->setValue("osd_delay", osd_delay);
//#ifdef MPV_SUPPORT
    set->setValue("osd_fractions", osd_fractions);
//#endif
    set->setValue("osd_bar_pos", osd_bar_pos);
    set->setValue("osd_show_filename_duration", osd_show_filename_duration);

	set->endGroup(); // General

    /* ***********
       Performance
       *********** */
	set->beginGroup( "performance");
    set->setValue("HD_height", HD_height);
	set->setValue("threads", threads);
	set->setValue("hwdec", hwdec);
	set->setValue("cache_for_files", cache_for_files);
	set->setValue("cache_for_streams", cache_for_streams);
	set->endGroup(); // performance

    /* *********
       Subtitles
       ********* */
	set->beginGroup("subtitles");
	set->setValue("subcp", subcp);
	set->setValue("use_enca", use_enca);
	set->setValue("enca_lang", enca_lang);
	set->setValue("sub_visibility", sub_visibility);
	set->endGroup(); // subtitles

    /* ********
       Advanced
       ******** */
	set->beginGroup( "advanced");
	set->setValue("use_idx", use_idx);
	set->setValue("use_lavf_demuxer", use_lavf_demuxer);
	set->setValue("use_edl_files", use_edl_files);
//#ifdef MPLAYER_SUPPORT
    set->setValue("use_playlist_option", use_playlist_option);
//#endif
	set->setValue("use_short_pathnames", use_short_pathnames);
	set->setValue("actions_to_run", actions_to_run);
	set->setValue("show_tag_in_window_title", show_tag_in_window_title);
	set->setValue("time_to_kill_mplayer", time_to_kill_mplayer);

//#ifdef MPV_SUPPORT
    set->setValue("mpv_osd_media_info", mpv_osd_media_info);
//#endif
//#ifdef MPLAYER_SUPPORT
    set->setValue("mplayer_osd_media_info", mplayer_osd_media_info);
//#endif


	set->endGroup(); // advanced

    /* *********
       GUI stuff
       ********* */
	set->beginGroup("gui");
	set->setValue("fullscreen", fullscreen);
    set->setValue("stay_on_top", (int) stay_on_top);
	set->setValue("mouse_wheel_function", wheel_function);
	set->setValue("wheel_function_cycle", (int) wheel_function_cycle);
	set->setValue("wheel_function_seeking_reverse", wheel_function_seeking_reverse);
	set->setValue("seeking1", seeking1);
	set->setValue("seeking2", seeking2);
	set->setValue("seeking3", seeking3);
	set->setValue("seeking4", seeking4);
	set->setValue("time_slider_drag_delay", time_slider_drag_delay);
    set->setValue("pause_when_hidden", pause_when_hidden);
    set->setValue("preview_when_playing", preview_when_playing);
	set->setValue("media_to_add_to_playlist", media_to_add_to_playlist);
    set->setValue("playlist_key", playlist_key);
    set->setValue("next_key", next_key);
    set->setValue("prev_key", prev_key);
	set->endGroup(); // gui


    /* **************
       Initial values
       ************** */
	set->beginGroup( "defaults");
    set->setValue("latest_dir", latest_dir);

    set->setValue("initial_sub_scale", initial_sub_scale);
    set->setValue("initial_sub_scale_ass", initial_sub_scale_ass);
    set->setValue("initial_volume", initial_volume);
    set->setValue("initial_contrast", initial_contrast);
    set->setValue("initial_brightness", initial_brightness);
    set->setValue("initial_hue", initial_hue);
    set->setValue("initial_saturation", initial_saturation);
    set->setValue("initial_gamma", initial_gamma);

    set->setValue("initial_audio_equalizer", initial_audio_equalizer);

    set->setValue("initial_zoom_factor", initial_zoom_factor);
    set->setValue("initial_sub_pos", initial_sub_pos);

    set->setValue("initial_volnorm", initial_volnorm);

//#ifdef INITIAL_BLACKBORDERS
    set->setValue("initial_blackborders", initial_blackborders);
//#endif

    set->setValue("initial_postprocessing", initial_postprocessing);

    set->setValue("initial_deinterlace", initial_deinterlace);

    set->setValue("audio_channels", initial_audio_channels);
    set->setValue("initial_stereo_mode", initial_stereo_mode);

//#if SELECT_TRACKS_ON_STARTUP
    set->setValue("preferred_audio_track", initial_audio_track);
    set->setValue("preferred_subtitle_track", initial_subtitle_track);
//#endif


	set->endGroup(); // defaults

    /* ************
       MPlayer info
       ************ */
	set->beginGroup( "mplayer_info");
	set->setValue("mplayer_detected_version", mplayer_detected_version);
	set->setValue("mplayer_user_supplied_version", mplayer_user_supplied_version);
	set->endGroup(); // mplayer_info

    /* *******
       History
       ******* */
	set->beginGroup("history");
	set->setValue("recents", history_recents->toStringList());
	set->setValue("recents/max_items", history_recents->maxItems());
	set->setValue("urls", history_urls->toStringList());
	set->setValue("urls/max_items", history_urls->maxItems());
	set->endGroup(); // history

    /* *******
       Filters
       ******* */

    filters->save(set);

	set->sync();
}

void Preferences::load() {
    //qDebug("Preferences::load");

	QSettings * set = settings;

    /* *******
       General
       ******* */
	set->beginGroup("General");
	mplayer_bin = set->value("mplayer_bin", mplayer_bin).toString();
    //edited by kobe 20180623
    if (mplayer_bin.isEmpty()) {
        mplayer_bin = "/usr/bin/mpv";//mplayer_bin = QString("%1/mpv").arg(Paths::appPath());
    }
	vo = set->value("driver/vo", vo).toString();
	ao = set->value("driver/audio_output", ao).toString();
	use_screenshot = set->value("use_screenshot", use_screenshot).toBool();
	screenshot_template = set->value("screenshot_template", screenshot_template).toString();
    if (screenshot_template.isEmpty())
        screenshot_template = "cap_%F_%p_%02n";
	screenshot_format = set->value("screenshot_format", screenshot_format).toString();
    if (screenshot_format.isEmpty())
        screenshot_format = "jpg";
	#if QT_VERSION >= 0x040400
	screenshot_directory = set->value("screenshot_folder", screenshot_directory).toString();
	setupScreenshotFolder();
	#else
	screenshot_directory = set->value("screenshot_directory", screenshot_directory).toString();
	#endif

	use_direct_rendering = set->value("use_direct_rendering", use_direct_rendering).toBool();
	use_double_buffer = set->value("use_double_buffer", use_double_buffer).toBool();
	
	use_soft_video_eq = set->value("use_soft_video_eq", use_soft_video_eq).toBool();
	use_slices = set->value("use_slices", use_slices ).toBool();

	use_soft_vol = set->value("use_soft_vol", use_soft_vol).toBool();
	softvol_max = set->value("softvol_max", softvol_max).toInt();
    use_scaletempo = (OptionState) set->value("use_scaletempo", use_scaletempo).toInt();
    use_hwac3 = set->value("use_hwac3", use_hwac3 ).toBool();
    use_audio_equalizer = set->value("use_audio_equalizer", use_audio_equalizer ).toBool();

	global_volume = set->value("global_volume", global_volume).toBool();
	volume = set->value("volume", volume).toInt();
    mute = set->value("mute", mute).toBool();

    global_audio_equalizer = set->value("global_audio_equalizer", global_audio_equalizer).toBool();
    audio_equalizer = set->value("audio_equalizer", audio_equalizer).toList();

    autosync = set->value("autosync", autosync).toBool();
    autosync_factor = set->value("autosync_factor", autosync_factor).toInt();

    min_step = set->value("min_step", min_step).toInt();

    osd = set->value("osd", osd).toInt();
    //osd = 0;//kobe:选项->屏幕显示->仅字幕，该版本会屏蔽“屏幕显示”菜单，全部默认为仅字幕，即配置文件~/.config/smplayer/smplayer.ini的osd字段不管是多少，定制版播放器启动后都重新设置该值为0（仅字幕）
    osd_scale = set->value("osd_scale", osd_scale).toDouble();
    subfont_osd_scale = set->value("subfont_osd_scale", subfont_osd_scale).toDouble();
    osd_delay = set->value("osd_delay", osd_delay).toInt();
//#ifdef MPV_SUPPORT
    osd_fractions = set->value("osd_fractions", osd_fractions).toBool();
//#endif
    osd_bar_pos = set->value("osd_bar_pos", osd_bar_pos).toInt();
    osd_show_filename_duration = set->value("osd_show_filename_duration", osd_show_filename_duration).toInt();


	set->endGroup(); // General

    /* ***********
       Performance
       *********** */
	set->beginGroup( "performance");
    HD_height = set->value("HD_height", HD_height).toInt();
	threads = set->value("threads", threads).toInt();
	hwdec = set->value("hwdec", hwdec).toString();
	cache_for_files = set->value("cache_for_files", cache_for_files).toInt();
	cache_for_streams = set->value("cache_for_streams", cache_for_streams).toInt();
	set->endGroup(); // performance

    /* *********
       Subtitles
       ********* */
	set->beginGroup("subtitles");

	subcp = set->value("subcp", subcp).toString();
    if (subcp.isEmpty())
        subcp = "UTF-8";//ISO-8859-1";//kobe
	use_enca = set->value("use_enca", use_enca).toBool();
	enca_lang = set->value("enca_lang", enca_lang).toString();
    if (enca_lang.isEmpty())
        enca_lang = "zh";//kobe
	sub_visibility = set->value("sub_visibility", sub_visibility).toBool();
	set->endGroup(); // subtitles

    /* ********
       Advanced
       ******** */
	set->beginGroup( "advanced");
	use_idx = set->value("use_idx", use_idx).toBool();
	use_lavf_demuxer = set->value("use_lavf_demuxer", use_lavf_demuxer).toBool();
	use_edl_files = set->value("use_edl_files", use_edl_files).toBool();
//#ifdef MPLAYER_SUPPORT
    use_playlist_option = set->value("use_playlist_option", use_playlist_option).toBool();
//#endif
	use_short_pathnames = set->value("use_short_pathnames", use_short_pathnames).toBool();
	actions_to_run = set->value("actions_to_run", actions_to_run).toString();
	show_tag_in_window_title = set->value("show_tag_in_window_title", show_tag_in_window_title).toBool();
	time_to_kill_mplayer = set->value("time_to_kill_mplayer", time_to_kill_mplayer).toInt();

//#ifdef MPV_SUPPORT
    mpv_osd_media_info = set->value("mpv_osd_media_info", mpv_osd_media_info).toString();
//#endif
//#ifdef MPLAYER_SUPPORT
    mplayer_osd_media_info = set->value("mplayer_osd_media_info", mplayer_osd_media_info).toString();
//#endif

	set->endGroup(); // advanced

    /* *********
       GUI stuff
       ********* */
	set->beginGroup("gui");
	fullscreen = set->value("fullscreen", fullscreen).toBool();
    stay_on_top = (Preferences::OnTop) set->value("stay_on_top", (int) stay_on_top).toInt();
	wheel_function = set->value("mouse_wheel_function", wheel_function).toInt();
	{
		int wheel_function_cycle_int = set->value("wheel_function_cycle", (int) wheel_function_cycle).toInt();
		wheel_function_cycle = (WheelFunctions) wheel_function_cycle_int;
	}
	wheel_function_seeking_reverse = set->value("wheel_function_seeking_reverse", wheel_function_seeking_reverse).toBool();

	seeking1 = set->value("seeking1", seeking1).toInt();
	seeking2 = set->value("seeking2", seeking2).toInt();
	seeking3 = set->value("seeking3", seeking3).toInt();
	seeking4 = set->value("seeking4", seeking4).toInt();

	time_slider_drag_delay = set->value("time_slider_drag_delay", time_slider_drag_delay).toInt();
    pause_when_hidden = set->value("pause_when_hidden", pause_when_hidden).toBool();
    preview_when_playing = set->value("preview_when_playing", preview_when_playing).toBool();
	media_to_add_to_playlist = (AutoAddToPlaylistFilter) set->value("media_to_add_to_playlist", media_to_add_to_playlist).toInt();
    playlist_key = set->value("playlist_key", playlist_key).toString();
    next_key = set->value("next_key", next_key).toString();
    prev_key = set->value("prev_key", prev_key).toString();
    set->endGroup(); // gui

    /* **************
       Initial values
       ************** */

	set->beginGroup( "defaults");
    latest_dir = set->value("latest_dir", latest_dir).toString();
    initial_sub_scale = set->value("initial_sub_scale", initial_sub_scale).toDouble();
    initial_sub_scale_ass = set->value("initial_sub_scale_ass", initial_sub_scale_ass).toDouble();
    initial_volume = set->value("initial_volume", initial_volume).toInt();
    initial_contrast = set->value("initial_contrast", initial_contrast).toInt();
    initial_brightness = set->value("initial_brightness", initial_brightness).toInt();
    initial_hue = set->value("initial_hue", initial_hue).toInt();
    initial_saturation = set->value("initial_saturation", initial_saturation).toInt();
    initial_gamma = set->value("initial_gamma", initial_gamma).toInt();

    initial_audio_equalizer = set->value("initial_audio_equalizer", initial_audio_equalizer).toList();

    initial_zoom_factor = set->value("initial_zoom_factor", initial_zoom_factor).toDouble();
    initial_sub_pos = set->value("initial_sub_pos", initial_sub_pos).toInt();

    initial_volnorm = set->value("initial_volnorm", initial_volnorm).toBool();

//#ifdef INITIAL_BLACKBORDERS
    initial_blackborders = set->value("initial_blackborders", initial_blackborders).toBool();
//#endif

    initial_postprocessing = set->value("initial_postprocessing", initial_postprocessing).toBool();

    initial_deinterlace = set->value("initial_deinterlace", initial_deinterlace).toInt();

    initial_audio_channels = set->value("audio_channels", initial_audio_channels).toInt();
    initial_stereo_mode = set->value("initial_stereo_mode", initial_stereo_mode).toInt();

//#if SELECT_TRACKS_ON_STARTUP
    initial_audio_track = set->value("preferred_audio_track", initial_audio_track).toInt();
    initial_subtitle_track = set->value("preferred_subtitle_track", initial_subtitle_track).toInt();
//#endif
	set->endGroup(); // defaults


    /* ************
       MPlayer info
       ************ */

	set->beginGroup( "mplayer_info");
	mplayer_detected_version = set->value("mplayer_detected_version", mplayer_detected_version).toInt();
	mplayer_user_supplied_version = set->value("mplayer_user_supplied_version", mplayer_user_supplied_version).toInt();
	set->endGroup(); // mplayer_info

    /* *******
       History
       ******* */
	set->beginGroup("history");

	history_recents->setMaxItems( set->value("recents/max_items", history_recents->maxItems()).toInt() );
	history_recents->fromStringList( set->value("recents", history_recents->toStringList()).toStringList() );
	history_urls->setMaxItems( set->value("urls/max_items", history_urls->maxItems()).toInt() );
	history_urls->fromStringList( set->value("urls", history_urls->toStringList()).toStringList() );
	set->endGroup(); // history

    /* *******
       Filters
       ******* */

    filters->load(set);

    //edited by kobe 20180623
    /*if (!QFile::exists(mplayer_bin)) {
		QString app_path = Helper::findExecutable(mplayer_bin);
		//qDebug("Preferences::load: app_path: %s", app_path.toUtf8().constData());
		if (!app_path.isEmpty()) {
			mplayer_bin = app_path;
		} else {
			// Executable not found, try to find an alternative
			if (mplayer_bin.startsWith("mplayer")) {
				app_path = Helper::findExecutable("mpv");
				if (!app_path.isEmpty()) mplayer_bin = app_path;
			}
			else
			if (mplayer_bin.startsWith("mpv")) {
				app_path = Helper::findExecutable("mplayer");
				if (!app_path.isEmpty()) mplayer_bin = app_path;
			}
		}
    }*/
}

void Preferences::setupScreenshotFolder() {
#if QT_VERSION >= 0x040400
	if (screenshot_directory.isEmpty()) {
		#if QT_VERSION >= 0x050000
		QString pdir = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
		if (pdir.isEmpty()) pdir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
		if (pdir.isEmpty()) pdir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
		#else
		QString pdir = QDesktopServices::storageLocation(QDesktopServices::PicturesLocation);
		if (pdir.isEmpty()) pdir = QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation);
		if (pdir.isEmpty()) pdir = QDesktopServices::storageLocation(QDesktopServices::HomeLocation);
		#endif
		if (pdir.isEmpty()) pdir = "/tmp";
		if (!QFile::exists(pdir)) {
			qWarning("Preferences::setupScreenshotFolder: folder '%s' does not exist. Using /tmp as fallback", pdir.toUtf8().constData());
			pdir = "/tmp";
		}
        QString default_screenshot_path = QDir::toNativeSeparators(pdir + "/kylin_video_screenshots");
		if (!QFile::exists(default_screenshot_path)) {
			qDebug("Preferences::setupScreenshotFolder: creating '%s'", default_screenshot_path.toUtf8().constData());
			if (!QDir().mkdir(default_screenshot_path)) {
				qWarning("Preferences::setupScreenshotFolder: failed to create '%s'", default_screenshot_path.toUtf8().constData());
			}
		}
		if (QFile::exists(default_screenshot_path)) {
			screenshot_directory = default_screenshot_path;
		}
        qDebug() << "setup default_screenshot_path=" << default_screenshot_path;
	}
	else {
		screenshot_directory = QDir::toNativeSeparators(screenshot_directory);
	}
#endif
}
