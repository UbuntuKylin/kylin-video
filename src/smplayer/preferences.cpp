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
#include "../smplayer/global.h"
#include "../smplayer/paths.h"
#include "../smplayer/mediasettings.h"
#include "../smplayer/recents.h"
#include "../smplayer/urlhistory.h"
#include "../smplayer/filters.h"

#include "../utils.h"

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

//#ifdef GLOBALSHORTCUTS
#include "../smplayer/globalshortcuts/globalshortcuts.h"
//#endif

#define USE_CONFIG_VERSION
#ifdef USE_CONFIG_VERSION
#define CURRENT_CONFIG_VERSION 5
#endif

using namespace Global;

Preferences::Preferences(const QString &arch) {
    arch_type = arch;
    history_recents = new Recents;
    history_urls = new URLHistory;
    filters = new Filters;

    m_decoingType = this->getHardwareDecodingType();

	reset();
	load();
}

Preferences::~Preferences() {
	save();
    delete history_recents;
    delete history_urls;
    delete filters;
    m_videoMap.clear();
}

VideoPtr Preferences::generateVedioData(const QString &filepath, QString &name, double duration)
{
    //filepath.absoluteFilePath()
    if (m_videoMap.contains(filepath)) {
        return m_videoMap.value(filepath);
    }

    VideoPtr video = VideoPtr(new VideoData);

    Q_ASSERT(video != nullptr);

    video->m_localpath = filepath;//filepath.absoluteFilePath();
    video->m_name = name;
    video->m_duration = duration;

    return video;
}

bool Preferences::isEmpty() const
{
    return this->m_videoMap.isEmpty();
}

const VideoPtr Preferences::video(const QString &hash) const
{
    return this->m_videoMap.value(hash);
}

bool Preferences::contains(const VideoPtr video) const
{
    if (!video.isNull())
        return m_videoMap.contains(video->m_localpath);

    return false;
}

bool Preferences::contains(const QString &filepath) const
{
    return this->m_videoMap.contains(filepath);
}

void Preferences::updatePlaylist(QStringList playlist)
{
    qDebug() << "playlist=" << playlist;
//    QMap<QString, VideoPtr> videoMap = m_videoMap;



//    QMap<QString, int> fileMap;
//    for (int i = 0; i < this->rowCount(); ++i) {
//        auto index = this->index(i, 0);
//        auto filepath = this->data(index).toString();//没有重写model的data函数时
//        Q_ASSERT(!filepath.isEmpty());
//        fileMap.insert(filepath, i);
//    }

//    QStringList sortList;
//    if (fileMap.count() > 0) {
//        QMap<QString, int>::iterator j;
//        for (j = fileMap.begin(); j != fileMap.end(); ++j) {
//            sortList.insert(j.value(), j.key());
//        }
//    }
//    qDebug() << "########sortList=" << sortList;

//    QMap<QString, VideoPtr> videos;
//    QMap<QString, QString> myvideos;
//    for(int n=0; n < sortList.size(); ++n) {
//        qDebug() << "===========" << n;
//        QMap<QString, VideoPtr>::iterator k;
//        int mydex = 0;
//        for (k = pref->m_videoMap.begin(); k != pref->m_videoMap.end(); ++k) {
//            qDebug() << "mydex===========" << mydex;
//            mydex ++;
//            if (sortList.at(n) == k.key()) {
//                videos.insert(k.key(), k.value());
//                qDebug() << "$$$$$2" << k.key() << k.value()->localpath();
//                myvideos.insert(k.key(), k.value()->localpath());
//                break;
//            }
//        }
//    }

//    QMap<QString, VideoPtr>::iterator m;
//    for (m = videos.begin(); m != videos.end(); ++m) {
//        qDebug() << "$$$$$3" << m.key() << m.value()->localpath();
//    }

////    myvideos.insert("a", "aaa");
////    myvideos.insert("b", "bbb");
////    myvideos.insert("c", "ccc");
//    QMap<QString, QString>::iterator l;
//    for (l = myvideos.begin(); l != myvideos.end(); ++l) {
//        qDebug() << "$$$$$4" << l.key() << l.value();
//    }
}

