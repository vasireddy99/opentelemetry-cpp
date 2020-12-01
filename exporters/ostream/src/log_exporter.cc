#include "opentelemetry/exporters/ostream/log_exporter.h"

#include <iostream>

namespace nostd   = opentelemetry::nostd;
namespace sdklogs = opentelemetry::sdk::logs;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace logs
{
OStreamLogExporter::OStreamLogExporter(std::ostream &sout) noexcept : sout_(sout) {}

sdklogs::ExportResult OStreamLogExporter::Export(
    const std::vector<std::unique_ptr<opentelemetry::logs::LogRecord>> &records) noexcept
{
  if (isShutdown_)
  {
    return sdklogs::ExportResult::kFailure;
  }

  for (auto &record : records)
  {
    // Convert trace, spanid, traceflags into string convertable representation 
    char trace_id[32]       = {0};
    record->trace_id.ToLowerBase16(trace_id);

    char span_id[16]       = {0};
    record->span_id.ToLowerBase16(span_id);

    char trace_flag[2] = {0};
    record->trace_flag.ToLowerBase16(trace_flag);

    // Print out each field of the log record

    sout_ << "{\n"
          << "    timestamp   : " << record->timestamp.time_since_epoch().count() << "\n"
          << "    severity    : " << static_cast<int>(record->severity) << "\n"
          << "    name        : " << record->name << "\n"
          << "    body        : " <<  record->body << "\n"
    //       << "    resource     : " <<   record->resource << "\n"
    //       << "    attributes     : " <<   record->attributes << "\n"
          << "    trace_id    : " <<   std::string(trace_id, 32) << "\n"
          << "    span_id     : " <<   std::string(span_id, 16) << "\n"
          << "    trace_flags : " <<  std::string(trace_flag, 2) << "\n"
          << "}\n";

    // Convert LogRecord to a JSON object
    // ordered_json log; // optionally instead "json log;"

    // log["timestamp"] =  record->timestamp.time_since_epoch().count();
    // log["severity"] =  record->severity;
    // log["name"] =  record->name;
    // log["body"] =  record->body;
    // // log["resource"] =  record->resource;
    // // log["attributes"] =  record->attributes;

    // char trace_id[32]       = {0};
    // record->trace_id.ToLowerBase16(trace_id);
    // // log["trace_id"] =  std::string(trace_id, 32);  
    // log["trace_id"] =  "";

    // char span_id[16]       = {0};
    // record->span_id.ToLowerBase16(span_id);
    // // log["span_id"] =  std::string(span_id, 16);  
    // log["span_id"] =  "";

    // char trace_flag[2] = {0};
    // record->trace_flag.ToLowerBase16(trace_flag);
    // log["trace_flags"] = std::string(trace_flag, 2);

    // pretty print with indentation of 4 spaces
    // sout_ << log.dump(4) << "\n"; 
  }

  return sdklogs::ExportResult::kSuccess;
}

void OStreamLogExporter::Shutdown(std::chrono::microseconds timeout) noexcept
{
  isShutdown_ = true;
}

}  // namespace logs
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
