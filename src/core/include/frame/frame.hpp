#pragma once

#include <cstdint>

namespace sc {

namespace core {
enum class PixelFormat { UNKNOWN = 0, RGB24, BGR24, GRAY8, YUYV, NV12 };

struct Frame {
  uint8_t *data = nullptr;

  uint32_t width = 0;
  uint32_t height = 0;
  uint32_t stride = 0; // the bytes of every line

  size_t capacity = 0;

  PixelFormat format = PixelFormat::UNKNOWN;

  uint64_t frame_id = 0;
  uint64_t timestamp = 0;

  Frame() = default;

  explicit Frame(const size_t buffer_size) { allocate(buffer_size); }
  ~Frame() { delete[] data; }

  void allocate(const size_t buffer_size) {
    delete[] data;
    data = new uint8_t[buffer_size];
    capacity = buffer_size;
  }
};
} // namespace core
} // namespace sc
