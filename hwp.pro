QT       += core gui serialport network printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS QT_USE_FAST_OPERATOR_PLUS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    datashow/dataarea.cpp \
    datashow/dock_popup.cpp \
    datashow/dock_tabbar.cpp \
    datashow/dock_tabwidget.cpp \
    datashow/input_common.cpp \
    datashow/subfilter.cpp \
    interfaces/formlayout.cpp \
    interfaces/getlocalip.cpp \
    interfaces/page_document.cpp \
    interfaces/page_tcpclient.cpp \
    interfaces/page_tcpserver.cpp \
    interfaces/tab_dataformat.cpp \
    interfaces/tab_interfaces.cpp \
    interfaces/treeitem_datafilter.cpp \
    main.cpp \
    interfaces/datahandler.cpp \
    interfaces/page_serialport.cpp \
    plotshow/algorithm_fft.cpp \
    plotshow/plot_common.cpp \
    plotshow/plot_fourier.cpp \
    plotshow/plot_multicurve.cpp \
    plotshow/qcustomplot.cpp \
    savefile.cpp \
    widget.cpp

HEADERS += \
    datashow/dataarea.h \
    datashow/dock_popup.h \
    datashow/dock_tabbar.h \
    datashow/dock_tabwidget.h \
    datashow/input_common.h \
    datashow/subfilter.h \
    interfaces/formlayout.h \
    interfaces/datahandler.h \
    interfaces/getlocalip.h \
    interfaces/page_document.h \
    interfaces/page_serialport.h \
    interfaces/page_tcpclient.h \
    interfaces/page_tcpserver.h \
    interfaces/tab_dataformat.h \
    interfaces/tab_interfaces.h \
    interfaces/treeitem_datafilter.h \
    plotshow/algorithm_fft.h \
    plotshow/plot_common.h \
    plotshow/plot_fourier.h \
    plotshow/plot_multicurve.h \
    plotshow/qcustomplot.h \
    savefile.h \
    widget.h

FORMS += \
    datashow/dataarea.ui \
    datashow/subfilter.ui \
    interfaces/page_document.ui \
    interfaces/page_serialport.ui \
    interfaces/page_tcpclient.ui \
    interfaces/page_tcpserver.ui \
    interfaces/tab_dataformat.ui \
    interfaces/tab_interfaces.ui \
    widget.ui

INCLUDEPATH += \
    ./interfaces
    ./datashow
    ./plotshow

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
