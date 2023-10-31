#pragma once

#include <cassert>
#include <cmath>
#include <cstdint>
#include <functional>
#include <iosfwd>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

struct big_integer {
  big_integer();
  big_integer(const big_integer& other);
  big_integer(int a);
  big_integer(unsigned int a);
  big_integer(unsigned long long a);
  big_integer(long a);
  big_integer(unsigned long a);
  big_integer(long long a);
  big_integer(short a);
  big_integer(unsigned short a);
  explicit big_integer(const std::string& str);
  ~big_integer();

  big_integer& operator=(const big_integer& other);

  big_integer& operator+=(const big_integer& rhs);
  big_integer& operator-=(const big_integer& rhs);
  big_integer& operator*=(const big_integer& rhs);
  big_integer& operator/=(const big_integer& rhs);
  big_integer& operator%=(const big_integer& rhs);

  big_integer& operator&=(const big_integer& rhs);
  big_integer& operator|=(const big_integer& rhs);
  big_integer& operator^=(const big_integer& rhs);

  big_integer& operator<<=(int rhs);
  big_integer& operator>>=(int rhs);

  big_integer operator+() const;
  big_integer operator-() const;
  big_integer operator~() const;

  big_integer& operator++();
  big_integer operator++(int);

  big_integer& operator--();
  big_integer operator--(int);

  friend bool operator==(const big_integer& a, const big_integer& b);
  friend bool operator!=(const big_integer& a, const big_integer& b);
  friend bool operator<(const big_integer& a, const big_integer& b);
  friend bool operator>(const big_integer& a, const big_integer& b);
  friend bool operator<=(const big_integer& a, const big_integer& b);
  friend bool operator>=(const big_integer& a, const big_integer& b);

  friend std::string to_string(const big_integer& a);

private:
  static std::string getReversedString(uint32_t x);
  static big_integer abs(const big_integer& other);
  big_integer& binaryOperation(const big_integer& rhs, const std::function<int(int, int)>& op);
  static std::pair<big_integer, big_integer> long_divide(big_integer& x, const big_integer& y);
  static void difference(big_integer& r, const big_integer& dq, int k, int m);
  static bool smaller(big_integer& r, const big_integer& dq, int k, int m);
  static uint32_t trial(big_integer& r, const big_integer& d, int k, int m);
  static int64_t quotient(big_integer& x, uint32_t k);
  static uint32_t getInt(const std::string& str);
  void clear_zeros();

  big_integer(bool sgn, std::vector<uint32_t> dgs) : sign(sgn), digits(std::move(dgs)) {}

  void swap(big_integer& other);
  static const unsigned short intSize = 32;
  const uint64_t systemBase = (1ULL << intSize);
  bool sign = false;
  std::vector<uint32_t> digits;
};

big_integer operator+(big_integer a, const big_integer& b);
big_integer operator-(big_integer a, const big_integer& b);
big_integer operator*(big_integer a, const big_integer& b);
big_integer operator/(big_integer a, const big_integer& b);
big_integer operator%(big_integer a, const big_integer& b);

big_integer operator&(big_integer a, const big_integer& b);
big_integer operator|(big_integer a, const big_integer& b);
big_integer operator^(big_integer a, const big_integer& b);

big_integer operator<<(big_integer a, int b);
big_integer operator>>(big_integer a, int b);

bool operator==(const big_integer& a, const big_integer& b);
bool operator!=(const big_integer& a, const big_integer& b);
bool operator<(const big_integer& a, const big_integer& b);
bool operator>(const big_integer& a, const big_integer& b);
bool operator<=(const big_integer& a, const big_integer& b);
bool operator>=(const big_integer& a, const big_integer& b);

std::string to_string(const big_integer& a);
std::ostream& operator<<(std::ostream& s, const big_integer& a);
