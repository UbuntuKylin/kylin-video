TARGET = kylin-video
TEMPLATE = app
LANGUAGE = C++

CONFIG += c++11
CONFIG += qt warn_on
CONFIG += release
#CONFIG += debug
#CONFIG += console

QT += network xml

RESOURCES = res.qrc

DEFINES += SINGLE_INSTANCE

QMAKE_CPPFLAGS *= $(shell dpkg-buildflags --get CPPFLAGS)
QMAKE_CFLAGS   *= $(shell dpkg-buildflags --get CFLAGS)
QMAKE_CXXFLAGS *= $(shell dpkg-buildflags --get CXXFLAGS)
QMAKE_LFLAGS   *= $(shell dpkg-buildflags --get LDFLAGS)

isEqual(QT_MAJOR_VERSION, 5) {
	QT += widgets gui
}

HEADERS += config.h \
        mplayerversion.h \
        mplayerprocess.h \
        inforeadermplayer.h \
        mpvprocess.h \
        inforeadermpv.h \
	version.h \
	global.h \
	paths.h \
	helper.h \
	colorutils.h \
	subtracks.h \
	tracks.h \
	titletracks.h \
	extensions.h \
	desktopinfo.h \
	myprocess.h \
	playerid.h \
	playerprocess.h \
	infoprovider.h \
	mplayerwindow.h \
	mediadata.h \
	mediasettings.h \
	preferences.h \
	images.h \
	inforeader.h \
	deviceinfo.h \
	recents.h \
	urlhistory.h \
	core.h \
	shortcutgetter.h \
	actionseditor.h \
	filechooser.h \
	lineedit_with_icon.h \
	mycombobox.h \
        mylineedit.h \
	tristatecombo.h \
	myslider.h \
	timeslider.h \
	myaction.h \
	myactiongroup.h \
	filedialog.h \
	errordialog.h \
	timedialog.h \
	basegui.h \
	cleanconfig.h \
	smplayer.h \
        myapplication.h \
        titlewidget.h \
        bottomwidget.h \
        soundvolume.h \
        playlist.h \
        playlistview.h \
        playlistitem.h \
        preferencesdialog.h \
        prefgeneral.h \
        prefwidget.h \
        prefperformance.h \
        prefsubtitles.h \
        prefscreenshot.h \
        prefshortcut.h \
        prefvideo.h \
        prefaudio.h \
        titlebutton.h \
        filepropertiesdialog.h \
        infofile.h \
        overlaywidget.h \
        aboutdialog.h \
        systemtray.h \
        translator.h \
        languages.h \
        playmask.h \
        timetip.h \
        esctip.h \
        tipwidget.h \
        audiodelaydialog.h \
#        logwindow.h \
        filesettings.h \
        messagedialog.h \
        videopreview.h \
        shortcutswidget.h \
        helpdialog.h \
        supportformats.h \
        supportshortcuts.h \
        inputurl.h \
        systembutton.h

SOURCES	+= version.cpp \
        mplayerversion.cpp \
        mplayerprocess.cpp \
        inforeadermplayer.cpp \
        mpvprocess.cpp \
        inforeadermpv.cpp \
        mpvoptions.cpp \
	global.cpp \
	paths.cpp \
	helper.cpp \
	colorutils.cpp \
	subtracks.cpp \
	tracks.cpp \
	titletracks.cpp \
	extensions.cpp \
	desktopinfo.cpp \
	myprocess.cpp \
	playerid.cpp \
	playerprocess.cpp \
        mplayeroptions.cpp \
	infoprovider.cpp \
	mplayerwindow.cpp \
	mediadata.cpp \
	mediasettings.cpp \
	preferences.cpp \
	images.cpp \
	inforeader.cpp \
	deviceinfo.cpp \
	recents.cpp \
	urlhistory.cpp \
	core.cpp \
	shortcutgetter.cpp \
	actionseditor.cpp \
	filechooser.cpp \
	lineedit_with_icon.cpp \
	mycombobox.cpp \
        mylineedit.cpp \
	tristatecombo.cpp \
	myslider.cpp \
	timeslider.cpp \
	myaction.cpp \
	myactiongroup.cpp \
	filedialog.cpp \
	errordialog.cpp \
	timedialog.cpp \
	basegui.cpp \
	cleanconfig.cpp \
	smplayer.cpp \
        main.cpp \
        titlewidget.cpp \
        bottomwidget.cpp \
        soundvolume.cpp \
        playlist.cpp \
        playlistview.cpp \
        playlistitem.cpp \
        preferencesdialog.cpp \
        prefgeneral.cpp \
        prefwidget.cpp \
        prefperformance.cpp \
        prefsubtitles.cpp \
        prefscreenshot.cpp \
        prefshortcut.cpp \
        prefvideo.cpp \
        prefaudio.cpp \
        titlebutton.cpp \
        filepropertiesdialog.cpp \
        infofile.cpp \
        overlaywidget.cpp \
        aboutdialog.cpp \
        systemtray.cpp \
        translator.cpp \
        languages.cpp \
        playmask.cpp \
        timetip.cpp \
        esctip.cpp \
        tipwidget.cpp \
        audiodelaydialog.cpp \
#        logwindow.cpp \
        filesettings.cpp \
        messagedialog.cpp \
        videopreview.cpp \
        shortcutswidget.cpp \
        helpdialog.cpp \
        supportformats.cpp \
        supportshortcuts.cpp \
        inputurl.cpp \
        systembutton.cpp

FORMS = errordialog.ui \
        timedialog.ui \
        preferencesdialog.ui \
        prefgeneral.ui \
        prefperformance.ui \
        prefsubtitles.ui \
        prefscreenshot.ui \
        prefshortcut.ui \
        prefvideo.ui \
        prefaudio.ui \
        filepropertiesdialog.ui \
        aboutdialog.ui \
        audiodelaydialog.ui \
#        logwindowbase.ui \
        helpdialog.ui \
        supportformats.ui \
        supportshortcuts.ui \
        inputurl.ui

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
