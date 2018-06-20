#include <QtTest>
#include <QList>
#include <QSharedPointer>

#include "buttonsstate.h"
#include "deviceidentity.h"

#include "blinkoptions.h"
#include "brightoptions.h"
#include "deviceaddress.h"
#include "displayimages.h"
#include "displayoptions.h"

using namespace test::serialization;

namespace
{

const QList<QSharedPointer<BasicTest>> makeTestData()
{
    return QList<QSharedPointer<BasicTest>>
    {
        QSharedPointer<BasicTest>(new DeviceIdentity()),
        QSharedPointer<BasicTest>(new ButtonsState()),
        QSharedPointer<BasicTest>(new DeviceAddress()),
        QSharedPointer<BasicTest>(new DisplayImages()),
        QSharedPointer<BasicTest>(new DisplayOptions()),
        QSharedPointer<BasicTest>(new BlinkOptions()),
        QSharedPointer<BasicTest>(new BrightOptions())
        // TODO
    };
}

}

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);
    Q_UNUSED(app);

    int res = EXIT_SUCCESS;
    for (const QSharedPointer<BasicTest>& each : ::makeTestData())
    {
        int eachRes = QTest::qExec(each.data(), argc, argv);
        res &= eachRes;
    }

    return res;
}
