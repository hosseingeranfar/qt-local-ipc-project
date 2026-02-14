#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QLocalSocket>

class Client : public QObject
{
    Q_OBJECT

public:
    explicit Client(const QString &serverName, QObject *parent = nullptr);
    ~Client();

    void sendMessage(const QString &message);

private slots:
    void onConnected();
    void onReadyRead();
    void onDisconnected();
    void onError(QLocalSocket::LocalSocketError socketError);

private:
    QLocalSocket *m_socket;
    QString m_serverName;
    QString m_pendingMessage;
    QByteArray m_buffer;
};

#endif // CLIENT_H
