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

// Regexp patterns for kernel blacklist
// failures must match all specified uname components
// components specified as NULL are not matched

// Pattern specification:
// { sysname, nodename, release, version, machine }

// minikube 4.15.0
{"Linux", "minikube", R"(^4\.15\.0.*)", NULL, NULL },

// Linux 4.19.17 from Ubuntu mainline repo
{"Linux", NULL, R"(^4\.19\.57.*)", NULL, NULL },

// Linux 5.1.16 from Ubuntu mainline repo
{"Linux", NULL, R"(^5\.1\.16.*)", NULL, NULL }
