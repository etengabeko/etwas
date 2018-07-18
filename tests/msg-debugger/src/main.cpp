#include <QCoreApplication>
#include <QCommandLineParser>

#include <QByteArray>
#include <QDebug>
#include <QHostAddress>
#include <QObject>

#include "protocol/protocol.h"
#include "ioservice/inputcontroller.h"
#include "ioservice/transport.h"

namespace
{

QString defaultAddress() { return "localhost"; }
QString defaultPortNumber() { return "30000"; }

class MessageHandler : public QObject
{
    Q_OBJECT

public:
    explicit MessageHandler(QObject* parent = nullptr) :
        QObject(parent)
    {

    }

public slots:
    void slotOnReceive(const QByteArray& data)
    {
        qInfo().noquote() << tr("Received bytes: %1").arg(QString::fromLatin1(data.toHex()).toUpper());
    }

    void slotOnParseIncoming(const QSharedPointer<protocol::AbstractMessage>& msg)
    {
        using namespace protocol::incoming;

        if (msg != nullptr)
        {
            const Message& current = dynamic_cast<const Message&>(*msg);
            qInfo().noquote() << tr("Message parsed: type = %1")
                                 .arg(typeToString(current.type()));
        }
    }

    void slotOnParseOutcoming(const QSharedPointer<protocol::AbstractMessage>& msg)
    {
        using namespace protocol::outcoming;

        if (msg != nullptr)
        {
            const Message& current = dynamic_cast<const Message&>(*msg);
            qInfo().noquote() << tr("Message parsed: type = %1")
                                 .arg(typeToString(current.type()));
        }
    }

};

}

int main(int argc, char* argv[])
{
    using namespace ioservice;

    QCoreApplication app(argc, argv);

    QCommandLineParser parser;
    parser.addHelpOption();

    QCommandLineOption addressOption(QStringList({ "a", "address" }),
                                     app.tr("Server IP-address"),
                                     app.tr("address"),
                                     ::defaultAddress());
    QCommandLineOption portOption(QStringList({ "p", "port" }),
                                  app.tr("Server port"),
                                  app.tr("number"),
                                  ::defaultPortNumber());
    parser.addOption(addressOption);
    parser.addOption(portOption);

    parser.process(app);

    const QString address = parser.isSet(addressOption) ? parser.value(addressOption)
                                                        : ::defaultAddress();
    const quint16 port = parser.isSet(portOption) ? parser.value(portOption).toUShort()
                                                  : ::defaultPortNumber().toUShort();

    MessageHandler hdlr;
    Transport tran((address.toLower() == ::defaultAddress() ? QHostAddress(QHostAddress::LocalHost)
                                                          : QHostAddress(address)),
                   port);
    InputController ctrlIn(&tran, protocol::MessageDirection::Incoming);
    InputController ctrlOut(&tran, protocol::MessageDirection::Outcoming);

    QObject::connect(&tran, &Transport::connected,
                     [&address, &port]() { qInfo().noquote() << qApp->tr("Connected to %1:%2")
                                                 .arg(address)
                                                 .arg(port); });
    QObject::connect(&tran, &Transport::disconnected,
                     [&address, &port]() { qInfo().noquote() << qApp->tr("Disconnected from %1:%2")
                                                 .arg(address)
                                                 .arg(port); });

    QObject::connect(&tran, &Transport::received,
                     &hdlr, &MessageHandler::slotOnReceive);

    QObject::connect(&ctrlIn, &InputController::messageReceived,
                     &hdlr, &MessageHandler::slotOnParseIncoming);
    QObject::connect(&ctrlOut, &InputController::messageReceived,
                     &hdlr, &MessageHandler::slotOnParseOutcoming);

    tran.start();

    return app.exec();
}

#include "main.moc"
