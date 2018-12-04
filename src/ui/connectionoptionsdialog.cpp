#include "connectionoptionsdialog.h"
#include "ui_connectionoptions.h"

#include <QApplication>
#include <QCursor>
#include <QFileDialog>
#include <QHostInfo>
#include <QString>

ConnectionOptionsDialog::ConnectionOptionsDialog(QWidget* parent) :
    QDialog(parent),
    m_ui(new Ui::ConnectionOptions())
{
    m_ui->setupUi(this);
    m_ui->errorLabel->setStyleSheet("color: red;");

    QObject::connect(m_ui->applyButton, &QPushButton::clicked,
                     this, &ConnectionOptionsDialog::slotApply);
    QObject::connect(m_ui->cancelButton, &QPushButton::clicked,
                     this, &ConnectionOptionsDialog::reject);
    QObject::connect(m_ui->selectButton, &QPushButton::clicked,
                     this, &ConnectionOptionsDialog::slotSelectFileName);
}

ConnectionOptionsDialog::~ConnectionOptionsDialog()
{
    delete m_ui;
    m_ui = nullptr;
}

void ConnectionOptionsDialog::slotSetErrorString(const QString& message)
{
    m_ui->errorLabel->setText(message);
}

void ConnectionOptionsDialog::slotApply()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    QString address = m_ui->addressLineEdit->text();
    if (address.isEmpty())
    {
        address = m_ui->addressLineEdit->placeholderText();
    }

    QHostInfo info = QHostInfo::fromName(address);
    if (!info.addresses().isEmpty())
    {
        m_address = info.addresses().first();
        QApplication::restoreOverrideCursor();
        accept();
    }
    else
    {
        slotSetErrorString(info.errorString());
        QApplication::restoreOverrideCursor();
    }
}

const QHostAddress& ConnectionOptionsDialog::address() const
{
    return m_address;
}

void ConnectionOptionsDialog::setAddress(const QHostAddress& addr)
{
    m_address = addr;
    m_ui->addressLineEdit->setText(m_address.toString());
}

quint16 ConnectionOptionsDialog::port() const
{
    return m_ui->portSpinBox->value();
}

void ConnectionOptionsDialog::setPort(quint16 portnum)
{
    m_ui->portSpinBox->setValue(portnum);
}

QString ConnectionOptionsDialog::logFileName() const
{
    return (m_ui->logfileGroupBox->isChecked() ? m_ui->fileNameLineEdit->text()
                                               : QString::null);
}

void ConnectionOptionsDialog::setLogFileName(const QString& fileName)
{
    m_ui->fileNameLineEdit->setText(fileName);
}

void ConnectionOptionsDialog::slotSelectFileName()
{
    const QString fileName = QFileDialog::getSaveFileName(nullptr,
                                                          tr("Select Log filename"),
                                                          QApplication::applicationDirPath());
    if (!fileName.isEmpty())
    {
        setLogFileName(fileName);
    }
}

void ConnectionOptionsDialog::hideLogOptions()
{
    m_ui->logfileGroupBox->hide();
}
