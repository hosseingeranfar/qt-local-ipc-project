#include "server.h"
#include <QDebug>
#include <QDataStream>
#include <algorithm>

Server::Server(const QString &serverName, QObject *parent)
    : QObject(parent)
    , m_server(new QLocalServer(this))
    , m_serverName(serverName)
{
    connect(m_server, &QLocalServer::newConnection,
            this, &Server::onNewConnection);
}

Server::~Server()
{
    stop();
}

bool Server::start()
{
    // Remove any existing server with the same name
    QLocalServer::removeServer(m_serverName);

    if (!m_server->listen(m_serverName)) {
        qCritical() << "Failed to start server:" << m_server->errorString();
        return false;
    }

    qInfo() << "Server started successfully. Listening on:" << m_serverName;
    return true;
}

void Server::stop()
{
    if (m_server->isListening()) {
        m_server->close();
        qInfo() << "Server stopped.";
    }
}

void Server::onNewConnection()
{
    QLocalSocket *clientSocket = m_server->nextPendingConnection();

    if (!clientSocket) {
        return;
    }

    qInfo() << "New client connected";

    // Connect signals
    connect(clientSocket, &QLocalSocket::readyRead,
            this, &Server::onReadyRead);
    connect(clientSocket, &QLocalSocket::disconnected,
            this, &Server::onDisconnected);
    connect(clientSocket, &QLocalSocket::errorOccurred,
            this, &Server::onClientError);

    // Initialize buffer for this client
    m_clientBuffers[clientSocket] = QByteArray();
}

void Server::onReadyRead()
{
    QLocalSocket *socket = qobject_cast<QLocalSocket*>(sender());
    if (!socket) return;

    // Read all available data
    QByteArray data = socket->readAll();
    m_clientBuffers[socket].append(data);

    // Check if we have at least the header (4 bytes)
    if (m_clientBuffers[socket].size() < 4) {
        return; // Wait for more data
    }

    // Read message length from header
    QDataStream headerStream(m_clientBuffers[socket].left(4));
    headerStream.setByteOrder(QDataStream::BigEndian);
    quint32 messageLength;
    headerStream >> messageLength;

    // Check if we have the complete message
    if (m_clientBuffers[socket].size() < static_cast<int>(4 + messageLength)) {
        return; // Wait for more data
    }

    // Extract the actual message
    QByteArray messageData = m_clientBuffers[socket].mid(4, messageLength);
    QString message = QString::fromUtf8(messageData);

    qInfo() << "Received data:" << message;

    // Process the data
    QString processed = processData(message);
    qInfo() << "Processed data:" << processed;

    // Send response
    sendResponse(socket, processed);

    // Clear the buffer
    m_clientBuffers[socket].clear();

    // Disconnect after sending response
    socket->disconnectFromServer();
}

void Server::onDisconnected()
{
    QLocalSocket *socket = qobject_cast<QLocalSocket*>(sender());
    if (!socket) return;

    qInfo() << "Client disconnected";

    // Clean up
    m_clientBuffers.remove(socket);
    socket->deleteLater();
}

void Server::onClientError(QLocalSocket::LocalSocketError socketError)
{
    Q_UNUSED(socketError);

    QLocalSocket *socket = qobject_cast<QLocalSocket*>(sender());
    if (!socket) return;

    qWarning() << "Client error:" << socket->errorString();
}

QString Server::processData(const QString &data)
{
    // Reverse the string
    QString reversed = data;
    std::reverse(reversed.begin(), reversed.end());
    return reversed;
}

void Server::sendResponse(QLocalSocket *socket, const QString &response)
{
    QByteArray responseData = response.toUtf8();
    quint32 messageLength = responseData.size();

    // Build packet with header (4 bytes length + payload)
    QByteArray packet;
    QDataStream stream(&packet, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::BigEndian);
    stream << messageLength;
    packet.append(responseData);

    // Send packet
    socket->write(packet);
    socket->flush();

    qInfo() << "Response sent:" << response;
}
