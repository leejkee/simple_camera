#pragma once
#include <frame/frame_lock_free_queue.hpp>
#include <frame/frame_pool.hpp>
#include <interface_camera/interface_camera.hpp>
#include <functional>
#include <memory>

namespace sc {
namespace ui {

class OpenCvRenderer {
public:
  OpenCvRenderer(core::FrameLockFreeQueue* in_queue, core::FramePool* pool)
      : m_in_queue(in_queue), m_pool(pool) {}

  void run_event_loop(const std::function<bool()>& check_upstream_alive);

  void set_camera_source(std::shared_ptr<devices::InterfaceCamera> camera);

private:
  core::FrameLockFreeQueue* m_in_queue;
  core::FramePool* m_pool;
  std::shared_ptr<devices::InterfaceCamera> m_camera;
};

} // namespace ui
} // namespace sc