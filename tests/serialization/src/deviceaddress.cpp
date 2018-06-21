#include "deviceaddress.h"

#include <QtTest>
#include <QByteArray>
#include <QDataStream>
#include <QString>

#include "protocol/protocol.h"

using protocol::AbstractMessage;
using namespace protocol::outcoming;

namespace test
{
namespace serialization
{

DeviceAddress::DeviceAddress(QObject* parent) :
    BasicTest(parent)
{
    setObjectName("DeviceAddress");
}

void DeviceAddress::makeTestData()
{
    BasicTest::makeTestData();

    DeviceAddressMessage message;
    QByteArray content;
    {
        QDataStream stream(&content, QIODevice::WriteOnly);
        stream.setByteOrder(QDataStream::BigEndian);
        stream << quint8(0x01) // type
               << quint32(0)   // ip-address
               << quint16(0);  // port number
    }
    QTest::newRow("Empty message")
            << QSharedPointer<AbstractMessage>(new DeviceAddressMessage(message))
            << content
            << QSharedPointer<AbstractMessage>(new DeviceAddressMessage(message));
    content.clear();

    message.setAddress("127.0.0.1");
    {
        QDataStream stream(&content, QIODevice::WriteOnly);
        stream.setByteOrder(QDataStream::BigEndian);
        stream << quint8(0x01) // type
               << quint8(127) << quint8(0) << quint8(0) << quint8(1) // ip-address
               << quint16(0);  // port number
    }
    QTest::newRow("Ip-address only")
            << QSharedPointer<AbstractMessage>(new DeviceAddressMessage(message))
            << content
            << QSharedPointer<AbstractMessage>(new DeviceAddressMessage(message));
    content.clear();

    message.setAddress(QString::null);
    message.setPort(33333);
    {
        QDataStream stream(&content, QIODevice::WriteOnly);
        stream.setByteOrder(QDataStream::BigEndian);
        stream << quint8(0x01)     // type
               << quint32(0)       // ip-address
               << quint16(33333);  // port number
    }
    QTest::newRow("Port only")
            << QSharedPointer<AbstractMessage>(new DeviceAddressMessage(message))
            << content
            << QSharedPointer<AbstractMessage>(new DeviceAddressMessage(message));
    content.clear();

    message.setAddress("192.168.1.100");
    message.setPort(65535);
    {
        QDataStream stream(&content, QIODevice::WriteOnly);
        stream.setByteOrder(QDataStream::BigEndian);
        stream << quint8(0x01)    // type
               << quint8(192) << quint8(168) << quint8(1) << quint8(100) // ip-address
               << quint16(65535); // port number
    }
    QTest::newRow("Address & port")
            << QSharedPointer<AbstractMessage>(new DeviceAddressMessage(message))
            << content
            << QSharedPointer<AbstractMessage>(new DeviceAddressMessage(message));
    content.clear();
}

} // serialization
} // test
