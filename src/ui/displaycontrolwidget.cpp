#include "displaycontrolwidget.h"
#include "ui_displaycontrol.h"

#include <QGraphicsOpacityEffect>
#include <QHBoxLayout>
#include <QLabel>
#include <QTimer>

#include "protocol/types.h"
#include "storage/imagestorage.h"

namespace
{

qreal transparentLevel() { return 0.3; }
qreal opaqueLevel() { return 1.0; }

}

DisplayControlWidget::DisplayControlWidget(const storage::ImageStorage* const storage,
                                           bool isDebugMode,
                                           QWidget* parent) :
    QWidget(parent),
    m_ui(new Ui::DisplayControl()),
    m_imgStorage(storage),
    m_isDebugMode(isDebugMode),
    m_firstImageIndex(storage::ImageStorage::kInvalidIndex),
    m_secondImageIndex(storage::ImageStorage::kInvalidIndex),
    m_timer(new QTimer(this)),
    m_displayLabel(new QLabel(this))
{
    Q_ASSERT(m_imgStorage);

    m_ui->setupUi(this);
    m_withoutPixmapLabel = m_ui->displayButton->text();

    if (m_ui->displayButton->layout() == nullptr)
    {
        m_ui->displayButton->setLayout(new QHBoxLayout());
    }
    m_ui->displayButton->layout()->addWidget(m_displayLabel);
    m_displayLabel->setAlignment(Qt::AlignCenter);

    QGraphicsOpacityEffect* effect = new QGraphicsOpacityEffect();
    effect->setOpacity(::transparentLevel());
    m_displayLabel->setGraphicsEffect(effect);

    QObject::connect(m_timer, &QTimer::timeout,
                     this, &DisplayControlWidget::slotTimeoutFirst);

    if (m_isDebugMode)
    {
        QObject::connect(this, &DisplayControlWidget::activated,
                         this, &DisplayControlWidget::highlight);
        QObject::connect(m_ui->displayButton, &QToolButton::pressed,
                         [this]() { emit activated(true); });
        QObject::connect(m_ui->displayButton, &QToolButton::released,
                         [this]() { emit activated(false); });
    }
    else
    {
        m_ui->displayButton->setCheckable(true);
        QObject::connect(m_ui->displayButton, &QToolButton::toggled,
                         this, &DisplayControlWidget::activated);
    }
}

DisplayControlWidget::~DisplayControlWidget()
{
    delete m_ui;
    m_ui = nullptr;
}

bool DisplayControlWidget::isFirstImageEnabled() const
{
    return !m_firstImagePixmap.isNull();
}

void DisplayControlWidget::setFirstImageEnable(bool enabled)
{
    if (enabled)
    {
        reloadImage(ImageNumber::First);
    }
    else
    {
        resetFirstImage();
    }
}

bool DisplayControlWidget::isSecondImageEnabled() const
{
    return !m_secondImagePixmap.isNull();
}

void DisplayControlWidget::setSecondImageEnable(bool enabled)
{
    if (enabled)
    {
        reloadImage(ImageNumber::Second);
    }
    else
    {
        resetSecondImage();
    }
}

void DisplayControlWidget::setCurrentImage(ImageNumber num)
{
    const QPixmap* currentPixmap = nullptr;
    switch (num)
    {
    case ImageNumber::First:
        currentPixmap = &m_firstImagePixmap;
        break;
    case ImageNumber::Second:
        currentPixmap = &m_secondImagePixmap;
        break;
    default:
        Q_ASSERT_X(false, "DisplayControlWidget::setCurrentImage", "Unexpected value of ImageNumber");
        return;
    }

    if (currentPixmap->isNull())
    {
        m_ui->displayButton->setText(m_withoutPixmapLabel);
    }
    else
    {
        m_ui->displayButton->setText(QString::null);
    }
    m_displayLabel->setPixmap(*currentPixmap);
}

void DisplayControlWidget::resetFirstImage()
{
    resetImage(ImageNumber::First);
}

void DisplayControlWidget::resetSecondImage()
{
    resetImage(ImageNumber::Second);
}

void DisplayControlWidget::resetImage(ImageNumber num)
{
    setImage(num, QPixmap());
}

int DisplayControlWidget::firstImageIndex() const
{
    return m_firstImageIndex;
}

int DisplayControlWidget::secondImageIndex() const
{
    return m_secondImageIndex;
}

