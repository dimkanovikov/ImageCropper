#-------------------------------------------------
#
# Project created by QtCreator 2011-10-27T06:42:05
#
#-------------------------------------------------

greaterThan(QT_MAJOR_VERSION, 4) {
	QT += widgets
}

TARGET = ImageCropper
TEMPLATE = app

SRCDIR = ../src
INCLUDEPATH += $${SRCDIR}

SOURCES += main.cpp \
    $${SRCDIR}/imagecropper.cpp \
    demowidget.cpp

HEADERS  += \
    $${SRCDIR}/imagecropper_p.h \
    $${SRCDIR}/imagecropper.h \
    $${SRCDIR}/imagecropper_e.h \
    demowidget.h

RESOURCES += \
    resources.qrc
