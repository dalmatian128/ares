#pragma once

#include <nall/hash/hash.hpp>

namespace nall::Hash {

struct CRC64 : Hash {
  using Hash::input;

  CRC64(array_view<u8> buffer = {}) {
    reset();
    input(buffer);
  }

  auto reset() -> void override {
    checksum = ~0;
  }

  auto input(u8 value) -> void override {
    checksum = (checksum >> 8) ^ table(checksum ^ value);
  }

  auto output() const -> vector<u8> override {
    vector<u8> result;
    for(auto n : reverse(range(8))) result.append(~checksum >> n * 8);
    return result;
  }

  auto value() const -> u64 {
    return ~checksum;
  }

private:
  static auto table(u8 index) -> u64 {
    static u64 table[256] = {};
    static bool initialized = false;

    if(!initialized) {
      initialized = true;
      for(auto index : range(256)) {
        u64 crc = index;
        for(auto bit : range(8)) {
          crc = (crc >> 1) ^ (crc & 1 ? 0xc96c'5795'd787'0f42 : 0);
        }
        table[index] = crc;
      }
    }

    return table[index];
  }

  u64 checksum = 0;
};

}
