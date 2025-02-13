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

#ifndef SRC_JITBUF_ID_MAPPING_H_
#define SRC_JITBUF_ID_MAPPING_H_

#include <platform/platform.h>
#include <google/protobuf/descriptor.h>

namespace jitbuf {

/**
 * Class representing a mapping of a single package
 */
class IdMapping {
public:
  /**
   * C'tor
   * @param file: the proto file that is being compiled.
   */
  IdMapping(const google::protobuf::FileDescriptor *file);

  /**
   * Maps the given package-internal RPC ID to a global RPC ID.
   */
  u32 map(u32 n);

private:
  /* interval starts */
  std::vector<u32> first;
  /* interval ends (inclusive) */
  std::vector<u32> last;
};

} // namespace jitbuf

#endif /* SRC_JITBUF_ID_MAPPING_H_ */
