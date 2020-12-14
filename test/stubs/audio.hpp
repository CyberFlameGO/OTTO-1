#pragma once

#include <thread>

#include "app/drivers/audio_driver.hpp"
#include "app/services/runtime.hpp"
#include "lib/chrono.hpp"
#include "lib/core/service.hpp"

namespace otto::stubs {
  struct NoProcessAudioDriver : drivers::IAudioDriver {
    void set_callback(Callback&& cb) override
    {
      callback = std::move(cb);
    }
    void start() override {}
    [[nodiscard]] std::size_t buffer_size() const override
    {
      return 64;
    }
    [[nodiscard]] std::size_t sample_rate() const override
    {
      return 44100;
    }

    Callback callback;
  };

  struct DummyAudioDriver final : drivers::IAudioDriver {
    DummyAudioDriver()
    {
      buffers_.resize(buffer_size() * 4);
    }
    void set_callback(Callback&& cb) override
    {
      callback_ = std::move(cb);
    }
    void start() override
    {
      thread_ = std::jthread([this] {
        auto input_buf = util::stereo_audio_buffer(
          util::audio_buffer(std::span(buffers_.data() + 0 * buffer_size(), buffer_size()), nullptr),
          util::audio_buffer(std::span(buffers_.data() + 1 * buffer_size(), buffer_size()), nullptr));
        auto output_buf = util::stereo_audio_buffer(
          util::audio_buffer(std::span(buffers_.data() + 2 * buffer_size(), buffer_size()), nullptr),
          util::audio_buffer(std::span(buffers_.data() + 3 * buffer_size(), buffer_size()), nullptr));
        CallbackData cbd = {
          .input = input_buf,
          .output = output_buf,
        };
        while (runtime->should_run()) {
          callback_(cbd);
          std::this_thread::sleep_for(chrono::nanoseconds((1ns / 1s) * buffer_size() / sample_rate()));
        }
      });
    }
    [[nodiscard]] std::size_t buffer_size() const override
    {
      return 64;
    }
    [[nodiscard]] std::size_t sample_rate() const override
    {
      return 44100;
    }

  private:
    std::vector<float> buffers_;
    [[no_unique_address]] core::ServiceAccessor<services::Runtime> runtime;
    Callback callback_;
    std::jthread thread_;
  };

} // namespace otto::stubs