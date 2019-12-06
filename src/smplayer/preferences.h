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


#ifndef _PREFERENCES_H_
#define _PREFERENCES_H_

/* Global settings */

#include <QString>
#include <QStringList>
#include <QSize>
#include <QMap>
#include <QMetaType>
#include <QDir>

#include "../smplayer/audioequalizerlist.h"
#include "../smplayer/assstyles.h"
#include "../datautils.h"

class Recents;
class URLHistory;
class Filters;

class PciePredefined
{
public:
    PciePredefined() {};

    int vid;
    int pid;
    int cid;
    int score;
    QString description;

};
class PcieInfo
{
public:
    PcieInfo() {};

    int vid;
    int pid;
    int cid;
};

Q_DECLARE_METATYPE(PciePredefined)
Q_DECLARE_METATYPE(PcieInfo)

class Preferences {

public:
    enum HardDecodingType {
        DefaultSoftDecoding = -1,
        AmdVdpau = 0,
        Jm7200Xv,
        Gp101X11,
        Sm768Xv,
        Others
        //    1. A卡，vdpau；
        //    2. JM7200，xv；
        //    3. GP101，x11(可能会改变)；
        //    4. SM768，xv；
        //    5. 其他，软解；
    };

    enum OSD { NoneOsd = 0, Seek = 1, SeekTimer = 2, SeekTimerTotal = 3 };
	enum OnTop { NeverOnTop = 0, AlwaysOnTop = 1, WhilePlayingOnTop = 2 };
    enum PlayOrder { OrderPlay = 0, RandomPlay = 1, ListLoopPlay = 2 };
	enum Resize { Never = 0, Always = 1, Afterload = 2 };
	enum Priority { Realtime = 0, High = 1, AboveNormal = 2, Normal = 3,
                    BelowNormal = 4, Idle = 5 };
	enum WheelFunction { DoNothing = 1, Seeking = 2, Volume = 4, Zoom = 8,
                         ChangeSpeed = 16 };
    enum DragFunction { DragDisabled = 0, MoveWindow = 1, Gestures = 2 };
	enum OptionState { Detect = -1, Disabled = 0, Enabled = 1 };
	enum H264LoopFilter { LoopDisabled = 0, LoopEnabled = 1, LoopDisabledOnHD = 2 };
	enum AutoAddToPlaylistFilter { NoFiles = 0, VideoFiles = 1, AudioFiles = 2, MultimediaFiles = 3, ConsecutiveFiles = 4 };
    enum Streaming { NoStreaming = 0, StreamingAuto = 1, StreamingYT = 2, StreamingYTDL = 3 };

	Q_DECLARE_FLAGS(WheelFunctions, WheelFunction);

    Preferences(const QString &arch);
	virtual ~Preferences();

	virtual void reset();

    VideoPtr generateVedioData(const QString &filepath, QString &name, double duration);
    const VideoPtr video(const QString &filepath) const;
    bool isEmpty() const;
    bool contains(const VideoPtr video) const;
    bool contains(const QString &filepath) const;
    void updatePlaylist(QStringList playlist);

	void save();
	void load();

    void setupScreenshotFolder();
    void updatePredefinedList();
    void updatePcieList();
    HardDecodingType getHardwareDecodingType();

    /* *******
       General
       ******* */
    int config_version;

	QString mplayer_bin;
	QString vo; // video output
	QString ao; // audio output

	bool use_screenshot;
//#ifdef MPV_SUPPORT
    QString screenshot_template;
    QString screenshot_format;
//#endif
	QString screenshot_directory;

    // Possibility to remember all media settings
    bool remember_media_settings;
    bool remember_time_pos;
    bool remember_stream_settings;

    QString alang;
    QString slang;

	// Video
	bool use_direct_rendering;
	bool use_double_buffer;
	bool use_soft_video_eq;
	bool use_slices;
    int autoq; 	//!< Postprocessing quality


//#ifdef ADD_BLACKBORDERS_FS
    bool add_blackborders_on_fullscreen;
//#endif

    bool disable_screensaver;

    struct VDPAU_settings {
        bool ffh264vdpau;
        bool ffmpeg12vdpau;
        bool ffwmv3vdpau;
        bool ffvc1vdpau;
        bool ffodivxvdpau;
        bool ffhevcvdpau;
        bool disable_video_filters;
    } vdpau;

	// Audio
	bool use_soft_vol;
	int softvol_max;
    OptionState use_scaletempo;
    bool use_hwac3; // -afm hwac3
    bool use_audio_equalizer;

	// Global volume options
	bool global_volume;
	int volume;
	bool mute;

