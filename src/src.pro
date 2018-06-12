TARGET = kylin-video
TEMPLATE = app
LANGUAGE = C++

CONFIG += c++11
CONFIG += qt warn_on
CONFIG += release

QT += network xml

RESOURCES = res.qrc

DEFINES += SINGLE_INSTANCE

#inst1.files += res/kylin-video.png
#inst1.path = /usr/share/pixmaps
#inst2.files += ../kylin-video.desktop
#inst2.path = /usr/share/applications
target.source  += $$TARGET
target.path = /usr/bin
INSTALLS += target
#INSTALLS += inst1 \
#    inst2 \
#    target

QMAKE_CPPFLAGS *= $(shell dpkg-buildflags --get CPPFLAGS)
QMAKE_CFLAGS   *= $(shell dpkg-buildflags --get CFLAGS)
QMAKE_CXXFLAGS *= $(shell dpkg-buildflags --get CXXFLAGS)
QMAKE_LFLAGS   *= $(shell dpkg-buildflags --get LDFLAGS)

isEqual(QT_MAJOR_VERSION, 5) {
	QT += widgets gui
}

HEADERS += smplayer/config.h \
        smplayer/mplayerversion.h \
        smplayer/mplayerprocess.h \
        smplayer/inforeadermplayer.h \
        smplayer/mpvprocess.h \
        smplayer/inforeadermpv.h \
        smplayer/version.h \
        smplayer/global.h \
        smplayer/paths.h \
        smplayer/helper.h \
        smplayer/colorutils.h \
        smplayer/subtracks.h \
        smplayer/tracks.h \
        smplayer/titletracks.h \
        smplayer/extensions.h \
        smplayer/desktopinfo.h \
        smplayer/myprocess.h \
        smplayer/playerid.h \
        smplayer/playerprocess.h \
        smplayer/infoprovider.h \
        smplayer/mplayerwindow.h \
        smplayer/mediadata.h \
        smplayer/mediasettings.h \
        smplayer/preferences.h \
        smplayer/images.h \
        smplayer/inforeader.h \
        smplayer/deviceinfo.h \
        smplayer/recents.h \
        smplayer/urlhistory.h \
        smplayer/core.h \
        smplayer/shortcutgetter.h \
        smplayer/actionseditor.h \
        smplayer/filechooser.h \
        smplayer/mycombobox.h \
        smplayer/mylineedit.h \
        smplayer/tristatecombo.h \
        smplayer/myslider.h \
        smplayer/timeslider.h \
        smplayer/myaction.h \
        smplayer/myactiongroup.h \
        smplayer/filedialog.h \
        smplayer/timedialog.h \
        smplayer/cleanconfig.h \
        smplayer/kylinvideo.h \
        smplayer/myapplication.h \
        smplayer/infofile.h \
        smplayer/translator.h \
        smplayer/languages.h \
        smplayer/filesettings.h \
        smplayer/videopreview.h \
        smplayer/prefwidget.h \
        merge/basegui.h \
        merge/playlist.h \
        merge/lineedit_with_icon.h \
        merge/preferencesdialog.h \
        merge/prefgeneral.h \
        merge/prefperformance.h \
        merge/prefsubtitles.h \
        merge/prefscreenshot.h \
        merge/prefshortcut.h \
        merge/prefvideo.h \
        merge/prefaudio.h \
        merge/filepropertiesdialog.h \
        merge/audiodelaydialog.h \
        merge/inputurl.h \
        merge/errordialog.h \
        kylin/titlewidget.h \
        kylin/bottomwidget.h \
        kylin/soundvolume.h \
        kylin/playlistview.h \
        kylin/playlistitem.h \
        kylin/titlebutton.h \
        kylin/aboutdialog.h \
        kylin/playmask.h \
        kylin/timetip.h \
        kylin/esctip.h \
        kylin/tipwidget.h \
        kylin/messagedialog.h \
        kylin/helpdialog.h \
        kylin/supportformats.h \
        kylin/systembutton.h

