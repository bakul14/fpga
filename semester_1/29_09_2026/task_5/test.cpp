#include <gtest/gtest.h>

#include "gtest_verilator_wrapper.hpp"

#include "Vasync_fifo.h"

#include <algorithm>
#include <cstdint>
#include <random>
#include <vector>

namespace
{

constexpr size_t k_depth = 16U;

constexpr size_t k_stream_size = 2000U;

constexpr uint32_t k_random_seed = 42U;

constexpr size_t k_max_edges_per_item = 64U;

constexpr size_t k_sync_latency_cycles = 4U;

constexpr bool k_rising  = true;
constexpr bool k_falling = false;

struct Clocks {
  uint64_t wr_half_period;
  uint64_t rd_half_period;
  uint64_t rd_phase_offset = 0U;
};

std::vector<uint8_t> random_stream(const size_t size)
{
  std::mt19937 generator(k_random_seed);
  std::uniform_int_distribution<int> distribution(0, UINT8_MAX);

  std::vector<uint8_t> inputs(size);
  for (uint8_t &input : inputs) { input = static_cast<uint8_t>(distribution(generator)); }

  return inputs;
}

}  // namespace

class AsyncFifoTest : public VerilatorWrapperTestBase<Vasync_fifo>
{
protected:
  void reset_() final
  {
    dut_->wr_clk  = 0;
    dut_->rd_clk  = 0;
    dut_->wr_en   = 0;
    dut_->wr_data = 0;
    dut_->rd_en   = 0;
    dut_->eval();
    dump_();
  }

  void set_clocks_(const Clocks &clocks)
  {
    clocks_       = clocks;
    next_wr_edge_ = clocks.wr_half_period;
    next_rd_edge_ = clocks.rd_half_period + clocks.rd_phase_offset;
  }

  void tick_(const size_t edges = 1) final
  {
    for (size_t i = 0; i < edges; ++i) {
      const uint64_t next_time = std::min(next_wr_edge_, next_rd_edge_);

      wr_toggled_ = next_wr_edge_ == next_time;
      rd_toggled_ = next_rd_edge_ == next_time;

      if (wr_toggled_) {
        dut_->wr_clk ^= 1;
        next_wr_edge_ += clocks_.wr_half_period;
      }
      if (rd_toggled_) {
        dut_->rd_clk ^= 1;
        next_rd_edge_ += clocks_.rd_half_period;
      }

      context_->time(next_time);
      dut_->eval();
      dump_();
      settle_();
    }
  }

  bool wr_edge_(const bool rising) const { return wr_toggled_ && dut_->wr_clk == rising; }
  bool rd_edge_(const bool rising) const { return rd_toggled_ && dut_->rd_clk == rising; }

  void wait_wr_(const bool rising)
  {
    do {
      tick_();
    } while (!wr_edge_(rising));
  }

  void wait_rd_(const bool rising)
  {
    do {
      tick_();
    } while (!rd_edge_(rising));
  }

  void check_stream_(const Clocks &clocks)
  {
    set_clocks_(clocks);

    const std::vector<uint8_t> inputs = random_stream(k_stream_size);
    size_t written                    = 0U;
    size_t read                       = 0U;

    const size_t max_edges = k_max_edges_per_item * (inputs.size() + k_depth);

    for (size_t edge = 0U; read < inputs.size(); ++edge) {
      ASSERT_LT(edge, max_edges) << "записано " << written << ", прочитано " << read;

      const bool wr_accepted = dut_->wr_en && !dut_->full;
      const bool rd_accepted = dut_->rd_en && !dut_->empty;
      const uint8_t head     = dut_->rd_data;

      tick_();

      if (wr_edge_(k_rising) && wr_accepted) { ++written; }
      if (rd_edge_(k_rising) && rd_accepted) {
        ASSERT_EQ(head, inputs[read]) << "элемент " << read;
        ++read;
      }

      if (wr_edge_(k_falling)) {
        dut_->wr_en   = written < inputs.size();
        dut_->wr_data = inputs[std::min(written, inputs.size() - 1U)];
      }
      if (rd_edge_(k_falling)) { dut_->rd_en = 1; }
    }
  }

protected:
  Clocks clocks_{};
  uint64_t next_wr_edge_ = 0U;
  uint64_t next_rd_edge_ = 0U;
  bool wr_toggled_       = false;
  bool rd_toggled_       = false;
};

TEST_F(AsyncFifoTest, initially_empty_and_not_full)
{
  set_clocks_({.wr_half_period = 5U, .rd_half_period = 7U});
  tick_(40U);

  EXPECT_EQ(dut_->empty, 1);
  EXPECT_EQ(dut_->full, 0);
}

TEST_F(AsyncFifoTest, fill_then_drain)
{
  set_clocks_({.wr_half_period = 5U, .rd_half_period = 7U});

  for (size_t index = 0U; index < k_depth; ++index) {
    wait_wr_(k_falling);
    EXPECT_EQ(dut_->full, 0) << "запись " << index;
    dut_->wr_en   = 1;
    dut_->wr_data = static_cast<uint8_t>(index);
    wait_wr_(k_rising);
  }

  wait_wr_(k_falling);
  EXPECT_EQ(dut_->full, 1);
  dut_->wr_data = 0xFF;
  wait_wr_(k_rising);
  EXPECT_EQ(dut_->full, 1);
  wait_wr_(k_falling);
  dut_->wr_en = 0;

  for (size_t cycle = 0U; cycle < k_sync_latency_cycles; ++cycle) { wait_rd_(k_rising); }

  for (size_t index = 0U; index < k_depth; ++index) {
    wait_rd_(k_falling);
    ASSERT_EQ(dut_->empty, 0) << "чтение " << index;
    EXPECT_EQ(dut_->rd_data, index);
    dut_->rd_en = 1;
    wait_rd_(k_rising);
  }

  wait_rd_(k_falling);
  EXPECT_EQ(dut_->empty, 1);
  wait_rd_(k_rising);
  EXPECT_EQ(dut_->empty, 1);
  wait_rd_(k_falling);
  dut_->rd_en = 0;

  for (size_t cycle = 0U; cycle < k_sync_latency_cycles; ++cycle) { wait_wr_(k_rising); }
  EXPECT_EQ(dut_->full, 0);
}

TEST_F(AsyncFifoTest, writer_faster_than_reader)
{ check_stream_({.wr_half_period = 3U, .rd_half_period = 11U}); }

TEST_F(AsyncFifoTest, reader_faster_than_writer)
{ check_stream_({.wr_half_period = 11U, .rd_half_period = 3U}); }

TEST_F(AsyncFifoTest, same_frequency_phase_shifted)
{ check_stream_({.wr_half_period = 5U, .rd_half_period = 5U, .rd_phase_offset = 2U}); }

TEST_F(AsyncFifoTest, odd_frequency_ratio)
{ check_stream_({.wr_half_period = 7U, .rd_half_period = 5U, .rd_phase_offset = 3U}); }
