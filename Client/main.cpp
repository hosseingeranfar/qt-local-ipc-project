#include <QCoreApplication>
#include <QTextStream>
#include <QDebug>
#include "client.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QTextStream cin(stdin);
    QTextStream cout(stdout);

    cout << "Please enter your message: " << Qt::flush;
    QString message = cin.readLine();

    if (message.isEmpty()) {
        cout << "Message is empty!" << Qt::endl;
        return 1;
    }

    Client client("MyLocalServer");
    client.sendMessage(message);

    return app.exec();
}
