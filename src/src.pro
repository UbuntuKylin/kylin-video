TARGET = kylin-video
TEMPLATE = app
LANGUAGE = C++

CONFIG += c++14
CONFIG += qt warn_on
CONFIG += release
CONFIG += link_pkgconfig

QT += network xml

RESOURCES = res.qrc

DEFINES += SINGLE_INSTANCE

inst1.files += res/kylin-video.png
inst1.path = /usr/share/pixmaps
inst2.files += ../kylin-video.desktop
inst2.path = /usr/share/applications
target.source  += $$TARGET
target.path = /usr/bin
INSTALLS += inst1 \
    inst2 \
    target

QMAKE_CPPFLAGS *= $(shell dpkg-buildflags --get CPPFLAGS)
QMAKE_CFLAGS   *= $(shell dpkg-buildflags --get CFLAGS)
QMAKE_CXXFLAGS *= $(shell dpkg-buildflags --get CXXFLAGS)
QMAKE_LFLAGS   *= $(shell dpkg-buildflags --get LDFLAGS)

isEqual(QT_MAJOR_VERSION, 5) {
	QT += widgets gui
}

unix {
        QT += gui-private dbus
        LIBS += $${QMAKE_LIBS_X11}
}

HEADERS += smplayer/mplayerversion.h \
        smplayer/mplayerprocess.h \
        smplayer/inforeadermplayer.h \
        smplayer/mpvprocess.h \
        smplayer/inforeadermpv.h \
        smplayer/version.h \
        smplayer/global.h \
        smplayer/paths.h \
        smplayer/colorutils.h \
        smplayer/subtracks.h \
        smplayer/tracks.h \
        smplayer/titletracks.h \
        smplayer/extensions.h \
        smplayer/desktopinfo.h \
        smplayer/myprocess.h \
        smplayer/playerprocess.h \
        smplayer/infoprovider.h \
        smplayer/mediadata.h \
        smplayer/mediasettings.h \
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
        smplayer/infofile.h \
        smplayer/translator.h \
        smplayer/languages.h \
        smplayer/filesettings.h \
        smplayer/filesettingsbase.h \
        smplayer/filesettingshash.h \
        smplayer/filehash.h \
        smplayer/prefwidget.h \
        smplayer/scrollermodule.h \
        smplayer/filters.h \
        smplayer/assstyles.h \
        smplayer/audioequalizer.h \
        smplayer/audioequalizerlist.h \
        smplayer/verticaltext.h \
        smplayer/eqslider.h \
        smplayer/chapters.h \
        smplayer/globalshortcuts/globalshortcuts.h \
        smplayer/globalshortcuts/globalshortcutsdialog.h \
        smplayer/discname.h \
        smplayer/lineedit_with_icon.h \
        smplayer/preferencesdialog.h \
        smplayer/prefgeneral.h \
        smplayer/prefperformance.h \
        smplayer/prefsubtitles.h \
        smplayer/prefscreenshot.h \
        smplayer/prefshortcut.h \
        smplayer/prefvideo.h \
        smplayer/prefaudio.h \
        smplayer/filepropertiesdialog.h \
        smplayer/audiodelaydialog.h \
        smplayer/inputurl.h \
        smplayer/errordialog.h \
        smplayer/preferences.h \
        smplayer/videopreview.h \
        titlewidget.h \
        bottomwidget.h \
        soundvolume.h \
        titlebutton.h \
        aboutdialog.h \
        playmask.h \
        timetip.h \
        esctip.h \
        tipwidget.h \
        messagedialog.h \
        helpdialog.h \
        supportformats.h \
        systembutton.h \
        playlist.h \
        playlistview.h \
        playlistmodel.h \
        playlistdelegate.h \
        datautils.h \
        myapplication.h \
        coverwidget.h \
        infoworker.h \
        kylinvideo.h \
        bottomcontroller.h \
        filterhandler.h \
        maskwidget.h \
        mainwindow.h \
        autohidecursorwidget.h \
        displaylayercomposer.h \
        videowindow.h \
        poweroffdialog.h \
        controllerworker.h \
        remotecontroller.h \
        utils.h

