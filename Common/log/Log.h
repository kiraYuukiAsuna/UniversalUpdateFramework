#pragma once

#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

#include <map>

namespace Seele {
    class Log {
    public:
        enum class Type : uint8_t {
            Core = 0, App
        };

        enum class Level : uint8_t {
            Trace = 0, Info, Warn, Error, Fatal
        };

        struct TagDetails {
            bool Enabled = true;
            Level LevelFilter = Level::Trace;
        };

        static void Init(const std::string&logsDirectory = "Logs",
                         const std::string&coreLogsFileName = "Core.log",
                         const std::string&appLogsFileName = "App.log",
                         const std::string&editorConsoleLogsFileName = "EditorConsole.log", bool hasConsole = true,
                         bool truncate = true);

        static void Shutdown();

        static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
        static std::shared_ptr<spdlog::logger>& GetAppLogger() { return s_AppLogger; }
        static std::shared_ptr<spdlog::logger>& GetEditorConsoleLogger() { return s_EditorConsoleLogger; }

        static bool HasTag(const std::string&tag) { return s_EnabledTags.find(tag) != s_EnabledTags.end(); }
        static std::map<std::string, TagDetails>& EnabledTags() { return s_EnabledTags; }

        template<typename... Args>
        static void PrintMessage(Log::Type type, Log::Level level, std::string_view tag,
                                 fmt::format_string<Args...> format, Args&&... args);

        // Enum utils
        static const char* LevelToString(Level level) {
            switch (level) {
                case Level::Trace: return "Trace";
                case Level::Info: return "Info";
                case Level::Warn: return "Warn";
                case Level::Error: return "Error";
                case Level::Fatal: return "Fatal";
            }
            return "";
        }

        static Level LevelFromString(std::string_view string) {
            if (string == "Trace") return Level::Trace;
            if (string == "Info") return Level::Info;
            if (string == "Warn") return Level::Warn;
            if (string == "Error") return Level::Error;
            if (string == "Fatal") return Level::Fatal;

            return Level::Trace;
        }

        static bool IsInitialized() { return m_Initialized; }

        static void Flush() {
            if (m_Initialized) {
                s_CoreLogger->flush();
                s_AppLogger->flush();
                s_EditorConsoleLogger->flush();
            }
        }

    private:
        static std::shared_ptr<spdlog::logger> s_CoreLogger;
        static std::shared_ptr<spdlog::logger> s_AppLogger;
        static std::shared_ptr<spdlog::logger> s_EditorConsoleLogger;

        inline static std::map<std::string, TagDetails> s_EnabledTags;

        static bool m_HasConsole;
        static bool m_Truncate;

