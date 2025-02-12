//
// Copyright 2021 Splunk Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include "util/error_handling.h"

#include "util/log.h"
#include "util/defer.h"

#include <absl/debugging/symbolize.h>
#include <absl/strings/str_cat.h>

#include <execinfo.h>
#include <iostream>
#include <sstream>
#include <string>

namespace {

#ifndef NDEBUG

void stacktrace(std::ostream &os)
{
  // Get the list of PCs
  static const int kMaxPcBufferSize = 1000;
  void *pc_buffer[kMaxPcBufferSize];
  const int pc_entries = backtrace(pc_buffer, kMaxPcBufferSize);

  // Print out the symbols for each stack entry.
  char **backtrace_names = backtrace_symbols(pc_buffer, pc_entries);
  if (backtrace_names != nullptr) {
    Defer free_names([backtrace_names] { free(backtrace_names); });

    static const int kMaxSymbolLength = 1024;
    static const char kUnknownSymbol[] = "(unknown)";
    for (int i = 0; i < pc_entries; i++) {
      const void *const pc = pc_buffer[i];

      char symbol_buffer[kMaxSymbolLength];
      const char *symbol = kUnknownSymbol;
      if (absl::Symbolize(pc, symbol_buffer, kMaxSymbolLength)) {
        symbol = symbol_buffer;
      }

      os << "  " << backtrace_names[i] << " - " << symbol << std::endl;
    }
  }
}

} // namespace

namespace internal {

Panicker::~Panicker()
{
  std::stringstream message_stream;

  // Buffer the preamble and custom message (if there is one).
  message_stream << preamble_;

  if (!custom_message_.empty()) {
    message_stream << " - " << custom_message_;
  }
  message_stream << std::endl;

  // Buffer  the stack trace.
  stacktrace(message_stream);

  LOG::critical(message_stream.str());
  std::exit(1);
}

#endif // NDEBUG

} // namespace internal
