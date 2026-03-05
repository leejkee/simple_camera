#pragma once
#include "frame.hpp"
#include <mutex>
#include <queue>
#include <vector>
#include <memory>

namespace sc {
namespace core {

class FramePool {
  public:
  explicit FramePool() = default;
  ~FramePool();

  bool initPool(size_t frame_count, size_t buffer_size);

  Frame* acquire();

  void release(Frame* frame);

private:
  std::vector<std::unique_ptr<Frame>> m_frames;
  std::queue<Frame*> m_frame_queue;
  std::mutex m_mutex;
};
}
}