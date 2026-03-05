//
// Created by 31305 on 2026/3/5.
//

#include <frame/frame_lock_free_queue.hpp>
#include <frame/frame_pool.hpp>
#include <hikvision/hikvision_camera.hpp>
#include <memory>
#include <thread>

int main() {
  const std::unique_ptr<sc::devices::InterfaceCamera> camera =
      std::make_unique<sc::devices::HikVisionCamera>();
  if (!camera->open()) {
    return -1;
  }
  if (!camera->start()) {
    return -1;
  }
  sc::core::FramePool frame_pool;
  frame_pool.initPool(8, 1920*1080*3);

  sc::core::FrameLockFreeQueue queue(4);

  bool is_running = true;

  std::thread thread_producer([&]() {
    while (is_running) {
      sc::core::Frame* frame = frame_pool.acquire();
      if (!frame) {
        continue;
      }

      if (!camera->grabFrame(*frame)) {
        frame_pool.release(frame);
        continue;
      }
      while (!queue.push(frame)) {
        std::this_thread::yield();
      }
    }
  });


  std::thread thread_consumer([&]() {
    int processed = 0;
    while (is_running) {
      sc::core::Frame* frame = queue.pop();
      if (!frame) {
        std::this_thread::yield();
        continue;
      }
      std::cout << "Frame "
                << frame->frame_id
                << " "
                << frame->width
                << "x"
                << frame->height
                << std::endl;
      processed++;
      frame_pool.release(frame);
      if (processed >= 20) {
        is_running = false;
      }
    }
  });

  thread_producer.join();
  thread_consumer.join();
  return 0;
}