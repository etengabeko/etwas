#ifndef TESTS_SEND_RECEIVE_RETRANSLATE_SERVER_H
#define TESTS_SEND_RECEIVE_RETRANSLATE_SERVER_H

#include <QObject>
#include <QList>

class QTcpServer;
class QTcpSocket;

namespace test
{
namespace sendrecv
{

class RetranslateServer : public QObject
{
    Q_OBJECT

public:
    explicit RetranslateServer(quint16 port, QObject* parent = nullptr);
    ~RetranslateServer() NOEXCEPT;

private slots:
    void slotOnNewConnection();
    void slotReadAndRetranslate();
    void slotOnClientDisconnect();

private:
    QTcpServer* m_server;

    QList<QTcpSocket*> m_clients;

};

} // sendrecv
} // test

#endif // TESTS_SEND_RECEIVE_RETRANSLATE_SERVER_H
