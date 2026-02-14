#include "client.h"
#include <QDebug>
#include <QCoreApplication>
#include <QDataStream>

Client::Client(const QString &serverName, QObject *parent)
    : QObject(parent)
    , m_socket(new QLocalSocket(this))
    , m_serverName(serverName)
{
    // Connect signals
    connect(m_socket, &QLocalSocket::connected,
            this, &Client::onConnected);
    connect(m_socket, &QLocalSocket::readyRead,
            this, &Client::onReadyRead);
    connect(m_socket, &QLocalSocket::disconnected,
            this, &Client::onDisconnected);
    connect(m_socket, &QLocalSocket::errorOccurred,
            this, &Client::onError);
}

Client::~Client()
{
    if (m_socket->state() != QLocalSocket::UnconnectedState) {
        m_socket->disconnectFromServer();
        m_socket->waitForDisconnected(1000);
    }
}

void Client::sendMessage(const QString &message)
{
    m_pendingMessage = message;

    qInfo() << "Connecting to server:" << m_serverName;
    m_socket->connectToServer(m_serverName);

    // Wait up to 5 seconds for connection
    if (!m_socket->waitForConnected(5000)) {
        qCritical() << "Failed to connect to server:" << m_socket->errorString();
        QCoreApplication::quit();
        return;
    }
}

void Client::onConnected()
{
    qInfo() << "Connected to server";

    QByteArray messageData = m_pendingMessage.toUtf8();
    quint32 messageLength = messageData.size();

    // Build packet with header (4 bytes length + payload)
    QByteArray packet;
    QDataStream stream(&packet, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::BigEndian);
    stream << messageLength;
    packet.append(messageData);

    // Send packet
    m_socket->write(packet);
    m_socket->flush();

    qInfo() << "Message sent:" << m_pendingMessage;
}

void Client::onReadyRead()
{
    m_buffer.append(m_socket->readAll());

    // Check if we have at least the header (4 bytes)
    if (m_buffer.size() < 4) {
        return;
    }

    // Read message length from header
    QDataStream headerStream(m_buffer.left(4));
    headerStream.setByteOrder(QDataStream::BigEndian);
    quint32 messageLength;
    headerStream >> messageLength;

    // Check if we have the complete message
    if (m_buffer.size() < static_cast<int>(4 + messageLength)) {
        return;
    }

    // Extract the actual message
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
    qCritical() << "Error:" << m_socket->errorString();
    QCoreApplication::quit();
}
