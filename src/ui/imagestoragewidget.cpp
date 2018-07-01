#include "imagestoragewidget.h"
#include "ui_imagestorage.h"

#include <QAction>
#include <QDebug>
#include <QHBoxLayout>
#include <QMenu>
#include <QToolBar>
#include <QVBoxLayout>

#include "storage/imagestorage.h"

ImageStorageWidget::ImageStorageWidget(storage::ImageStorage* storage,
                                       QWidget* parent) :
    SubWindow(parent),
    m_ui(new Ui::ImageStorage()),
    m_storage(storage)
{
    Q_ASSERT(m_storage);

    m_ui->setupUi(this);

    QVBoxLayout* mainLayout = qobject_cast<QVBoxLayout*>(layout());
    if (mainLayout != nullptr)
    {
        QHBoxLayout* toolbarLayout = new QHBoxLayout();
        mainLayout->insertLayout(0, toolbarLayout);
        QToolBar* tbar = new QToolBar(this);
        toolbarLayout->addWidget(tbar);

        m_addAction = tbar->addAction(tr("Load"));
        m_rmAction = tbar->addAction(tr("Remove"));

        QObject::connect(m_addAction, &QAction::triggered,
                         this, &ImageStorageWidget::slotAddImages);
        QObject::connect(m_rmAction, &QAction::triggered,
                         this, &ImageStorageWidget::slotRemoveImages);
    }

    QObject::connect(m_ui->applyButton, &QPushButton::clicked,
                     this, &ImageStorageWidget::slotApply);
    QObject::connect(m_ui->cancelButton, &QPushButton::clicked,
                     this, &ImageStorageWidget::close);
}

ImageStorageWidget::~ImageStorageWidget()
{
    delete m_ui;
    m_ui = nullptr;
}

void ImageStorageWidget::slotApply()
{
    // TODO
    qDebug() << "slotApply()";
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