void Preferences::reset() {

#ifdef USE_CONFIG_VERSION
    config_version = CURRENT_CONFIG_VERSION;
#endif

    //edited by kobe 20180623
    mplayer_bin = "/usr/bin/mpv";//mplayer_bin = QString("%1/mpv").arg(Paths::appPath());

    //20191206
    if (arch_type == "aarch64") {
        vo = "vdpau";
    }
    else {
        vo = "xv";
    }
    //ao = "pulse";//ao = "";

    //vo = "player_default";
    ao = "player_default";

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

    remember_media_settings = true;
    remember_time_pos = true;
    remember_stream_settings = true;

    alang = "";
    slang = "";

	use_direct_rendering = false;
	use_double_buffer = true;

	use_soft_video_eq = false;
	use_slices = false;
    autoq = 6;

//#ifdef ADD_BLACKBORDERS_FS
    add_blackborders_on_fullscreen = false;
//#endif

    disable_screensaver = true;

    vdpau.ffh264vdpau = true;
    vdpau.ffmpeg12vdpau = true;
    vdpau.ffwmv3vdpau = true;
    vdpau.ffvc1vdpau = true;
    vdpau.ffodivxvdpau = false;
    vdpau.ffhevcvdpau = false;
    vdpau.disable_video_filters = true;//kobe 20180612

	use_soft_vol = true;
	softvol_max = 110; // 110 = default value in mplayer
    use_scaletempo = Detect;
    use_hwac3 = false;
    use_audio_equalizer = true;

	global_volume = true;
	volume = 50;
	mute = false;

    global_audio_equalizer = true;
    audio_equalizer << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0; // FIXME: use initial_audio_equalizer (but it's set later)

    autosync = false;
    autosync_factor = 30;//100 调整音视频同步,帮助文件指出 置为30为最佳

    use_mc = false;
    mc_value = 0;

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
    file_settings_method = "normal";//"hash"; // Possible values: normal & hash

    tablet_mode = false;

    /* ***************
       Drives (CD/DVD)
       *************** */

    dvd_device = "";
    cdrom_device = "";
//#ifdef BLURAY_SUPPORT
//	bluray_device = "";
//#endif

//#ifndef Q_OS_WIN
    // Try to set default values
    if (QFile::exists("/dev/dvd")) dvd_device = "/dev/dvd";
    if (QFile::exists("/dev/cdrom")) cdrom_device = "/dev/cdrom";
//#endif

//#ifdef Q_OS_WIN
//	enable_audiocd_on_windows = false;
//#endif

    vcd_initial_title = 2; // Most VCD's start at title #2

//#if DVDNAV_SUPPORT
//	use_dvdnav = false;
//#endif


    /* ***********
       Performance
       *********** */
    frame_drop = false;
    hard_frame_drop = false;
    coreavc = false;
    h264_skip_loop_filter = LoopEnabled;
	HD_height = 720;

//#ifdef OBSOLETE_FAST_AUDIO_CHANGE
//	// MPlayer 1.0rc1 require restart, new versions don't
//	fast_audio_change = Detect;
//#endif

    threads = 4;//1
    hwdec = "no";//"auto"
    //hwdec = "rkmpp-copy";

    cache_auto = true;
    cache_for_files = 2048;
    cache_for_streams = 2048;
    cache_for_dvds = 0; // not recommended to use cache for dvds
    cache_for_vcds = 1024;
    cache_for_audiocds = 1024;
//#ifdef TV_SUPPORT
//	cache_for_tv = 3000;
//#endif


    /* *********
       Subtitles
       ********* */
    subcp = "UTF-8";//"ISO-8859-1";
	use_enca = false;
	enca_lang = QString(QLocale::system().name()).section("_",0,0);
    subfuzziness = 1;
    autoload_sub = true;

    use_ass_subtitles = true;
    enable_ass_styles = true;
    ass_line_spacing = 0;

    use_forced_subs_only = false;

    sub_visibility = true;

    subtitles_on_screenshots = false;

    change_sub_scale_should_restart = Detect;

    fast_load_sub = true;

    // ASS styles
    // Nothing to do, default values are given in
    // AssStyles constructor
    force_ass_styles = false;
    user_forced_ass_style.clear();

    freetype_support = true;

    /* ********
       Advanced
       ******** */
    use_mplayer_window = false;

	use_idx = false;
	use_lavf_demuxer = false;

    mplayer_additional_options="";
//	#if defined(PORTABLE_APP) && defined(FONTS_HACK)
//	mplayer_additional_options="-nofontconfig";
//	#endif
    mplayer_additional_video_filters="";
    mplayer_additional_audio_filters="";

    verbose_log = false;

    // "Repaint video background" in the preferences dialog
//    #ifndef Q_OS_WIN
    // Note: on linux there could be flickering when using mplayer if this option is true
    // but setting it to false could display a corrupted window
    // from the moment the user press play until playback actually starts
    repaint_video_background = true;


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
    time_to_kill_mplayer = 5000;//1000

//#ifdef MPV_SUPPORT
    mpv_osd_media_info = "";
//#endif
//#ifdef MPLAYER_SUPPORT
    mplayer_osd_media_info = "";
//#endif
//#ifdef MPV_SUPPORT
    emulate_mplayer_ab_section = false;
//#endif

    use_native_open_dialog = true;

    /* *********
       GUI stuff
       ********* */
    fullscreen = false;
    start_in_fullscreen = false;
    compact_mode = false;
    stay_on_top = NeverOnTop;
    size_factor = 100; // 100%
    play_order = OrderPlay;

//    resize_method = Afterload;//kobe: Never;
    resize_method = Never;//Afterload

    center_window = false;
    center_window_if_outside = false;

//#ifdef GLOBALSHORTCUTS
    use_global_shortcuts = false;
    global_shortcuts_grabbed_keys = GlobalShortcuts::MediaPlay | GlobalShortcuts::MediaStop |
                                    GlobalShortcuts::MediaPrevious | GlobalShortcuts::MediaNext |
                                    GlobalShortcuts::MediaPause | GlobalShortcuts::MediaRecord |
                                    GlobalShortcuts::VolumeMute | GlobalShortcuts::VolumeDown |
                                    GlobalShortcuts::VolumeUp;
//#endif

//#if DVDNAV_SUPPORT
//	mouse_left_click_function = "dvdnav_mouse";
//#else
    mouse_left_click_function = "";
//#endif

    mouse_right_click_function = "show_context_menu";
    mouse_double_click_function = "fullscreen";
    mouse_middle_click_function = "mute";
    mouse_xbutton1_click_function = "";
    mouse_xbutton2_click_function = "";
    wheel_function = Seeking;
    wheel_function_cycle = Seeking | Volume | Zoom | ChangeSpeed;
    wheel_function_seeking_reverse = false;

    drag_function = MoveWindow;

	seeking1 = 10;
	seeking2 = 60;
	seeking3 = 10*60;
	seeking4 = 30;

    update_while_seeking = false;
//#if ENABLE_DELAYED_DRAGGING
    time_slider_drag_delay = 100;
//#endif

    relative_seeking = false;

    precise_seeking = true;

    reset_stop = false;
    delay_left_click = false;

    language = "";

    balloon_count = 5;

    restore_pos_after_fullscreen = false;

    save_window_size_on_exit = true;

    close_on_finish = false;

    default_font = "";

    pause_when_hidden = false;

    allow_video_movement = false;

    gui = "DefaultGUI";
    iconset = "H2O";

    gui_minimum_width = 0; // 0 == disabled
    default_size = QSize(683, 509);

//#ifdef Q_OS_WIN
//	report_mplayer_crashes = false;
//#else
    report_mplayer_crashes = true;
//#endif


    reported_mplayer_is_old = false;


    auto_add_to_playlist = true;
    media_to_add_to_playlist = NoFiles;

    animated_logo = true;

    preview_when_playing = true;
//    playlist_key = "F3";
//    prev_key = "<, Media Previous";
//    next_key = ">, Media Next";

    /* ***********
       Directories
       *********** */
    latest_dir = QDir::homePath();
//    last_dvd_directory="";
//    save_dirs = true;

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
;
    m_videoMap.clear();
}

