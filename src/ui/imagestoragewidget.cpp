#include "imagestoragewidget.h"
#include "ui_imagestorage.h"

#include <QDebug>
#include <QMenu>

#include "storage/imagestorage.h"

ImageStorageWidget::ImageStorageWidget(storage::ImageStorage* storage,
                                       QWidget* parent) :
    SubWindow(parent),
    m_ui(new Ui::ImageStorage()),
    m_storage(storage)
{
    Q_ASSERT(m_storage);

    m_ui->setupUi(this);

    QObject::connect(m_ui->addButton, &QToolButton::clicked,
                     this, &ImageStorageWidget::slotAddImages);
    QObject::connect(m_ui->removeButton, &QToolButton::clicked,
                     this, &ImageStorageWidget::slotRemoveImages);
    QObject::connect(m_ui->selectButton, &QPushButton::clicked,
                     this, &ImageStorageWidget::slotSelect);
    QObject::connect(m_ui->cancelButton, &QPushButton::clicked,
                     this, &ImageStorageWidget::close);
}

ImageStorageWidget::~ImageStorageWidget()
{
    delete m_ui;
    m_ui = nullptr;
}

void ImageStorageWidget::slotSelect()
{
    // TODO
    qDebug() << "slotSelect()";
}

void ImageStorageWidget::slotAddImages()
{
    // TODO
    qDebug() << "slotAddImages()";
}

void ImageStorageWidget::slotRemoveImages()
{
    // TODO
    qDebug() << "slotRemoveImages()";
}
