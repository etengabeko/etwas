#include "imagestoragewidget.h"
#include "ui_imagestorage.h"

#include <QFileDialog>
#include <QLabel>
#include <QMessageBox>
#include <QPixmap>
#include <QTableWidgetItem>

#include "storage/imagestorage.h"

namespace
{

enum Column
{
    Number = 0,
    Image,
    FileName
};

}

ImageStorageWidget::ImageStorageWidget(storage::ImageStorage* storage,
                                       QWidget* parent) :
    SubWindow(parent),
    m_ui(new Ui::ImageStorage()),
    m_storage(storage)
{
    Q_ASSERT(m_storage);

    m_ui->setupUi(this);

    m_ui->imagesTableWidget->setSelectionBehavior(QTableWidget::SelectionBehavior::SelectRows);
    m_ui->imagesTableWidget->setSelectionMode(QTableWidget::SingleSelection);

    QObject::connect(m_ui->imagesTableWidget->selectionModel(), &QItemSelectionModel::selectionChanged,
                     this, &ImageStorageWidget::slotChangeSelection);

    QObject::connect(m_ui->addButton, &QToolButton::clicked,
                     this, &ImageStorageWidget::slotAddImages);
    QObject::connect(m_ui->removeButton, &QToolButton::clicked,
                     this, &ImageStorageWidget::slotRemoveImages);
    QObject::connect(m_ui->selectButton, &QPushButton::clicked,
                     this, &ImageStorageWidget::slotSelect);
    QObject::connect(m_ui->cancelButton, &QPushButton::clicked,
                     this, &ImageStorageWidget::close);

    loadAllImages();
}

ImageStorageWidget::~ImageStorageWidget()
{
    delete m_ui;
    m_ui = nullptr;
}

void ImageStorageWidget::loadAllImages()
{
    m_ui->imagesTableWidget->clearContents();

    QMapIterator<quint8, QString> it(m_storage->availableImages());
    while (it.hasNext())
    {
        quint8 index = it.peekNext().key();
        const QString& fileName = it.next().value();
        addRow(index, fileName);
    }

    m_ui->imagesTableWidget->resizeColumnsToContents();
}

void ImageStorageWidget::addRow(quint8 imageIndex, const QString& fileName)
{
    const int row = m_ui->imagesTableWidget->rowCount();
    m_ui->imagesTableWidget->setRowCount(row + 1);

    QTableWidgetItem* item = new QTableWidgetItem(QString::number(imageIndex));
    item->setFlags(item->flags() ^ Qt::ItemIsEditable);
    m_ui->imagesTableWidget->setItem(row, Column::Number, item);
    item = new QTableWidgetItem(fileName);
    item->setFlags(item->flags() ^ Qt::ItemIsEditable);
    m_ui->imagesTableWidget->setItem(row, Column::FileName, item);

    QLabel* label = new QLabel(this);
    label->setPixmap(QPixmap(fileName));
    m_ui->imagesTableWidget->setCellWidget(row, Column::Image, label);
}

void ImageStorageWidget::slotChangeSelection()
{
    m_ui->removeButton->setEnabled(m_ui->imagesTableWidget->selectionModel()->hasSelection());
}

void ImageStorageWidget::slotSelect()
{
    if (m_ui->imagesTableWidget->selectionModel()->hasSelection())
    {
        const int row = m_ui->imagesTableWidget->selectedItems().first()->row();
        emit imageSelected(static_cast<quint8>(m_ui->imagesTableWidget->item(row, Column::Number)->text().toUShort()));
    }
}

void ImageStorageWidget::slotAddImages()
{
    const QStringList fileNames = QFileDialog::getOpenFileNames(nullptr,
                                                                tr("Select Images files"),
                                                                QApplication::applicationDirPath(),
                                                                tr("Images (*.bmp);;All files (*)"));
    int index = m_ui->imagesTableWidget->rowCount();
    for (const QString& each : fileNames)
    {
        addRow(static_cast<quint8>(++index), each);
    }

    m_ui->imagesTableWidget->resizeColumnsToContents();

    syncStorage();
}

void ImageStorageWidget::syncStorage()
{
    const int kMaxImagesCount = 255;
    const int count = m_ui->imagesTableWidget->rowCount();
    if (count > kMaxImagesCount)
    {
        QMessageBox::warning(nullptr,
                             tr("Error Load images"),
                             tr("Count of loaded images (%1) greater than available (%2). Images will be not saved. Please, remove some images.")
                             .arg(count).arg(kMaxImagesCount));
        return;
    }

    for (int row = 0; row < count; ++row)
    {
        m_storage->addImage(static_cast<quint8>(row),
                            m_ui->imagesTableWidget->item(row, Column::FileName)->text());
    }
}

void ImageStorageWidget::slotRemoveImages()
{
    if (m_ui->imagesTableWidget->selectionModel()->hasSelection())
    {
        const int row = m_ui->imagesTableWidget->selectedItems().first()->row();
        m_ui->imagesTableWidget->removeRow(row);
        m_storage->removeImage(static_cast<quint8>(row));
    }
}
