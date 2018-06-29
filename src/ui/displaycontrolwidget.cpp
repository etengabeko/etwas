#include "displaycontrolwidget.h"
#include "ui_displaycontrol.h"

#include <QGraphicsOpacityEffect>
#include <QHBoxLayout>
#include <QLabel>
#include <QTimer>

#include "protocol/types.h"

namespace
{

qreal transparentLevel() { return 0.3; }
qreal opaqueLevel() { return 1.0; }

}

DisplayControlWidget::DisplayControlWidget(bool isDebugMode, QWidget* parent) :
    QWidget(parent),
    m_ui(new Ui::DisplayControl()),
    m_isDebugMode(isDebugMode),
    m_timer(new QTimer(this)),
    m_displayLabel(new QLabel(this))
{
    m_ui->setupUi(this);

    if (m_ui->displayButton->layout() == nullptr)
    {
        m_ui->displayButton->setLayout(new QHBoxLayout());
    }
    m_ui->displayButton->layout()->addWidget(m_displayLabel);
    m_displayLabel->setAlignment(Qt::AlignCenter);

    QGraphicsOpacityEffect* effect = new QGraphicsOpacityEffect();
    effect->setOpacity(::opaqueLevel());
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

const QString& DisplayControlWidget::firstImage() const
{
    return m_firstImage;
}

bool DisplayControlWidget::isSecondImageEnabled() const
{
    return !m_secondImagePixmap.isNull();
}

const QString& DisplayControlWidget::secondImage() const
{
    return m_secondImage;
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

void DisplayControlWidget::setFirstImage(const QString& pixmapFileName)
{
    m_firstImage = pixmapFileName;
    setImage(ImageNumber::First, QPixmap(pixmapFileName));
}

void DisplayControlWidget::setSecondImage(const QString& pixmapFileName)
{
    m_secondImage = pixmapFileName;
    setImage(ImageNumber::Second, QPixmap(pixmapFileName));
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
