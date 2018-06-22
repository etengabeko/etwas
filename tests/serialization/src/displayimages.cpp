#include "displayimages.h"

#include <QtTest>
#include <QByteArray>
#include <QDataStream>

#include "protocol/protocol.h"

using protocol::AbstractMessage;
using namespace protocol::outcoming;

namespace test
{
namespace serialization
{

DisplayImages::DisplayImages(QObject* parent) :
    BasicTest(parent)
{
    setObjectName("DisplayImages");
}

void DisplayImages::makeTestData()
{
    BasicTest::makeTestData();

    DisplayImagesMessage message;
    QByteArray content;
    {
        QDataStream stream(&content, QIODevice::WriteOnly);
        stream.setByteOrder(QDataStream::LittleEndian);
        stream << quint8(0x02) // type
               << quint8(0)    // display number
               << quint8(0)    // first image number
               << quint8(0);   // second image number
    }
    QTest::newRow("Empty message")
            << QSharedPointer<AbstractMessage>(new DisplayImagesMessage(message))
            << content
            << QSharedPointer<AbstractMessage>(new DisplayImagesMessage(message));
    content.clear();

    message.setDisplayNumber(127);
    {
        QDataStream stream(&content, QIODevice::WriteOnly);
        stream.setByteOrder(QDataStream::LittleEndian);
        stream << quint8(0x02) // type
               << quint8(127)  // display number
               << quint8(0)    // first image number
               << quint8(0);   // second image number
    }
    QTest::newRow("Display number only")
            << QSharedPointer<AbstractMessage>(new DisplayImagesMessage(message))
            << content
            << QSharedPointer<AbstractMessage>(new DisplayImagesMessage(message));
    content.clear();

    message.setDisplayNumber(0);
    message.setFirstImageNumber(100);
    message.setSecondImageNumber(200);
    {
        QDataStream stream(&content, QIODevice::WriteOnly);
        stream.setByteOrder(QDataStream::LittleEndian);
        stream << quint8(0x02) // type
               << quint8(0)    // display number
               << quint8(100)  // first image number
               << quint8(200); // second image number
    }
    QTest::newRow("Images numbers only")
            << QSharedPointer<AbstractMessage>(new DisplayImagesMessage(message))
            << content
            << QSharedPointer<AbstractMessage>(new DisplayImagesMessage(message));
    content.clear();

    message.setDisplayNumber(255);
    message.setFirstImageNumber(255);
    message.setSecondImageNumber(255);
    {
        QDataStream stream(&content, QIODevice::WriteOnly);
        stream.setByteOrder(QDataStream::LittleEndian);
        stream << quint8(0x02) // type
               << quint8(255)  // display number
               << quint8(255)  // first image number
               << quint8(255); // second image number
    }
    QTest::newRow("Display number & images numbers")
            << QSharedPointer<AbstractMessage>(new DisplayImagesMessage(message))
            << content
            << QSharedPointer<AbstractMessage>(new DisplayImagesMessage(message));
    content.clear();
}

} // serialization
} // test
