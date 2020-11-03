/*
 * Copyright The OpenTelemetry Authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <memory>
#include <vector>
#include "opentelemetry/logs/log_record.h"
#include "opentelemetry/sdk/logs/processor.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace logs
{
/**
 * ExportResult is returned as result of exporting a batch of Log Records.
 */
enum class ExportResult
{
  // The batch was exported successfully
  kSuccess = 0,
  // The batch was exported unsuccessfully and was dropped
  kFailure
};

/**
 * LogExporter defines the interface that log exporters must implement.
 */
class LogExporter
{
public:
  virtual ~LogExporter() = default;

  /**
   * Exporters that implement this should typically format each LogRecord into the format
   * required by the exporter destination (e.g. JSON), then send the LogRecord to the exporter.
   * The exporter may retry logs a maximum of 3 times before dropping and returning kFailure.
   * If this exporter is already shut down, should return kFailure.
   * @param records: a vector of unique pointers to log records
   * @returns an ExportResult code (whether export was success or failure)
   *
   * TODO: This method should not block indefinitely. Should abort within timeout.
   */
  virtual ExportResult Export(
      const std::vector<std::unique_ptr<opentelemetry::logs::LogRecord>> &records) noexcept = 0;

  /**
   * Marks the exporter as ShutDown and cleans up any resources as required.
   * Shutdown should be called only once for each Exporter instance.
   * @param timeout this method should not block indefinitely; should abort within timeout.
   * @return a ShutDownResult code (if it succeeded, failed or timed out)
   */
  virtual ShutdownResult Shutdown(
      std::chrono::microseconds timeout = std::chrono::microseconds(0)) noexcept = 0;
};
}  // namespace logs
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
