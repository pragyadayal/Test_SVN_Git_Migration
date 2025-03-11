TEMPLATE = app

CONFIG += c++11

SOURCES += main.cpp \
    ../../Common_Source/Linux_Devices_Common_Source/QtGui/SocketTask/SocketComm.cpp \
    ../../Common_Source/Linux_Devices_Common_Source/QtGui/applcontroller.cpp \
    ../../Common_Source/Linux_Devices_Common_Source/QtGui/EventFilter.cpp \
    ../../Common_Source/Linux_Devices_Common_Source/QtGui/QmlLogger/QmlLogger.cpp \
    ../../../../../../../Common_Include/Branches/Releases/V20R9.1/CommonMultiLanguage/QtGui/dispstr.cpp \
    ../../../../../../../Common_Include/Branches/Releases/V20R9.1/CommonMultiLanguage/Application/MxLanguageString.c

RESOURCES += \
    ARGOResource.qrc

INCLUDEPATH += ../../Common_Source/Linux_Devices_Common_Source/QtGui
INCLUDEPATH += ../../Common_Source/Linux_Devices_Common_Source/QtGui/SocketTask
INCLUDEPATH += ../../Common_Source/Linux_Devices_Common_Source/QtGui/QmlLogger
INCLUDEPATH += ../../Common_Source/Linux_Devices_Common_Source/QtGui/QmlLogger/plog
INCLUDEPATH += ../../Common_Source/Linux_Devices_Common_Source/QtGui/QmlLogger/plog/Appenders
INCLUDEPATH += ../../Common_Source/Linux_Devices_Common_Source/QtGui/QmlLogger/plog/Converters
INCLUDEPATH += ../../Common_Source/Linux_Devices_Common_Source/QtGui/QmlLogger/plog/Formatters
INCLUDEPATH += ../../../../../../../Common_Include/Branches/Releases/V20R9.1
INCLUDEPATH += ../../../../../../../Common_Include/Branches/Releases/V20R9.1/CommonMultiLanguage/QtGui

# Additional import path used to resolve QML modules in Creator's code model
QML_IMPORT_PATH =

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# Please do not modify the following two lines. Required for deployment.
include(qmlapplicationviewer/qmlapplicationviewer.pri)
qtcAddDeployment()

HEADERS += \
    ../../Common_Source/Linux_Devices_Common_Source/QtGui/SocketTask/SocketComm.h \
    EnumFile.h \
    ../../Common_Source/Linux_Devices_Common_Source/QtGui/applcontroller.h \
    ../../Common_Source/Linux_Devices_Common_Source/QtGui/EventFilter.h \
    ../../Common_Source/Linux_Devices_Common_Source/QtGui/QmlLogger/QmlLogger.h \
    ../../Common_Source/Linux_Devices_Common_Source/QtGui/QmlLogger/plog/Appenders/AndroidAppender.h \
    ../../Common_Source/Linux_Devices_Common_Source/QtGui/QmlLogger/plog/Appenders/ColorConsoleAppender.h \
    ../../Common_Source/Linux_Devices_Common_Source/QtGui/QmlLogger/plog/Appenders/ConsoleAppender.h \
    ../../Common_Source/Linux_Devices_Common_Source/QtGui/QmlLogger/plog/Appenders/DebugOutputAppender.h \
    ../../Common_Source/Linux_Devices_Common_Source/QtGui/QmlLogger/plog/Appenders/EventLogAppender.h \
    ../../Common_Source/Linux_Devices_Common_Source/QtGui/QmlLogger/plog/Appenders/IAppender.h \
    ../../Common_Source/Linux_Devices_Common_Source/QtGui/QmlLogger/plog/Appenders/RollingFileAppender.h \
    ../../Common_Source/Linux_Devices_Common_Source/QtGui/QmlLogger/plog/Appenders/SysLogAppender.h \
    ../../Common_Source/Linux_Devices_Common_Source/QtGui/QmlLogger/plog/Converters/NativeEOLConverter.h \
    ../../Common_Source/Linux_Devices_Common_Source/QtGui/QmlLogger/plog/Converters/UTF8Converter.h \
    ../../Common_Source/Linux_Devices_Common_Source/QtGui/QmlLogger/plog/Formatters/CsvFormatter.h \
    ../../Common_Source/Linux_Devices_Common_Source/QtGui/QmlLogger/plog/Formatters/FuncMessageFormatter.h \
    ../../Common_Source/Linux_Devices_Common_Source/QtGui/QmlLogger/plog/Formatters/MessageOnlyFormatter.h \
    ../../Common_Source/Linux_Devices_Common_Source/QtGui/QmlLogger/plog/Formatters/TxtFormatter.h \
    ../../Common_Source/Linux_Devices_Common_Source/QtGui/QmlLogger/plog/Init.h \
    ../../Common_Source/Linux_Devices_Common_Source/QtGui/QmlLogger/plog/Log.h \
    ../../Common_Source/Linux_Devices_Common_Source/QtGui/QmlLogger/plog/Logger.h \
    ../../Common_Source/Linux_Devices_Common_Source/QtGui/QmlLogger/plog/Record.h \
    ../../Common_Source/Linux_Devices_Common_Source/QtGui/QmlLogger/plog/Severity.h \
    ../../Common_Source/Linux_Devices_Common_Source/QtGui/QmlLogger/plog/Util.h \
    ../../Common_Source/Linux_Devices_Common_Source/QtGui/QmlLogger/plog/WinApi.h \
    ../../../../../../../Common_Include/Branches/Releases/V20R9.1/CommonMultiLanguage/QtGui/dispstr.h \

LIBS += -lssl -lcrypto    
