#include "createmessageswidget.h"
#include "ui_createmessages.h"

#include "protocol/protocol.h"
#include "protocol/types.h"

CreateMessagesWidget::CreateMessagesWidget(QWidget* parent) :
    QWidget(parent),
    m_ui(new Ui::CreateMessages())
{
    m_ui->setupUi(this);

    QObject::connect(m_ui->sendButton, &QPushButton::clicked,
                     this, &CreateMessagesWidget::slotCreateMessage);
}

CreateMessagesWidget::~CreateMessagesWidget()
{
    delete m_ui;
    m_ui = nullptr;
}

void CreateMessagesWidget::slotCreateMessage()
{
    QSharedPointer<protocol::AbstractMessage> message;

    if (m_ui->deviceIdentityRadioButton->isChecked())
    {
        message = createDeviceIdentityMessage();
    }
    else if (m_ui->buttonsStateRadioButton->isChecked())
    {
        message = createButtonsStateMessage();
    }
    else if (m_ui->deviceAddressRadioButton->isChecked())
    {
        message = createDeviceAddressMessage();
    }
    else if (m_ui->displayImagesRadioButton->isChecked())
    {
        message = createDisplayImagesMessage();
    }
    else if (m_ui->displayOptionsRadioButton->isChecked())
    {
        message = createDisplayOptionsMessage();
    }
    else if (m_ui->blinkOptionsRadioButton->isChecked())
    {
        message = createBlinkOptionsMessage();
    }
    else if (m_ui->brightOptionsRadioButton->isChecked())
    {
        message = createBrightOptionsMessage();
    }
    else if (m_ui->imagesDataRadioButton->isChecked())
    {
        message = createImagesDataMessage();
    }

    if (message != nullptr)
    {
        emit messageCreated(message);
    }
}

QSharedPointer<protocol::AbstractMessage> CreateMessagesWidget::createDeviceIdentityMessage() const
{
    using namespace protocol;

    incoming::DeviceIdentityMessage* message = new incoming::DeviceIdentityMessage();
    QSharedPointer<AbstractMessage> result(message);

    return result;
}

QSharedPointer<protocol::AbstractMessage> CreateMessagesWidget::createButtonsStateMessage() const
{
    using namespace protocol;

    incoming::ButtonsStateMessage* message = new incoming::ButtonsStateMessage();
    QSharedPointer<AbstractMessage> result(message);

    return result;
}

QSharedPointer<protocol::AbstractMessage> CreateMessagesWidget::createDeviceAddressMessage() const
{
    using namespace protocol;

    outcoming::DeviceAddressMessage* message = new outcoming::DeviceAddressMessage();
    QSharedPointer<AbstractMessage> result(message);

    return result;
}

QSharedPointer<protocol::AbstractMessage> CreateMessagesWidget::createDisplayImagesMessage() const
{
    using namespace protocol;

    outcoming::DisplayImagesMessage* message = new outcoming::DisplayImagesMessage();
    QSharedPointer<AbstractMessage> result(message);

    return result;
}

QSharedPointer<protocol::AbstractMessage> CreateMessagesWidget::createDisplayOptionsMessage() const
{
    using namespace protocol;

    outcoming::DisplayOptionsMessage* message = new outcoming::DisplayOptionsMessage();
    QSharedPointer<AbstractMessage> result(message);

    return result;
}

QSharedPointer<protocol::AbstractMessage> CreateMessagesWidget::createBlinkOptionsMessage() const
{
    using namespace protocol;

    outcoming::BlinkOptionsMessage* message = new outcoming::BlinkOptionsMessage();
    QSharedPointer<AbstractMessage> result(message);

    return result;
}

QSharedPointer<protocol::AbstractMessage> CreateMessagesWidget::createBrightOptionsMessage() const
{
    using namespace protocol;

    outcoming::BrightOptionsMessage* message = new outcoming::BrightOptionsMessage();
    QSharedPointer<AbstractMessage> result(message);

    return result;
}

QSharedPointer<protocol::AbstractMessage> CreateMessagesWidget::createImagesDataMessage() const
{
    using namespace protocol;

    outcoming::ImagesDataMessage* message = new outcoming::ImagesDataMessage();
    QSharedPointer<AbstractMessage> result(message);

    return result;
}
