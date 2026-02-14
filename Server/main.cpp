#include <QCoreApplication>
#include <QDebug>
#include "server.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    Server server("MyLocalServer");

    if (!server.start()) {
        return 1;
    }

    qInfo() << "Press Ctrl+C to exit...";

    return app.exec();
}
