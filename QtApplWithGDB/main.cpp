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
//   Project      : Gate Controller
//   Owner        : Kuldip Gor
//   File         : main.cpp
//   Description  :
//////////////////////////////////////////////////////////////////////////////

// System Includes
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <QtGui/QApplication>
#include "qmlapplicationviewer.h"

// Matrix Includes
#include "../SocketTask/SocketComm.h"
#include "applcontroller.h"
#include "EnumFile.h"

#define DUMP_FILE_SIZE				102400
#define DUMP_FILE1					"/matrix/applData/log/GuiCrashReport1.txt"
#define DUMP_FILE2					"/matrix/applData/log/GuiCrashReport2.txt"

// Function Prototype
static void HandlerFun(int dummy);

//******** Function Definations ******
int main(int argc, char *argv[])
{
    signal(SIGPIPE, SIG_IGN);
    signal(SIGSEGV, HandlerFun);

    qDebug() << "\n*******GATE UI APPLICATION: last Modified On" <<  __DATE__ << "at" << __TIME__;
    //QFont sansFont("DejaVuSans");

    //Create the QApplication instance to manage the GUI application's control
    //flow and main settings and pass the arguments of main to it
    QApplication app(argc, argv);

    //this class object provides widget to display QtDeclarative userInterface
    QDeclarativeView viewer;

    // register class to use that class in QML
    qmlRegisterType<ApplController>("ApplControl", 1, 0,"ApplController");
    qmlRegisterType<EnumClass>("EnumClass", 1, 0, "EnumClass");

    //to remove the minimize, maximize and close toolbar at top
    viewer.setWindowFlags(Qt::FramelessWindowHint);

    viewer.setSource(QUrl("qrc:/qml/GateController/main.qml"));

    viewer.show();

    //enters the main event loop and waits until exit() is called,
    return app.exec();
}


static void HandlerFun(int dummy)
{
    char 			nameBuf[128];
    char 			sysCmd[256];
    char			fileName[100];
    struct stat 	buf1, buf2;

    if(stat(DUMP_FILE1, &buf1) < 0)
    {
        sprintf(fileName, ">> %s", DUMP_FILE1);
    }
    else if(buf1.st_size < DUMP_FILE_SIZE)
    {
        sprintf(fileName, ">> %s", DUMP_FILE1);
    }
    else if(stat(DUMP_FILE2, &buf2) < 0)
    {
        sprintf(fileName, ">> %s", DUMP_FILE2);
    }
    else if(buf2.st_size < DUMP_FILE_SIZE)
    {
        sprintf(fileName, ">> %s", DUMP_FILE2);
    }
    else
    {
        // last modify file if LOG_FILE
        if(buf1.st_mtime > buf2.st_mtime)
        {
            sprintf(fileName, "> %s", DUMP_FILE2);
        }
        else
        {
            sprintf(fileName, "> %s", DUMP_FILE1);
        }
    }
    sprintf(sysCmd, "echo \"##### Dump Started ######\" %s", fileName);
    system(sysCmd);
    nameBuf[readlink("/proc/self/exe", nameBuf, 127)] = 0;

    sprintf(sysCmd, "gdb --batch -n %s %d -ex 'thread apply all bt full' %s", nameBuf, getpid(), fileName);
    system(sysCmd);
    exit(0);
}
