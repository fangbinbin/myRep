#include <QtNetwork>

#include <stdlib.h>
#include <stdio.h>
#include <QTextStream>

#include "server.h"

quint32 Server::blockSize = 0;

Server::Server(QObject *parent) :
    QObject(parent), tcpServer(0), networkSession(0), memBuf(NULL)
{
    QNetworkConfigurationManager manager;
    if (manager.capabilities() & QNetworkConfigurationManager::NetworkSessionRequired) {
        // Get saved network configuration
        QSettings settings(QSettings::UserScope, QLatin1String("QtProject"));
        settings.beginGroup(QLatin1String("QtNetwork"));
        const QString id = settings.value(QLatin1String("DefaultNetworkConfiguration")).toString();
        settings.endGroup();

        // If the saved network configuration is not currently discovered use the system default
        QNetworkConfiguration config = manager.configurationFromIdentifier(id);
        if ((config.state() & QNetworkConfiguration::Discovered) !=
            QNetworkConfiguration::Discovered) {
            config = manager.defaultConfiguration();
        }

        networkSession = new QNetworkSession(config, this);
        connect(networkSession, SIGNAL(opened()), this, SLOT(sessionOpened()));

        QTextStream out(stdout);
        out << tr("Opening network session.");

        networkSession->open();
    } else {
        sessionOpened();
    }

    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(sendImage()));
}

Server::~Server()
{
    if(memBuf != NULL)
        free(memBuf);
}

void Server::sessionOpened()
{
    // Save the used configuration
    if (networkSession) {
        QNetworkConfiguration config = networkSession->configuration();
        QString id;
        if (config.type() == QNetworkConfiguration::UserChoice)
            id = networkSession->sessionProperty(QLatin1String("UserChoiceConfiguration")).toString();
        else
            id = config.identifier();

        QSettings settings(QSettings::UserScope, QLatin1String("QtProject"));
        settings.beginGroup(QLatin1String("QtNetwork"));
        settings.setValue(QLatin1String("DefaultNetworkConfiguration"), id);
        settings.endGroup();
    }

    tcpServer = new QTcpServer(this);
    //if (!tcpServer->listen()) {
    //    return;
    //}
    if (!tcpServer->listen(QHostAddress::Any, 54321)) {
        return;
    }

    QString ipAddress;
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    // use the first non-localhost IPv4 address
    for (int i = 0; i < ipAddressesList.size(); ++i) {
        if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
            ipAddressesList.at(i).toIPv4Address()) {
            ipAddress = ipAddressesList.at(i).toString();
            break;
        }
    }
    // if we did not find one, use IPv4 localhost
    if (ipAddress.isEmpty())
        ipAddress = QHostAddress(QHostAddress::LocalHost).toString();

    QTextStream out(stdout);
    out << tr("The server is running on\n\nIP: %1\nport: %2\n\n"
                                      "Run the Fortune Client example now.\n")
                                   .arg(ipAddress).arg(tcpServer->serverPort());

}

void Server::sendImage()
{
    //screenshot();

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);

    out << (quint32)0;
    /*******************************/
    virConnectPtr conn = NULL; /* the hypervisor connection */
    virDomainPtr dom = NULL; /* the domain to be screenshotted */
    virStreamPtr st = NULL;
    char *mimetype = NULL;

    conn = virConnectOpen("qemu:///system");
    if (conn == NULL) {
        fprintf(stderr, "Failed to connect to hypervisor\n");
        if(st != NULL)
            virStreamFree(st);

        if(mimetype !=NULL)
            free(mimetype);

        if (dom != NULL)
            virDomainFree(dom);

        if (conn != NULL)
            virConnectClose(conn);

        return;
    }

    /* Find the domain of the given id */
    dom = virDomainLookupByName(conn, "win7-1");
    if (dom == NULL) {
        fprintf(stderr, "Failed to connect to hypervisor\n");
        if(st != NULL)
            virStreamFree(st);

        if(mimetype !=NULL)
            free(mimetype);

        if (dom != NULL)
            virDomainFree(dom);

        if (conn != NULL)
            virConnectClose(conn);

        return;
    }

    st = virStreamNew(conn, 0);
    mimetype = virDomainScreenshot(dom, st, 0, 0);
    if(mimetype == NULL) {
        fprintf(stderr, "Failed in virDomainScreenshot funcation\n");
        if(st != NULL)
            virStreamFree(st);

        if(mimetype !=NULL)
            free(mimetype);

        if (dom != NULL)
            virDomainFree(dom);

        if (conn != NULL)
            virConnectClose(conn);

        return;
    }

    if(virStreamRecvAll(st, mysink, &out) < 0) {
        fprintf(stderr, "Failed in virStreamRecvAll funcation\n");
        if(st != NULL)
            virStreamFree(st);

        if(mimetype !=NULL)
            free(mimetype);

        if (dom != NULL)
            virDomainFree(dom);

        if (conn != NULL)
            virConnectClose(conn);

        return;
    }

    if (virStreamFinish(st) < 0) {
        fprintf(stderr, "Failed in virStreamFinish funcation\n");
        if(st != NULL)
            virStreamFree(st);

        if(mimetype !=NULL)
            free(mimetype);

        if (dom != NULL)
            virDomainFree(dom);

        if (conn != NULL)
            virConnectClose(conn);

        return;
    }

    if(st != NULL)
        virStreamFree(st);

    if(mimetype !=NULL)
        free(mimetype);

    if (dom != NULL)
        virDomainFree(dom);

    if (conn != NULL)
        virConnectClose(conn);
    /*******************************/

    out.device()->seek(0);
    out << (quint32)(block.size() - sizeof(quint32));

    QTcpSocket *clientConnection = tcpServer->nextPendingConnection();
    connect(clientConnection, SIGNAL(disconnected()),
            clientConnection, SLOT(deleteLater()));

    clientConnection->write(block);
    clientConnection->disconnectFromHost();

    QTextStream info(stdout);
    info << tr("has sended %1 bytes\n").arg(block.size() - 4);
}

