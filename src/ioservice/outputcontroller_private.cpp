#include "outputcontroller_private.h"

#include <QByteArray>
#include <QDataStream>

#include "transport.h"
#include "protocol/protocol.h"

namespace ioservice
{
namespace details
{

OutputControllerPrivate::OutputControllerPrivate(Transport* transport) :
    m_transport(transport)
{
    Q_CHECK_PTR(transport);
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

    m_transport->slotSend(std::move(ba));
}

} // details
} // ioservice
