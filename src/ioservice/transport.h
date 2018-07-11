#ifndef IOSERVICE_TRANSPORT_H
#define IOSERVICE_TRANSPORT_H

#include <QObject>

class QByteArray;
class QHostAddress;
class QString;

namespace ioservice
{
namespace details
{
class TransportPrivate;
} // details

class Transport : public QObject
{
    Q_OBJECT

public:
    explicit Transport(const QHostAddress& address,
                       quint16 port,
                       QObject* parent = nullptr);
    ~Transport() NOEXCEPT;

    const QHostAddress& currentAddress() const NOEXCEPT;
    quint16 currentPort() const NOEXCEPT;

    const QString errorString() const;

signals:
    void connected();
    void disconnected();
    void error();

    void received(const QByteArray& data);
    void sent(const QByteArray& data);

public slots:
    void start();
    void stop();

    void send(const QByteArray& data);
    void send(QByteArray&& data);

private:
    details::TransportPrivate* m_pimpl;

};

} // ioservice

#endif // IOSERVICE_TRANSPORT_H
