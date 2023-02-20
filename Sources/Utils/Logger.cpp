#include "Logger.hpp"

#define FMT_HEADER_ONLY
#include <memory>

#include <boost/log/attributes/attribute_value.hpp>
#include <boost/log/sinks/async_frontend.hpp>
#include <boost/log/sinks/block_on_overflow.hpp>
#include <boost/log/sinks/bounded_fifo_queue.hpp>
#include <boost/log/sinks/bounded_ordering_queue.hpp>
#include <boost/log/sinks/drop_on_overflow.hpp>
#include <boost/log/sinks/unbounded_fifo_queue.hpp>
#include <boost/log/sinks/unbounded_ordering_queue.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <fmt/color.h>
#include <fmt/format-inl.h>

namespace Lucid::Logger
{

void LucidFormatter(logging::record_view const& rec, logging::formatting_ostream& strm);

template <typename E>
constexpr typename std::underlying_type<E>::type
to_underlying(E e) noexcept
{
    return static_cast<typename std::underlying_type<E>::type>(e);
}

void
LucidFormatter(logging::record_view const& rec, logging::formatting_ostream& strm)
{
    // Serverity section
    auto severity = rec[SeverityAttr];

    if (severity)
    {
        // Set the color
        switch (severity.get())
        {
        case Severity::Info:
            strm << fmt::format(fg(fmt::color::medium_violet_red) | fmt::emphasis::bold, "ℹ info");
            break;
        case Severity::Warning:
            strm << fmt::format(fg(fmt::color::light_yellow) | fmt::emphasis::bold, "⚠ warn");
            break;
        case Severity::Debug:
            strm << fmt::format(fg(fmt::color::light_yellow) | fmt::emphasis::bold, "⚠ debug");
            break;
        case Severity::Error:
            strm << fmt::format(fg(fmt::color::orange_red) | fmt::emphasis::bold, "× error");
            break;
        }
    }

    // Message section
    strm << fmt::format(fg(fmt::color::gray) | fmt::emphasis::bold, " ") << rec[expr::smessage] << " ";

    // Function and line section
    strm << fmt::format(fg(fmt::color::light_steel_blue) | fmt::emphasis::bold, "(")
         << fmt::format(
                fg(fmt::color::steel_blue) | fmt::emphasis::bold, logging::extract<std::string>("Function", rec).get())
         << fmt::format(fg(fmt::color::light_steel_blue) | fmt::emphasis::bold, ":")
         << fmt::format(
                fg(fmt::color::steel_blue) | fmt::emphasis::bold,
                std::to_string(logging::extract<unsigned long>("Line", rec).get()))
         << fmt::format(fg(fmt::color::light_steel_blue) | fmt::emphasis::bold, ")");
}

BOOST_LOG_GLOBAL_LOGGER_INIT(LoggerInstance, LucidRenderLogger)
{
    using OstreamBackend = sinks::text_ostream_backend;
    using AsyncSink = sinks::asynchronous_sink<OstreamBackend>;

    auto backend = boost::make_shared<OstreamBackend>();
    auto sink = boost::make_shared<AsyncSink>(backend);

    logging::add_common_attributes();
    backend->add_stream(boost::shared_ptr<std::ostream>(&std::cout, boost::null_deleter()));
    backend->auto_flush(true);
    sink->set_formatter(&LucidFormatter);
    boost::log::core::get()->add_sink(sink);

    return {};
}

} // namespace Lucid::Logger