    // Global equalizer
    bool global_audio_equalizer;
    AudioEqualizerList audio_equalizer;

    bool autosync;
    int autosync_factor;

    // For the -mc option
    bool use_mc;
    double mc_value;

    // When playing a mp4 file, it will use a m4a file for audio if a there's a file with same name but extension m4a
    bool autoload_m4a;
    int min_step; //<! Step to increase of decrease the controls for color, contrast, brightness and so on

	// Misc
	int osd;
	double osd_scale; // mpv
	double subfont_osd_scale; // mplayer
	int osd_delay; //<! Delay in ms to show the OSD.
//#ifdef MPV_SUPPORT
    bool osd_fractions; //<! Show OSD times with milliseconds.
//#endif
    int osd_bar_pos;
    int osd_show_filename_duration;

    QString file_settings_method; //!< Method to be used for saving file settings

    bool tablet_mode;



    /* ***************
       Drives (CD/DVD)
       *************** */

    QString dvd_device;
    QString cdrom_device;
//#ifdef BLURAY_SUPPORT
//	QString bluray_device;
//#endif

//#ifdef Q_OS_WIN
//	bool enable_audiocd_on_windows;
//#endif

    int vcd_initial_title;

//#if DVDNAV_SUPPORT
//	bool use_dvdnav; //!< Opens DVDs using dvdnav: instead of dvd:
//#endif




    /* ***********
       Performance
       *********** */
    bool frame_drop;
    bool hard_frame_drop;
    bool coreavc;
    H264LoopFilter h264_skip_loop_filter;
	int HD_height; //!< An HD is a video which height is equal or greater than this.

	int threads; //!< number of threads to use for decoding (-lavdopts threads <1-8>)
	QString hwdec; //!< hardware video decoding (mpv only)


    bool cache_auto;
    int cache_for_files;
    int cache_for_streams;
    int cache_for_dvds;
    int cache_for_vcds;
    int cache_for_audiocds;

	/* *********
	   Subtitles
	   ********* */

	QString subcp; // -subcp
	bool use_enca;
	QString enca_lang;
    int subfuzziness;
    bool autoload_sub;

    bool use_ass_subtitles;
    bool enable_ass_styles;
    int ass_line_spacing;

    bool use_forced_subs_only;

    bool sub_visibility;

    bool subtitles_on_screenshots;

    OptionState change_sub_scale_should_restart;

    //! If true, loading an external subtitle will be done
    //! by using the sub_load slave command. Otherwise
    //! mplayer will be restarted.
    bool fast_load_sub;

    // ASS styles
    AssStyles ass_styles;
    bool force_ass_styles; // Use ass styles even for ass files
    QString user_forced_ass_style; //!< Specifies a style defined by the user to be used with -ass-force-style

    //! If false, options requiring freetype won't be used
    bool freetype_support;

    /* ********
       Advanced
       ******** */

    bool use_mplayer_window;

	bool use_idx; //!< Use -idx
	bool use_lavf_demuxer;

    // Let the user pass options to mplayer
    QString mplayer_additional_options;
    QString mplayer_additional_video_filters;
    QString mplayer_additional_audio_filters;

    bool verbose_log;
    bool repaint_video_background;

	//! If true it will autoload edl files with the same name of the file
    //! to play
	bool use_edl_files;

//#ifdef MPLAYER_SUPPORT
    //! If true it will pass to mplayer the -playlist option
    bool use_playlist_option;
//#endif

	//! Preferred connection method: ipv4 or ipv6
	bool prefer_ipv4;

	//! Windows only. If true, smplayer will pass short filenames to mplayer.
	//! To workaround a bug in mplayer.
	bool use_short_pathnames; 

	//! If false, -brightness, -contrast and so on, won't be passed to
	//! mplayer. It seems that some graphic cards don't support those options.
	bool change_video_equalizer_on_startup;

	//! If true, smplayer will use the prefix pausing_keep_force to keep
	//! the pause on slave commands. This experimental prefix was added
	//! in mplayer svn r27665.
    bool use_pausing_keep_force;

	OptionState use_correct_pts; //!< Pass -correct-pts to mplayer

	QString actions_to_run; //!< List of actions to run every time a video loads.

	//! Show file tag in window title
	bool show_tag_in_window_title;

	int time_to_kill_mplayer;

//#ifdef MPV_SUPPORT
    QString mpv_osd_media_info;
//#endif
//#ifdef MPLAYER_SUPPORT
    QString mplayer_osd_media_info;
//#endif

//#ifdef MPV_SUPPORT
    bool emulate_mplayer_ab_section;
//#endif

