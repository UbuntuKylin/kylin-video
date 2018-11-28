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
#include "config.h"
#include "audioequalizerlist.h"
#include "assstyles.h"

class Recents;
class URLHistory;
class Filters;

class Preferences {

public:
	enum OSD { None = 0, Seek = 1, SeekTimer = 2, SeekTimerTotal = 3 };
	enum OnTop { NeverOnTop = 0, AlwaysOnTop = 1, WhilePlayingOnTop = 2 };
    enum PlayOrder { OrderPlay = 0, RandomPlay = 1, ListLoopPlay = 2 };
	enum Resize { Never = 0, Always = 1, Afterload = 2 };
	enum Priority { Realtime = 0, High = 1, AboveNormal = 2, Normal = 3,
                    BelowNormal = 4, Idle = 5 };
	enum WheelFunction { DoNothing = 1, Seeking = 2, Volume = 4, Zoom = 8,
                         ChangeSpeed = 16 };
	enum OptionState { Detect = -1, Disabled = 0, Enabled = 1 };
	enum H264LoopFilter { LoopDisabled = 0, LoopEnabled = 1, LoopDisabledOnHD = 2 };
	enum AutoAddToPlaylistFilter { NoFiles = 0, VideoFiles = 1, AudioFiles = 2, MultimediaFiles = 3, ConsecutiveFiles = 4 };

	Q_DECLARE_FLAGS(WheelFunctions, WheelFunction);

    Preferences(const QString &arch);
	virtual ~Preferences();

	virtual void reset();

	void save();
	void load();
	void setupScreenshotFolder();



    /* *******
       General
       ******* */
	QString mplayer_bin;
	QString vo; // video output
	QString ao; // audio output

	bool use_screenshot;

	QString screenshot_template;
	QString screenshot_format;
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



    /* ***********
       Performance
       *********** */
//    bool frame_drop;
//	bool hard_frame_drop;
    bool coreavc;
    H264LoopFilter h264_skip_loop_filter;
	int HD_height; //!< An HD is a video which height is equal or greater than this.

	int threads; //!< number of threads to use for decoding (-lavdopts threads <1-8>)
	QString hwdec; //!< hardware video decoding (mpv only)


    bool cache_auto;
    int cache_for_files;
    int cache_for_streams;
//	int cache_for_dvds;
//	int cache_for_vcds;
//	int cache_for_audiocds;

	/* *********
	   Subtitles
	   ********* */

    bool use_ass_subtitles;
    bool enable_ass_styles;
    int ass_line_spacing;

    bool use_forced_subs_only;

	QString subcp; // -subcp
	bool use_enca;
	QString enca_lang;
	bool sub_visibility;
    int subfuzziness;
    bool autoload_sub;

    bool subtitles_on_screenshots;

    // ASS styles
    AssStyles ass_styles;
    bool force_ass_styles; // Use ass styles even for ass files
    QString user_forced_ass_style; //!< Specifies a style defined by the user to be used with -ass-force-style

    //! If false, options requiring freetype won't be used
    bool freetype_support;

    /* ********
       Advanced
       ******** */

//#if USE_ADAPTER
    int adapter; //Screen for overlay. If -1 it won't be used.
//#endif

	bool use_idx; //!< Use -idx
	bool use_lavf_demuxer;

    // Let the user pass options to mplayer
    QString mplayer_additional_options;
    QString mplayer_additional_video_filters;
    QString mplayer_additional_audio_filters;

    bool log_mplayer;
    bool verbose_log;
//    bool autosave_mplayer_log;
//    QString mplayer_log_saveto;
    bool log_smplayer;
    QString log_filter;
    bool save_smplayer_log;

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

	/* *********
	   GUI stuff
	   ********* */

	bool fullscreen;
    OnTop stay_on_top;
    PlayOrder play_order;

	int resize_method; 	//!< Mainwindow resize method
	int wheel_function;

	WheelFunctions wheel_function_cycle;
	bool wheel_function_seeking_reverse;

	// Configurable seeking
	int seeking1; // By default 10s
	int seeking2; // By default 1m
	int seeking3; // By default 10m
	int seeking4; // For mouse wheel, by default 30s

//	bool update_while_seeking;
	int time_slider_drag_delay;

	//!< Pause the current file when the main window is not visible
    bool pause_when_hidden;

    bool preview_when_playing;

	AutoAddToPlaylistFilter media_to_add_to_playlist;

    QString playlist_key;
    QString next_key;
    QString prev_key;

    /* ***********
       Directories
       *********** */

	QString latest_dir; //!< Directory of the latest file loaded

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

//#if SELECT_TRACKS_ON_STARTUP
    int initial_audio_track;
    int initial_subtitle_track;
//#endif

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


    QString arch_type;
    QString m_snap;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(Preferences::WheelFunctions)

#endif
