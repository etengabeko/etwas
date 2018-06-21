#ifndef TESTS_RETRANSLATE_SERVER_H
#define TESTS_RETRANSLATE_SERVER_H

#include <QObject>
#include <QList>

class QTcpServer;
class QTcpSocket;

namespace test
{

class RetranslateServer : public QObject
{
    Q_OBJECT

public:
    explicit RetranslateServer(QObject* parent = nullptr);
    ~RetranslateServer() NOEXCEPT;

    bool start(quint16 port);

private slots:
    void slotOnNewConnection();
    void slotReadAndRetranslate();
    void slotOnClientDisconnect();

private:
    QTcpServer* m_server;

    QList<QTcpSocket*> m_clients;

};

} // test

#endif // TESTS_RETRANSLATE_SERVER_H