void Preferences::save() {
//    qDebug("Preferences::save");
	QSettings * set = settings;


    /* *******
       General
       ******* */

	set->beginGroup("General");
//    set->setValue("config_version", config_version);
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

//    set->setValue("remember_media_settings", remember_media_settings);
//	set->setValue("remember_time_pos", remember_time_pos);
//	set->setValue("remember_stream_settings", remember_stream_settings);

//    set->setValue("alang", alang);
//	set->setValue("slang", slang);

	set->setValue("use_direct_rendering", use_direct_rendering);
	set->setValue("use_double_buffer", use_double_buffer);
	set->setValue("use_soft_video_eq", use_soft_video_eq);
	set->setValue("use_slices", use_slices );
    set->setValue("autoq", autoq);

//#ifdef ADD_BLACKBORDERS_FS
//	set->setValue("add_blackborders_on_fullscreen", add_blackborders_on_fullscreen);
//#endif
//    set->setValue("disable_screensaver", disable_screensaver);

//    set->setValue("vdpau_ffh264vdpau", vdpau.ffh264vdpau);
//    set->setValue("vdpau_ffmpeg12vdpau", vdpau.ffmpeg12vdpau);
//    set->setValue("vdpau_ffwmv3vdpau", vdpau.ffwmv3vdpau);
//    set->setValue("vdpau_ffvc1vdpau", vdpau.ffvc1vdpau);
//    set->setValue("vdpau_ffodivxvdpau", vdpau.ffodivxvdpau);
//    set->setValue("vdpau_ffhevcvdpau", vdpau.ffhevcvdpau);
//    set->setValue("vdpau_disable_video_filters", vdpau.disable_video_filters);

	set->setValue("use_soft_vol", use_soft_vol);
	set->setValue("softvol_max", softvol_max);
    set->setValue("use_scaletempo", use_scaletempo);
    set->setValue("use_hwac3", use_hwac3 );
//    set->setValue("use_audio_equalizer", use_audio_equalizer );

	set->setValue("global_volume", global_volume);
	set->setValue("volume", volume);
    set->setValue("mute", mute);

    set->setValue("global_audio_equalizer", global_audio_equalizer);
    set->setValue("audio_equalizer", audio_equalizer);

    set->setValue("autosync", autosync);
    set->setValue("autosync_factor", autosync_factor);

//    set->setValue("use_mc", use_mc);
//	set->setValue("mc_value", mc_value);

//	set->setValue("autoload_m4a", autoload_m4a);
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

//    set->setValue("file_settings_method", file_settings_method);

//	set->setValue("tablet_mode", tablet_mode);

	set->endGroup(); // General


    /* ***************
       Drives (CD/DVD)
       *************** */
/*
    set->beginGroup( "drives");

    set->setValue("dvd_device", dvd_device);
    set->setValue("cdrom_device", cdrom_device);
//#ifdef BLURAY_SUPPORT
//	set->setValue("bluray_device", bluray_device);
//#endif

//#ifdef Q_OS_WIN
//	set->setValue("enable_audiocd_on_windows", enable_audiocd_on_windows);
//#endif

    set->setValue("vcd_initial_title", vcd_initial_title);

//#if DVDNAV_SUPPORT
//	set->setValue("use_dvdnav", use_dvdnav);
//#endif

    set->endGroup(); // drives
*/


    /* ***********
       Performance
       *********** */
	set->beginGroup( "performance");
//    set->setValue("frame_drop", frame_drop);
//	set->setValue("hard_frame_drop", hard_frame_drop);
//	set->setValue("coreavc", coreavc);
//	set->setValue("h264_skip_loop_filter", h264_skip_loop_filter);
    set->setValue("HD_height", HD_height);

	set->setValue("threads", threads);
	set->setValue("hwdec", hwdec);

//    set->setValue("cache_auto", cache_auto);
    set->setValue("cache_for_files", cache_for_files);
    set->setValue("cache_for_streams", cache_for_streams);
//	set->setValue("cache_for_dvds", cache_for_dvds);
//	set->setValue("cache_for_vcds", cache_for_vcds);
//	set->setValue("cache_for_audiocds", cache_for_audiocds);

	set->endGroup(); // performance

    /* *********
       Subtitles
       ********* */
	set->beginGroup("subtitles");
	set->setValue("subcp", subcp);
	set->setValue("use_enca", use_enca);
	set->setValue("enca_lang", enca_lang);
//    set->setValue("subfuzziness", subfuzziness);
//	set->setValue("autoload_sub", autoload_sub);

//    set->setValue("use_ass_subtitles", use_ass_subtitles);
//	set->setValue("enable_ass_styles", enable_ass_styles);
//	set->setValue("ass_line_spacing", ass_line_spacing);
//	set->setValue("use_forced_subs_only", use_forced_subs_only);

    set->setValue("sub_visibility", sub_visibility);

//	set->setValue("subtitles_on_screenshots", subtitles_on_screenshots);

//	set->setValue("change_sub_scale_should_restart", change_sub_scale_should_restart);

//	set->setValue("fast_load_sub", fast_load_sub);

    // ASS styles
//	ass_styles.save(set);
//	set->setValue("force_ass_styles", force_ass_styles);
//	set->setValue("user_forced_ass_style", user_forced_ass_style);
//	set->setValue("freetype_support", freetype_support);

	set->endGroup(); // subtitles


    /* ********
       Advanced
       ******** */
	set->beginGroup( "advanced");
//    set->setValue("use_mplayer_window", use_mplayer_window);
	set->setValue("use_idx", use_idx);
	set->setValue("use_lavf_demuxer", use_lavf_demuxer);
//    set->setValue("mplayer_additional_options", mplayer_additional_options);
//	set->setValue("mplayer_additional_video_filters", mplayer_additional_video_filters);
//	set->setValue("mplayer_additional_audio_filters", mplayer_additional_audio_filters);

//	set->setValue("repaint_video_background", repaint_video_background);

    set->setValue("use_edl_files", use_edl_files);

//#ifdef MPLAYER_SUPPORT
    set->setValue("use_playlist_option", use_playlist_option);
//#endif
//    set->setValue("prefer_ipv4", prefer_ipv4);
	set->setValue("use_short_pathnames", use_short_pathnames);
//    set->setValue("change_video_equalizer_on_startup", change_video_equalizer_on_startup);
//	set->setValue("use_pausing_keep_force", use_pausing_keep_force);
//    set->setValue("correct_pts", use_correct_pts);

    set->setValue("actions_to_run", actions_to_run);

    set->setValue("show_tag_in_window_title", show_tag_in_window_title);

    set->setValue("time_to_kill_player", time_to_kill_mplayer);

//#ifdef MPV_SUPPORT
    set->setValue("mpv_osd_media_info", mpv_osd_media_info);
//#endif
//#ifdef MPLAYER_SUPPORT
    set->setValue("mplayer_osd_media_info", mplayer_osd_media_info);
//#endif
//#ifdef MPV_SUPPORT
//	set->setValue("emulate_mplayer_ab_section", emulate_mplayer_ab_section);
//#endif
//	set->setValue("use_native_open_dialog", use_native_open_dialog);

	set->endGroup(); // advanced

    /* *********
       GUI stuff
       ********* */
	set->beginGroup("gui");
    set->setValue("fullscreen", fullscreen);
//    set->setValue("start_in_fullscreen", start_in_fullscreen);
//    set->setValue("compact_mode", compact_mode);
    set->setValue("stay_on_top", (int) stay_on_top);
//    set->setValue("size_factor", size_factor);
//	set->setValue("resize_method", resize_method);
//    set->setValue("center_window", center_window);
//	set->setValue("center_window_if_outside", center_window_if_outside);
//#ifdef GLOBALSHORTCUTS
//	set->setValue("use_global_shortcuts", use_global_shortcuts);
//	set->setValue("global_shortcuts_grabbed_keys", global_shortcuts_grabbed_keys);
//#endif
//    set->setValue("mouse_left_click_function", mouse_left_click_function);
//	set->setValue("mouse_right_click_function", mouse_right_click_function);
//	set->setValue("mouse_double_click_function", mouse_double_click_function);
//	set->setValue("mouse_middle_click_function", mouse_middle_click_function);
//	set->setValue("mouse_xbutton1_click_function", mouse_xbutton1_click_function);
//	set->setValue("mouse_xbutton2_click_function", mouse_xbutton2_click_function);
    set->setValue("mouse_wheel_function", wheel_function);
    set->setValue("wheel_function_cycle", (int) wheel_function_cycle);
    set->setValue("wheel_function_seeking_reverse", wheel_function_seeking_reverse);
//    set->setValue("drag_function", drag_function);
	set->setValue("seeking1", seeking1);
	set->setValue("seeking2", seeking2);
	set->setValue("seeking3", seeking3);
	set->setValue("seeking4", seeking4);
//    set->setValue("update_while_seeking", update_while_seeking);
//#if ENABLE_DELAYED_DRAGGING
    set->setValue("time_slider_drag_delay", time_slider_drag_delay);
//#endif

//	set->setValue("relative_seeking", relative_seeking);

//	set->setValue("precise_seeking", precise_seeking);
//    set->setValue("reset_stop", reset_stop);
//	set->setValue("delay_left_click", delay_left_click);
//    set->setValue("language", language);
//	set->setValue("iconset", iconset);
//	set->setValue("balloon_count", balloon_count);
//	set->setValue("restore_pos_after_fullscreen", restore_pos_after_fullscreen);
//	set->setValue("save_window_size_on_exit", save_window_size_on_exit);
//	set->setValue("close_on_finish", close_on_finish);

//	set->setValue("default_font", default_font);
    set->setValue("pause_when_hidden", pause_when_hidden);
    set->setValue("allow_video_movement", allow_video_movement);
//	set->setValue("gui", gui);
//	set->setValue("gui_minimum_width", gui_minimum_width);
//	set->setValue("default_size", default_size);

//	set->setValue("report_player_crashes", report_mplayer_crashes);

//	set->setValue("reported_mplayer_is_old", reported_mplayer_is_old);

//	set->setValue("auto_add_to_playlist", auto_add_to_playlist);
    set->setValue("media_to_add_to_playlist", media_to_add_to_playlist);

    set->setValue("preview_when_playing", preview_when_playing);
//    set->setValue("playlist_key", playlist_key);
//    set->setValue("next_key", next_key);
//    set->setValue("prev_key", prev_key);
	set->endGroup(); // gui

    /* ***********
       Directories
       *********** */

    set->beginGroup( "directories");
//    if (save_dirs) {
//        set->setValue("latest_dir", latest_dir);
//        set->setValue("last_dvd_directory", last_dvd_directory);
//    } else {
//        set->setValue("latest_dir", "");
//        set->setValue("last_dvd_directory", "");
//    }
//    set->setValue("save_dirs", save_dirs);
    set->setValue("latest_dir", latest_dir);
    set->endGroup(); // directories*/

    /* **************
       Initial values
       ************** */
	set->beginGroup( "defaults");

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

    set->setValue("preferred_audio_track", initial_audio_track);
    set->setValue("preferred_subtitle_track", initial_subtitle_track);

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
//    config_version = set->value("config_version", 0).toInt();
	mplayer_bin = set->value("mplayer_bin", mplayer_bin).toString();
    //edited by kobe 20180623
    if (mplayer_bin.isEmpty()) {
        mplayer_bin = "/usr/bin/mpv";//mplayer_bin = QString("%1/mpv").arg(Paths::appPath());
    }
	vo = set->value("driver/vo", vo).toString();
    if (vo.isNull() || vo.isEmpty()) {
        vo = "player_default";
    }
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

//    remember_media_settings = set->value("remember_media_settings", remember_media_settings).toBool();
//	remember_time_pos = set->value("remember_time_pos", remember_time_pos).toBool();
//	remember_stream_settings = set->value("remember_stream_settings", remember_stream_settings).toBool();

//    alang = set->value("alang", alang).toString();
//    slang = set->value("slang", slang).toString();

	use_direct_rendering = set->value("use_direct_rendering", use_direct_rendering).toBool();
	use_double_buffer = set->value("use_double_buffer", use_double_buffer).toBool();
	
	use_soft_video_eq = set->value("use_soft_video_eq", use_soft_video_eq).toBool();
	use_slices = set->value("use_slices", use_slices ).toBool();
//    autoq = set->value("autoq", autoq).toInt();

//    disable_screensaver = set->value("disable_screensaver", disable_screensaver).toBool();
//	add_blackborders_on_fullscreen = set->value("add_blackborders_on_fullscreen", add_blackborders_on_fullscreen).toBool();

//    vdpau.ffh264vdpau = set->value("vdpau_ffh264vdpau", vdpau.ffh264vdpau).toBool();
//	vdpau.ffmpeg12vdpau = set->value("vdpau_ffmpeg12vdpau", vdpau.ffmpeg12vdpau).toBool();
//	vdpau.ffwmv3vdpau = set->value("vdpau_ffwmv3vdpau", vdpau.ffwmv3vdpau).toBool();
//	vdpau.ffvc1vdpau = set->value("vdpau_ffvc1vdpau", vdpau.ffvc1vdpau).toBool();
//	vdpau.ffodivxvdpau = set->value("vdpau_ffodivxvdpau", vdpau.ffodivxvdpau).toBool();
//	vdpau.ffhevcvdpau = set->value("vdpau_ffhevcvdpau", vdpau.ffhevcvdpau).toBool();
//	vdpau.disable_video_filters = set->value("vdpau_disable_video_filters", vdpau.disable_video_filters).toBool();

	use_soft_vol = set->value("use_soft_vol", use_soft_vol).toBool();
	softvol_max = set->value("softvol_max", softvol_max).toInt();
    use_scaletempo = (OptionState) set->value("use_scaletempo", use_scaletempo).toInt();
    use_hwac3 = set->value("use_hwac3", use_hwac3 ).toBool();
//    use_audio_equalizer = set->value("use_audio_equalizer", use_audio_equalizer ).toBool();
    use_audio_equalizer = true;

	global_volume = set->value("global_volume", global_volume).toBool();
	volume = set->value("volume", volume).toInt();
    mute = set->value("mute", mute).toBool();

    global_audio_equalizer = set->value("global_audio_equalizer", global_audio_equalizer).toBool();
    audio_equalizer = set->value("audio_equalizer", audio_equalizer).toList();

    autosync = set->value("autosync", autosync).toBool();
    autosync_factor = set->value("autosync_factor", autosync_factor).toInt();

//    use_mc = set->value("use_mc", use_mc).toBool();
//	mc_value = set->value("mc_value", mc_value).toDouble();

//	autoload_m4a = set->value("autoload_m4a", autoload_m4a).toBool();
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

//    file_settings_method = set->value("file_settings_method", file_settings_method).toString();

//	tablet_mode = set->value("tablet_mode", tablet_mode).toBool();

	set->endGroup(); // General

    /* ***************
       Drives (CD/DVD)
       *************** */
/*
    set->beginGroup( "drives");

    dvd_device = set->value("dvd_device", dvd_device).toString();
    cdrom_device = set->value("cdrom_device", cdrom_device).toString();
//#ifdef BLURAY_SUPPORT
//	bluray_device = set->value("bluray_device", bluray_device).toString();
//#endif

    vcd_initial_title = set->value("vcd_initial_title", vcd_initial_title ).toInt();

//#if DVDNAV_SUPPORT
//	use_dvdnav = set->value("use_dvdnav", use_dvdnav).toBool();
//#endif

    set->endGroup(); // drives
*/

    /* ***********
       Performance
       *********** */
	set->beginGroup( "performance");
//    frame_drop = set->value("frame_drop", frame_drop).toBool();
//	hard_frame_drop = set->value("hard_frame_drop", hard_frame_drop).toBool();
//	coreavc = set->value("coreavc", coreavc).toBool();
//	h264_skip_loop_filter = (H264LoopFilter) set->value("h264_skip_loop_filter", h264_skip_loop_filter).toInt();
    HD_height = set->value("HD_height", HD_height).toInt();

	threads = set->value("threads", threads).toInt();
	hwdec = set->value("hwdec", hwdec).toString();

//    cache_auto = set->value("cache_auto", cache_auto).toBool();
    cache_for_files = set->value("cache_for_files", cache_for_files).toInt();
    cache_for_streams = set->value("cache_for_streams", cache_for_streams).toInt();
//	cache_for_dvds = set->value("cache_for_dvds", cache_for_dvds).toInt();
//	cache_for_vcds = set->value("cache_for_vcds", cache_for_vcds).toInt();
//	cache_for_audiocds = set->value("cache_for_audiocds", cache_for_audiocds).toInt();

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
//    subfuzziness = set->value("subfuzziness", subfuzziness).toInt();
//	autoload_sub = set->value("autoload_sub", autoload_sub).toBool();

//    use_ass_subtitles = set->value("use_ass_subtitles", use_ass_subtitles).toBool();
//	enable_ass_styles = set->value("enable_ass_styles", enable_ass_styles).toBool();
//	ass_line_spacing = set->value("ass_line_spacing", ass_line_spacing).toInt();

//	use_forced_subs_only = set->value("use_forced_subs_only", use_forced_subs_only).toBool();

	sub_visibility = set->value("sub_visibility", sub_visibility).toBool();
//    subtitles_on_screenshots = set->value("subtitles_on_screenshots", subtitles_on_screenshots).toBool();

//	change_sub_scale_should_restart = (OptionState) set->value("change_sub_scale_should_restart", change_sub_scale_should_restart).toInt();

//	fast_load_sub = set->value("fast_load_sub", fast_load_sub).toBool();

//    // ASS styles
//	ass_styles.load(set);
//	force_ass_styles = set->value("force_ass_styles", force_ass_styles).toBool();
//	user_forced_ass_style = set->value("user_forced_ass_style", user_forced_ass_style).toString();

//	freetype_support = set->value("freetype_support", freetype_support).toBool();
	set->endGroup(); // subtitles

    /* ********
       Advanced
       ******** */
	set->beginGroup( "advanced");
//    use_mplayer_window = set->value("use_mplayer_window", use_mplayer_window).toBool();


	use_idx = set->value("use_idx", use_idx).toBool();
	use_lavf_demuxer = set->value("use_lavf_demuxer", use_lavf_demuxer).toBool();
//    mplayer_additional_options = set->value("mplayer_additional_options", mplayer_additional_options).toString();
//	mplayer_additional_video_filters = set->value("mplayer_additional_video_filters", mplayer_additional_video_filters).toString();
//	mplayer_additional_audio_filters = set->value("mplayer_additional_audio_filters", mplayer_additional_audio_filters).toString();

//	verbose_log = set->value("verbose_log", verbose_log).toBool();


//	repaint_video_background = set->value("repaint_video_background", repaint_video_background).toBool();

    use_edl_files = set->value("use_edl_files", use_edl_files).toBool();
//#ifdef MPLAYER_SUPPORT
    use_playlist_option = set->value("use_playlist_option", use_playlist_option).toBool();
//#endif

//    prefer_ipv4 = set->value("prefer_ipv4", prefer_ipv4).toBool();

    use_short_pathnames = set->value("use_short_pathnames", use_short_pathnames).toBool();

//	use_pausing_keep_force = set->value("use_pausing_keep_force", use_pausing_keep_force).toBool();

//	use_correct_pts = (OptionState) set->value("correct_pts", use_correct_pts).toInt();

    actions_to_run = set->value("actions_to_run", actions_to_run).toString();

    show_tag_in_window_title = set->value("show_tag_in_window_title", show_tag_in_window_title).toBool();

    time_to_kill_mplayer = set->value("time_to_kill_player", time_to_kill_mplayer).toInt();

//#ifdef MPV_SUPPORT
    mpv_osd_media_info = set->value("mpv_osd_media_info", mpv_osd_media_info).toString();
//#endif
//#ifdef MPLAYER_SUPPORT
    mplayer_osd_media_info = set->value("mplayer_osd_media_info", mplayer_osd_media_info).toString();
//#endif

////#ifdef MPV_SUPPORT
//	emulate_mplayer_ab_section = set->value("emulate_mplayer_ab_section", emulate_mplayer_ab_section).toBool();
////#endif
//	use_native_open_dialog = set->value("use_native_open_dialog", use_native_open_dialog).toBool();

	set->endGroup(); // advanced

    /* *********
       GUI stuff
       ********* */
	set->beginGroup("gui");
	fullscreen = set->value("fullscreen", fullscreen).toBool();
//    start_in_fullscreen = set->value("start_in_fullscreen", start_in_fullscreen).toBool();

//	compact_mode = set->value("compact_mode", compact_mode).toBool();
    stay_on_top = (Preferences::OnTop) set->value("stay_on_top", (int) stay_on_top).toInt();
//	size_factor = set->value("size_factor", size_factor).toInt();
//	resize_method = set->value("resize_method", resize_method).toInt();


//    center_window = set->value("center_window", center_window).toBool();
//	center_window_if_outside = set->value("center_window_if_outside", center_window_if_outside).toBool();
//	use_global_shortcuts = set->value("use_global_shortcuts", use_global_shortcuts).toBool();
//	global_shortcuts_grabbed_keys = set->value("global_shortcuts_grabbed_keys", global_shortcuts_grabbed_keys).toInt();
//	mouse_left_click_function = set->value("mouse_left_click_function", mouse_left_click_function).toString();
//	mouse_right_click_function = set->value("mouse_right_click_function", mouse_right_click_function).toString();
//	mouse_double_click_function = set->value("mouse_double_click_function", mouse_double_click_function).toString();
//	mouse_middle_click_function = set->value("mouse_middle_click_function", mouse_middle_click_function).toString();
//	mouse_xbutton1_click_function = set->value("mouse_xbutton1_click_function", mouse_xbutton1_click_function).toString();
//	mouse_xbutton2_click_function = set->value("mouse_xbutton2_click_function", mouse_xbutton2_click_function).toString();
    wheel_function = set->value("mouse_wheel_function", wheel_function).toInt();
    {
        int wheel_function_cycle_int = set->value("wheel_function_cycle", (int) wheel_function_cycle).toInt();
        wheel_function_cycle = (WheelFunctions) wheel_function_cycle_int;
    }
    wheel_function_seeking_reverse = set->value("wheel_function_seeking_reverse", wheel_function_seeking_reverse).toBool();

//	drag_function = set->value("drag_function", drag_function).toInt();

	seeking1 = set->value("seeking1", seeking1).toInt();
	seeking2 = set->value("seeking2", seeking2).toInt();
	seeking3 = set->value("seeking3", seeking3).toInt();
	seeking4 = set->value("seeking4", seeking4).toInt();




//    update_while_seeking = set->value("update_while_seeking", update_while_seeking).toBool();
//#if ENABLE_DELAYED_DRAGGING
    time_slider_drag_delay = set->value("time_slider_drag_delay", time_slider_drag_delay).toInt();
//#endif

//	relative_seeking = set->value("relative_seeking", relative_seeking).toBool();

//	precise_seeking = set->value("precise_seeking", precise_seeking).toBool();

//	reset_stop = set->value("reset_stop", reset_stop).toBool();
//	delay_left_click = set->value("delay_left_click", delay_left_click).toBool();

//	language = set->value("language", language).toString();
//	iconset= set->value("iconset", iconset).toString();

//	balloon_count = set->value("balloon_count", balloon_count).toInt();

//	restore_pos_after_fullscreen = set->value("restore_pos_after_fullscreen", restore_pos_after_fullscreen).toBool();
//	save_window_size_on_exit = 	set->value("save_window_size_on_exit", save_window_size_on_exit).toBool();

//	close_on_finish = set->value("close_on_finish", close_on_finish).toBool();

//	default_font = set->value("default_font", default_font).toString();

    pause_when_hidden = set->value("pause_when_hidden", pause_when_hidden).toBool();

//	allow_video_movement = set->value("allow_video_movement", allow_video_movement).toBool();

//	gui = set->value("gui", gui).toString();

//	gui_minimum_width = set->value("gui_minimum_width", gui_minimum_width).toInt();
//	default_size = set->value("default_size", default_size).toSize();


//	report_mplayer_crashes = set->value("report_player_crashes", report_mplayer_crashes).toBool();

    reported_mplayer_is_old = set->value("reported_mplayer_is_old", reported_mplayer_is_old).toBool();

//	auto_add_to_playlist = set->value("auto_add_to_playlist", auto_add_to_playlist).toBool();
    media_to_add_to_playlist = (AutoAddToPlaylistFilter) set->value("media_to_add_to_playlist", media_to_add_to_playlist).toInt();

    preview_when_playing = set->value("preview_when_playing", preview_when_playing).toBool();
    if (!preview_when_playing) {
        preview_when_playing = true;
    }
//	playlist_key = set->value("playlist_key", playlist_key).toString();
//    next_key = set->value("next_key", next_key).toString();
//    prev_key = set->value("prev_key", prev_key).toString();
    set->endGroup(); // gui


    /* ***********
       Directories
       *********** */
    set->beginGroup( "directories");
//    save_dirs = set->value("save_dirs", save_dirs).toBool();
//    if (save_dirs) {
//        latest_dir = set->value("latest_dir", latest_dir).toString();
//        last_dvd_directory = set->value("last_dvd_directory", last_dvd_directory).toString();
//    }
    latest_dir = set->value("latest_dir", latest_dir).toString();
    set->endGroup(); // directories


    /* **************
       Initial values
       ************** */

	set->beginGroup( "defaults");
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

    initial_audio_track = set->value("preferred_audio_track", initial_audio_track).toInt();
    initial_subtitle_track = set->value("preferred_subtitle_track", initial_subtitle_track).toInt();
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

    //lixiang reset vo and hwdec for arm64 (only mpv support hwdec)
    //20191206
    if (arch_type == "aarch64") {
        if (m_decoingType == AmdVdpau) {
            vo = "vdpau";
            hwdec = "vdpau";
        }
        else if (m_decoingType == Jm7200Xv || m_decoingType == Sm768Xv) {
            vo = "xv";
            hwdec = "no";
        }
        else if (m_decoingType == Gp101X11) {
            vo = "x11";
            hwdec = "no";
        }
        else {
            vo = "xv";
            hwdec = "no";
        }
    }

    //edited by kobe 20180623
    /*if (!QFile::exists(mplayer_bin)) {
        QString app_path = Utils::findExecutable(mplayer_bin);
		//qDebug("Preferences::load: app_path: %s", app_path.toUtf8().constData());
		if (!app_path.isEmpty()) {
			mplayer_bin = app_path;
		} else {
			// Executable not found, try to find an alternative
			if (mplayer_bin.startsWith("mplayer")) {
                app_path = Utils::findExecutable("mpv");
				if (!app_path.isEmpty()) mplayer_bin = app_path;
			}
			else
			if (mplayer_bin.startsWith("mpv")) {
                app_path = Utils::findExecutable("mplayer");
				if (!app_path.isEmpty()) mplayer_bin = app_path;
			}
		}
    }*/
}