SOURCES	+= smplayer/version.cpp \
        smplayer/mplayerversion.cpp \
        smplayer/mplayerprocess.cpp \
        smplayer/inforeadermplayer.cpp \
        smplayer/mpvprocess.cpp \
        smplayer/inforeadermpv.cpp \
        smplayer/mpvoptions.cpp \
        smplayer/global.cpp \
        smplayer/paths.cpp \
        smplayer/colorutils.cpp \
        smplayer/subtracks.cpp \
        smplayer/tracks.cpp \
        smplayer/titletracks.cpp \
        smplayer/extensions.cpp \
        smplayer/desktopinfo.cpp \
        smplayer/myprocess.cpp \
        smplayer/playerprocess.cpp \
        smplayer/mplayeroptions.cpp \
        smplayer/infoprovider.cpp \
        smplayer/mediadata.cpp \
        smplayer/mediasettings.cpp \
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
        smplayer/infofile.cpp \
        smplayer/translator.cpp \
        smplayer/languages.cpp \
        smplayer/filesettings.cpp \
        smplayer/filesettingsbase.cpp \
        smplayer/filesettingshash.cpp \
        smplayer/filehash.cpp \
        smplayer/prefwidget.cpp \
        smplayer/scrollermodule.cpp \
        smplayer/filters.cpp \
        smplayer/assstyles.cpp \
        smplayer/audioequalizer.cpp \
        smplayer/audioequalizerlist.cpp \
        smplayer/verticaltext.cpp \
        smplayer/eqslider.cpp \
        smplayer/chapters.cpp \
        smplayer/globalshortcuts/globalshortcuts.cpp \
        smplayer/globalshortcuts/globalshortcutsdialog.cpp \
        smplayer/discname.cpp \
        smplayer/lineedit_with_icon.cpp \
        smplayer/preferencesdialog.cpp \
        smplayer/prefgeneral.cpp \
        smplayer/prefperformance.cpp \
        smplayer/prefsubtitles.cpp \
        smplayer/prefscreenshot.cpp \
        smplayer/prefshortcut.cpp \
        smplayer/prefvideo.cpp \
        smplayer/prefaudio.cpp \
        smplayer/filepropertiesdialog.cpp \
        smplayer/audiodelaydialog.cpp \
        smplayer/inputurl.cpp \
        smplayer/errordialog.cpp \
        smplayer/preferences.cpp \
        smplayer/videopreview.cpp \
        titlewidget.cpp \
        bottomwidget.cpp \
        soundvolume.cpp \
        titlebutton.cpp \
        aboutdialog.cpp \
        playmask.cpp \
        timetip.cpp \
        esctip.cpp \
        tipwidget.cpp \
        messagedialog.cpp \
        helpdialog.cpp \
        supportformats.cpp \
        systembutton.cpp \
        playlist.cpp \
        playlistview.cpp \
        playlistmodel.cpp \
        playlistdelegate.cpp \
        main.cpp \
        coverwidget.cpp \
        infoworker.cpp \
        kylinvideo.cpp \
        bottomcontroller.cpp \
        filterhandler.cpp \
        maskwidget.cpp \
        mainwindow.cpp \
        autohidecursorwidget.cpp \
        displaylayercomposer.cpp \
        videowindow.cpp \
        poweroffdialog.cpp \
        controllerworker.cpp \
        remotecontroller.cpp \
        utils.cpp

FORMS = smplayer/timedialog.ui \
        smplayer/preferencesdialog.ui \
        smplayer/prefgeneral.ui \
        smplayer/prefperformance.ui \
        smplayer/prefsubtitles.ui \
        smplayer/prefscreenshot.ui \
        smplayer/prefshortcut.ui \
        smplayer/prefvideo.ui \
        smplayer/prefaudio.ui \
        smplayer/filepropertiesdialog.ui \
        smplayer/inputurl.ui \
        smplayer/audiodelaydialog.ui \
        smplayer/errordialog.ui \
        helpdialog.ui \
        supportformats.ui \
        aboutdialog.ui \
        smplayer/globalshortcuts/globalshortcutsdialog.ui

# qtsingleapplication
contains( DEFINES, SINGLE_INSTANCE ) {
	INCLUDEPATH += qtsingleapplication
	DEPENDPATH += qtsingleapplication

	SOURCES += qtsingleapplication/qtsingleapplication.cpp qtsingleapplication/qtlocalpeer.cpp
	HEADERS += qtsingleapplication/qtsingleapplication.h qtsingleapplication/qtlocalpeer.h
}

#contains( DEFINES, GLOBALSHORTCUTS ) {
#        lessThan(QT_MAJOR_VERSION, 5) {
#                DEFINES -= GLOBALSHORTCUTS
#                message("GLOBALSHORTCUTS requires Qt 5. Disabled.")
#        }
#}


unix {
    UI_DIR = .ui
    MOC_DIR = .moc
    OBJECTS_DIR = .obj
}

#TRANSLATIONS += \
#    translation/kylin-video_zh_CN.ts