        static bool m_Initialized;
    };
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Tagged logs (prefer these!)                                                                                      //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Core logging
#define SEELE_CORE_TRACE_TAG(tag, ...) ::Seele::Log::PrintMessage(::Seele::Log::Type::Core, ::Seele::Log::Level::Trace, tag, __VA_ARGS__)
#define SEELE_CORE_INFO_TAG(tag, ...)  ::Seele::Log::PrintMessage(::Seele::Log::Type::Core, ::Seele::Log::Level::Info, tag, __VA_ARGS__)
#define SEELE_CORE_WARN_TAG(tag, ...)  ::Seele::Log::PrintMessage(::Seele::Log::Type::Core, ::Seele::Log::Level::Warn, tag, __VA_ARGS__)
#define SEELE_CORE_ERROR_TAG(tag, ...) ::Seele::Log::PrintMessage(::Seele::Log::Type::Core, ::Seele::Log::Level::Error, tag, __VA_ARGS__)
#define SEELE_CORE_FATAL_TAG(tag, ...) ::Seele::Log::PrintMessage(::Seele::Log::Type::Core, ::Seele::Log::Level::Fatal, tag, __VA_ARGS__)

// App logging
#define SEELE_TRACE_TAG(tag, ...) ::Seele::Log::PrintMessage(::Seele::Log::Type::App, ::Seele::Log::Level::Trace, tag, __VA_ARGS__)
#define SEELE_INFO_TAG(tag, ...)  ::Seele::Log::PrintMessage(::Seele::Log::Type::App, ::Seele::Log::Level::Info, tag, __VA_ARGS__)
#define SEELE_WARN_TAG(tag, ...)  ::Seele::Log::PrintMessage(::Seele::Log::Type::App, ::Seele::Log::Level::Warn, tag, __VA_ARGS__)
#define SEELE_ERROR_TAG(tag, ...) ::Seele::Log::PrintMessage(::Seele::Log::Type::App, ::Seele::Log::Level::Error, tag, __VA_ARGS__)
#define SEELE_FATAL_TAG(tag, ...) ::Seele::Log::PrintMessage(::Seele::Log::Type::App, ::Seele::Log::Level::Fatal, tag, __VA_ARGS__)

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Core Logging
#define SEELE_CORE_TRACE(...)  ::Seele::Log::PrintMessage(::Seele::Log::Type::Core, ::Seele::Log::Level::Trace, "", __VA_ARGS__)
#define SEELE_CORE_INFO(...)   ::Seele::Log::PrintMessage(::Seele::Log::Type::Core, ::Seele::Log::Level::Info, "", __VA_ARGS__)
#define SEELE_CORE_WARN(...)   ::Seele::Log::PrintMessage(::Seele::Log::Type::Core, ::Seele::Log::Level::Warn, "", __VA_ARGS__)
#define SEELE_CORE_ERROR(...)  ::Seele::Log::PrintMessage(::Seele::Log::Type::Core, ::Seele::Log::Level::Error, "", __VA_ARGS__)
#define SEELE_CORE_FATAL(...)  ::Seele::Log::PrintMessage(::Seele::Log::Type::Core, ::Seele::Log::Level::Fatal, "", __VA_ARGS__)

// App Logging
#define SEELE_TRACE(...)   ::Seele::Log::PrintMessage(::Seele::Log::Type::App, ::Seele::Log::Level::Trace, "", __VA_ARGS__)
#define SEELE_INFO(...)    ::Seele::Log::PrintMessage(::Seele::Log::Type::App, ::Seele::Log::Level::Info, "", __VA_ARGS__)
#define SEELE_WARN(...)    ::Seele::Log::PrintMessage(::Seele::Log::Type::App, ::Seele::Log::Level::Warn, "", __VA_ARGS__)
#define SEELE_ERROR(...)   ::Seele::Log::PrintMessage(::Seele::Log::Type::App, ::Seele::Log::Level::Error, "", __VA_ARGS__)
#define SEELE_FATAL(...)   ::Seele::Log::PrintMessage(::Seele::Log::Type::App, ::Seele::Log::Level::Fatal, "", __VA_ARGS__)

// Editor Console Logging Macros
#define SEELE_CONSOLE_LOG_TRACE(...)   Seele::Log::GetEditorConsoleLogger()->trace(__VA_ARGS__)
#define SEELE_CONSOLE_LOG_INFO(...)    Seele::Log::GetEditorConsoleLogger()->info(__VA_ARGS__)
#define SEELE_CONSOLE_LOG_WARN(...)    Seele::Log::GetEditorConsoleLogger()->warn(__VA_ARGS__)
#define SEELE_CONSOLE_LOG_ERROR(...)   Seele::Log::GetEditorConsoleLogger()->error(__VA_ARGS__)
#define SEELE_CONSOLE_LOG_FATAL(...)   Seele::Log::GetEditorConsoleLogger()->critical(__VA_ARGS__)

namespace Seele {
    template<typename... Args>
    void Log::PrintMessage(Log::Type type, Log::Level level, std::string_view tag, fmt::format_string<Args...> format,
                           Args&&... args) {
        auto detail = s_EnabledTags[std::string(tag)];
        if (detail.Enabled && detail.LevelFilter <= level) {
            auto logger = (type == Type::Core) ? GetCoreLogger() : GetAppLogger();
            auto userLogString = fmt::format(format, std::forward<Args>(args)...);
            auto finaLogString = tag.empty()
                                     ? fmt::format("{}{}", tag, userLogString)
                                     : fmt::format("[{}] {}", tag, userLogString);
            switch (level) {
                case Level::Trace:
                    logger->trace(finaLogString);
                    break;
                case Level::Info:
                    logger->info(finaLogString);
                    break;
                case Level::Warn:
                    logger->warn(finaLogString);
                    break;
                case Level::Error:
                    logger->error(finaLogString);
                    break;
                case Level::Fatal:
                    logger->critical(finaLogString);
                    break;
            }
        }
    }
}
