#include <QtTest>
#include <QObject>
#include <QPixmap>

#include "storage/imagestorage.h"

class ConvertPixmapTest : public QObject
{
    Q_OBJECT

private slots:
    void convert_data()
    {
        QTest::addColumn<QPixmap>("original");

        QTest::newRow("on") << QPixmap(":/on.bmp");
        QTest::newRow("off") << QPixmap(":/off.bmp");
    }

    void convert()
    {
        using storage::ImageStorage;

        QFETCH(QPixmap, original);

        const QVector<QRgb> colors = ImageStorage::pixmapToColors(original);
        const QPixmap repaired = ImageStorage::pixmapFromColors(colors, original.size());

        QCOMPARE(original, repaired);
    }

};

QTEST_MAIN(ConvertPixmapTest)

#include "main.moc"
