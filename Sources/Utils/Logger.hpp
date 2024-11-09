#pragma once

#include <iostream>
#include <sstream>

#include <fmt/color.h>
#include <fmt/format.h>

namespace Lucid::Logger
{

enum class Severity
{
    Error,
    Warning,
    Info,
    Debug,
    Plain
};

class LoggerImpl
{
public:
    LoggerImpl(Severity severity, const std::string& file, std::int32_t line, const std::string& function);
    ~LoggerImpl();

    template <typename T> LoggerImpl& operator<<(const T& message)
    {
        mStream << fmt::format(fg(fmt::color::gray) | fmt::emphasis::bold, " ") << message;
        return *this;
    }

private:
    Severity mSeverity;
    std::string mFile;
    std::int32_t mLine;
    std::string mFunction;
    std::stringstream mStream;
};

#define LoggerWarning Lucid::Logger::LoggerImpl(Lucid::Logger::Severity::Warning, __FILE__, __LINE__, __func__)
#define LoggerError Lucid::Logger::LoggerImpl(Lucid::Logger::Severity::Error, __FILE__, __LINE__, __func__)
#define LoggerInfo Lucid::Logger::LoggerImpl(Lucid::Logger::Severity::Info, __FILE__, __LINE__, __func__)
#define LoggerDebug Lucid::Logger::LoggerImpl(Lucid::Logger::Severity::Debug, __FILE__, __LINE__, __func__)
#define LoggerPlain Lucid::Logger::LoggerImpl(Lucid::Logger::Severity::Plain, __FILE__, __LINE__, __func__)

} // namespace Lucid::Logger