void DisplayControlWidget::setFirstImage(quint8 imageIndex)
{
    m_firstImageIndex = static_cast<int>(imageIndex);
    reloadImage(ImageNumber::First);
}

void DisplayControlWidget::setSecondImage(quint8 imageIndex)
{
    m_secondImageIndex = static_cast<int>(imageIndex);
    reloadImage(ImageNumber::Second);
}

void DisplayControlWidget::reloadImage(ImageNumber num)
{
    const int current = (num == ImageNumber::First ? m_firstImageIndex
                                                   : m_secondImageIndex);

    if (   current != storage::ImageStorage::kInvalidIndex
        && m_imgStorage->availableImages().contains(static_cast<quint8>(current)))
    {
        setImage(num, m_imgStorage->imageByIndex(static_cast<quint8>(current)));
    }
    else
    {
        resetImage(num);
    }
}

void DisplayControlWidget::reloadImages()
{
    reloadImage(ImageNumber::First);
    reloadImage(ImageNumber::Second);
}

void DisplayControlWidget::setImage(ImageNumber num, const QPixmap& img)
{
    switch (num)
    {
    case ImageNumber::First:
        m_firstImagePixmap = img;
        break;
    case ImageNumber::Second:
        m_secondImagePixmap = img;
        break;
    default:
        Q_ASSERT_X(false, "DisplayControl::setImage", "Unexpected value of ImageNumber");
        return;
    }

    resetCurrentImage();
}

int DisplayControlWidget::brightLevel() const
{
    return m_brightLevel;
}

void DisplayControlWidget::setBrightLevel(int level)
{
    m_brightLevel = std::min(std::max(level, static_cast<int>(protocol::BrightLevel::Min)),
                             static_cast<int>(protocol::BrightLevel::Max));
    QGraphicsOpacityEffect* effect = qobject_cast<QGraphicsOpacityEffect*>(m_displayLabel->graphicsEffect());
    if (effect != nullptr)
    {
        const qreal opacity = ::transparentLevel() + (::opaqueLevel() - ::transparentLevel()) * m_brightLevel/static_cast<qreal>(protocol::BrightLevel::Max);
        effect->setOpacity(opacity);
    }
}

int DisplayControlWidget::timeOn() const
{
    return m_timeOnMsec;
}

void DisplayControlWidget::setTimeOn(int msec)
{
    m_timeOnMsec = msec;
    resetTimer();
}

int DisplayControlWidget::timeOff() const
{
    return m_timeOffMsec;
}

void DisplayControlWidget::setTimeOff(int msec)
{
    m_timeOffMsec = msec;
    resetTimer();
}

bool DisplayControlWidget::isBlinkingEnabled() const
{
    return m_blinkingEnabled;
}

void DisplayControlWidget::setBlinkingEnabled(bool enabled)
{
    m_blinkingEnabled = enabled;
    resetTimer();
    resetCurrentImage();
}

void DisplayControlWidget::resetTimer()
{
    if (   m_blinkingEnabled
        && m_timeOnMsec > 0
        && m_timeOffMsec > 0)
    {
        m_timer->start(m_timeOnMsec + m_timeOffMsec);
        slotTimeoutFirst();
    }
    else
    {
        m_timer->stop();
    }
}

void DisplayControlWidget::resetCurrentImage()
{
    if (!m_blinkingEnabled)
    {
        if (!m_firstImagePixmap.isNull())
        {
            setCurrentImage(ImageNumber::First);
        }
        else
        {
            setCurrentImage(ImageNumber::Second);
        }
    }
}

void DisplayControlWidget::slotTimeoutFirst()
{
    setCurrentImage(ImageNumber::First);

    QTimer::singleShot(m_timeOnMsec, this, &DisplayControlWidget::slotTimeoutSecond);
}

void DisplayControlWidget::slotTimeoutSecond()
{
     if (m_blinkingEnabled)
     {
         setCurrentImage(ImageNumber::Second);
     }
}

bool DisplayControlWidget::isActive() const
{
    return (m_isDebugMode ? m_ui->displayButton->isDown()
                          : false);
}

void DisplayControlWidget::setActive(bool enabled)
{
    if (!m_isDebugMode)
    {
        m_ui->displayButton->setChecked(enabled);
    }
}

void DisplayControlWidget::highlight(bool enabled)
{
    const QString ss = QString("background-color: %1;").arg((enabled ? "yellow" : "none"));
    m_ui->displayButton->setStyleSheet(ss);
}
