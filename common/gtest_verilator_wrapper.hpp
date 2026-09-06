#pragma once

#include <gtest/gtest.h>

#include <cstdint>
#include <memory>
#include <string>

#include "verilated.h"
#include "verilated_vcd_c.h"

constexpr int k_trace_levels = 1;

template <typename Model>
class VerilatorWrapperTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    const std::string test_name = ::testing::UnitTest::GetInstance()->current_test_info()->name();

    context_->traceEverOn(true);
    dut_->trace(trace_.get(), k_trace_levels);
    trace_->open(("wave_" + test_name + ".vcd").c_str());
    reset_();
  }

  void TearDown() override { trace_->close(); }

  void tick_(const size_t cycles = 1)
  {
    for (size_t i = 0; i < cycles; ++i) {
      dut_->clk = 0;
      dut_->eval();
      trace_->dump(sim_time_++);

      dut_->clk = 1;
      dut_->eval();
      trace_->dump(sim_time_++);
    }
  }

  virtual void reset_() {}

  const std::unique_ptr<VerilatedContext> context_ = std::make_unique<VerilatedContext>();
  const std::unique_ptr<Model> dut_                = std::make_unique<Model>(context_.get());
  const std::unique_ptr<VerilatedVcdC> trace_      = std::make_unique<VerilatedVcdC>();
  uint64_t sim_time_                               = 0U;
};
