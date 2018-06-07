#include <QtTest>

#include <QObject>

class SendReceiveTest
{
    Q_OBJECT

private slots:
    void initialize();

};

QTEST_MAIN(SendReceiveTest)

#include "main.moc"
