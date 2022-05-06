// Copyright 2021 The Pigweed Authors
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not
// use this file except in compliance with the License. You may obtain a copy of
// the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations under
// the License.

#include "pw_gpio/gpio.h"

#include "gtest/gtest.h"
#include "pw_status/status.h"

namespace pw::gpio {
namespace {

// The base class should be compact.
static_assert(sizeof(DigitalIoOptional) <= 2 * sizeof(void*),
              "DigitalIo should be no larger than two pointers (vtable pointer "
              "& packed members)");

// Skeleton implementations to test DigitalIo methods.
class TestDigitalInterrupt : public DigitalInterrupt {
 public:
  TestDigitalInterrupt() = default;

 private:
  Status DoEnable(bool) override { return OkStatus(); }

  Status DoSetInterruptHandler(InterruptTrigger, InterruptHandler&&) override {
    return OkStatus();
  }
  Status DoEnableInterruptHandler(bool) override { return OkStatus(); }
};

class TestDigitalIn : public DigitalIn {
 public:
  TestDigitalIn() : state_(State::kInactive) {}

 private:
  Status DoEnable(bool) override { return OkStatus(); }
  Result<State> DoGetState() override { return state_; }

  const State state_;
};

class TestDigitalInInterrupt : public DigitalInInterrupt {
 public:
  TestDigitalInInterrupt() : state_(State::kInactive) {}

 private:
  Status DoEnable(bool) override { return OkStatus(); }
  Result<State> DoGetState() override { return state_; }

  Status DoSetInterruptHandler(InterruptTrigger, InterruptHandler&&) override {
    return OkStatus();
  }
  Status DoEnableInterruptHandler(bool) override { return OkStatus(); }

  const State state_;
};

class TestDigitalOut : public DigitalOut {
 public:
  TestDigitalOut() {}

 private:
  Status DoEnable(bool) override { return OkStatus(); }
  Status DoSetState(State) override { return OkStatus(); }
};

class TestDigitalOutInterrupt : public DigitalOutInterrupt {
 public:
  TestDigitalOutInterrupt() {}

 private:
  Status DoEnable(bool) override { return OkStatus(); }
  Status DoSetState(State) override { return OkStatus(); }

  Status DoSetInterruptHandler(InterruptTrigger, InterruptHandler&&) override {
    return OkStatus();
  }
  Status DoEnableInterruptHandler(bool) override { return OkStatus(); }
};

class TestDigitalInOut : public DigitalInOut {
 public:
  TestDigitalInOut() : state_(State::kInactive) {}

 private:
  Status DoEnable(bool) override { return OkStatus(); }
  Result<State> DoGetState() override { return state_; }
  Status DoSetState(State state) override {
    state_ = state;
    return OkStatus();
  }

  State state_;
};

class TestDigitalInOutInterrupt : public DigitalInOutInterrupt {
 public:
  TestDigitalInOutInterrupt() : state_(State::kInactive) {}

 private:
  Status DoEnable(bool) override { return OkStatus(); }
  Result<State> DoGetState() override { return state_; }
  Status DoSetState(State state) override {
    state_ = state;
    return OkStatus();
  }

  Status DoSetInterruptHandler(InterruptTrigger, InterruptHandler&&) override {
    return OkStatus();
  }
  Status DoEnableInterruptHandler(bool) override { return OkStatus(); }

