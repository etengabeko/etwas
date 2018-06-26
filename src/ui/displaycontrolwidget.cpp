#include "displaycontrolwidget.h"
#include "ui_displaycontrol.h"

#include <QGraphicsOpacityEffect>
#include <QHBoxLayout>
#include <QLabel>
#include <QString>
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
                     this, &DisplayControlWidget::slotTimeout);

    if (m_isDebugMode)
    {
        QObject::connect(this, &DisplayControlWidget::activated,
                         this, &DisplayControlWidget::setActive);
        QObject::connect(m_ui->displayButton, &QToolButton::pressed,
                         [this]() { emit activated(true); });
        QObject::connect(m_ui->displayButton, &QToolButton::released,
                         [this]() { emit activated(false); });
    }
}

DisplayControlWidget::~DisplayControlWidget()
{
    delete m_ui;
    m_ui = nullptr;
}

void DisplayControlWidget::setCurrentImage(ImageNumber num)
{
    const QPixmap* current = nullptr;
    switch (num)
    {
    case ImageNumber::First:
        current = &m_firstImage;
        break;
    case ImageNumber::Second:
        current = &m_secondImage;
        break;
    default:
        Q_ASSERT_X(false, "DisplayControlWidget::setCurrentImage", "Unexpected value of ImageNumber");
        return;
    }

    m_displayLabel->setPixmap(*current);
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
    setImage(ImageNumber::First, QPixmap(pixmapFileName));
}

void DisplayControlWidget::setSecondImage(const QString& pixmapFileName)
{
    setImage(ImageNumber::Second, QPixmap(pixmapFileName));
}

void DisplayControlWidget::setImage(ImageNumber num, const QPixmap& img)
{
    switch (num)
    {
    case ImageNumber::First:
        m_firstImage = img;
        break;
    case ImageNumber::Second:
        m_secondImage = img;
        break;
    default:
        Q_ASSERT_X(false, "DisplayControl::setImage", "Unexpected value of ImageNumber");
        return;
    }

    resetCurrentImage();
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

void DisplayControlWidget::setTimeOn(int msec)
{
    m_timeOnMsec = msec;
    resetTimer();
}

void DisplayControlWidget::setTimeOff(int msec)
{
    m_timeOffMsec = msec;
    resetTimer();
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
        slotTimeout();
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
        if (!m_firstImage.isNull())
        {
            setCurrentImage(ImageNumber::First);
        }
        else
        {
            setCurrentImage(ImageNumber::Second);
        }
    }
}

void DisplayControlWidget::slotTimeout()
{
    setCurrentImage(ImageNumber::First);

    QTimer::singleShot(m_timeOnMsec, [this]() { if (m_blinkingEnabled) setCurrentImage(ImageNumber::Second); });
}

void DisplayControlWidget::setActive(bool enabled)
{
    const QString ss = QString("background-color: %1;").arg((enabled ? "yellow" : "none"));
    m_ui->displayButton->setStyleSheet(ss);
}
