#include "buttonsstate.h"

#include <algorithm>

#include <QtTest>
#include <QByteArray>
#include <QDataStream>
#include <QVector>

#include "protocol/protocol.h"
#include "protocol/types.h"

namespace
{

int maxButtonsStatesCount() { return 256; }

}

using protocol::AbstractMessage;
using protocol::ButtonState;
using namespace protocol::incoming;

namespace test
{
namespace serialization
{

void ButtonsState::makeTestData()
{
    BasicTest::makeTestData();

    auto makeContentFunc = [](const QByteArray& statesBytes)
    {
        QByteArray result;

        QDataStream stream(&result, QIODevice::WriteOnly);
        stream.setByteOrder(QDataStream::BigEndian);
        stream << quint8(0x02); // type
        stream.writeRawData(statesBytes.constData(), statesBytes.size());

        return result;
    };
    auto addBenchmarkFunc = [makeContentFunc](const char* benchmarkTag,
                                              const QSharedPointer<AbstractMessage>& actual,
                                              const QSharedPointer<AbstractMessage>& expected,
                                              const QByteArray& statesBytes)
    {
        QTest::newRow(benchmarkTag)
                << actual
                << makeContentFunc(statesBytes)
                << expected;
    };

    QByteArray statesBytes(::maxButtonsStatesCount()/8, '\0');
    QVector<ButtonState> states(::maxButtonsStatesCount(), ButtonState::Off);
    ButtonsStateMessage message;
    message.setButtonsStates(states);

    addBenchmarkFunc("Empty message",
                     QSharedPointer<AbstractMessage>(new ButtonsStateMessage(message)),
                     QSharedPointer<AbstractMessage>(new ButtonsStateMessage(message)),
                     statesBytes);
    for (int i = 0; i < 8; ++i)
    {
//        std::fill(statesBytes.begin(),
//                  statesBytes.end(),
//                  '\0');

        QVector<ButtonState> tmp(states);
        tmp[i] = ButtonState::On;
        message.setButtonsStates(std::move(tmp));
        statesBytes[0] = static_cast<char>(0x80 >> i);

        addBenchmarkFunc(QString("Only one %1").arg(i+1).toStdString().c_str(),
                         QSharedPointer<AbstractMessage>(new ButtonsStateMessage(message)),
                         QSharedPointer<AbstractMessage>(new ButtonsStateMessage(message)),
                         statesBytes);
    }
}

} // serialization
} // test
