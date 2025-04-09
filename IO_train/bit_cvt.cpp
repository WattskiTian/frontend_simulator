#include <boost/pfr.hpp>
#include <cstdint>
#include <iostream>
#include <vector>

// assign_bits 函数同上，略

// 通用赋值函数
template <typename Struct>
void assign_from_bits(Struct &s, const std::vector<bool> &bits) {
  size_t index = 0;
  boost::pfr::for_each_field(
      s, [&bits, &index](auto &member) { assign_bits(member, bits, index); });
}

// 测试用 struct
struct X {
  bool a;
  uint8_t b;
  bool c;
};

int main() {
  std::vector<bool> bits = {true,  false, false, false, false,
                            false, false, false, true};
  X x;
  assign_from_bits(x, bits);
  std::cout << "a = " << x.a << "\n";      // 1
  std::cout << "b = " << (int)x.b << "\n"; // 0
  std::cout << "c = " << x.c << "\n";      // 1
  return 0;
}