#pragma once

#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/manipulators/add_value.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

namespace logging = boost::log;
namespace sinks = boost::log::sinks;
namespace src = boost::log::sources;
namespace expr = boost::log::expressions;
namespace attrs = boost::log::attributes;
namespace keywords = boost::log::keywords;

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

using LucidRenderLogger = boost::log::sources::severity_logger_mt<Severity>;
BOOST_LOG_GLOBAL_LOGGER(LoggerInstance, LucidRenderLogger)
BOOST_LOG_ATTRIBUTE_KEYWORD(SeverityAttr, "Severity", Severity)

#define _LOG(_severity)                                                                     \
    BOOST_LOG_SEV(Lucid::Logger::LoggerInstance::get(), Lucid::Logger::Severity::_severity) \
        << boost::log::add_value("Function", __FUNCTION__)                                  \
        << boost::log::add_value("Line", static_cast<unsigned long>(__LINE__))
#define LoggerWarning _LOG(Warning)
#define LoggerError _LOG(Error)
#define LoggerInfo _LOG(Info)
#define LoggerDebug _LOG(Debug)
#define LoggerPlain _LOG(Plain)

#define _LOG_SET(_severity) boost::log::core::get()->set_filter(SeverityAttr >= Severity::_severity)
inline void
SetWarning()
{
    _LOG_SET(Warning);
}
inline void
SetError()
{
    _LOG_SET(Error);
}
inline void
SetInfo()
{
    _LOG_SET(Info);
}
inline void
SetDebug()
{
    _LOG_SET(Debug);
}

} // namespace Lucid::Logger
