#include "client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <QMessageBox>
#include <QImage>
#include <QPainter>
#include <QPixmap>
#include <QFile>

Client::Client()
    : tcpSocket(NULL), blockSize(0), memBuf(NULL), size(0)
{
    imageLabel = new QLabel(this);
    tcpSocket = new QTcpSocket(this);

    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(getImage()));
    connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(displayError(QAbstractSocket::SocketError)));

    //tcpSocket->connectToHost(QHostAddress("192.168.3.120"), 4399);

    setWindowFlags(Qt::WindowMinimizeButtonHint|Qt::WindowCloseButtonHint);

    //drawOnClient();
}

Client::~Client()
{
    if(memBuf != NULL)
        free(memBuf);
}
void Client::getImage()
{
    QDataStream in(tcpSocket);
    in.setVersion(QDataStream::Qt_4_0);

    if (blockSize == 0) {
        if (tcpSocket->bytesAvailable() < (int)sizeof(quint32))
            return;
        in >> blockSize;
    }

    if (tcpSocket->bytesAvailable() < blockSize)
        return;

    //memBuf = (char *)malloc(blockSize);
    //memset(memBuf, 0, blockSize);
    //in >> memBuf;
    //in >> size;

    QFile file("image.ppm");
    if (!file.open(QIODevice::WriteOnly)) {
        return;
    }

    char * buf = (char *)malloc(65535);
    if(buf == NULL)
        return;

    QDataStream out(&file);
    while(tcpSocket->bytesAvailable() >= 65535) {
        in.readRawData(buf, 65535);
        out.writeRawData(buf, 65535);
    }

    if(tcpSocket->bytesAvailable() > 0) {
        int len = tcpSocket->bytesAvailable();
        in.readRawData(buf, len);
        out.writeRawData(buf, len);
    }

    //file.flush();

    file.close();
    free(buf);

    drawOnClient();

    show();
}

void Client::displayError(QAbstractSocket::SocketError socketError)
{
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
        QMessageBox::information(this, tr("remote Client"),
                                 tr("The host was not found. Please check the "
                                    "host name and port settings."));
        break;
    case QAbstractSocket::ConnectionRefusedError:
        QMessageBox::information(this, tr("remote Client"),
                                 tr("The connection was refused by the peer. "
                                    "Make sure the fortune server is running, "
                                    "and check that the host name and port "
                                    "settings are correct."));
        break;
    default:
        QMessageBox::information(this, tr("remote Client"),
                                 tr("The following error occurred: %1.")
                                 .arg(tcpSocket->errorString()));
    }
}

void Client::drawOnClient()
{
    //QImage image((unsigned char *)(memBuf+16), 1280, 800, QImage::Format_RGB32);
    QPixmap image("image.ppm");
    imageLabel->setPixmap(image);
    //imageLabel->setPixmap(QPixmap::fromImage(image));
    setCentralWidget(imageLabel);

    setFixedSize(image.size());
    //setFixedSize(1280, 800);
}

void Client::beginConnecting(const QHostAddress address, quint16 port)
{
    tcpSocket->connectToHost(address, port);
}
