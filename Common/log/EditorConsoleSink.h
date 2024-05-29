#pragma once

#include "spdlog/sinks/base_sink.h"
#include <mutex>
#include <string>
#include <ctime>

namespace Seele {
#define BIT(x) (1u << x)

    enum class ConsoleMessageFlags : int16_t {
        None = -1,
        Info = BIT(0),
        Warning = BIT(1),
        Error = BIT(2),
        All = Info | Warning | Error
    };

    struct ConsoleMessage {
        std::string ShortMessage;
        std::string LongMessage;
        int16_t Flags{};
        time_t Time{};
    };
}

namespace Seele {
    class EditorConsoleSink : public spdlog::sinks::base_sink<std::mutex> {
    public:
        explicit EditorConsoleSink(uint32_t bufferCapacity)
            : m_MessageBufferCapacity(bufferCapacity), m_MessageBuffer(bufferCapacity) {
        }

        ~EditorConsoleSink() override = default;

        EditorConsoleSink(const EditorConsoleSink&other) = delete;

        EditorConsoleSink& operator=(const EditorConsoleSink&other) = delete;

    protected:
        void sink_it_(const spdlog::details::log_msg&msg) override {
            spdlog::memory_buf_t formatted;
            spdlog::sinks::base_sink<std::mutex>::formatter_->format(msg, formatted);
            std::string longMessage = fmt::to_string(formatted);
            std::string shortMessage = longMessage;

            if (shortMessage.length() > 100) {
                size_t spacePos = shortMessage.find_first_of(' ', 100);
                if (spacePos != std::string::npos)
                    shortMessage.replace(spacePos, shortMessage.length() - 1, "...");
            }

            m_MessageBuffer[m_MessageCount++] = ConsoleMessage{
                shortMessage, longMessage, GetMessageFlags(msg.level), std::chrono::system_clock::to_time_t(msg.time)
            };

            if (m_MessageCount == m_MessageBufferCapacity)
                flush_();
        }

        void flush_() override {
            for (const auto&message: m_MessageBuffer) {
                // Show Message
                // EditorConsolePanel::PushMessage(message);
            }
            m_MessageCount = 0;
        }

    private:
        static int16_t GetMessageFlags(spdlog::level::level_enum level) {
            int16_t flags = 0;

            switch (level) {
                case spdlog::level::trace:
                    break;
                case spdlog::level::debug:
                    break;
                case spdlog::level::info: {
                    flags |= (int16_t)ConsoleMessageFlags::Info;
                    break;
                }
                case spdlog::level::warn: {
                    flags |= (int16_t)ConsoleMessageFlags::Warning;
                    break;
                }
                case spdlog::level::err:
                case spdlog::level::critical: {
                    flags |= (int16_t)ConsoleMessageFlags::Error;
                    break;
                }
                default: break;
            }

            return flags;
        }

    private:
        uint32_t m_MessageBufferCapacity;
        std::vector<ConsoleMessage> m_MessageBuffer;
        uint32_t m_MessageCount = 0;
    };
}
