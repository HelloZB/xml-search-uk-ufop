#-------------------------------------------------
#
# Project created by QtCreator 2017-08-17T20:18:40
#
#-------------------------------------------------

QT       += core gui xml printsupport svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = xml-search
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += SIMPL_EXPORT_2_FILE=1


# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        widget.cpp \
    xmlsearcher.cpp \
    fltrrezwdgt.cpp \
    mysortfilterproxymodel.cpp \
    resizetablehelper.cpp \
    zbyratorfilehelper.cpp \
    filterwidgetform.cpp \
    settloader.cpp

HEADERS  += widget.h \
    xmlsearcher.h \
    fltrrezwdgt.h \
    mysortfilterproxymodel.h \
    resizetablehelper.h \
    zbyratorfilehelper.h \
    filterwidgetform.h \
    settloader.h

FORMS    += widget.ui \
    fltrrezwdgt.ui \
    filterwidgetform.ui

RESOURCES += \
    font.qrc \
    res.qrc


RC_FILE += file.rc
OTHER_FILES += \
    file.rc

DISTFILES += \
    favicon.ico \
    LICENSE.txt \
    README.md
