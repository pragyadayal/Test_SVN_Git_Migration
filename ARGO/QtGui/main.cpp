///////////////////////////////////////////////////////////////////////////
//
//   MMM     MMM       AAA       TTTTTTTTTT  RRRRRR    IIIIIIII  XX   XX
//   MMMM   MMMM      AA AA          TT      RR   RR      II      XX XX
//   MM MM MM MM     AA   AA         TT      RR    RR     II       XXX
//   MM  MM   MM    AAAAAAAAA        TT      RRRRRRR      II       XXX
//   MM       MM   AA       AA       TT      RR  RR       II      XX XX
//   MM       MM  AA         AA      TT      RR   RR   IIIIIIII  XX   XX
//
//   Company      : Matrix Telecom Pvt. Ltd., Baroda, India.
//   Project      : Access Control System
//   Owner        : Kaival SHah
//   File         : main.cpp
//   Description  :
//////////////////////////////////////////////////////////////////////////////

// System Includes
#include <QtGui/QApplication>
#include <QTextCodec>
#include <QMetaType>
#include <QtGui/QFontDatabase>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "qmlapplicationviewer.h"
#include <QFont>

// Matrix Includes
#include "SocketComm.h"
#include "applcontroller.h"
#include "EnumFile.h"
#include "dispstr.h"
#include "EventFilter.h"
#include "QmlLogger.h"

//******** Globle Variable ******//
bool    multiLangEnblF;

// Definition of Static Variabel Declared in ApplController Class
SocketComm* ApplController :: socketComm;

//******** Function Declaration ******//

//******** Function Definations ******//
int main(int argc, char *argv[])
{
    qDebug() << "\n*******GUI APPLICATION: last Modified On" <<__DATE__ << "at" << __TIME__ <<"Wtih Q.T.Version:"<<QT_VERSION_STR;

    //Create the QApplication instance to manage the GUI application's control
    //flow and main settings and pass the arguments of main to it
    QApplication app(argc, argv);

    //this class object provides widget to display QtDeclarative userInterface
    QDeclarativeView viewer;
    EventFilter * filter = new EventFilter(&viewer);
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    // register class to use that class in QML
    qmlRegisterType<ApplController>("ApplControl", 1, 0,"ApplController");
    qmlRegisterType<EnumClass>("EnumClass", 1, 0, "EnumClass");
    qmlRegisterType<DispStr>("DispStr", 1, 0, "DispStr");
    qmlRegisterType<QmlLogger>("QmlLogger", 1, 0, "QmlLogger");
    qRegisterMetaType<DispStr::MSG_MULTI_LANGUAGE_STRING_e>("MSG_MULTI_LANGUAGE_STRING_e");

    //to remove the minimize, maximize and close toolbar at top
    viewer.setWindowFlags(Qt::FramelessWindowHint);
    app.installEventFilter(filter);
    viewer.setSource(QUrl("qrc:/qml/Appl/main.qml"));
    viewer.show();

    //enters the main event loop and waits until exit() is called,
    return app.exec();
}
