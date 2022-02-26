#pragma once

#include "../../macros.h"

// Much of this file (and ./log.cpp) is derived from a gist by kevinkreiser
// licensed under the public domain. Original source code can be found at
// https://gist.github.com/kevinkreiser/39f2e39273c625d96790

namespace Alkahest
{
    enum class log_level : uint8_t { TRACE = 0, DEBUG = 1, INFO = 2, WARN = 3, ERROR = 4 };
    struct enum_hasher { template <typename T> std::size_t operator()(T t) const { return static_cast<std::size_t>(t); } };
    const std::unordered_map<log_level, std::string, enum_hasher> uncoloredText
    {
        { log_level::TRACE, " [TRACE] " },
        { log_level::DEBUG, " [DEBUG] " },
        { log_level::INFO, " [INFO] " },
        { log_level::WARN, " [WARNING] " },
        { log_level::ERROR, " [ERROR] " }
    };
    const std::unordered_map<log_level, std::string, enum_hasher> coloredText
    {
        { log_level::TRACE, " \x1b[37;1m[TRACE]\x1b[0m " },
        { log_level::DEBUG, " \x1b[34;1m[DEBUG]\x1b[0m " },
        { log_level::INFO, " \x1b[32;1m[INFO]\x1b[0m " },
        { log_level::WARN, " \x1b[33;1m[WARNING]\x1b[0m " },
        { log_level::ERROR, " \x1b[31;1m[ERROR]\x1b[0m " }
    };

    #if defined(LOGGING_LEVEL_ALL) || defined(LOGGING_LEVEL_TRACE)
        constexpr log_level LOG_LEVEL_CUTOFF = log_level::TRACE;
    #elif defined(LOGGING_LEVEL_DEBUG)
        constexpr log_level LOG_LEVEL_CUTOFF = log_level::DEBUG;
    #elif defined(LOGGING_LEVEL_WARN)
        constexpr log_level LOG_LEVEL_CUTOFF = log_level::WARN;
    #elif defined(LOGGING_LEVEL_ERROR)
        constexpr log_level LOG_LEVEL_CUTOFF = log_level::ERROR;
    #elif defined(LOGGING_LEVEL_NONE)
        constexpr log_level LOG_LEVEL_CUTOFF = log_level::ERROR + 1;
    #else
        constexpr log_level LOG_LEVEL_CUTOFF = log_level::INFO;
    #endif

    namespace _
    {
        void log(const std::string& message, const log_level level, bool color);
    }

    template<typename... Args>
    void logTrace(const std::string& fmtStr, Args&&... args)
    {
        std::string message = fmt::vformat(fmtStr, fmt::make_format_args(args...));
        _::log(message, log_level::TRACE, true);
    };

    template<typename... Args>
    void logDebug(const std::string& fmtStr, Args&&... args)
    {
        std::string message = fmt::vformat(fmtStr, fmt::make_format_args(args...));
        _::log(message, log_level::DEBUG, true);
    };

    template<typename... Args>
    void logInfo(const std::string& fmtStr, Args&&... args)
    {
        std::string message = fmt::vformat(fmtStr, fmt::make_format_args(args...));
        _::log(message, log_level::INFO, true);
    };

    template<typename... Args>
    void logWarning(const std::string& fmtStr, Args&&... args)
    {
        std::string message = fmt::vformat(fmtStr, fmt::make_format_args(args...));
        _::log(message, log_level::WARN, true);
    };

    template<typename... Args>
    void logError(const std::string& fmtStr, Args&&... args)
    {
        std::string message = fmt::vformat(fmtStr, fmt::make_format_args(args...));
        _::log(message, log_level::ERROR, true);
    };

    // Overload for simple string logging
    API void logTrace(const std::string& message, const bool color = true);
    API void logDebug(const std::string& message, const bool color = true);
    API void logInfo(const std::string& message, const bool color = true);
    API void logWarning(const std::string& message, const bool color = true);
    API void logError(const std::string& message, const bool color = true);
}