#ifndef UI_CONNECTION_OPTIONS_DIALOG_H
#define UI_CONNECTION_OPTIONS_DIALOG_H

#include <QDialog>
#include <QHostAddress>

class QHostAddress;

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

    bool isDebugMode() const;
    void setDebugMode(bool enabled);

public slots:
    void setErrorString(const QString& message);

private:
    void slotApply();

private:
    Ui::ConnectionOptions* m_ui = nullptr;
    bool m_isDebugMode = false;

    QHostAddress m_address;

};

#endif // UI_CONNECTION_OPTIONS_DIALOG_H
