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

ButtonsState::ButtonsState(QObject* parent) :
    BasicTest(parent)
{
    setObjectName("ButtonsState");
}

void ButtonsState::makeTestData()
{
    BasicTest::makeTestData();

    enum
    {
        BitsCount = 8
    };

    auto makeContentFunc = [](const QByteArray& statesBytes)
    {
        QByteArray result;

        QDataStream stream(&result, QIODevice::WriteOnly);
        stream.setByteOrder(QDataStream::LittleEndian);
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

    QByteArray statesBytes(::maxButtonsStatesCount()/BitsCount, '\0');
    QVector<ButtonState> states(::maxButtonsStatesCount(), ButtonState::Off);
    ButtonsStateMessage message;

    addBenchmarkFunc("Empty message",
                     QSharedPointer<AbstractMessage>(new ButtonsStateMessage(message)),
                     QSharedPointer<AbstractMessage>(new ButtonsStateMessage(message)),
                     statesBytes);
    quint8 byteNum = 0;

    for (int i = 0; i < BitsCount; ++i)
    {
        QVector<ButtonState> tmp(states);
        tmp[i + (byteNum * BitsCount)] = ButtonState::On;
        message.setButtonsStates(std::move(tmp));
        statesBytes[byteNum] = static_cast<char>(0x80 >> i);

        addBenchmarkFunc(QString("Only one %1").arg(i+1).toStdString().c_str(),
                         QSharedPointer<AbstractMessage>(new ButtonsStateMessage(message)),
                         QSharedPointer<AbstractMessage>(new ButtonsStateMessage(message)),
                         statesBytes);
    }
    statesBytes[byteNum++] = 0x00;

    for (int i = 0; i < BitsCount; ++i)
    {
        states[i + (byteNum * BitsCount)] = ButtonState::On;
        message.setButtonsStates(states);
        statesBytes[byteNum] = statesBytes.at(byteNum) + static_cast<char>(0x80 >> i);

        addBenchmarkFunc(QString("From big till little %1").arg(i+1).toStdString().c_str(),
                         QSharedPointer<AbstractMessage>(new ButtonsStateMessage(message)),
                         QSharedPointer<AbstractMessage>(new ButtonsStateMessage(message)),
                         statesBytes);
    }
    std::fill(states.begin(),
              states.end(),
              ButtonState::Off);
    statesBytes[byteNum++] = 0x00;

    for (int i = BitsCount-1; i >= 0; --i)
    {
        states[i + (byteNum * BitsCount)] = ButtonState::On;
        message.setButtonsStates(states);
        statesBytes[byteNum] = statesBytes.at(byteNum) + static_cast<char>(0x80 >> i);

        addBenchmarkFunc(QString("From little till big %1").arg(i+1).toStdString().c_str(),
                         QSharedPointer<AbstractMessage>(new ButtonsStateMessage(message)),
                         QSharedPointer<AbstractMessage>(new ButtonsStateMessage(message)),
                         statesBytes);
    }
    std::fill(states.begin(),
              states.end(),
              ButtonState::Off);
    statesBytes[byteNum++] = 0x00;

    for (int i = 0; i < BitsCount/2; ++i)
    {
        QVector<ButtonState> tmp(states);
        tmp[i + (byteNum * BitsCount)] = ButtonState::On;
        tmp[(BitsCount - i - 1) + (byteNum * BitsCount)] = ButtonState::On;
        message.setButtonsStates(std::move(tmp));
        statesBytes[byteNum] = static_cast<char>(0x80 >> i) + static_cast<char>(0x80 >> (BitsCount - i - 1));

        addBenchmarkFunc(QString("Empty Bowl %1 and %2").arg(i+1).arg(BitsCount-i).toStdString().c_str(),
                         QSharedPointer<AbstractMessage>(new ButtonsStateMessage(message)),
                         QSharedPointer<AbstractMessage>(new ButtonsStateMessage(message)),
                         statesBytes);
    }
    statesBytes[byteNum++] = 0x00;

    for (int i = BitsCount/2; i < BitsCount; ++i)
    {
        states[i + (byteNum * BitsCount)] = ButtonState::On;
        states[(BitsCount - i - 1) + (byteNum * BitsCount)] = ButtonState::On;
        message.setButtonsStates(states);
        statesBytes[byteNum] =   statesBytes.at(byteNum)
                               + static_cast<char>(0x80 >> i) + static_cast<char>(0x80 >> (BitsCount - i - 1));

        addBenchmarkFunc(QString("Full Bowl from %1 till %2").arg(BitsCount-i).arg(i+1).toStdString().c_str(),
                         QSharedPointer<AbstractMessage>(new ButtonsStateMessage(message)),
                         QSharedPointer<AbstractMessage>(new ButtonsStateMessage(message)),
                         statesBytes);
    }
    std::fill(states.begin(),
              states.end(),
              ButtonState::Off);
    statesBytes[byteNum++] = 0x00;
}

} // serialization
} // test