void Preferences::setupScreenshotFolder()
{
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
        if (!QFile::exists(screenshot_directory)) {
            if (!QDir().mkdir(screenshot_directory)) {
                qWarning("Preferences::setupScreenshotFolder: failed to create '%s'", screenshot_directory.toUtf8().constData());
            }
        }
	}
#endif
}

void Preferences::updatePredefinedList()
{
    //# 对于PCIe设备的预处理
    //# 格式："VID:PID:ClassID:预设分数:描述信息"
    //# 一行一条规则
    //# VID、PID中，-1或者0xFFFF代表匹配所有
    //# ClassID中，-1或者0xFFFFFF代表匹配所有
    //# 例如："0x8086:0x1901:0x0:0:Intel PCIe Controller (x16)"

    QStringList itemList;
    itemList.append("0731:7200:-1:0:JINGJIA MICRO JM7200 Graphics Card");
    itemList.append("126f:-1:-1:0:SM750/SM768");
    itemList.append("1a03:-1:-1:0:BMCd");
    itemList.append("1002:-1:-1:300:AMD Graphics Card");
    itemList.append("0709:0001:-1:0:709 GP101 Graphics Card");

    foreach (QString item, itemList) {
        item = item.trimmed();
        if (item.startsWith("#"))
            continue;
        if (item.startsWith("//"))
            continue;

        PciePredefined device;
        QStringList stringList = item.split(":");
        bool ok;

        if (stringList.count() < 5)
            continue;

        device.vid = stringList[0].toInt(&ok, 16);
        if (device.vid == -1)
            device.vid = 0xFFFF;
        stringList.removeAt(0);

        device.pid = stringList[0].toInt(&ok, 16);
        if (device.pid == -1)
            device.pid = 0xFFFF;
        stringList.removeAt(0);

        device.cid = stringList[0].toInt(&ok, 16);
        if (device.cid == -1)
            device.cid = 0xFFFFFF;
        stringList.removeAt(0);

        device.score = stringList[0].toInt(&ok, 10);
        stringList.removeAt(0);

        device.description = stringList.join(":").trimmed();
        predefinedList.append(device);
    }
}

