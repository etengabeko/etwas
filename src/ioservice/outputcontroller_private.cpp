#include "outputcontroller_private.h"

#include <QByteArray>
#include <QDataStream>

#include "transport.h"
#include "protocol/protocol.h"

namespace ioservice
{
namespace details
{

OutputControllerPrivate::OutputControllerPrivate(Transport* transport,
                                                 QObject *parent) :
    QObject(parent),
    m_transport(transport)
{
    Q_CHECK_PTR(transport);

    QObject::connect(this, &OutputControllerPrivate::sent,
                     m_transport, static_cast<void(Transport::*)(const QByteArray&)>(&Transport::slotSend));
}

OutputControllerPrivate::~OutputControllerPrivate() NOEXCEPT
{

}

void OutputControllerPrivate::send(const protocol::AbstractMessage& message)
{
    QByteArray ba;
    {
        QDataStream stream(&ba, QIODevice::WriteOnly);
        stream.setByteOrder(QDataStream::LittleEndian);
        stream << message;
    }

    emit sent(ba);
}

} // details
} // ioservice