SOURCES	+= smplayer/version.cpp \
        smplayer/mplayerversion.cpp \
        smplayer/mplayerprocess.cpp \
        smplayer/inforeadermplayer.cpp \
        smplayer/mpvprocess.cpp \
        smplayer/inforeadermpv.cpp \
        smplayer/mpvoptions.cpp \
        smplayer/global.cpp \
        smplayer/paths.cpp \
        smplayer/helper.cpp \
        smplayer/colorutils.cpp \
        smplayer/subtracks.cpp \
        smplayer/tracks.cpp \
        smplayer/titletracks.cpp \
        smplayer/extensions.cpp \
        smplayer/desktopinfo.cpp \
        smplayer/myprocess.cpp \
        smplayer/playerid.cpp \
        smplayer/playerprocess.cpp \
        smplayer/mplayeroptions.cpp \
        smplayer/infoprovider.cpp \
        smplayer/mplayerwindow.cpp \
        smplayer/mediadata.cpp \
        smplayer/mediasettings.cpp \
        smplayer/preferences.cpp \
        smplayer/images.cpp \
        smplayer/inforeader.cpp \
        smplayer/deviceinfo.cpp \
        smplayer/recents.cpp \
        smplayer/urlhistory.cpp \
        smplayer/core.cpp \
        smplayer/shortcutgetter.cpp \
        smplayer/actionseditor.cpp \
        smplayer/filechooser.cpp \
        smplayer/mycombobox.cpp \
        smplayer/mylineedit.cpp \
        smplayer/tristatecombo.cpp \
        smplayer/myslider.cpp \
        smplayer/timeslider.cpp \
        smplayer/myaction.cpp \
        smplayer/myactiongroup.cpp \
        smplayer/filedialog.cpp \
        smplayer/timedialog.cpp \
        smplayer/cleanconfig.cpp \
        smplayer/kylinvideo.cpp \
        smplayer/main.cpp \
        smplayer/infofile.cpp \
        smplayer/translator.cpp \
        smplayer/languages.cpp \
        smplayer/filesettings.cpp \
        smplayer/videopreview.cpp \
        smplayer/prefwidget.cpp \
        merge/lineedit_with_icon.cpp \
        merge/basegui.cpp \
        merge/playlist.cpp \
        merge/preferencesdialog.cpp \
        merge/prefgeneral.cpp \
        merge/prefperformance.cpp \
        merge/prefsubtitles.cpp \
        merge/prefscreenshot.cpp \
        merge/prefshortcut.cpp \
        merge/prefvideo.cpp \
        merge/prefaudio.cpp \
        merge/filepropertiesdialog.cpp \
        merge/audiodelaydialog.cpp \
        merge/inputurl.cpp \
        merge/errordialog.cpp \
        kylin/titlewidget.cpp \
        kylin/bottomwidget.cpp \
        kylin/soundvolume.cpp \
        kylin/playlistview.cpp \
        kylin/playlistitem.cpp \
        kylin/titlebutton.cpp \
        kylin/aboutdialog.cpp \
        kylin/playmask.cpp \
        kylin/timetip.cpp \
        kylin/esctip.cpp \
        kylin/tipwidget.cpp \
        kylin/messagedialog.cpp \
        kylin/helpdialog.cpp \
        kylin/supportformats.cpp \
        kylin/systembutton.cpp

FORMS = smplayer/timedialog.ui \
        merge/preferencesdialog.ui \
        merge/prefgeneral.ui \
        merge/prefperformance.ui \
        merge/prefsubtitles.ui \
        merge/prefscreenshot.ui \
        merge/prefshortcut.ui \
        merge/prefvideo.ui \
        merge/prefaudio.ui \
        merge/filepropertiesdialog.ui \
        merge/inputurl.ui \
        merge/audiodelaydialog.ui \
        merge/errordialog.ui \
        kylin/helpdialog.ui \
        kylin/supportformats.ui \
        kylin/aboutdialog.ui


# qtsingleapplication
contains( DEFINES, SINGLE_INSTANCE ) {
	INCLUDEPATH += qtsingleapplication
	DEPENDPATH += qtsingleapplication

	SOURCES += qtsingleapplication/qtsingleapplication.cpp qtsingleapplication/qtlocalpeer.cpp
	HEADERS += qtsingleapplication/qtsingleapplication.h qtsingleapplication/qtlocalpeer.h
}

unix {
    UI_DIR = .ui
    MOC_DIR = .moc
    OBJECTS_DIR = .obj
}

TRANSLATIONS += \
    translations/kylin-video_zh_CN.ts
