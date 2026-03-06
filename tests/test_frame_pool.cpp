//
// Created by 31305 on 2026/3/5.
//
#include <catch2/catch_test_macros.hpp>
#include <frame/frame_pool.hpp>

using sc::core::Frame;
using sc::core::FramePool;

TEST_CASE("FramePool initialization", "[frame_pool]") {

  FramePool pool;

  const size_t frame_count = 8;
  const size_t buffer_size = 1024;

  REQUIRE(pool.initPool(frame_count, buffer_size) == true);

  size_t acquired = 0;

  for (size_t i = 0; i < frame_count; i++) {
    Frame* f = pool.acquire();
    REQUIRE(f != nullptr);
    REQUIRE(f->capacity == buffer_size);
    acquired++;
  }

  REQUIRE(acquired == frame_count);

  Frame* f = pool.acquire();
  REQUIRE(f == nullptr);
}

TEST_CASE("FramePool acquire and release", "[frame_pool]") {

  FramePool pool;

  const size_t frame_count = 4;
  const size_t buffer_size = 512;

  pool.initPool(frame_count, buffer_size);

  Frame* frames[4];

  for (auto & frame : frames) {
    frame = pool.acquire();
    REQUIRE(frame != nullptr);
  }

  REQUIRE(pool.acquire() == nullptr);

  pool.release(frames[0]);

  Frame* f = pool.acquire();
  REQUIRE(f != nullptr);

  REQUIRE(f == frames[0]);
}

TEST_CASE("FramePool reuse frames", "[frame_pool]") {

  FramePool pool;

  const size_t frame_count = 2;
  const size_t buffer_size = 256;

  pool.initPool(frame_count, buffer_size);

  Frame* f1 = pool.acquire();
  Frame* f2 = pool.acquire();

  REQUIRE(f1 != nullptr);
  REQUIRE(f2 != nullptr);

  REQUIRE(pool.acquire() == nullptr);

  pool.release(f1);
  pool.release(f2);

  Frame* a = pool.acquire();
  Frame* b = pool.acquire();

  REQUIRE(a != nullptr);
  REQUIRE(b != nullptr);
}