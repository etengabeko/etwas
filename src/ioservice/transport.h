#ifndef IOSERVICE_TRANSPORT_H
#define IOSERVICE_TRANSPORT_H

#include <QObject>

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

    const settings::Settings& currentSettings() const;
    const QString errorString() const;

signals:
    void connected();
    void disconnected();
    void error();

    void received(const QByteArray& data);
    void sent(const QByteArray& data);

public slots:
    void start();

    void slotSend(const QByteArray& data);
    void slotSend(QByteArray&& data);

private:
    details::TransportPrivate* m_pimpl;

};

} // ioservice

#endif // IOSERVICE_TRANSPORT_H
