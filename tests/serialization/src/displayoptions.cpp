#include "displayoptions.h"

#include <QtTest>
#include <QByteArray>
#include <QDataStream>

#include "protocol/protocol.h"
#include "protocol/types.h"

using protocol::AbstractMessage;
using protocol::BlinkState;
using protocol::ImageSelection;
using namespace protocol::outcoming;

namespace test
{
namespace serialization
{

DisplayOptions::DisplayOptions(QObject* parent) :
    BasicTest(parent)
{
    setObjectName("DisplayOptions");
}

void DisplayOptions::makeTestData()
{
    BasicTest::makeTestData();

    DisplayOptionsMessage message;
    QByteArray content;
    {
        QDataStream stream(&content, QIODevice::WriteOnly);
        stream.setByteOrder(QDataStream::LittleEndian);
        stream << quint8(0x03) // type
               << quint8(0)    // display number
               << quint8(0)    // images count
               << quint8(0);   // blinking
    }
    QTest::newRow("Empty message")
            << QSharedPointer<AbstractMessage>(new DisplayOptionsMessage(message))
            << content
            << QSharedPointer<AbstractMessage>(new DisplayOptionsMessage(message));
    content.clear();

    message.setDisplayNumber(15);
    {
        QDataStream stream(&content, QIODevice::WriteOnly);
        stream.setByteOrder(QDataStream::LittleEndian);
        stream << quint8(0x03) // type
               << quint8(15)   // display number
               << quint8(0)    // images count
               << quint8(0);   // blinking
    }
    QTest::newRow("Display number only")
            << QSharedPointer<AbstractMessage>(new DisplayOptionsMessage(message))
            << content
            << QSharedPointer<AbstractMessage>(new DisplayOptionsMessage(message));
    content.clear();

    message.setDisplayNumber(16);
    message.setImageSelection(ImageSelection::First);
    {
        QDataStream stream(&content, QIODevice::WriteOnly);
        stream.setByteOrder(QDataStream::LittleEndian);
        stream << quint8(0x03) // type
               << quint8(16)   // display number
               << quint8(1)    // images count
               << quint8(0);   // blinking
    }
    QTest::newRow("Display number & images count")
            << QSharedPointer<AbstractMessage>(new DisplayOptionsMessage(message))
            << content
            << QSharedPointer<AbstractMessage>(new DisplayOptionsMessage(message));
    content.clear();

    message.setDisplayNumber(255);
    message.setImageSelection(ImageSelection::Second);
    message.setBlinkState(BlinkState::On);
    {
        QDataStream stream(&content, QIODevice::WriteOnly);
        stream.setByteOrder(QDataStream::LittleEndian);
        stream << quint8(0x03) // type
               << quint8(255)  // display number
               << quint8(2)    // images count
               << quint8(1);   // blinking
    }
    QTest::newRow("Display number & images count & blinking")
            << QSharedPointer<AbstractMessage>(new DisplayOptionsMessage(message))
            << content
            << QSharedPointer<AbstractMessage>(new DisplayOptionsMessage(message));
    content.clear();
}

} // serialization
} // test
