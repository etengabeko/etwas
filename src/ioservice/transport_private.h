#ifndef IOSERVICE_TRANSPORT_PRIVATE_H
#define IOSERVICE_TRANSPORT_PRIVATE_H

#include <QHostAddress>

class QByteArray;
class QString;
class QTcpSocket;

namespace ioservice
{
namespace details
{
class TransportPrivate : public QObject
{
    Q_OBJECT

public:
    TransportPrivate(const QHostAddress& address,
                     quint16 port,
                     QObject* parent = nullptr);
    ~TransportPrivate() NOEXCEPT;

    void start();
    void stop();

    void send(const QByteArray& data);
    void send(QByteArray&& data);

    const QHostAddress& address() const;
    quint16 port() const;

    const QString errorString() const;

signals:
    void connected();
    void disconnected();
    void error();

    void received(const QByteArray& data);
    void sent(const QByteArray& data);

private slots:
    void slotOnReceive();

private:
    QTcpSocket* m_socket = nullptr;

    QHostAddress m_address;
    quint16 m_port = 0;

};

} // details
} // ioservice

#endif // IOSERVICE_TRANSPORT_PRIVATE_H
