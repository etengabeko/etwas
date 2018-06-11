#ifndef UI_CREATE_MESSAGES_WIDGET_H
#define UI_CREATE_MESSAGES_WIDGET_H

#include <QWidget>

template <typename T> class QSharedPointer;

namespace protocol
{
class AbstractMessage;
} // protocol

namespace Ui
{
class CreateMessages;
} // Ui

class CreateMessagesWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CreateMessagesWidget(QWidget* parent = nullptr);
    ~CreateMessagesWidget();

signals:
    void messageCreated(const QSharedPointer<protocol::AbstractMessage>& message);

private slots:
    void slotCreateMessage();

private:
    QSharedPointer<protocol::AbstractMessage> createDeviceIdentityMessage() const;
    QSharedPointer<protocol::AbstractMessage> createButtonsStateMessage() const;
    QSharedPointer<protocol::AbstractMessage> createDeviceAddressMessage() const;
    QSharedPointer<protocol::AbstractMessage> createDisplayImagesMessage() const;
    QSharedPointer<protocol::AbstractMessage> createDisplayOptionsMessage() const;
    QSharedPointer<protocol::AbstractMessage> createBlinkOptionsMessage() const;
    QSharedPointer<protocol::AbstractMessage> createBrightOptionsMessage() const;
    QSharedPointer<protocol::AbstractMessage> createImagesDataMessage() const;

private:
    Ui::CreateMessages* m_ui = nullptr;

};

#endif // UI_CREATE_MESSAGES_WIDGET_H
