#include <gtest/gtest.h>

#include "gtest_verilator_wrapper.hpp"

#include "Viir_filter.h"

#include <cstdint>
#include <random>
#include <vector>

namespace
{

constexpr uint64_t k_y_coeff = 3U;

constexpr uint64_t k_x_coeff = 2U;

constexpr size_t k_latency = 4U;

constexpr size_t k_stream_size = 1000U;

constexpr uint32_t k_random_seed = 42U;

std::vector<int64_t> axioms(const std::vector<int32_t> &inputs)
{
  std::vector<int64_t> axioms;
  axioms.reserve(inputs.size());

  uint64_t output = 0U;

  for (const int32_t input : inputs) {
    const uint64_t extended_input = static_cast<uint64_t>(static_cast<int64_t>(input));

    output = (k_y_coeff * output) + (k_x_coeff * extended_input);
    axioms.push_back(static_cast<int64_t>(output));
  }

  return axioms;
}

std::vector<int32_t> random_stream(const size_t size)
{
  std::mt19937 generator(k_random_seed);
  std::uniform_int_distribution<int32_t> distribution(INT32_MIN, INT32_MAX);

  std::vector<int32_t> inputs(size);
  for (int32_t &input : inputs) { input = distribution(generator); }

  return inputs;
}

}  // namespace

class IirFilterTest : public SyncVerilatorWrapperTest<Viir_filter>
{
protected:
  void reset_() final
  {
    dut_->clk     = 0;
    dut_->data_in = 0;
    dut_->eval();
  }

  std::vector<int64_t> run_(const std::vector<int32_t> &inputs)
  {
    std::vector<int64_t> outputs;
    outputs.reserve(inputs.size() + 1U);

    const size_t cycles_count = inputs.size() + k_latency;

    for (size_t cycle = 0U; cycle < cycles_count; ++cycle) {
      dut_->data_in = (cycle < inputs.size()) ? inputs[cycle] : 0;
      tick_();

      const size_t elapsed_cycles = cycle + 1U;

      if (elapsed_cycles < k_latency) {
        EXPECT_EQ(dut_->valid_out, 0) << "такт " << cycle;
      } else {
        EXPECT_EQ(dut_->valid_out, 1) << "такт " << cycle;
        outputs.push_back(static_cast<int64_t>(dut_->data_out));
      }
    }

    outputs.resize(inputs.size());
    return outputs;
  }

  void check_(const std::vector<int32_t> &inputs)
  {
    const std::vector<int64_t> expected_outputs = axioms(inputs);
    const std::vector<int64_t> actual_outputs   = run_(inputs);

    ASSERT_EQ(actual_outputs.size(), expected_outputs.size());
    for (size_t index = 0U; index < expected_outputs.size(); ++index) {
      EXPECT_EQ(actual_outputs[index], expected_outputs[index]);
    }
  }
};

TEST_F(IirFilterTest, impulse)
{ check_({1, 0, 0, 0, 0, 0, 0, 0}); }

TEST_F(IirFilterTest, step)
{ check_({1, 1, 1, 1, 1, 1, 1, 1}); }

TEST_F(IirFilterTest, negative)
{ check_({-1, 2, -3, 4, -5, 6, -7, 8}); }

TEST_F(IirFilterTest, limits)
{ check_({INT32_MAX, INT32_MIN, INT32_MIN, INT32_MAX, -1, 1}); }

TEST_F(IirFilterTest, random_stream)
{ check_(random_stream(k_stream_size)); }
