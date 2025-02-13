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

#include <platform/types.h>

#include <string_view>
#include <tuple>
#include <utility>

class VersionInfo {
public:
  constexpr explicit VersionInfo(
    u32 major = 0,
    u32 minor = 0,
    u32 build = 0,
    std::string_view signature = {}
  ):
    version_(major, minor, build, signature)
  {}

  constexpr VersionInfo(VersionInfo const &) = default;
  constexpr VersionInfo(VersionInfo &&) = default;

  void set(u32 major, u32 minor, u32 build = 0, std::string_view signature = {}) {
    version_ = std::make_tuple(major, minor, build, signature);
  }

  constexpr u32 major() const { return std::get<0>(version_); }
  constexpr u32 minor() const { return std::get<1>(version_); }
  constexpr u32 build() const { return std::get<2>(version_); }
  constexpr std::string_view signature() const { return std::get<3>(version_); }

  
  constexpr bool operator ==(VersionInfo const &rhs) const { return version_ == rhs.version_; }
  constexpr bool operator !=(VersionInfo const &rhs) const { return !(*this == rhs); }

  constexpr bool operator <(VersionInfo const &rhs) const { return version_ < rhs.version_; }
  constexpr bool operator <=(VersionInfo const &rhs) const { return !(rhs < *this); }
  constexpr bool operator >(VersionInfo const &rhs) const { return rhs < *this; }
  constexpr bool operator >=(VersionInfo const &rhs) const { return !(*this < rhs); }

  VersionInfo &operator =(VersionInfo const &) = default;
  VersionInfo &operator =(VersionInfo &&) = default;

  template <typename Out>
  friend Out &&operator <<(Out &&out, VersionInfo const &value) {
    out << value.major() << '.' << value.minor() << '.' << value.build();
    if (auto signature = value.signature(); !signature.empty()) {
      out << '-' << signature;
    }
    return std::forward<Out>(out);
  }

private:
  std::tuple<u32, u32, u32, std::string_view> version_;
};
