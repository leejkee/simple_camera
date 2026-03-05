//
// Created by 31305 on 2026/3/5.
//

#include <frame/frame_queue.hpp>
#include <frame/frame_pool.hpp>
#include <hikvision/hikvision_camera.hpp>
#include <memory>
#include <thread>
#include <chrono>

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

  std::atomic<bool> is_running = true;

  std::thread thread_producer([&]() {
    while (is_running.load()) {
      sc::core::Frame* frame = frame_pool.acquire();
      if (!frame) {
        continue;
      }

      frame->width = 1920;
      frame->height = 1080;
      frame->stride = 1920*1080*3;
      while (!queue.push(frame)) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
      }
    }
  });


  std::thread thread_consumer([&]() {
    int processed = 0;
    while (is_running.load()) {
      sc::core::Frame* frame = queue.pop();
      if (!frame) {
        std::this_thread::sleep_for(std::chrono::microseconds(1));
        continue;
      }

      camera->grabFrame(*frame);
      std::this_thread::sleep_for(std::chrono::microseconds(50));
      processed++;
      frame_pool.release(frame);
      if (processed >= 20) {
        is_running.store(false);
      }
    }
  });

  thread_producer.join();
  thread_consumer.join();
  return 0;
}