void Server::screenshot()
{
    virConnectPtr conn = NULL; /* the hypervisor connection */
    virDomainPtr dom = NULL; /* the domain to be screenshotted */
    virStreamPtr st = NULL;
    char *mimetype = NULL;
    FILE *fp = NULL;

    conn = virConnectOpen("qemu:///system");
    if (conn == NULL) {
        fprintf(stderr, "Failed to connect to hypervisor\n");

        if(st != NULL)
            virStreamFree(st);

        if(fp != NULL)
            fclose(fp);

        if(mimetype !=NULL)
            free(mimetype);

        if (dom != NULL)
            virDomainFree(dom);

        if (conn != NULL)
            virConnectClose(conn);

        return;
    }

    /* Find the domain of the given id */
    dom = virDomainLookupByName(conn, "win7-1");
    if (dom == NULL) {
        fprintf(stderr, "Failed to connect to hypervisor\n");

        if(st != NULL)
            virStreamFree(st);

        if(fp != NULL)
            fclose(fp);

        if(mimetype !=NULL)
            free(mimetype);

        if (dom != NULL)
            virDomainFree(dom);

        if (conn != NULL)
            virConnectClose(conn);

        return;
    }

    st = virStreamNew(conn, 0);
    mimetype = virDomainScreenshot(dom, st, 0, 0);
    if(mimetype == NULL) {
        fprintf(stderr, "Failed in virDomainScreenshot funcation\n");

        if(st != NULL)
            virStreamFree(st);

        if(fp != NULL)
            fclose(fp);

        if(mimetype !=NULL)
            free(mimetype);

        if (dom != NULL)
            virDomainFree(dom);

        if (conn != NULL)
            virConnectClose(conn);

        return;
    }

    fp = fopen("shot.ppm", "w");
    if(fp == NULL) {
        fprintf(stderr, "Failed in fopen funcation\n");

        if(st != NULL)
            virStreamFree(st);

        if(fp != NULL)
            fclose(fp);

        if(mimetype !=NULL)
            free(mimetype);

        if (dom != NULL)
            virDomainFree(dom);

        if (conn != NULL)
            virConnectClose(conn);

        return;
    }
    if(virStreamRecvAll(st, mysink, fp) < 0) {
        fprintf(stderr, "Failed in virStreamRecvAll funcation\n");

        if(st != NULL)
            virStreamFree(st);

        if(fp != NULL)
            fclose(fp);

        if(mimetype !=NULL)
            free(mimetype);

        if (dom != NULL)
            virDomainFree(dom);

        if (conn != NULL)
            virConnectClose(conn);

        return;
    }

    if (virStreamFinish(st) < 0) {
        fprintf(stderr, "Failed in virStreamFinish funcation\n");

        if(st != NULL)
            virStreamFree(st);

        if(fp != NULL)
            fclose(fp);

        if(mimetype !=NULL)
            free(mimetype);

        if (dom != NULL)
            virDomainFree(dom);

        if (conn != NULL)
            virConnectClose(conn);

        return;
    }

    if(st != NULL)
        virStreamFree(st);

    if(fp != NULL)
        fclose(fp);

    if(mimetype !=NULL)
        free(mimetype);

    if (dom != NULL)
        virDomainFree(dom);

    if (conn != NULL)
        virConnectClose(conn);
}

int Server::mysink(virStreamPtr st, const char *buf, long unsigned int nbytes, void *opaque)
{
    //char **mBuf = (char **)opaque;
    //*mBuf = (char *)malloc(nbytes);
    //memset(*mBuf, 0, nbytes);
    //memcpy(*mBuf, buf, nbytes);

    //Server::blockSize = (quint32)nbytes;
    //QTextStream out(stdout);
    //out << tr("Server::blockSize = %1").arg(Server::blockSize);

    //QDataStream *out = (QDataStream *)opaque;
    //(*out) << buf;

    //return 0;
    //FILE *fp = (FILE *)opaque;
    //return fwrite(buf, 1, nbytes, fp);
    QDataStream *out = (QDataStream *)opaque;
    return out->writeRawData(buf, nbytes);
}
