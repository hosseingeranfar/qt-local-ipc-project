#include "client.h"
#include <QDebug>
#include <QCoreApplication>
#include <QDataStream>

Client::Client(const QString &serverName, QObject *parent)
    : QObject(parent)
    , m_socket(new QLocalSocket(this))
    , m_connectionTimer(new QTimer(this))
    , m_serverName(serverName)
{
    // تنظیم timer برای timeout
    m_connectionTimer->setSingleShot(true);
    m_connectionTimer->setInterval(5000); // 5 ثانیه

    // اتصال سیگنال‌ها
    connect(m_socket, &QLocalSocket::connected,
            this, &Client::onConnected);
    connect(m_socket, &QLocalSocket::readyRead,
            this, &Client::onReadyRead);
    connect(m_socket, &QLocalSocket::disconnected,
            this, &Client::onDisconnected);
    connect(m_socket, &QLocalSocket::errorOccurred,
            this, &Client::onError);

    // اگر timer تموم شد یعنی timeout
    connect(m_connectionTimer, &QTimer::timeout,
            this, &Client::onConnectionTimeout);
}

Client::~Client()
{
    if (m_socket->state() != QLocalSocket::UnconnectedState) {
        m_socket->disconnectFromServer();
    }
}

void Client::setMessage(const QString &message)
{
    m_pendingMessage = message;
}

void Client::connectToServer()
{
    qInfo() << "Connecting to server:" << m_serverName;

    // شروع timer برای timeout
    m_connectionTimer->start();

    // connect به سرور (non-blocking)
    m_socket->connectToServer(m_serverName);
}

void Client::onConnectionTimeout()
{
    qCritical() << "Connection timeout! Server is not available.";
    QCoreApplication::quit();
}

void Client::onConnected()
{
    // وقتی connect شد، timer رو متوقف کن
    m_connectionTimer->stop();

    qInfo() << "Connected to server";

    // حالا پیام رو بفرست
    sendMessage();
}

void Client::sendMessage()
{
    if (m_pendingMessage.isEmpty()) {
        qWarning() << "No message to send!";
        return;
    }

    QByteArray messageData = m_pendingMessage.toUtf8();
    quint32 messageLength = messageData.size();

    // ساخت packet با header
    QByteArray packet;
    QDataStream stream(&packet, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::BigEndian);
    stream << messageLength;
    packet.append(messageData);

    m_socket->write(packet);
    m_socket->flush();

    qInfo() << "Message sent:" << m_pendingMessage;
}

void Client::onReadyRead()
{
    m_buffer.append(m_socket->readAll());

    if (m_buffer.size() < 4) {
        return;
    }

    QDataStream headerStream(m_buffer.left(4));
    headerStream.setByteOrder(QDataStream::BigEndian);
    quint32 messageLength;
    headerStream >> messageLength;

    if (m_buffer.size() < static_cast<int>(4 + messageLength)) {
        return;
    }

    QByteArray messageData = m_buffer.mid(4, messageLength);
    QString response = QString::fromUtf8(messageData);

    qInfo() << "Response received:" << response;
    qInfo() << "";
    qInfo() << "================================";
    qInfo() << "   Final response:" << response;
    qInfo() << "================================";

    m_buffer.clear();
}

void Client::onDisconnected()
{
    qInfo() << "Disconnected from server";
    QCoreApplication::quit();
}

void Client::onError(QLocalSocket::LocalSocketError socketError)
{
    Q_UNUSED(socketError);

    // اگر هنوز در حال connect بودیم، timer رو متوقف کن
    m_connectionTimer->stop();

    qCritical() << "Error:" << m_socket->errorString();
    QCoreApplication::quit();
}
