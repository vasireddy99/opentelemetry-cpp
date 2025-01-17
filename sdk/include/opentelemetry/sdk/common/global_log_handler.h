// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <iostream>
#include <sstream>

#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/sdk/common/attribute_utils.h"
#include "opentelemetry/version.h"

#define OTEL_INTERNAL_LOG_LEVEL_ERROR 0
#define OTEL_INTERNAL_LOG_LEVEL_WARN 1
#define OTEL_INTERNAL_LOG_LEVEL_INFO 2
#define OTEL_INTERNAL_LOG_LEVEL_DEBUG 3  // to be disabled in release

#ifndef OTEL_INTERNAL_LOG_LEVEL
#  define OTEL_INTERNAL_LOG_LEVEL OTEL_INTERNAL_LOG_LEVEL_WARN  // ERROR and WARN
#endif

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace common
{
namespace internal_log
{

enum class LogLevel
{
  Error = 0,
  Warning,
  Info,
  Debug
};

inline std::string LevelToString(LogLevel level)
{
  switch (level)
  {
    case LogLevel::Error:
      return "Error";
    case LogLevel::Warning:
      return "Warning";
    case LogLevel::Info:
      return "Info";
    case LogLevel::Debug:
      return "Debug";
  }
  return {};
}

class LogHandler
{
public:
  virtual void Handle(LogLevel level,
                      const char *file,
                      int line,
                      const char *msg,
                      const sdk::common::AttributeMap &attributes) noexcept = 0;
};

class DefaultLogHandler : public LogHandler
{
public:
  void Handle(LogLevel level,
              const char *file,
              int line,
              const char *msg,
              const sdk::common::AttributeMap &attributes) noexcept override
  {
    std::stringstream output_s;
    output_s << "[" << LevelToString(level) << "] ";
    if (file != nullptr)
    {
      output_s << "File: " << file << ":" << line;
    }
    if (msg != nullptr)
    {
      output_s << msg;
    }
    output_s << std::endl;
    // TBD - print attributes
    std::cout << output_s.str();  // thread safe.
  }
};

class NoopLogHandler : public LogHandler
{
public:
  void Handle(LogLevel level,
              const char *file,
              int line,
              const char *msg,
              const sdk::common::AttributeMap &error_attributes) noexcept override
  {
    // ignore the log message
  }
};

/**
 * Stores the singleton global LogHandler.
 */
class GlobalLogHandler
{
public:
  /**
   * Returns the singleton LogHandler.
   *
   * By default, a default LogHandler is returned. This will never return a
   * nullptr LogHandler.
   */
  static nostd::shared_ptr<LogHandler> GetLogHandler() noexcept
  {
    return nostd::shared_ptr<LogHandler>(GetHandler());
  }

  /**
   * Changes the singleton LogHandler.
   * This should be called once at the start of application before creating TracerProvider
   * instance.
   */
  static void SetLogHandler(nostd::shared_ptr<LogHandler> eh) noexcept { GetHandler() = eh; }

private:
  static nostd::shared_ptr<LogHandler> &GetHandler() noexcept
  {
    static nostd::shared_ptr<LogHandler> handler(new DefaultLogHandler);
    return handler;
  }
};

}  // namespace internal_log
}  // namespace common
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE

#define OTEL_INTERNAL_LOG_DISPATCH(level, message, attributes)                                     \
  do                                                                                               \
  {                                                                                                \
    using namespace opentelemetry::sdk::common::internal_log;                                      \
    std::stringstream tmp_stream;                                                                  \
    tmp_stream << message;                                                                         \
    GlobalLogHandler::GetLogHandler()->Handle(level, __FILE__, __LINE__, tmp_stream.str().c_str(), \
                                              attributes);                                         \
  } while (0)

#define OTEL_INTERNAL_LOG_GET_3RD_ARG(arg1, arg2, arg3, ...) arg3

#if OTEL_INTERNAL_LOG_LEVEL >= OTEL_INTERNAL_LOG_LEVEL_ERROR
#  define OTEL_INTERNAL_LOG_ERROR_1_ARGS(message)                                                  \
    OTEL_INTERNAL_LOG_DISPATCH(opentelemetry::sdk::common::internal_log::LogLevel::Error, message, \
                               {})
#  define OTEL_INTERNAL_LOG_ERROR_2_ARGS(message, attributes)                                      \
    OTEL_INTERNAL_LOG_DISPATCH(opentelemetry::sdk::common::internal_log::LogLevel::Error, message, \
                               attributes)