  State state_;
};

// Test conversions between different interfaces.
static_assert(!std::is_convertible<TestDigitalInterrupt, DigitalIn&>());
static_assert(!std::is_convertible<TestDigitalInterrupt, DigitalOut&>());
static_assert(
    !std::is_convertible<TestDigitalInterrupt, DigitalInInterrupt&>());
static_assert(
    !std::is_convertible<TestDigitalInterrupt, DigitalOutInterrupt&>());
static_assert(
    !std::is_convertible<TestDigitalInterrupt, DigitalInOutInterrupt&>());

static_assert(!std::is_convertible<TestDigitalIn, DigitalOut&>());
static_assert(!std::is_convertible<TestDigitalIn, DigitalInterrupt&>());
static_assert(!std::is_convertible<TestDigitalIn, DigitalInInterrupt&>());
static_assert(!std::is_convertible<TestDigitalIn, DigitalOutInterrupt&>());

static_assert(std::is_convertible<TestDigitalInInterrupt, DigitalIn&>());
static_assert(!std::is_convertible<TestDigitalInInterrupt, DigitalOut&>());
static_assert(std::is_convertible<TestDigitalInInterrupt, DigitalInterrupt&>());
static_assert(
    !std::is_convertible<TestDigitalInInterrupt, DigitalOutInterrupt&>());

static_assert(!std::is_convertible<TestDigitalOut, DigitalIn&>());
static_assert(!std::is_convertible<TestDigitalOut, DigitalInterrupt&>());
static_assert(!std::is_convertible<TestDigitalOut, DigitalInInterrupt&>());
static_assert(!std::is_convertible<TestDigitalOut, DigitalOutInterrupt&>());

static_assert(!std::is_convertible<TestDigitalOutInterrupt, DigitalIn&>());
static_assert(std::is_convertible<TestDigitalOutInterrupt, DigitalOut&>());
static_assert(
    std::is_convertible<TestDigitalOutInterrupt, DigitalInterrupt&>());
static_assert(
    !std::is_convertible<TestDigitalOutInterrupt, DigitalInInterrupt&>());

static_assert(std::is_convertible<TestDigitalInOut, DigitalIn&>());
static_assert(std::is_convertible<TestDigitalInOut, DigitalOut&>());
static_assert(!std::is_convertible<TestDigitalInOut, DigitalInterrupt&>());
static_assert(!std::is_convertible<TestDigitalInOut, DigitalInInterrupt&>());
static_assert(!std::is_convertible<TestDigitalInOut, DigitalOutInterrupt&>());

static_assert(std::is_convertible<TestDigitalInOutInterrupt, DigitalIn&>());
static_assert(std::is_convertible<TestDigitalInOutInterrupt, DigitalOut&>());
static_assert(
    std::is_convertible<TestDigitalInOutInterrupt, DigitalInterrupt&>());
static_assert(
    std::is_convertible<TestDigitalInOutInterrupt, DigitalInInterrupt&>());
static_assert(
    std::is_convertible<TestDigitalInOutInterrupt, DigitalOutInterrupt&>());

void FakeInterruptHandler(State) {}

void TestInput(DigitalIoOptional& line) {
  ASSERT_EQ(OkStatus(), line.Enable());

  auto state_result = line.GetState();
  ASSERT_EQ(OkStatus(), state_result.status());
  ASSERT_EQ(State::kInactive, state_result.value());

  ASSERT_EQ(OkStatus(), line.Disable());
}

void TestOutput(DigitalIoOptional& line) {
  ASSERT_EQ(OkStatus(), line.Enable());

  ASSERT_EQ(OkStatus(), line.SetState(State::kActive));

  ASSERT_EQ(OkStatus(), line.Disable());
}

void TestOutputReadback(DigitalIoOptional& line) {
  ASSERT_EQ(OkStatus(), line.Enable());

  ASSERT_EQ(OkStatus(), line.SetState(State::kActive));
  auto state_result = line.GetState();
  ASSERT_EQ(OkStatus(), state_result.status());
  ASSERT_EQ(State::kActive, state_result.value());

  ASSERT_EQ(OkStatus(), line.Disable());
}

void TestInterrupt(DigitalIoOptional& line) {
  ASSERT_EQ(OkStatus(), line.Enable());

  ASSERT_EQ(OkStatus(),
            line.SetInterruptHandler(InterruptTrigger::kBothEdges,
                                     FakeInterruptHandler));
  ASSERT_EQ(OkStatus(), line.EnableInterruptHandler());
  ASSERT_EQ(OkStatus(), line.EnableInterruptHandler());
  ASSERT_EQ(OkStatus(), line.DisableInterruptHandler());
  ASSERT_EQ(OkStatus(), line.ClearInterruptHandler());

  ASSERT_EQ(OkStatus(), line.Disable());
}

TEST(Digital, Interrupt) {
  TestDigitalInterrupt line;

  ASSERT_EQ(false, line.provides_input());
  ASSERT_EQ(false, line.provides_output());
  ASSERT_EQ(true, line.provides_interrupt());

  TestInterrupt(line);
}

TEST(Digital, In) {
  TestDigitalIn line;

  ASSERT_EQ(true, line.provides_input());
  ASSERT_EQ(false, line.provides_output());
  ASSERT_EQ(false, line.provides_interrupt());

  TestInput(line);
}

TEST(Digital, InInterrupt) {
  TestDigitalInInterrupt line;

  ASSERT_EQ(true, line.provides_input());
  ASSERT_EQ(false, line.provides_output());
  ASSERT_EQ(true, line.provides_interrupt());

  TestInput(line);
  TestInterrupt(line);
}

TEST(Digital, Out) {
  TestDigitalOut line;

  ASSERT_EQ(false, line.provides_input());
  ASSERT_EQ(true, line.provides_output());
  ASSERT_EQ(false, line.provides_interrupt());

  TestOutput(line);
}

TEST(Digital, OutInterrupt) {
  TestDigitalOutInterrupt line;

  ASSERT_EQ(false, line.provides_input());
  ASSERT_EQ(true, line.provides_output());
  ASSERT_EQ(true, line.provides_interrupt());

  TestOutput(line);
  TestInterrupt(line);
}

TEST(Digital, InOut) {
  TestDigitalInOut line;

  ASSERT_EQ(true, line.provides_input());
  ASSERT_EQ(true, line.provides_output());
  ASSERT_EQ(false, line.provides_interrupt());

  TestInput(line);
  TestOutputReadback(line);
}

TEST(DigitalIo, InOutInterrupt) {
  TestDigitalInOutInterrupt line;

  ASSERT_EQ(true, line.provides_input());
  ASSERT_EQ(true, line.provides_output());
  ASSERT_EQ(true, line.provides_interrupt());

  TestInput(line);
  TestOutputReadback(line);
  TestInterrupt(line);
}

}  // namespace
}  // namespace pw::gpio
