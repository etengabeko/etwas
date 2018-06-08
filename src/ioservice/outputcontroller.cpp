#include "outputcontroller.h"
#include "outputcontroller_private.h"

#include "transport.h"
#include "protocol/protocol.h"

namespace ioservice
{

OutputController::OutputController(Transport* transport,
                                   QObject* parent) :
    QObject(parent),
    m_pimpl(new details::OutputControllerPrivate(transport))
{

}

OutputController::~OutputController() NOEXCEPT
{
    m_pimpl.reset();
}

void OutputController::slotSend(const protocol::AbstractMessage& message)
{
    m_pimpl->send(message);
}

} // ioservice
