#include "devicelogwidget.h"
#include "ui_devicelog.h"

#include <QByteArray>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QScrollBar>
#include <QString>

DeviceLogWidget::DeviceLogWidget(QWidget* parent) :
    SubWindow(parent),
    m_ui(new Ui::DeviceLog())
{
    m_ui->setupUi(this);

    setProgressVisible(false);

    QObject::connect(m_ui->clearButton, &QPushButton::clicked,
                     m_ui->contentTextEdit, &QPlainTextEdit::clear);
    QObject::connect(m_ui->saveButton, &QPushButton::clicked,
                     this, &DeviceLogWidget::slotSave);
}

DeviceLogWidget::~DeviceLogWidget()
{
    delete m_ui;
    m_ui = nullptr;
}

void DeviceLogWidget::slotBeginReceiving(quint32 count)
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    showProgress(count);
}

void DeviceLogWidget::slotEndReceiving()
{
    hideProgress();
    QApplication::restoreOverrideCursor();
}

void DeviceLogWidget::showProgress(quint32 count)
{
    m_ui->clearButton->setEnabled(false);
    m_ui->saveButton->setEnabled(false);

    m_ui->receivedLabel->setText(QString::number(0));
    m_ui->totalLabel->setText(QString::number(count));
    m_ui->receivingProgressBar->setValue(0);

    setProgressVisible(true);
}

void DeviceLogWidget::hideProgress()
{
    setProgressVisible(false);

    m_ui->clearButton->setEnabled(true);
    m_ui->saveButton->setEnabled(true);
}

void DeviceLogWidget::setProgressVisible(bool enabled)
{
    for (QWidget* each : m_ui->progressWidget->findChildren<QWidget*>())
    {
        each->setVisible(enabled);
    }
}

void DeviceLogWidget::slotReceiveMessage(const QByteArray& message)
{
    int current = m_ui->receivedLabel->text().toInt();
    const int total = m_ui->totalLabel->text().toInt();

    ++current;
    m_ui->receivedLabel->setText(QString::number(current));
    m_ui->receivingProgressBar->setValue(static_cast<int>(current / static_cast<double>(total) * 100.0));

    QString log = m_ui->contentTextEdit->toPlainText();
    if (!log.isEmpty())
    {
        log += "\n";
    }
    log += QString::fromUtf8(message);
    m_ui->contentTextEdit->setPlainText(log);
    m_ui->contentTextEdit->verticalScrollBar()->setValue(m_ui->contentTextEdit->verticalScrollBar()->maximum());
}

void DeviceLogWidget::slotSave()
{
    const QString fileName = QFileDialog::getSaveFileName(nullptr,
                                                          tr("Select Device Log filename"),
                                                          QApplication::applicationDirPath());
    if (!fileName.isEmpty())
    {
        QFile file(fileName);
        if (file.open(QFile::WriteOnly))
        {
            file.write(m_ui->contentTextEdit->toPlainText().toUtf8());
            file.close();
            QMessageBox::information(nullptr,
                                     tr("Save Device Log successful"),
                                     tr("Device Log saved into %1").arg(fileName),
                                     QMessageBox::Ok);
        }
        else
        {
            QMessageBox::warning(nullptr,
                                 tr("Save Device Log failure"),
                                 tr("Device Log not saved: %1").arg(file.errorString()),
                                 QMessageBox::Ok);
        }
    }
}