void Preferences::updatePcieList()
{
    QDir dir("/sys/bus/pci/devices/");
    if (!dir.exists())
            return;

    dir.setFilter(QDir::Dirs);
    QStringList busList = dir.entryList();
    busList.removeOne(".");
    busList.removeOne("..");

    foreach(QString bus, busList) {
        PcieInfo info;
        QString path;
        QFile file;
        QByteArray charArray;
        bool ok;
        int id;

        path = dir.absoluteFilePath(bus + "/" + "vendor");
        file.setFileName(path);
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        charArray = file.readAll();
        file.close();
        id = QString(charArray).toInt(&ok, 16);
        info.vid = id;

        path = dir.absoluteFilePath(bus + "/" + "device");
        file.setFileName(path);
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        charArray = file.readAll();
        file.close();
        id = QString(charArray).toInt(&ok, 16);
        info.pid = id;

        path = dir.absoluteFilePath(bus + "/" + "class");
        file.setFileName(path);
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        charArray = file.readAll();
        file.close();
        id = QString(charArray).toInt(&ok, 16);
        info.cid = id;

        pcieList.append(info);
        //qDebug() << path + QString(": 0x%1, 0x%2, 0x%3").arg(info.vid, 4, 16).arg(info.pid, 4, 16,   QChar('0')).arg(info.cid, 4, 16);
    }
//      foreach(PcieInfo info, pcieList) {
//              qDebug() << QString(": 0x%1, 0x%2, 0x%3").arg(info.vid, 4, 16).arg(info.pid, 4, 16, QChar('0')).arg(info.cid, 4, 16);
//      }
}

