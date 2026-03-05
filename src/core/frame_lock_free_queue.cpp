//
// Created by 31305 on 2026/3/5.
//
#include <frame/frame.hpp>
#include <frame/frame_lock_free_queue.hpp>

namespace sc {
namespace core {
FrameLockFreeQueue::FrameLockFreeQueue(const size_t capacity) : m_capacity(capacity), m_head(0), m_tail(0) {
  m_buffer.resize(m_capacity);
}

bool FrameLockFreeQueue::push(Frame *frame) {
  const size_t tail = m_tail.load(std::memory_order_relaxed);
  const size_t next_tail = (tail + 1) % m_capacity;
  if (next_tail == m_head.load(std::memory_order_acquire)) {
    return false;
  }
  m_buffer[tail] = frame;
  m_tail.store(next_tail, std::memory_order_release);
  return true;
}

Frame *FrameLockFreeQueue::pop() {
  const size_t head = m_head.load(std::memory_order_relaxed);
  if (head == m_tail.load(std::memory_order_acquire)) {
    return nullptr;
  }
  Frame *frame = m_buffer[head];
  const size_t next_head = (head + 1) % m_capacity;
  m_head.store(next_head, std::memory_order_release);
  return frame;
}

size_t FrameLockFreeQueue::size() const {
  const size_t head = m_head.load(std::memory_order_acquire);
  const size_t tail = m_tail.load(std::memory_order_acquire);
  return (tail + m_capacity - head) % m_capacity;
}

} // namespace core
}