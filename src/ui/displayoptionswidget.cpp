#include "displayoptionswidget.h"
#include "ui_displayoptions.h"

#include <QGraphicsOpacityEffect>
#include <QPixmap>
#include <QString>

#include "protocol/types.h"
#include "storage/imagestorage.h"

namespace
{

qreal transparentLevel() { return 0.3; }
qreal opaqueLevel() { return 1.0; }

}

DisplayOptionsWidget::DisplayOptionsWidget(const storage::ImageStorage* const storage,
                                           QWidget* parent) :
    SubWindow(parent),
    m_ui(new Ui::DisplayOptions()),
    m_imgStorage(storage),
    m_firstImageIndex(storage::ImageStorage::kInvalidIndex),
    m_secondImageIndex(storage::ImageStorage::kInvalidIndex)
{
    Q_ASSERT(m_imgStorage);

    m_ui->setupUi(this);

    m_ui->brightSlider->setRange(static_cast<int>(protocol::BrightLevel::Min),
                                 static_cast<int>(protocol::BrightLevel::Max));

    QGraphicsOpacityEffect* effect = new QGraphicsOpacityEffect();
    effect->setOpacity(::opaqueLevel());
    m_ui->imageFirstLabel->setGraphicsEffect(effect);
    effect = new QGraphicsOpacityEffect();
    effect->setOpacity(::opaqueLevel());
    m_ui->imageSecondLabel->setGraphicsEffect(effect);

    QObject::connect(m_ui->imageFirstButton, &QPushButton::clicked,
                     this, &DisplayOptionsWidget::imageFirstSelected);
    QObject::connect(m_ui->imageSecondButton, &QPushButton::clicked,
                     this, &DisplayOptionsWidget::imageSecondSelected);
    QObject::connect(m_ui->imageFirstCheckBox, &QCheckBox::toggled,
                     this, &DisplayOptionsWidget::imageFirstEnabled);
    QObject::connect(m_ui->imageSecondCheckBox, &QCheckBox::toggled,
                     this, &DisplayOptionsWidget::imageSecondEnabled);
    QObject::connect(m_ui->blinkingCheckBox, &QCheckBox::toggled,
                     this, &DisplayOptionsWidget::blinkingEnabled);
    QObject::connect(m_ui->timeOnSpinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
                     this, &DisplayOptionsWidget::timeOnChanged);
    QObject::connect(m_ui->timeOffSpinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
                     this, &DisplayOptionsWidget::timeOffChanged);
    QObject::connect(m_ui->brightSlider, &QSlider::valueChanged,
                     this, &DisplayOptionsWidget::brightChanged);
    QObject::connect(m_ui->brightSlider, &QSlider::valueChanged,
                     this, &DisplayOptionsWidget::slotChangeBrightness);
}

DisplayOptionsWidget::~DisplayOptionsWidget()
{
    delete m_ui;
    m_ui = nullptr;
}

bool DisplayOptionsWidget::isFirstImageEnable() const
{
    return m_ui->imageFirstCheckBox->isChecked();
}

bool DisplayOptionsWidget::isSecondImageEnable() const
{
    return m_ui->imageSecondCheckBox->isChecked();
}

void DisplayOptionsWidget::reloadImages()
{
    reloadImage(m_firstImageIndex, m_ui->imageFirstLabel);
    reloadImage(m_secondImageIndex, m_ui->imageSecondLabel);
}

void DisplayOptionsWidget::setFirstImage(int imageIndex)
{
    m_firstImageIndex = imageIndex;
    reloadImage(m_firstImageIndex, m_ui->imageFirstLabel);
    emit imageFirstChanged(m_firstImageIndex);
}

void DisplayOptionsWidget::setSecondImage(int imageIndex)
{
    m_secondImageIndex = imageIndex;
    reloadImage(m_secondImageIndex, m_ui->imageSecondLabel);
    emit imageSecondChanged(m_secondImageIndex);
}

void DisplayOptionsWidget::reloadImage(int imageIndex, QLabel* dest)
{
    Q_CHECK_PTR(dest);

    if (   imageIndex != storage::ImageStorage::kInvalidIndex
        && m_imgStorage->availableImages().contains(static_cast<quint8>(imageIndex)))
    {
        dest->setPixmap(QPixmap(m_imgStorage->imageByIndex(static_cast<quint8>(imageIndex))));
    }
    else
    {
        dest->setPixmap(QPixmap());
    }
}

void DisplayOptionsWidget::setFirstImageEnabled(bool enabled)
{
    m_ui->imageFirstCheckBox->setChecked(enabled);
}

void DisplayOptionsWidget::setSecondImageEnabled(bool enabled)
{
    m_ui->imageSecondCheckBox->setChecked(enabled);
}

void DisplayOptionsWidget::setBlinkingEnabled(bool enabled)
{
    m_ui->blinkingCheckBox->setChecked(enabled);
}

void DisplayOptionsWidget::setTimeOn(int msec)
{
    m_ui->timeOnSpinBox->setValue(msec);
}

void DisplayOptionsWidget::setTimeOff(int msec)
{
    m_ui->timeOffSpinBox->setValue(msec);
}

void DisplayOptionsWidget::setBrightLevel(int level)
{
    m_ui->brightSlider->setValue(level);
}

void DisplayOptionsWidget::slotChangeBrightness(int level)
{
    const qreal opacity = ::transparentLevel() + (::opaqueLevel() - ::transparentLevel()) * level/static_cast<qreal>(protocol::BrightLevel::Max);

    QGraphicsOpacityEffect* effect = qobject_cast<QGraphicsOpacityEffect*>(m_ui->imageFirstLabel->graphicsEffect());
    if (effect != nullptr)
    {
        effect->setOpacity(opacity);
    }
    effect = qobject_cast<QGraphicsOpacityEffect*>(m_ui->imageSecondLabel->graphicsEffect());
    if (effect != nullptr)
    {
        effect->setOpacity(opacity);
    }
}
