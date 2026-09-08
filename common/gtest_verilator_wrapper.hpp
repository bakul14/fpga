#pragma once

#include <gtest/gtest.h>

#include <cstdint>
#include <memory>
#include <string>

#include "verilated.h"
#include "verilated_vcd_c.h"

constexpr int k_trace_levels = 1;

constexpr size_t k_max_settle_iterations = 10'000U;

template <typename Model>
class VerilatorWrapperTestBase : public ::testing::Test
{
protected:
  void SetUp() final
  {
    const std::string test_name = ::testing::UnitTest::GetInstance()->current_test_info()->name();

    context_->traceEverOn(true);
    dut_->trace(trace_.get(), k_trace_levels);
    trace_->open(("wave_" + test_name + ".vcd").c_str());
    reset_();
  }

  void TearDown() final
  {
    dut_->final();
    trace_->close();
  }

  void dump_() { trace_->dump(context_->time()); }

  void step_()
  {
    context_->timeInc(1);
    dut_->eval();
    dump_();
  }

  void settle_()
  {
    for (size_t i = 0U; !context_->gotFinish() && dut_->eventsPending(); ++i) {
      if (i >= k_max_settle_iterations) {
        ADD_FAILURE() << "settle_(): очередь событий не опустела за " << k_max_settle_iterations
                      << " итераций";
        break;
      }

      const uint64_t next_precision_slot = dut_->nextTimeSlot();
      if (next_precision_slot > context_->time()) { context_->time(next_precision_slot); }

      dut_->eval();
      dump_();
    }
  }

  virtual void reset_() {}

  virtual void tick_(const size_t cycles = 1) = 0;

protected:
  const std::unique_ptr<VerilatedContext> context_ = std::make_unique<VerilatedContext>();
  const std::unique_ptr<Model> dut_                = std::make_unique<Model>(context_.get());
  const std::unique_ptr<VerilatedVcdC> trace_      = std::make_unique<VerilatedVcdC>();
};

template <typename Model>
class SyncVerilatorWrapperTest : public VerilatorWrapperTestBase<Model>
{
protected:
  void tick_(const size_t cycles = 1) final
  {
    for (size_t i = 0; i < cycles; ++i) {
      this->dut_->clk = 1;
      this->step_();
      this->settle_();

      this->dut_->clk = 0;
      this->step_();
      this->settle_();
    }
  }
};

template <typename Model>
class AsyncVerilatorWrapperTest : public VerilatorWrapperTestBase<Model>
{
protected:
  void tick_(const size_t cycles = 1) final
  {
    (void)cycles;
    this->step_();
    this->settle_();
  }
};
