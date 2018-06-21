#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QHash>
#include <QMainWindow>

class QMdiArea;
class QMdiSubWindow;
class QString;

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    enum class Mode
    {
        Debug,
        Work
    };

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void slotNewConnection();
    void slotNewDebugConnection();
    void slotNewControlPanel();

    void slotCloseSubWindow();

    void slotOnError(const QString& message);

private:
    void initMenu();
    void createNewConnection(Mode mode);
    void closeSubWindow(QWidget* subwindow);

private:
    QMdiArea* m_central = nullptr;
    QHash<QWidget*, QMdiSubWindow*> m_subwindows;

};

#endif // UI_MAINWINDOW_H