#  define OTEL_INTERNAL_LOG_ERROR_MACRO(...)                                   \
    OTEL_INTERNAL_LOG_GET_3RD_ARG(__VA_ARGS__, OTEL_INTERNAL_LOG_ERROR_2_ARGS, \
                                  OTEL_INTERNAL_LOG_ERROR_1_ARGS)
#  define OTEL_INTERNAL_LOG_ERROR(...) OTEL_INTERNAL_LOG_ERROR_MACRO(__VA_ARGS__)(__VA_ARGS__)
#else
#  define OTEL_INTERNAL_LOG_ERROR(...)
#endif

#if OTEL_INTERNAL_LOG_LEVEL >= OTEL_INTERNAL_LOG_LEVEL_WARN
#  define OTEL_INTERNAL_LOG_WARN_1_ARGS(message)                                            \
    OTEL_INTERNAL_LOG_DISPATCH(opentelemetry::sdk::common::internal_log::LogLevel::Warning, \
                               message, {})
#  define OTEL_INTERNAL_LOG_WARN_2_ARGS(message, attributes)                                \
    OTEL_INTERNAL_LOG_DISPATCH(opentelemetry::sdk::common::internal_log::LogLevel::Warning, \
                               message, attributes)
#  define OTEL_INTERNAL_LOG_WARN_MACRO(...)                                   \
    OTEL_INTERNAL_LOG_GET_3RD_ARG(__VA_ARGS__, OTEL_INTERNAL_LOG_WARN_2_ARGS, \
                                  OTEL_INTERNAL_LOG_WARN_1_ARGS)
#  define OTEL_INTERNAL_LOG_WARN(...) OTEL_INTERNAL_LOG_WARN_MACRO(__VA_ARGS__)(__VA_ARGS__)
#else
#  define OTEL_INTERNAL_LOG_ERROR(...)
#endif

#if OTEL_INTERNAL_LOG_LEVEL >= OTEL_INTERNAL_LOG_LEVEL_DEBUG
#  define OTEL_INTERNAL_LOG_DEBUG_1_ARGS(message)                                                  \
    OTEL_INTERNAL_LOG_DISPATCH(opentelemetry::sdk::common::internal_log::LogLevel::Debug, message, \
                               {})
#  define OTEL_INTERNAL_LOG_DEBUG_2_ARGS(message, attributes)                              \
    OTEL_INTERNAL_LOG_DISPATCH(opentelemetry::sdk::common::internal_logg::LogLevel::Debug, \
                               message, attributes)
#  define OTEL_INTERNAL_LOG_DEBUG_MACRO(...)                                   \
    OTEL_INTERNAL_LOG_GET_3RD_ARG(__VA_ARGS__, OTEL_INTERNAL_LOG_DEBUG_2_ARGS, \
                                  OTEL_INTERNAL_LOG_DEBUG_1_ARGS)
#  define OTEL_INTERNAL_LOG_DEBUG(...) OTEL_INTERNAL_LOG_DEBUG_MACRO(__VA_ARGS__)(__VA_ARGS__)
#else
#  define OTEL_INTERNAL_LOG_DEBUG(...)
#endif

#if OTEL_INTERNAL_LOG_LEVEL >= OTEL_INTERNAL_LOG_LEVEL_INFO
#  define OTEL_INTERNAL_LOG_INFO_1_ARGS(message)                                            \
    OTEL_INTERNAL_LOG_DISPATCH(opentelemetry::sdk::common::internal_logger::LogLevel::Info, \
                               message, {})
#  define OTEL_INTERNAL_LOG_INFO_2_ARGS(message, attributes)                                \
    OTEL_INTERNAL_LOG_DISPATCH(opentelemetry::sdk::common::internal_logger::LogLevel::Info, \
                               message, attributes)
#  define OTEL_INTERNAL_LOG_INFO_MACRO(...)                                    \
    OTEL_INTERNAL_LOG_GET_3RD_ARG(__VA_ARGS__, OTEL_INTERNAL_LOG_ERROR_2_ARGS, \
                                  OTEL_INTERNAL_LOG_ERROR_1_ARGS)
#  define OTEL_INTERNAL_LOG_INFO(...) OTEL_INTERNAL_LOG_INFO_MACRO(__VA_ARGS__)(__VA_ARGS__)
#else
#  define OTEL_INTERNAL_LOG_INFO(...)
#endif
