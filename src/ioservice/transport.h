#ifndef IOSERVICE_TRANSPORT_H
#define IOSERVICE_TRANSPORT_H

#include <memory>

#include <QObject>
#include <QPair>

class QByteArray;
class QString;

namespace settings
{
class Settings;
} // settings

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
    explicit Transport(const settings::Settings& settings,
                       QObject* parent = nullptr);
    ~Transport() NOEXCEPT;

    QPair<bool, QString> start();

signals:
    void received(const QByteArray& data);

public slots:
    void slotSend(const QByteArray& data);
    void slotSend(QByteArray&& data);

private:
    void onReceived(const QByteArray& data);

private:
    std::unique_ptr<details::TransportPrivate> m_pimpl;

};

} // ioservice

#endif // IOSERVICE_TRANSPORT_H
