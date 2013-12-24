#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QStringList>
#include <libvirt/libvirt.h>

QT_BEGIN_NAMESPACE
class QTcpServer;
class QNetworkSession;
QT_END_NAMESPACE

class Server : public QObject
{
    Q_OBJECT
public:
    explicit Server(QObject *parent = 0);
    ~Server();

signals:

public slots:
    void sessionOpened();
    void sendImage();

private:
    QTcpServer *tcpServer;
    QNetworkSession *networkSession;

    void screenshot();
    static int mysink(virStreamPtr st, const char *buf, long unsigned int nbytes, void *opaque);

    char *memBuf;



    static quint32 blockSize;
};

#endif // SERVER_H
