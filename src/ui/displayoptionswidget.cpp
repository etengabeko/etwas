#include "displayoptionswidget.h"
#include "ui_displayoptions.h"

#include <QPixmap>

DisplayOptionsWidget::DisplayOptionsWidget(QWidget* parent) :
    QWidget(parent),
    m_ui(new Ui::DisplayOptions())
{
    m_ui->setupUi(this);

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
}

DisplayOptionsWidget::~DisplayOptionsWidget()
{
    delete m_ui;
    m_ui = nullptr;
}

void DisplayOptionsWidget::setFirstImage(const QPixmap& img)
{
    m_ui->imageFirstLabel->setPixmap(img);
}

void DisplayOptionsWidget::setSecondImage(const QPixmap& img)
{
    m_ui->imageSecondLabel->setPixmap(img);
}
