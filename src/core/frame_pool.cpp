//
// Created by 31305 on 2026/3/5.
//
#include <frame/frame_pool.hpp>

namespace sc {
namespace core {

FramePool::~FramePool() = default;
bool FramePool::initPool(const size_t frame_count, const size_t buffer_size) {
  std::lock_guard<std::mutex> lock(m_mutex);
  m_frames.reserve(frame_count);
  for (size_t i = 0; i < frame_count; i++) {
    auto f = std::make_unique<Frame>();
    f->data = new uint8_t[buffer_size];
    f->buffer_size = buffer_size;
    m_frame_queue.push(f.get());
    m_frames.push_back(std::move(f));
  }
  return true;
}

Frame* FramePool::acquire() {
  std::lock_guard<std::mutex> lock(m_mutex);
  if (m_frame_queue.empty()) {
    return nullptr;
  }
  Frame* frame = m_frame_queue.front();
  m_frame_queue.pop();
  return frame;
}

void FramePool::release(Frame *frame) {
  std::lock_guard<std::mutex> lock(m_mutex);
  m_frame_queue.push(frame);
}

}

}