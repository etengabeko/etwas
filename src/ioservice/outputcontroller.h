#ifndef IOSERVICE_OUTPUT_CONTROLLER_H
#define IOSERVICE_OUTPUT_CONTROLLER_H

#include <memory>

#include <QObject>

namespace protocol
{
class AbstractMessage;
} // protocol

namespace ioservice
{
namespace details
{
class OutputControllerPrivate;
} // details

class Transport;

class OutputController : public QObject
{
    Q_OBJECT

public:
    explicit OutputController(Transport* transport,
                              QObject* parent = nullptr);
    ~OutputController() NOEXCEPT;

public slots:
    void slotSend(const protocol::AbstractMessage& message);

private:
    std::unique_ptr<details::OutputControllerPrivate> m_pimpl;

};

} // ioservice

#endif // IOSERVICE_OUTPUT_CONTROLLER_H