Preferences::HardDecodingType Preferences::getHardwareDecodingType()
{
    //默认值为-1,表示使用软解
    HardDecodingType type = DefaultSoftDecoding;

    updatePredefinedList();

    int size = predefinedList.size();
    if (size > 0) {
        updatePcieList();

        foreach (PciePredefined predefined, predefinedList) {
            foreach(PcieInfo info, pcieList) {
                if ( ((predefined.vid == info.vid) || (predefined.vid == 0xFFFF))
                         && ((predefined.pid == info.pid) || (predefined.pid == 0xFFFF))
                         && ((predefined.cid == info.cid) || (predefined.cid == 0xFFFFFF))) {
                    printf("Find %s device(%04x:%04x.%04x), "
                               "use predefine score: %d\n",
                               predefined.description.toUtf8().data(),
                               predefined.vid, predefined.pid,
                               predefined.cid, predefined.score);
                    char vidstr[128] = {0};
                    snprintf(vidstr, sizeof(vidstr), "%04x", predefined.vid);
                    //printf("vidstr:%s\n", vidstr);
                    QString vid = QString::fromStdString(std::string(vidstr));
                    //qDebug() << "vid:" << vid;
                    //Find AMD Graphics Card device(1002:ffff.ffffff), use predefine score: 300
                    if (predefined.description == "AMD Graphics Card" && vid == "1002") {
                        type = AmdVdpau;
                    }
                    else if (predefined.description == "JINGJIA MICRO JM7200 Graphics Card" && vid == "0731") {
                        type = Jm7200Xv;
                    }
                    else if (predefined.description == "709 GP101 Graphics Card" && vid == "0709") {
                        type = Gp101X11;
                    }
                    else if (predefined.description == "SM750/SM768" && vid == "126f") {
                        type = Sm768Xv;
                    }
                    return type;
                }
            }
        }
    }

    return type;
}
