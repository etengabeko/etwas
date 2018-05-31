#include <QtTest>
#include <QList>
#include <QSharedPointer>

#include "deviceidentity.h"

using namespace test::serialization;

namespace
{

const QList<QSharedPointer<BasicTest>> makeTestData()
{
    return QList<QSharedPointer<BasicTest>>
    {
        QSharedPointer<BasicTest>(new DeviceIdentity())
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
