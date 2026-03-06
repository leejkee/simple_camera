#pragma once
#include <functional>
#include <memory>

namespace sc {
namespace core {
class FrameLockFreeQueue;
class FramePool;
} // namespace core
namespace devices {
class InterfaceCamera;
}
namespace ui {

class OpenCvRenderer {
public:
  OpenCvRenderer(core::FrameLockFreeQueue *in_queue, core::FramePool *pool)
      : m_in_queue(in_queue), m_pool(pool) {}

  ~OpenCvRenderer();

  void run_event_loop(const std::function<bool()> &check_upstream_alive);

  void set_camera_source(std::shared_ptr<devices::InterfaceCamera> camera);

private:
  core::FrameLockFreeQueue *m_in_queue;
  core::FramePool *m_pool;
  std::shared_ptr<devices::InterfaceCamera> m_camera;
};

} // namespace ui
} // namespace sc