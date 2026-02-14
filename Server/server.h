#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QLocalServer>
#include <QLocalSocket>
#include <QMap>

class Server : public QObject
{
    Q_OBJECT

public:
    explicit Server(const QString &serverName, QObject *parent = nullptr);
    ~Server();

    bool start();
    void stop();

private slots:
    void onNewConnection();
    void onReadyRead();
    void onDisconnected();
    void onClientError(QLocalSocket::LocalSocketError socketError);

private:
    QString processData(const QString &data);
    void sendResponse(QLocalSocket *socket, const QString &response);

    QLocalServer *m_server;
    QString m_serverName;
    QMap<QLocalSocket*, QByteArray> m_clientBuffers;
};

#endif // SERVER_H
