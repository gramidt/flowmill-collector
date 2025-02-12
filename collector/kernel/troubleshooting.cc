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

#include <collector/kernel/troubleshooting.h>

#include <common/linux_distro.h>
#include <util/log_formatters.h>

#include <spdlog/fmt/fmt.h>
#include <spdlog/fmt/ostr.h>

#include <iostream>
#include <thread>

#include <cstdlib>

static constexpr auto EXIT_GRACE_PERIOD = 60s;

constexpr std::string_view KERNEL_HEADERS_MANUAL_INSTALL_INSTRUCTIONS = R"INSTRUCTIONS(
In the meantime, please install kernel headers manually on each host before running
Flowmill Kernel Collector.

To manually install kernel headers, follow the instructions below:

  - for Debian/Ubuntu based distros, run:

      sudo apt-get install --yes "linux-headers-`uname -r`"

  - for RedHat based distros like CentOS and Amazon Linux, run:

      sudo yum install -y "kernel-devel-`uname -r`"
)INSTRUCTIONS";

void close_agent(int exit_code) {
  std::this_thread::sleep_for(EXIT_GRACE_PERIOD);
  exit(exit_code);
}

void print_troubleshooting_message_and_exit(
  logging::Logger &log,
  HostInfo const &info,
  EntrypointError error
) {
  if (error == EntrypointError::none) { return; }

  log.error(
    "entrypoint error {} (os={},flavor={},headers_src={},kernel={})",
    error, info.os, info.os_flavor, info.kernel_headers_source, info.kernel_version
  );

  switch (error) {
    case EntrypointError::unsupported_distro: {
      std::cout << fmt::format(R"TROUBLESHOOTING(
Automatically fetching kernel headers for the Linux distro '{}' is currently unsupported.

We're regularly adding kernel headers fetching support for popular Linux distros so if
you're using a well known distro, please reach out to Flowmill so we can better support
your use case.
)TROUBLESHOOTING", static_cast<LinuxDistro>(info.os_flavor))
        << std::endl << KERNEL_HEADERS_MANUAL_INSTALL_INSTRUCTIONS;
      break;
    }

    case EntrypointError::kernel_headers_fetch_error: {
      std::cout << fmt::format(R"TROUBLESHOOTING(
Problem while installing kernel headers for the host's Linux distro '{}'.

Please reach out to Flowmill and include this log in its entirety so we can diagnose and fix
the problem.
)TROUBLESHOOTING", static_cast<LinuxDistro>(info.os_flavor))
        << std::endl << KERNEL_HEADERS_MANUAL_INSTALL_INSTRUCTIONS;
      break;
    }

    case EntrypointError::kernel_headers_fetch_refuse: {
      std::cout << fmt::format(R"TROUBLESHOOTING(
As requested, refusing to automatically fetch kernel headers for the hosts's Linux distro '{}'.

In order to allow it, follow the instructions below:

  - for deployments using our helm charts from https://github.com/Flowmill/flowmill-k8s, set
    `agent.installKernelHeaders` to `true` in `values.yaml`:

      agent:
        installKernelHeaders: true

  - for deployments using Docker, set environment variable `FLOWMILL_KERNEL_HEADERS_AUTO_FETCH`
    to `true` by passing these additional command line flags to Docker:

      --env "FLOWMILL_KERNEL_HEADERS_AUTO_FETCH=true"

  - for ECS deployments, ensure that environment variable `FLOWMILL_KERNEL_HEADERS_AUTO_FETCH`
    is set to `true` in the Agent's task definitions;

  - for any other deployments, ensure that Flowmill Kernel Collector container will be started
    with environment variable `FLOWMILL_KERNEL_HEADERS_AUTO_FETCH` set to `true`.
)TROUBLESHOOTING", static_cast<LinuxDistro>(info.os_flavor))
        << std::endl << KERNEL_HEADERS_MANUAL_INSTALL_INSTRUCTIONS;
      break;
    }

    case EntrypointError::kernel_headers_missing_repo: {
      std::cout << fmt::format(R"TROUBLESHOOTING(
Unable to locate the configuration for the host's package manager in order to automatically
install kernel headers for the Linux distro '{}'.

Please reach out to Flowmill and include this log in its entirety so we can diagnose and fix
the problem.
)TROUBLESHOOTING", static_cast<LinuxDistro>(info.os_flavor))
        << std::endl << KERNEL_HEADERS_MANUAL_INSTALL_INSTRUCTIONS;
      break;
    }

    case EntrypointError::kernel_headers_misconfigured_repo: {
      std::cout << fmt::format(R"TROUBLESHOOTING(
Unable to use the host's package manager configuration to automatically install kernel headers
for the Linux distro '{}'.

Please reach out to Flowmill and include this log in its entirety so we can diagnose and fix
the problem.
)TROUBLESHOOTING", static_cast<LinuxDistro>(info.os_flavor))
        << std::endl << KERNEL_HEADERS_MANUAL_INSTALL_INSTRUCTIONS;
      break;
    }

    default: {
      std::cout << R"TROUBLESHOOTING(
Unknown error happened during boot up of the Flowmill Kernel Collector.

Please reach out to Flowmill and include this log in its entirety so we can diagnose and fix
the problem.
)TROUBLESHOOTING";
      break;
    }

    std::cout << std::endl << FLOWMILL_CONTACT_INFO_MESSAGE << std::endl;
    std::cout.flush();
  }

  close_agent(-1);
}

void print_troubleshooting_message_and_exit(
  logging::Logger &log,
  HostInfo const &info,
  TroubleshootItem item,
  std::exception const &e
) {
  if (item == TroubleshootItem::none) { return; }

  log.error(
    "troubleshoot item {} (os={},flavor={},headers_src={},kernel={}): {}",
    item, info.os, info.os_flavor, info.kernel_headers_source, info.kernel_version, e
  );

  switch (item) {
    case TroubleshootItem::bpf_compilation_failed: {
      std::cout << fmt::format(R"TROUBLESHOOTING(
Failed to compile eBPF code for the Linux distro '{}' running kernel version {}.

This usually means that kernel headers weren't installed correctly.

Please reach out to Flowmill and include this log in its entirety so we can diagnose and fix
the problem.
)TROUBLESHOOTING", static_cast<LinuxDistro>(info.os_flavor), info.kernel_version)
        << std::endl << KERNEL_HEADERS_MANUAL_INSTALL_INSTRUCTIONS;
      break;
    }

    default: {
      std::cout << R"TROUBLESHOOTING(
Unknown error happened in Flowmill Kernel Collector.

Please reach out to Flowmill and include this log in its entirety so we can diagnose and fix
the problem.
)TROUBLESHOOTING";
      break;
    }

    std::cout << std::endl << FLOWMILL_CONTACT_INFO_MESSAGE << std::endl;
    std::cout.flush();
  }

  close_agent(-1);
}
