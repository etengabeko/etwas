#include <QtTest>
#include <QDebug>
#include <QList>
#include <QSharedPointer>

#include "beginlog.h"
#include "buttonsstate.h"
#include "deviceidentity.h"

#include "blinkoptions.h"
#include "brightoptions.h"
#include "currenttime.h"
#include "deviceaddress.h"
#include "displayimages.h"
#include "displayoptions.h"
#include "imagedata.h"
#include "nextlog.h"

using namespace test::serialization;

namespace
{

const QList<QSharedPointer<BasicTest>> makeTestData()
{
    return QList<QSharedPointer<BasicTest>>
    {
        QSharedPointer<BasicTest>(new DeviceIdentity()),
        QSharedPointer<BasicTest>(new ButtonsState()),
        QSharedPointer<BasicTest>(new BeginLog()),
        QSharedPointer<BasicTest>(new NextLog()),
        QSharedPointer<BasicTest>(new DeviceAddress()),
        QSharedPointer<BasicTest>(new DisplayImages()),
        QSharedPointer<BasicTest>(new DisplayOptions()),
        QSharedPointer<BasicTest>(new BlinkOptions()),
        QSharedPointer<BasicTest>(new BrightOptions()),
        QSharedPointer<BasicTest>(new ImageData()),
        QSharedPointer<BasicTest>(new CurrentTime())
    };
}

}

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    int res = EXIT_SUCCESS;
    for (const QSharedPointer<BasicTest>& each : ::makeTestData())
    {
        qInfo().noquote() << app.tr("Start test case for %1").arg(each->objectName());
        int eachRes = QTest::qExec(each.data(), app.arguments());
        res &= eachRes;
    }

    return res;
}
