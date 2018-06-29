#include "logger.h"

#include <chrono>
#include <ctime>
#include <fstream>
#include <iostream>
#include <string>

namespace
{

Logger::Device operator& (Logger::Device lhs,
                          Logger::Device rhs)
{
    return static_cast<Logger::Device>(static_cast<uint>(rhs) & static_cast<uint>(lhs));
}

std::string expandValue(int value)
{
    std::string res = std::to_string(value);
    if (res.size() < 2)
    {
        res = "0" + res;
    }
    return res;
}

const std::tm currentTime()
{
    using namespace std::chrono;

    const time_t now = system_clock::to_time_t(system_clock::now());
    std::tm result = *std::localtime(&now);

    return result;
}

const std::string currentTimeString()
{
    const std::tm tm = currentTime();
    std::string res;
    res.reserve(8);

    res  = expandValue(tm.tm_hour);
    res += ":";
    res += expandValue(tm.tm_min);
    res += ":";
    res += expandValue(tm.tm_sec);

    return res;
}

}

class LoggerPrivate
{
public:
    LoggerPrivate(Logger::Level level,
                  Logger::Device device,
                  const std::string& fileName) :
        m_level(level)
    {
        if ((device & Logger::Device::Console) == Logger::Device::Console)
        {
            initConsole();
        }
        if ((device & Logger::Device::File) == Logger::Device::File)
        {
            initFile(fileName);
        }
    }

    ~LoggerPrivate()
    {
        if (m_file != nullptr)
        {
            m_file->close();
            delete m_file;
            m_file = nullptr;
        }
    }

    void info(const std::string& message)
    {
        static const std::string levelLabel("INFO");
        printMessage(levelLabel, message);
    }

    void warning(const std::string& message)
    {
        static const std::string levelLabel("WARN");
        printMessage(levelLabel, message);
    }

    void error(const std::string& message)
    {
        const std::string levelLabel("ERROR");
        printMessage(levelLabel, message);
    }

    void debug(const std::string& message)
    {
        if (m_level >= Logger::Level::Debug)
        {
            static const std::string levelLabel("DEBUG");
            printMessage(levelLabel, message);
        }
    }

    void trace(const std::string& message)
    {
        if (m_level >= Logger::Level::Trace)
        {
            static const std::string levelLabel("TRACE");
            printMessage(levelLabel, message);
        }
    }

private:
    void initConsole()
    {
        m_console = &(std::cout);
    }

    void initFile(const std::string& fileName)
    {
        if (m_file == nullptr)
        {
            m_file = new std::ofstream();
            m_file->exceptions(std::ofstream::failbit | std::ofstream::badbit);
            m_file->open(fileName, std::ios_base::app);
        }
    }

    void printMessage(const std::string& levelLabel,
                      const std::string& message)
    {
        if (m_console != nullptr)
        {
            printMessageToDevice(*m_console, levelLabel, message);
        }
        if (m_file != nullptr)
        {
            printMessageToDevice(*m_file, levelLabel, message);
        }
    }

    void printMessageToDevice(std::ostream& device,
                              const std::string& levelLabel,
                              const std::string& message)
    {
        device << ::currentTimeString()
               << " [" << levelLabel << "]: "
               << message
               << std::endl;
    }

private:
    Logger::Level m_level;
    std::ostream* m_console = nullptr;
    std::ofstream* m_file = nullptr;

};

Logger::Logger(Level level,
               Device device,
               const QString& fileName) :
    m_pimpl(new LoggerPrivate(level, device, fileName.toStdString()))
{

}

Logger::~Logger() NOEXCEPT
{
    m_pimpl.reset();
}

Logger::Logger(Logger&& other) NOEXCEPT :
    m_pimpl(std::move(other.m_pimpl))
{

}

Logger& Logger::operator =(Logger&& other) NOEXCEPT
{
    m_pimpl.swap(other.m_pimpl);
    return *this;
}

void Logger::info(const QString& message)
{
    m_pimpl->info(message.toStdString());
}

void Logger::warning(const QString& message)
{
    m_pimpl->warning(message.toStdString());
}

void Logger::error(const QString& message)
{
    m_pimpl->error(message.toStdString());
}

void Logger::debug(const QString& message)
{
    m_pimpl->debug(message.toStdString());
}

void Logger::trace(const QString& message)
{
    m_pimpl->trace(message.toStdString());
}
