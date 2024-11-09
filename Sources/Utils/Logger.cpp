#include "Logger.hpp"

namespace Lucid::Logger
{

LoggerImpl::LoggerImpl(Severity severity, const std::string& file, std::int32_t line, const std::string& function)
    : mSeverity(severity)
    , mFile(file)
    , mLine(line)
    , mFunction(function)
{
    switch (mSeverity)
    {
    case Severity::Info:
        mStream << fmt::format(fg(fmt::color::medium_violet_red) | fmt::emphasis::bold, "ℹ info");
        break;
    case Severity::Warning:
        mStream << fmt::format(fg(fmt::color::light_yellow) | fmt::emphasis::bold, "⚠ warn");
        break;
    case Severity::Debug:
        mStream << fmt::format(fg(fmt::color::light_yellow) | fmt::emphasis::bold, "⚠ debug");
        break;
    case Severity::Error:
        mStream << fmt::format(fg(fmt::color::orange_red) | fmt::emphasis::bold, "× error");
        break;
    case Severity::Plain:
        break;
    }

    // Message section
    strm << fmt::format(fg(fmt::color::gray) | fmt::emphasis::bold, " ") << rec[expr::smessage] << " ";

    std::string function = logging::extract<std::string>("Function", rec).get();
    std::string line = std::to_string(logging::extract<unsigned long>("Line", rec).get());

    // Function and line section
    strm << fmt::format(fg(fmt::color::light_steel_blue) | fmt::emphasis::bold, "(")
         << fmt::format(fg(fmt::color::steel_blue) | fmt::emphasis::bold, fmt::runtime(function))
         << fmt::format(fg(fmt::color::light_steel_blue) | fmt::emphasis::bold, ":")
         << fmt::format(fg(fmt::color::steel_blue) | fmt::emphasis::bold, fmt::runtime(line))
         << fmt::format(fg(fmt::color::light_steel_blue) | fmt::emphasis::bold, ")");
}

LoggerImpl::~LoggerImpl()
{
    // Function and line section
    if (mSeverity != Severity::Plain)
    {
        mStream << " ";
        mStream << fmt::format(fg(fmt::color::light_steel_blue) | fmt::emphasis::bold, "(")
                << fmt::format(fg(fmt::color::steel_blue) | fmt::emphasis::bold, mFunction)
                << fmt::format(fg(fmt::color::light_steel_blue) | fmt::emphasis::bold, ":")
                << fmt::format(fg(fmt::color::steel_blue) | fmt::emphasis::bold, std::to_string(mLine))
                << fmt::format(fg(fmt::color::light_steel_blue) | fmt::emphasis::bold, ")");
    }

    mStream << "\n";
    std::cout << mStream.str();
}

} // namespace Lucid::Logger
