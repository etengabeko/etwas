#include <QtTest>

#include <memory>

#include <QByteArray>
#include <QSharedPointer>

#include "protocol/protocol.h"

Q_DECLARE_METATYPE(QSharedPointer<protocol::AbstractMessage>)

class MessageSerializationTest : public QObject
{
    Q_OBJECT

private slots:
    void serializeMessage_data();
    void serializeMessage();

    void deserializeMessage_data();
    void deserializeMessage();

private:
    void makeTestData();

};

void MessageSerializationTest::deserializeMessage_data()
{
    makeTestData();
}

void MessageSerializationTest::serializeMessage_data()
{
    makeTestData();
}

void MessageSerializationTest::serializeMessage()
{
    QFETCH(QSharedPointer<protocol::AbstractMessage>, message);
    QFETCH(QByteArray, content);

    QCOMPARE(message->serialize(), content);
}

void MessageSerializationTest::deserializeMessage()
{
    QFETCH(QSharedPointer<protocol::AbstractMessage>, message);
    QFETCH(QByteArray, content);

    std::unique_ptr<protocol::AbstractMessage> repaired = protocol::AbstractMessage::deserialize(message->direction(), content);

    QVERIFY(repaired != nullptr);
    if (repaired)
    {
        QCOMPARE(repaired->serialize(), message->serialize());
    }
}

void MessageSerializationTest::makeTestData()
{
    QTest::addColumn<QSharedPointer<protocol::AbstractMessage>>("message");
    QTest::addColumn<QByteArray>("content");

    // incoming
    QTest::newRow("DeviceIdentity")
            << QSharedPointer<protocol::AbstractMessage>(new protocol::incoming::DeviceIdentityMessage())
            << QByteArray(protocol::incoming::DeviceIdentityMessage::size(), '\0'); // TODO
    QTest::newRow("ButtonsState")
            << QSharedPointer<protocol::AbstractMessage>(new protocol::incoming::ButtonsStateMessage())
            << QByteArray(protocol::incoming::ButtonsStateMessage::size(), '\0'); // TODO

    // outcoming
    QTest::newRow("DeviceAddress")
            << QSharedPointer<protocol::AbstractMessage>(new protocol::outcoming::DeviceAddressMessage())
            << QByteArray(protocol::outcoming::DeviceAddressMessage::size(), '\0'); // TODO
    QTest::newRow("DisplayImages")
            << QSharedPointer<protocol::AbstractMessage>(new protocol::outcoming::DisplayImagesMessage())
            << QByteArray(protocol::outcoming::DisplayImagesMessage::size(), '\0'); // TODO
    QTest::newRow("DisplayOptions")
            << QSharedPointer<protocol::AbstractMessage>(new protocol::outcoming::DisplayOptionsMessage())
            << QByteArray(protocol::outcoming::DisplayOptionsMessage::size(), '\0'); // TODO
    QTest::newRow("BlinkOptions")
            << QSharedPointer<protocol::AbstractMessage>(new protocol::outcoming::BlinkOptionsMessage())
            << QByteArray(protocol::outcoming::BlinkOptionsMessage::size(), '\0'); // TODO
    QTest::newRow("BrightOptions")
            << QSharedPointer<protocol::AbstractMessage>(new protocol::outcoming::BrightOptionsMessage())
            << QByteArray(protocol::outcoming::BrightOptionsMessage::size(), '\0'); // TODO
    QTest::newRow("ImagesData")
            << QSharedPointer<protocol::AbstractMessage>(new protocol::outcoming::ImagesDataMessage())
            << QByteArray(protocol::outcoming::ImagesDataMessage::size(), '\0'); // TODO
}

QTEST_MAIN(MessageSerializationTest)

#include "main.moc"
