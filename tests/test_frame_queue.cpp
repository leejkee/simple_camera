//
// Created by 31305 on 2026/3/5.
//
#include <frame/frame_pool.hpp>
#include <catch2/catch_test_macros.hpp>
#include <frame/frame_lock_free_queue.hpp>

using sc::core::Frame;
using sc::core::FramePool;
using sc::core::FrameLockFreeQueue;

TEST_CASE("FrameQueue initialization", "[frame_lock_free_queue]") {
  const FrameLockFreeQueue queue(4);
  REQUIRE(queue.size() == 0);
}

TEST_CASE("FrameQueue push and pop", "[frame_lock_free_queue]") {
  FramePool pool;
  const size_t frame_count = 8;
  const size_t buffer_size = 1024;
  pool.initPool(frame_count, buffer_size);
  FrameLockFreeQueue queue(4);
  REQUIRE(queue.size() == 0);

  Frame* before_frame = pool.acquire();
  before_frame->height = 100;
  before_frame->width = 100;
  before_frame->format = sc::core::PixelFormat::RGB24;

  REQUIRE(before_frame != nullptr);
  queue.push(before_frame);
  REQUIRE(queue.size() == 1);
  Frame* after_frame = queue.pop();
  REQUIRE(after_frame != nullptr);
  REQUIRE(queue.size() == 0);
  REQUIRE(after_frame->format == sc::core::PixelFormat::RGB24);
  REQUIRE(after_frame->height == 100);
  REQUIRE(after_frame->width == 100);
}
TEST_CASE("FrameQueue capacity trap and wrap-around", "[frame_lock_free_queue]") {
  // 声明容量为 4，实际上只能存 3 个元素
  FrameLockFreeQueue queue(4);
  Frame f1, f2, f3, f4;

  REQUIRE(queue.push(&f1) == true);
  REQUIRE(queue.push(&f2) == true);
  REQUIRE(queue.push(&f3) == true);

  // 刁钻点 1：第 4 个 push 必须失败，且 size 必须为 3
  REQUIRE(queue.push(&f4) == false);
  REQUIRE(queue.size() == 3);

  // 刁钻点 2：触发游标绕回（Wrap-around）
  REQUIRE(queue.pop() == &f1); // pop 一个，head 前进
  REQUIRE(queue.push(&f4) == true); // 再 push 一个，tail 绕回到数组开头
  REQUIRE(queue.size() == 3);

  // 验证绕回后 pop 的顺序和正确性
  REQUIRE(queue.pop() == &f2);
  REQUIRE(queue.pop() == &f3);
  REQUIRE(queue.pop() == &f4);
  REQUIRE(queue.size() == 0);

  // 刁钻点 3：空队列继续 pop 不应崩溃，应返回 nullptr
  REQUIRE(queue.pop() == nullptr);
}

TEST_CASE("FrameQueue extreme capacity", "[frame_lock_free_queue]") {
  // 刁钻点 4：当容量声明为 1 时，(tail+1)%1 永远等于 head(0)
  // 这个队列将变成一个“永远满”且“永远空”的废队列，不应崩溃但应始终拒绝 push
  FrameLockFreeQueue queue(1);
  Frame f;
  REQUIRE(queue.push(&f) == false);
  REQUIRE(queue.size() == 0);
}

#include <thread>
#include <atomic>

TEST_CASE("FrameQueue SPSC concurrency stress test", "[frame_lock_free_queue]") {
  const size_t queue_capacity = 1024;
  FrameLockFreeQueue queue(queue_capacity);
  const int total_frames = 1000000; // 百万次级别的高频并发
  std::atomic<int> consumed_count{0};

  // 生产者线程
  std::thread producer([&]() {
    for (int i = 0; i < total_frames; ++i) {
      // 刁钻技巧：直接用整数强转为指针，省去分配上百万个 Frame 的内存开销
      // 因为无锁队列内部不解引用指针，只做搬运
      Frame* dummy_frame = reinterpret_cast<Frame*>(static_cast<uintptr_t>(i + 1));
      while (!queue.push(dummy_frame)) {
        std::this_thread::yield(); // 队列满了就让出 CPU
      }
    }
  });

  // 消费者线程
  std::thread consumer([&]() {
    for (int i = 0; i < total_frames; ++i) {
      Frame* f = nullptr;
      while ((f = queue.pop()) == nullptr) {
        std::this_thread::yield(); // 队列空了就让出 CPU
      }
      // 验证无锁情况下，数据是否发生了乱序或丢失
      REQUIRE(reinterpret_cast<uintptr_t>(f) == (i + 1));
      consumed_count++;
    }
  });

  producer.join();
  consumer.join();

  REQUIRE(consumed_count == total_frames);
  REQUIRE(queue.size() == 0);
}

TEST_CASE("FramePool exhaustion and malicious release", "[frame_pool]") {
  FramePool pool;
  pool.initPool(2, 1024); // 仅分配 2 个 Frame

  Frame* f1 = pool.acquire();
  Frame* f2 = pool.acquire();
  REQUIRE(f1 != nullptr);
  REQUIRE(f2 != nullptr);

  // 刁钻点 1：榨干对象池
  Frame* f3 = pool.acquire();
  REQUIRE(f3 == nullptr); // 应该优雅返回 nullptr 而不是一直阻塞或崩溃

  // 刁钻点 2：恶意释放 nullptr (源码中 release 并未做判空检查)
  // 此处验证目前的实现特性：如果放进了 nullptr，下次就会取回 nullptr
  pool.release(nullptr);
  Frame* malicious_frame = pool.acquire();
  REQUIRE(malicious_frame == nullptr);

  // 正常回收再复用
  pool.release(f1);
  Frame* recycled_frame = pool.acquire();
  REQUIRE(recycled_frame == f1); // 必须是同一个内存地址
}

#include <vector>

TEST_CASE("FramePool MPMC thread safety", "[frame_pool]") {
  FramePool pool;
  const size_t pool_size = 5;
  pool.initPool(pool_size, 1024);

  auto worker_func = [&pool]() {
    for (int i = 0; i < 500; ++i) {
      Frame* f = pool.acquire();
      if (f) {
        // 模拟拿到帧之后的业务处理时间
        std::this_thread::yield();
        pool.release(f);
      }
    }
  };

  std::vector<std::thread> threads;
  // 启动 10 个线程去抢 5 个 Frame
  threads.reserve(10);
for (int i = 0; i < 10; ++i) {
    threads.emplace_back(worker_func);
  }

  for (auto& t : threads) {
    t.join();
  }

  // 刁钻点：无论中间怎么抢夺，结束后池子里必须完好无损地剩下最初的 5 个 Frame
  int count = 0;
  while (pool.acquire() != nullptr) {
    count++;
  }
  REQUIRE(count == pool_size);
}