#include "client.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Client client;
    //client.show();

    client.beginConnecting(QHostAddress("192.168.3.120"), 54321);

    return a.exec();
}
