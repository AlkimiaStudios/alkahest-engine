#include "./log.h"

namespace Alkahest
{
    namespace
    {
        // Forward definitions
        void log(const std::string& message, const log_level level, const bool color);
        void log(const std::string& message);

        std::string timestamp()
        {
            std::chrono::system_clock::time_point tp = std::chrono::system_clock::now();
            std::time_t tt = std::chrono::system_clock::to_time_t(tp);
            std::tm gmt{}; gmtime_r(&tt, &gmt);
            std::chrono::duration<double> fractional_seconds =
                (tp - std::chrono::system_clock::from_time_t(tt)) + std::chrono::seconds(gmt.tm_sec);
            std::string buffer("YYYY-MM-DDThh:mm:ss.xxxxxx");
            sprintf(&buffer.front(), "%04d-%02d-%02dT%02d:%02d:%09.6f", gmt.tm_year + 1900, gmt.tm_mon + 1,
                gmt.tm_mday, gmt.tm_hour, gmt.tm_min, fractional_seconds.count());
            return buffer;
        }
        
        void log(const std::string& message, const log_level level, const bool color)
        {
            if (level < LOG_LEVEL_CUTOFF)
                return;
            std::string output;
            std::unordered_map<log_level, std::string, enum_hasher> levels =
                color ? coloredText : uncoloredText;
            output.reserve(message.length() + 64);
            output.append(timestamp());
            output.append(levels.find(level)->second);
            output.append(message);
            output.push_back('\n');
            log(output);
        }

        void log(const std::string& message)
        {
            std::cout << message;
            std::cout.flush();
        }
    }

    void logTrace(const std::string& message, const bool color)
    {
        log(message, log_level::TRACE, color);
    }

    void logDebug(const std::string& message, const bool color)
    {
        log(message, log_level::DEBUG, color);
    }

    void logInfo(const std::string& message, const bool color)
    {
        log(message, log_level::INFO, color);
    }

    void logWarning(const std::string& message, const bool color)
    {
        log(message, log_level::WARN, color);
    }

    void logError(const std::string& message, const bool color)
    {
        log(message, log_level::ERROR, color);
    }
}