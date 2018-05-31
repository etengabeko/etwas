#include "deviceidentity.h"

#include <algorithm>
#include <memory>

#include <QtTest>
#include <QByteArray>
#include <QDataStream>

#include "protocol/protocol.h"

using protocol::AbstractMessage;
using namespace protocol::incoming;

namespace test
{
namespace serialization
{
void DeviceIdentity::makeTestData()
{
    BasicTest::makeTestData();

    DeviceIdentityMessage message;
    QByteArray content;
    {
        QDataStream stream(&content, QIODevice::WriteOnly);
        stream.setByteOrder(QDataStream::BigEndian);
        stream << quint8(0x01) // type
               << quint8(0)    // firmware version
               << quint16(0);  // buttons count
    }
    QTest::newRow("Empty message")
            << QSharedPointer<AbstractMessage>(new DeviceIdentityMessage(message))
            << content
            << QSharedPointer<AbstractMessage>(new DeviceIdentityMessage(message));
    content.clear();

    message.setFirmwareVersion(1);
    {
        QDataStream stream(&content, QIODevice::WriteOnly);
        stream.setByteOrder(QDataStream::BigEndian);
        stream << quint8(0x01) // type
               << quint8(1)    // firmware version
               << quint16(0);  // buttons count
    }
    QTest::newRow("Firmware version only")
            << QSharedPointer<AbstractMessage>(new DeviceIdentityMessage(message))
            << content
            << QSharedPointer<AbstractMessage>(new DeviceIdentityMessage(message));
    content.clear();

    message.setFirmwareVersion(255);
    message.setButtonsNumbers({ 0 });
    {
        QDataStream stream(&content, QIODevice::WriteOnly);
        stream.setByteOrder(QDataStream::BigEndian);
        stream << quint8(0x01) // type
               << quint8(255)  // firmware version
               << quint16(1)   // buttons count
               << quint8(0);   // buttons numbers
    }
    QTest::newRow("One button")
            << QSharedPointer<AbstractMessage>(new DeviceIdentityMessage(message))
            << content
            << QSharedPointer<AbstractMessage>(new DeviceIdentityMessage(message));
    content.clear();

    message.setFirmwareVersion(2);
    QVector <quint8> buttons;
    buttons.resize(10);
    std::fill(buttons.begin(),
              buttons.end(),
              1);
    message.setButtonsNumbers(buttons);
    {
        QDataStream stream(&content, QIODevice::WriteOnly);
        stream.setByteOrder(QDataStream::BigEndian);
        stream << quint8(0x01)      // type
               << quint8(2)         // firmware version
               << quint16(10);      // buttons count
        for (quint8 each : buttons) // buttons numbers
        {
            stream << each;
        }
    }
    QTest::newRow("Ten same buttons")
            << QSharedPointer<AbstractMessage>(new DeviceIdentityMessage(message))
            << content
            << QSharedPointer<AbstractMessage>(new DeviceIdentityMessage(message));
    content.clear();

    message.setFirmwareVersion(55);
    buttons.resize(255);
    std::for_each(buttons.begin(),
                  buttons.end(),
                  [](quint8) { static quint8 i = 0; return ++i; });
    message.setButtonsNumbers(buttons);
    {
        QDataStream stream(&content, QIODevice::WriteOnly);
        stream.setByteOrder(QDataStream::BigEndian);
        stream << quint8(0x01)      // type
               << quint8(55)        // firmware version
               << quint16(255);     // buttons count
        for (quint8 each : buttons) // buttons numbers
        {
            stream << each;
        }
    }
    QTest::newRow("Maximum buttons")
            << QSharedPointer<AbstractMessage>(new DeviceIdentityMessage(message))
            << content
            << QSharedPointer<AbstractMessage>(new DeviceIdentityMessage(message));
    content.clear();
}

} // serialization
} // test
