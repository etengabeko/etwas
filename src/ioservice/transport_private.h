#ifndef IOSERVICE_TRANSPORT_PRIVATE_H
#define IOSERVICE_TRANSPORT_PRIVATE_H

#include <functional>

#include <QHostAddress>
#include <QPair>

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
                     std::function<void(const QByteArray&)> onReceive,
                     QObject* parent = nullptr);
    ~TransportPrivate() NOEXCEPT;

    QPair<bool, QString> start();

    void send(const QByteArray& data);
    void send(QByteArray&& data);

private slots:
    void slotReceive();

private:
    QTcpSocket* m_socket = nullptr;

    QHostAddress m_address;
    quint16 m_port = 0;
    std::function<void(const QByteArray&)> m_onReceive;

};

} // details
} // ioservice

#endif // IOSERVICE_TRANSPORT_PRIVATE_H
