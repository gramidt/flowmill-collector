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

#include <common/port_protocol.h>

#include <nlohmann/json.hpp>

#include <map>
#include <optional>
#include <string>
#include <string_view>

#include <cstdint>

/**
 * Reads metadata posted by Kubernetes to the Docker engine.
 */
class K8sMetadata {
public:
  static constexpr std::string_view CONTAINER_PORTS = "annotation.io.kubernetes.container.ports";
  static constexpr std::string_view CONTAINER_NAME = "io.kubernetes.container.name";
  static constexpr std::string_view POD_NAME = "io.kubernetes.pod.name";
  static constexpr std::string_view POD_NS = "io.kubernetes.pod.namespace";
  static constexpr std::string_view POD_UID = "io.kubernetes.pod.uid";
  static constexpr std::string_view SANDBOX_ID = "io.kubernetes.sandbox.id";

  static constexpr std::string_view POD_CONTAINER_NAME_VALUE = "POD";

  struct PortInfo {
    std::uint16_t port;
    PortProtocol protocol;
    std::string name;
  };

  // `labels` is the `.Config.Labels` section from the equivalent of `docker inspect`
  explicit K8sMetadata(nlohmann::json const &labels);

  std::string_view container_name() const { return container_name_; }
  std::string_view pod_name() const { return pod_name_; }
  std::string_view pod_ns() const { return pod_ns_; }
  std::string_view pod_uid() const { return pod_uid_; }
  std::string_view sandbox_uid() const { return sandbox_uid_; }
  std::map<std::uint16_t, PortInfo> ports() const { return ports_; }

  explicit operator bool() const;

  template <typename Out>
  friend Out &&operator <<(Out &&out, K8sMetadata const &what);

private:
  std::string container_name_;
  std::string pod_name_;
  std::string pod_ns_;
  std::string pod_uid_;
  std::string sandbox_uid_;
  std::map<std::uint16_t, PortInfo> ports_;
};

#include <util/k8s_metadata.inl>
