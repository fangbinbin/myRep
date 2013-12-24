#ifndef CLIENT_H
#define CLIENT_H

#include <QWidget>
#include <QtNetwork>
#include <QLabel>
#include <QMainWindow>

class Client : public QMainWindow
{
    Q_OBJECT

public:
    Client();
    ~Client();

    void beginConnecting(const QHostAddress, quint16 port);

private slots:
    void getImage();
    void displayError(QAbstractSocket::SocketError socketError);

private:
    void drawOnClient();
    QTcpSocket *tcpSocket;

    quint32 blockSize;
    char *memBuf;

    QLabel* imageLabel;

    quint32 size;
};

#endif // CLIENT_H
