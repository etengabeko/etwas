#include "displaycontrolwidget.h"
#include "ui_displaycontrol.h"

#include <QBitmap>
#include <QTimer>

#include "logger/logger.h"
#include "protocol/types.h"

DisplayControlWidget::DisplayControlWidget(bool isDebugMode, QWidget* parent) :
    QWidget(parent),
    m_ui(new Ui::DisplayControl()),
    m_isDebugMode(isDebugMode),
    m_timerOn(new QTimer(this)),
    m_timerOff(new QTimer(this))
{
    m_ui->setupUi(this);

    QObject::connect(m_timerOn, &QTimer::timeout,
                     this, &DisplayControlWidget::slotTimeoutOn);
    QObject::connect(m_timerOff, &QTimer::timeout,
                     this, &DisplayControlWidget::slotTimeoutOff);

    QObject::connect(this, &DisplayControlWidget::pressed,
                     [this]() { emit activated(true); });
    QObject::connect(this, &DisplayControlWidget::released,
                     [this]() { emit activated(false); });

    if (m_isDebugMode)
    {
        QObject::connect(m_ui->displayButton, &QToolButton::pressed,
                         this, &DisplayControlWidget::pressed);
        QObject::connect(m_ui->displayButton, &QToolButton::released,
                         this, &DisplayControlWidget::released);
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

    // TODO: add mask
    m_ui->displayButton->setIcon(QIcon(*current));
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

void DisplayControlWidget::setFirstImage(const QPixmap& img)
{
    setImage(ImageNumber::First, img);
}

void DisplayControlWidget::setSecondImage(const QPixmap& img)
{
    setImage(ImageNumber::Second, img);
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
        break;
    }
}

void DisplayControlWidget::setBrightLevel(int level)
{
    m_brightLevel = std::min(std::max(level, static_cast<int>(protocol::BrightLevel::Min)),
                             static_cast<int>(protocol::BrightLevel::Max));
    // TODO: set opacity
    // m_brightMask
}

void DisplayControlWidget::setTimeOn(int msec)
{
    m_timeOnMsec = msec;
    resetTimers();
}

void DisplayControlWidget::setTimeOff(int msec)
{
    m_timeOffMsec = msec;
    resetTimers();
}

void DisplayControlWidget::resetTimers()
{
    m_timerOn->setInterval(m_timeOnMsec + m_timeOffMsec);
    m_timerOff->setInterval(m_timeOnMsec + m_timeOffMsec);
}

void DisplayControlWidget::setBlinkingEnabled(bool enabled)
{
    if (enabled)
    {
        if (m_timeOnMsec > 0)
        {
            m_timerOn->start();
        }
        if (m_timeOffMsec > 0)
        {
            QTimer::singleShot(m_timeOnMsec, [this]() { m_timerOff->start(); });
        }
    }
    else
    {
        m_timerOn->stop();
        m_timerOff->stop();
    }
}

void DisplayControlWidget::slotTimeoutOn()
{
    Logger::instance().trace(tr("set first image"));
    setCurrentImage(ImageNumber::First);
}

void DisplayControlWidget::slotTimeoutOff()
{
    Logger::instance().trace(tr("set second image"));
    setCurrentImage(ImageNumber::Second);
}
