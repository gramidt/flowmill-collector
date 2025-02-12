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

#pragma once

#include <channel/ibuffered_writer.h>
#include <platform/platform.h>
#include <util/element_queue_cpp.h>

// Adapter class for writing to ElementQueues through the
// IBufferedWriter interface.
//
class ElementQueueWriter : public IBufferedWriter {
public:
  ElementQueueWriter(ElementQueue &queue);
  virtual ~ElementQueueWriter();
  
  // Starts a write operation of size \p length.
  //
  // Will block until there is enough space in the queue for the write.
  //
  // Returns the memory where caller should write the data, or nullptr in case
  // of an error.
  //
  Expected<u8 *, std::error_code> start_write(u32 length) override;

  void finish_write() override;

  std::error_code flush() override;

  u32 buf_size() const override;

  // Number of times writing has stalled because of no space in the queue.
  u64 num_write_stalls() const { return num_write_stalls_; }

  // Queue this writer is writing to.
  ElementQueue const &queue() const { return queue_; }

  bool is_writable() const override { return true; }

private:
  ElementQueue &queue_;
  u64 num_write_stalls_{0};
};
