#ifndef STORAGE_IMAGE_STORAGE_H
#define STORAGE_IMAGE_STORAGE_H

#include <QObject>
#include <QMap>
#include <QRgb>
#include <QString>

class QPixmap;
class QTimer;
template <typename T> class QVector;

namespace storage
{
/**
 * @class ImageStorage
 * @brief Инкапсулирует хранилище изображений,
 *        предоставляет методы загрузки/удаления содержимого и методы доступа и преобразования содержимого.
 *        Оповещает об изменении состояния сигналом imagesChanged().
 */
class ImageStorage : public QObject
{
    Q_OBJECT

public:
    explicit ImageStorage(QObject* parent = nullptr);
    ~ImageStorage();

    void addImage(quint8 imageIndex, const QString& imageName);
    void removeImage(quint8 imageIndex);

    const QString& imageByIndex(quint8 imageIndex) const;
    const QMap<quint8, QString>& availableImages() const;

    static QVector<QRgb> pixmapToColors(const QPixmap& image);
    static QPixmap pixmapFromColors(const QVector<QRgb>& colors, const QSize& size);
    static const int kInvalidIndex;

signals:
    void imagesChanged();

private slots:
    void slotTimeout();

private:
    QTimer* m_timer = nullptr;
    QMap<quint8, QString> m_images;

};

} // storage

#endif // STORAGE_IMAGE_STORAGE_H
