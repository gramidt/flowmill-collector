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

#include <util/counter.h>

#include <platform/types.h>

#include <gtest/gtest.h>

#include <chrono>

namespace data {

TEST(counter_int, default_ctor) {
  Counter<int> const counter;
  EXPECT_TRUE(counter.empty());
  EXPECT_EQ(nullptr, counter.try_value());
}

TEST(counter_int, cast_constructor) {
  Counter<int> const counter(10);
  EXPECT_FALSE(counter.empty());
  EXPECT_EQ(10, counter.value());
  EXPECT_EQ(10, *counter.try_value());
}

TEST(counter_int, default_ctor_add_4_values) {
  Counter<int> counter;
  counter += 10;
  counter += 20;
  counter += 30;
  counter += 40;
  EXPECT_FALSE(counter.empty());
  EXPECT_EQ(40, counter.value());
  EXPECT_EQ(40, *counter.try_value());
}

TEST(counter_int, cast_ctor_add_4_values) {
  Counter<int> counter(60);
  counter += 90;
  counter += 70;
  counter += 50;
  counter += 80;
  EXPECT_FALSE(counter.empty());
  EXPECT_EQ(80, counter.value());
  EXPECT_EQ(80, *counter.try_value());
}

TEST(counter_int, reset) {
  Counter<int> counter(60);
  counter += 90;
  counter += 70;
  counter += 50;
  counter += 80;

  counter.reset();
  EXPECT_TRUE(counter.empty());
  EXPECT_EQ(nullptr, counter.try_value());
}

TEST(counter_chrono, default_ctor) {
  Counter<std::chrono::seconds> const counter;
  EXPECT_TRUE(counter.empty());
  EXPECT_EQ(nullptr, counter.try_value());
}

TEST(counter_chrono, cast_constructor) {
  Counter<std::chrono::seconds> const counter(10s);
  EXPECT_FALSE(counter.empty());
  EXPECT_EQ(10s, counter.value());
  EXPECT_EQ(10s, *counter.try_value());
}

TEST(counter_chrono, default_ctor_add_4_values) {
  Counter<std::chrono::seconds> counter;
  counter += 10s;
  counter += 20s;
  counter += 30s;
  counter += 40s;
  EXPECT_FALSE(counter.empty());
  EXPECT_EQ(40s, counter.value());
  EXPECT_EQ(40s, *counter.try_value());
}

TEST(counter_chrono, cast_ctor_add_4_values) {
  Counter<std::chrono::seconds> counter(60s);
  counter += 90s;
  counter += 70s;
  counter += 50s;
  counter += 80s;
  EXPECT_FALSE(counter.empty());
  EXPECT_EQ(80s, counter.value());
  EXPECT_EQ(80s, *counter.try_value());
}

TEST(counter_chrono, reset) {
  Counter<std::chrono::seconds> counter(60s);
  counter += 90s;
  counter += 70s;
  counter += 50s;
  counter += 80s;

  counter.reset();
  EXPECT_TRUE(counter.empty());
  EXPECT_EQ(nullptr, counter.try_value());
}

} // namespace data
