#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QLocalSocket>
#include <QTimer>

class Client : public QObject
{
    Q_OBJECT

public:
    explicit Client(const QString &serverName, QObject *parent = nullptr);
    ~Client();

    void connectToServer();             // فقط connect می‌کنه
    void setMessage(const QString &message);  // پیام رو set می‌کنه

private slots:
    void onConnected();
    void onReadyRead();
    void onDisconnected();
    void onError(QLocalSocket::LocalSocketError socketError);
    void onConnectionTimeout();         // برای مدیریت timeout

private:
    void sendMessage();                 // فقط send می‌کنه (private)

    QLocalSocket *m_socket;
    QTimer *m_connectionTimer;
    QString m_serverName;
    QString m_pendingMessage;
    QByteArray m_buffer;
};

#endif // CLIENT_H
