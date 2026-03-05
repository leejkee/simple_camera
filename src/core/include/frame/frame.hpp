#pragma once

#include <cstdint>

namespace sc {

namespace core {
enum class PixelFormat { UNKNOWN = 0, RGB24, BGR24, GRAY8, YUYV, NV12 };

struct Frame {
  uint8_t *data = nullptr;

  int width = 0;
  int height = 0;
  int stride = 0; // the bytes of every line

  size_t buffer_size = 0;

  PixelFormat format = PixelFormat::UNKNOWN;

  uint64_t timestamp = 0;

  ~Frame() {
    delete[] data;
  }
};
} // namespace core
} // namespace sc
