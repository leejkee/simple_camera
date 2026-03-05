#pragma once
#include <atomic>
#include <memory>
#include <vector>

namespace sc {
namespace core {
struct Frame;

class FrameLockFreeQueue {
public:
  explicit FrameLockFreeQueue(size_t capacity);

  bool push(Frame *frame);

  Frame *pop();

  size_t size() const;

private:
  std::vector<Frame *> m_buffer;
  const size_t m_capacity;
  std::atomic<size_t> m_head;
  std::atomic<size_t> m_tail;
};

} // namespace core
} // namespace sc