#include "imagestorage.h"

#include <QColor>
#include <QImage>
#include <QPixmap>
#include <QTimer>
#include <QVector>

namespace
{

int notificationDelayMsec() { return 1000; }

}

namespace storage
{

const int ImageStorage::kInvalidIndex = -1;

ImageStorage::ImageStorage(QObject* parent) :
    QObject(parent),
    m_timer(new QTimer(this))
{
    m_timer->setInterval(::notificationDelayMsec());
    m_timer->setSingleShot(true);

    QObject::connect(m_timer, &QTimer::timeout,
                     this, &ImageStorage::slotTimeout);
}

ImageStorage::~ImageStorage()
{

}

QVector<QRgb> ImageStorage::pixmapToColors(const QPixmap& image)
{
    QVector<QRgb> result;
    result.reserve(image.height() * image.width());

    const QImage img = image.toImage();
    for (int y = 0, h = img.height(); y < h; ++y)
    {
        for (int x = 0, w = img.width(); x < w; ++x)
        {
            result.append(img.pixel(x, y));
        }
    }

    return result;
}

QPixmap ImageStorage::pixmapFromColors(const QVector<QRgb>& colors, const QSize& size)
{
    QImage result(size, QImage::Format_RGB32);

    int x = 0;
    int y = 0;
    for (const QRgb& each : colors)
    {
        result.setPixel(x, y, each);

        if (++x >= size.width())
        {
            x = 0;
            ++y;
        }
    }

    return QPixmap::fromImage(result);
}

void ImageStorage::addImage(quint8 imageIndex, const QString& imageName)
{
    QString& value = m_images[imageIndex];
    if (value != imageName)
    {
        value = imageName;
        m_timer->stop();
        m_timer->start();
    }
}

void ImageStorage::removeImage(quint8 imageIndex)
{
    if (m_images.remove(imageIndex) > 0)
    {
        m_timer->stop();
        m_timer->start();
    }
}

void ImageStorage::slotTimeout()
{
    m_timer->stop();
    emit imagesChanged();
}

const QString& ImageStorage::imageByIndex(quint8 imageIndex) const
{
    static const QString kInvalid = QString::null;

    auto founded = m_images.find(imageIndex);
    return (founded != m_images.cend() ? *founded
                                       : kInvalid);
}

const QMap<quint8, QString>& ImageStorage::availableImages() const
{
    return m_images;
}

} // storage
