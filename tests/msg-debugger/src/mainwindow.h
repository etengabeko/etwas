#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QScopedPointer>

class QByteArray;
class QString;
template <typename T>
class QSharedPointer;

namespace Ui
{
class MainWindow;
} // Ui

namespace ioservice
{
class Transport;
class InputController;
} // ioservice

namespace protocol
{
class AbstractMessage;

namespace incoming { class Message; }
namespace outcoming { class Message; }

} // protocol

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void slotStart();
    void slotStop();

    void slotClearImages();

    void slotConnect();
    void slotDisconnect();

    void slotReceiveBytes(const QByteArray& data);
    void slotParseIncoming(const QSharedPointer<protocol::AbstractMessage>& msg);
    void slotParseOutcoming(const QSharedPointer<protocol::AbstractMessage>& msg);

private:
    void addLogMessage(const QString& str);
    void addImageFromMessage(const protocol::outcoming::Message& msg);

    QString messageToString(const protocol::incoming::Message& msg) const;
    QString messageToString(const protocol::outcoming::Message& msg) const;

private:
    Ui::MainWindow* m_ui = nullptr;

    QScopedPointer<ioservice::Transport> m_transport;
    QScopedPointer<ioservice::InputController> m_ctrlIn;
    QScopedPointer<ioservice::InputController> m_ctrlOut;

};

#endif // MAINWINDOW_H
