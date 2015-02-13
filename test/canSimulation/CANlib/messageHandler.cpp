#include "messageHandler.h"
//#include "logMessageHandlerer.h"
#include "color.h"
#include <QDebug>

MessageHandler::MessageHandler(QTcpSocket *client, QMutex *mu, QVector<QString> &msgPull)
    : QObject(), mutex(mu), messagePull(msgPull)
{
    clientConnection = client;

    connect(clientConnection, SIGNAL(readyRead()),this, SLOT(readFromTCP()));
    connect(clientConnection, SIGNAL(disconnected()),this, SLOT(disconnected()));
    connect(clientConnection, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(displayError(QAbstractSocket::SocketError)));
}

MessageHandler::~MessageHandler()
{
    clientConnection->deleteLater();
}

void MessageHandler::readFromTCP()
{
    QByteArray qb = clientConnection->readAll();

    const char *cMessage = qb.data();

    QString qMessage = QString::fromUtf8(cMessage);

    emit logMessageHandler("TCP Received..." + qMessage, GREEN);

    // Read all data came from client and redirect it to QML request handler

    emit requestFromTCP(qMessage);
}

void MessageHandler::writeToTCP(const QString &qMessage)
{
    QByteArray qb = qMessage.toUtf8();

    if ((clientConnection != NULL) && (clientConnection->state() == QTcpSocket::ConnectedState)){

        clientConnection->write(qb);
        clientConnection->flush();
        emit logMessageHandler("TCP Send:" + qMessage, BLUE);
    } else {
        emit logMessageHandler("TCP Client is not connected yet...", RED);
    }
}

void MessageHandler::disconnected()
{
    emit logMessageHandler("TCP disconnected...", RED);

    emit disconnect();

    clientConnection->deleteLater();
}

void MessageHandler::process()
{
    qDebug() << "Thread process()!";
    qDebug() << messagePull.count();

    while (messagePull.count() > 0) {

        mutex->lock();
        writeToTCP(messagePull[0]);
        messagePull.pop_front();
        mutex->unlock();
    }
}

void MessageHandler::stop()
{
    //
}

void MessageHandler::displayError(QAbstractSocket::SocketError socketError)
{
    emit logMessageHandler("TCP Error: " + socketError, RED);
}
