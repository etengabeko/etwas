#ifndef UI_CONNECTION_OPTIONS_DIALOG_H
#define UI_CONNECTION_OPTIONS_DIALOG_H

#include <QDialog>
#include <QHostAddress>

class QString;

namespace Ui
{
class ConnectionOptions;
} // Ui

class ConnectionOptionsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConnectionOptionsDialog(QWidget* parent = nullptr);
    ~ConnectionOptionsDialog();

    const QHostAddress& address() const;
    void setAddress(const QHostAddress& addr);

    quint16 port() const;
    void setPort(quint16 portnum);

    QString logFileName() const;
    void setLogFileName(const QString& fileName);

private slots:
    void slotApply();
    void slotSelectFileName();
    void slotSetErrorString(const QString& message);

private:
    Ui::ConnectionOptions* m_ui = nullptr;

    QHostAddress m_address;

};

#endif // UI_CONNECTION_OPTIONS_DIALOG_H
