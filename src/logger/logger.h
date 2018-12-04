#ifndef LOGGER_LOGGER_H
#define LOGGER_LOGGER_H

#include <memory>

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

public:
    Logger(Level level,
           Device device = Device::Console,
           const QString& fileName = QString::null);
    ~Logger() NOEXCEPT;

    Logger(const Logger& other) = delete;
    Logger& operator= (const Logger& other) = delete;

    Logger(Logger&& other) NOEXCEPT;
    Logger& operator= (Logger&& other) NOEXCEPT;

    Level level() const NOEXCEPT;

    void info(const QString& message);
    void warning(const QString& message);
    void error(const QString& message);
    void debug(const QString& message);
    void trace(const QString& message);

private:
    std::unique_ptr<LoggerPrivate> m_pimpl;

};

#endif // LOGGER_LOGGER_H
