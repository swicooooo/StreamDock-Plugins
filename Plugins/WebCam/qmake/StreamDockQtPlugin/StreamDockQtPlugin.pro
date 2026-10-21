QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG -= debug debug_and_release
CONFIG += c++17 release

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# 版本
VERSION = 1.0.0
# 版本号（VERSION_STR可以在代码中调用）
DEFINES += VERSION_STR=\\\"$$VERSION\\\"
# Static/compile-time major version of the Stream Dock.
DEFINES += SD_VERSION_MAJOR=1
# Static/compile-time minor version of the Stream Dock.
DEFINES += SD_VERSION_MINOR=0
# Static/compile-time patch version of the Stream Dock.
DEFINES += SD_VERSION_PATCH=0
# 目标文件名
TARGET = "WebCam"

# 仅生成 Release，并将可执行文件直接输出到插件目录。
DEFINES += QT_NO_DEBUG_OUTPUT
DEFINES += QT_MESSAGELOGCONTEXT
DESTDIR = $$clean_path($$PWD/../../com.hotspot.webcam.sdPlugin)
# MOC命令将含Q_OBJECT的头文件转换为标准的头文件寄存的目录
MOC_DIR = ./temp/moc
# RCC将qrc文件转化为头文件所寄存的目录
RCC_DIR = ./temp/rcc
# UIC将ui转化为头文件所寄存的目录
UI_DIR = ./temp/ui
# 指定所有中间文件.o（.obj）放置的目录。
OBJECTS_DIR = ./temp/obj

SOURCES += \
    ExampleAction.cpp \
    ExamplePlugin.cpp \
    main.cpp

HEADERS += \
    Camera.hpp \
    ExampleAction.h \
    ExamplePlugin.h

include(../../SDK/SDK.pri)

LIBS += $$PWD/lib/OleAut32.Lib  \
        $$PWD/lib/Ole32.Lib  \

# Win
win32 {
    QMAKE_CFLAGS += /utf-8
    QMAKE_CXXFLAGS += /utf-8
}

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
