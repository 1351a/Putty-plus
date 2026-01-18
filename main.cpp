#include "widget.h"
#include <QTranslator>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    // QTranslator translator;
    // translator.load("D:/QtProject/SSHClient/SSHClient_za_CN.qm");
    // app.installTranslator(&translator);

    Widget w;
    w.show();
    return app.exec();
}
