#include "inputcontroller.h"
#include "inputcontroller_private.h"

#include <QSharedPointer>

#include "transport.h"
#include "protocol/protocol.h"

namespace ioservice
{

InputController::InputController(Transport* transport,
                                 protocol::MessageDirection direction,
                                 QObject* parent) :
    QObject(parent),
    m_pimpl(new details::InputControllerPrivate(transport, direction))
{
    QObject::connect(m_pimpl.get(), &details::InputControllerPrivate::messageReceived,
                     this, &InputController::messageReceived);
}

InputController::~InputController() NOEXCEPT
{
    m_pimpl.reset();
}

} // ioservice