    bool use_native_open_dialog;

	/* *********
	   GUI stuff
	   ********* */

	bool fullscreen;
    bool start_in_fullscreen;
    bool compact_mode;
    OnTop stay_on_top;
    int size_factor;

    PlayOrder play_order;

	int resize_method; 	//!< Mainwindow resize method

    bool center_window; //!< Center the main window when playback starts
    bool center_window_if_outside; //!< Center the main window after an autoresize if it's outside of the screen

//#ifdef GLOBALSHORTCUTS
    bool use_global_shortcuts;
    int global_shortcuts_grabbed_keys;
//#endif

    // Function of mouse buttons:
    QString mouse_left_click_function;
    QString mouse_right_click_function;
    QString mouse_double_click_function;
    QString mouse_middle_click_function;
    QString mouse_xbutton1_click_function;
    QString mouse_xbutton2_click_function;
    int wheel_function;

    WheelFunctions wheel_function_cycle;
    bool wheel_function_seeking_reverse;

    int drag_function;

	// Configurable seeking
	int seeking1; // By default 10s
	int seeking2; // By default 1m
	int seeking3; // By default 10m
	int seeking4; // For mouse wheel, by default 30s

    bool update_while_seeking;
//#if ENABLE_DELAYED_DRAGGING
    int time_slider_drag_delay;
//#endif

    //! If true, seeking will be done using a
    //! percentage (with fractions) instead of time.
    bool relative_seeking;

    bool precise_seeking; //! Enable precise_seeking (only available with mpv)

    bool reset_stop; //! Pressing the stop button resets the position

    //! If true, the left click in the video is delayed some ms
    //! to check if the user double clicked
    bool delay_left_click;

    QString language;
    QString iconset;

    //! Number of times to show the balloon remembering that the program
    //! is still running in the system tray.
    int balloon_count;

    //! If true, the position of the main window will be saved before
    //! entering in fullscreen and will restore when going back to
    //! window mode.
    bool restore_pos_after_fullscreen;

    bool save_window_size_on_exit;

    //! Close the main window when a file or playlist finish
    bool close_on_finish;

    QString default_font;

	//!< Pause the current file when the main window is not visible
    bool pause_when_hidden;

    //!< Allow frre movement of the video window
    bool allow_video_movement;

    QString gui; //!< The name of the GUI to use

    int gui_minimum_width;
    QSize default_size; // Default size of the main window

    bool report_mplayer_crashes;

    bool reported_mplayer_is_old;

    bool auto_add_to_playlist; //!< Add files to open to playlist
    AutoAddToPlaylistFilter media_to_add_to_playlist;

    bool animated_logo;

    bool preview_when_playing;
//    QString playlist_key;
//    QString next_key;
//    QString prev_key;

    /* ***********
       Directories
       *********** */

    QString latest_dir; //!< Directory of the latest file loaded
//    QString last_dvd_directory;
//    bool save_dirs; // Save or not the latest dirs


    /* **************
       Initial values
       ************** */
    double initial_sub_scale;
    double initial_sub_scale_ass;
    int initial_volume;
    int initial_contrast;
    int initial_brightness;
    int initial_hue;
    int initial_saturation;
    int initial_gamma;

    AudioEqualizerList initial_audio_equalizer;

    //! Default value for zoom (1.0 = no zoom)
    double initial_zoom_factor;

    //! Default value for position of subtitles on screen
    //! 100 = 100% at the bottom
    int initial_sub_pos;

//#ifdef INITIAL_BLACKBORDERS
    bool initial_blackborders;
//#endif

    bool initial_postprocessing; //!< global postprocessing filter
    bool initial_volnorm;

    int initial_deinterlace;

    int initial_audio_channels;
    int initial_stereo_mode;

    int initial_audio_track;
    int initial_subtitle_track;


    /* ************
       MPlayer info
       ************ */
	int mplayer_detected_version; 	//!< Latest version of mplayer parsed

	//! Version of mplayer supplied by the user which will be used if
	//! the version can't be parsed from mplayer output
	int mplayer_user_supplied_version;

    /* *******
       History
       ******* */
	Recents * history_recents;
	URLHistory * history_urls;

    /* *******
       Filters
       ******* */
    Filters * filters;


    /* *********
       Kylin Video info
       ********* */
    QString arch_type;
    QString m_snap;
    QMap<QString, VideoPtr> m_videoMap;

    QList<PciePredefined> predefinedList;
    QList<PcieInfo> pcieList;
    HardDecodingType m_decoingType;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(Preferences::WheelFunctions)

#endif
