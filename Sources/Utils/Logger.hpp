#pragma once

#include <fmt/format.h>
#include <rang.hpp>
#include <string>
#include <vector>

class Logger
{
public:
    template<typename ... Args>
    static void Info(const std::string& format, const Args&... args)
    {
        std::cout
            << rang::fgB::blue << "info | " << rang::fg::reset
            << fmt::format(format, args...)
            << '\n';
    }

    template<typename ... Args>
    static void Error(const std::string& format, const Args&... args)
    {
        std::cerr 
            << rang::fgB::red << "error | " << rang::fg::reset 
            << fmt::format(format, args...)
            << '\n';
    }

    static void Error(const std::string& format)
    {
        std::cerr
            << rang::fgB::red << "error | " << rang::fg::reset
            << format
            << '\n';
    }

    template<typename ... Args>
    static void Validation(const std::string& format, const Args&... args)
    {
        std::cerr
            << rang::fgB::red << "validation | " << rang::fg::reset
            << fmt::format(format, args...)
            << '\n';
    }

    template<typename ... Args>
    static void Warning(const std::string& format, const Args&... args)
    {
        std::cerr
            << rang::fgB::yellow << "warning | " << rang::fg::reset
            << fmt::format(format, args...)
            << '\n';
    }

    template<typename T>
    static void List(const std::string& list, const std::vector<T>& items)
    {
        std::cout 
            << rang::fgB::magenta << "list " 
            << rang::fg::reset << list 
            << rang::fgB::magenta << ": " << rang::fg::reset << "\n";
        for (const auto& item : items)
        {
            std::cout
                << rang::fgB::magenta << "   > " << rang::fg::reset
                << item << '\n';
        }
    }
};