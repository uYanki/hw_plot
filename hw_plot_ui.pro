QT       += core gui serialport network printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    algorithm_fft.cpp \
    main.cpp \
    qcustomplot.cpp \
    tab_dataformat.cpp \
    tab_interfaces.cpp \
    uyk_custom_action.cpp \
    uyk_delayms.cpp \
    uyk_savefile.cpp \
    uyk_treeitem_channel.cpp \
    uyk_treeitem_command.cpp \
    uyk_treeitem_common.cpp \
    uykplot_common.cpp \
    uykplot_fourier.cpp \
    uykplot_multicurve.cpp \
    widget.cpp

HEADERS += \
    algorithm_fft.h \
    qcustomplot.h \
    tab_dataformat.h \
    tab_interfaces.h \
    uyk_custom_action.h \
    uyk_delayms.h \
    uyk_savefile.h \
    uyk_treeitem_channel.h \
    uyk_treeitem_command.h \
    uyk_treeitem_common.h \
    uykplot_common.h \
    uykplot_fourier.h \
    uykplot_multicurve.h \
    widget.h

FORMS += \
    tab_dataformat.ui \
    tab_interfaces.ui \
    widget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
