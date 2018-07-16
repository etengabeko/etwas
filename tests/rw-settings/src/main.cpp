#include <QtTest>
#include <QFile>
#include <QMap>
#include <QObject>
#include <QSet>
#include <QString>
#include <QStringList>
#include <QVector>

#include "settings/displaysettings.h"
#include "settings/settings.h"

namespace
{

const QString tmpFileName() { return QString("%1/%3").arg(QCoreApplication::applicationDirPath()).arg("tmp.ini"); }

}

using namespace protocol;
using namespace settings;

Q_DECLARE_METATYPE(QVector<DisplaySettings>)

class ReadWriteSettingsTest : public QObject
{
    Q_OBJECT

private slots:
    void saveload_data()
    {
        QTest::addColumn<QStringList>("images");
        QTest::addColumn<QVector<DisplaySettings>>("displays");

        QTest::newRow("empty")
                << QStringList()
                << QVector<DisplaySettings>();
        QTest::newRow("images only")
                << QStringList({ "settings.ini", "/tmp/settings.ini", "C:\\tmp\\settings.ini" })
                << QVector<DisplaySettings>();

        {
            QVector<DisplaySettings> ds;
            ds.append([]() { DisplaySettings ds; ds.setDisplayNumber(quint8(1)); return ds; }());
            ds.append([]() { DisplaySettings ds; ds.setDisplayNumber(quint8(2)); return ds; }());
            ds.append([]() { DisplaySettings ds; ds.setDisplayNumber(quint8(3)); return ds; }());
            QTest::newRow("displays only")
                    << QStringList()
                    << ds;
        }

        {
            QVector<DisplaySettings> ds;
            ds.append([]() { DisplaySettings ds; ds.setDisplayNumber(quint8(1)); return ds; }());
            ds.append([]() { DisplaySettings ds; ds.setDisplayNumber(quint8(2)); return ds; }());
            ds.append([]() { DisplaySettings ds; ds.setDisplayNumber(quint8(3)); return ds; }());
            QTest::newRow("both")
                    << QStringList({ "settings.ini", "/tmp/settings.ini", "C:\\tmp\\settings.ini" })
                    << ds;
        }
    }

    void saveload()
    {
        QFETCH(QStringList, images);
        QFETCH(QVector<DisplaySettings>, displays);

        QFile tmpFile(::tmpFileName());
        QVERIFY2(tmpFile.open(QFile::WriteOnly), tmpFile.errorString().toStdString().c_str());
        tmpFile.close();

        {
            Settings saved(tmpFile.fileName());
            saved.setImagesFileNames(images);
            saved.setDisplaysOptions(displays);
            QVERIFY(saved.save());
        }

        {
            Settings loaded(tmpFile.fileName());
            loaded.load();

            QCOMPARE(QSet<QString>::fromList(images),
                     QSet<QString>::fromList(loaded.imagesFileNames()));

            auto displaysVectorToMap = [](const QVector<DisplaySettings>& vec)
            {
                QMap<quint8, DisplaySettings> result;
                for (const DisplaySettings& each : vec)
                {
                    result.insert(each.displayNumber(), each);
                }
                return result;
            };

            QCOMPARE(displaysVectorToMap(displays),
                     displaysVectorToMap(loaded.displaysOptions()));
        }

        tmpFile.remove();
    }

    void parse_data()
    {
        QTest::addColumn<QStringList>("images");
        QTest::addColumn<QVector<DisplaySettings>>("displays");
        QTest::addColumn<QString>("content");

        QTest::newRow("empty")
                << QStringList()
                << QVector<DisplaySettings>()
                << QString();
        QTest::newRow("images only")
                << QStringList({ "settings.ini", "/tmp/settings.ini", "C:\\tmp\\settings.ini" })
                << QVector<DisplaySettings>()
                << QString("[images]\n"
                           "size=3\n"
                           "1/filename=settings.ini\n"
                           "2/filename=/tmp/settings.ini\n"
                           "3/filename=C:\\\\tmp\\\\settings.ini");

        {
            QVector<DisplaySettings> ds;
            ds.append([]() { DisplaySettings ds; ds.setDisplayNumber(quint8(1)); return ds; }());
            ds.append([]() { DisplaySettings ds; ds.setDisplayNumber(quint8(2)); return ds; }());
            ds.append([]() { DisplaySettings ds; ds.setDisplayNumber(quint8(3)); return ds; }());
            QTest::newRow("displays only")
                    << QStringList()
                    << ds
                    << QString("[displays]\n"
                               "size=3\n"
                               "1/number=1\n"
                               "1/selected_images=none\n"
                               "1/blinking=false\n"
                               "2/number=2\n"
                               "2/selected_images=none\n"
                               "2/blinking=false\n"
                               "3/number=3\n"
                               "3/selected_images=none\n"
                               "3/blinking=false");
        }

        {
            QVector<DisplaySettings> ds;
            ds.append([]()
            {
                DisplaySettings ds;
                ds.setDisplayNumber(quint8(255));
                ds.setFirstImageNumber(quint8(2));
                ds.setSecondImageNumber(quint8(3));
                ds.setImageSelection(ImageSelection::Both);
                ds.setBlinkState(BlinkState::On);
                ds.setTimeOn(quint16(1000));
                ds.setTimeOff(quint16(500));
                ds.setBrightLevel(quint8(15));
                return ds;
            }());
            QTest::newRow("all_fields")
                    << QStringList({ "settings.ini", "/tmp/settings.ini", "C:\\tmp\\settings.ini" })
                    << ds
                    << QString("[images]\n"
                               "size=3\n"
                               "1/filename=settings.ini\n"
                               "2/filename=/tmp/settings.ini\n"
                               "3/filename=C:\\\\tmp\\\\settings.ini\n"
                               "[displays]\n"
                               "size=1\n"
                               "1/number=255\n"
                               "1/first_image=2\n"
                               "1/second_image=3\n"
                               "1/selected_images=both\n"
                               "1/blinking=true\n"
                               "1/time_on_msec=1000\n"
                               "1/time_off_msec=500\n"
                               "1/brightness=15\n");
        }
    }

    void parse()
    {
        QFETCH(QStringList, images);
        QFETCH(QVector<DisplaySettings>, displays);
        QFETCH(QString, content);

        QFile tmpFile(::tmpFileName());
        QVERIFY2(tmpFile.open(QFile::WriteOnly), tmpFile.errorString().toStdString().c_str());
        tmpFile.write(content.toLatin1());
        tmpFile.close();

        Settings loaded(tmpFile.fileName());
        loaded.load();

        QCOMPARE(QSet<QString>::fromList(images),
                 QSet<QString>::fromList(loaded.imagesFileNames()));

        auto displaysVectorToMap = [](const QVector<DisplaySettings>& vec)
        {
            QMap<quint8, DisplaySettings> result;
            for (const DisplaySettings& each : vec)
            {
                result.insert(each.displayNumber(), each);
            }
            return result;
        };

        QCOMPARE(displaysVectorToMap(displays),
                 displaysVectorToMap(loaded.displaysOptions()));

        tmpFile.remove();
    }
};

QTEST_MAIN(ReadWriteSettingsTest)

#include "main.moc"
