#ifndef LOGGER_LOGGER_H
#define LOGGER_LOGGER_H

#include <QScopedPointer>
#include <QString>

class LoggerPrivate;

class Logger
{
public:
    enum class Level
    {
        Error = 0,
        Debug = 1,
        Trace = 2
    };

    enum class Device
    {
        Console = 0x01,
        File    = 0x02,
        Both    = Console | File
    };

private:
    Logger(Level level,
           Device device,
           const QString& fileName = QString::null);

public:
    ~Logger();

    Logger(const Logger& other) = delete;
    Logger& operator= (const Logger& other) = delete;

    Logger(Logger&& other) = default;
    Logger& operator= (Logger&& other) = default;

    static Logger& initialize(Level level,
                              Device device = Device::Console,
                              const QString& fileName = QString::null);
    static Logger& instance();

    void info(const QString& message);
    void warning(const QString& message);
    void error(const QString& message);
    void debug(const QString& message);
    void trace(const QString& message);

private:
    static QScopedPointer<Logger> m_instance;

    QScopedPointer<LoggerPrivate> m_pimpl;

};

#endif // LOGGER_LOGGER_H
