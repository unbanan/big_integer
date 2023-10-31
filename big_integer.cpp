#include "big_integer.h"

big_integer::big_integer() = default;

big_integer::big_integer(const big_integer& other) : sign(other.sign), digits(other.digits) {}

big_integer::big_integer(long long a) {
  uint64_t abs_a = static_cast<uint64_t>((a == std::numeric_limits<long long>::min() || a >= 0) ? a : -a);
  while (abs_a != 0) {
    digits.push_back(abs_a % systemBase);
    abs_a /= systemBase;
  }
  sign = (a < 0);
}

big_integer::big_integer(unsigned long long a) {
  while (a != 0) {
    digits.push_back(a % systemBase);
    a /= systemBase;
  }
  sign = false;
}

big_integer::big_integer(int a) {
  *this = big_integer(static_cast<long long>(a));
}

big_integer::big_integer(unsigned int a) {
  *this = big_integer(static_cast<unsigned long long>(a));
}

big_integer::big_integer(long a) {
  *this = big_integer(static_cast<long long>(a));
}

big_integer::big_integer(unsigned long a) {
  *this = big_integer(static_cast<unsigned long long>(a));
}

big_integer::big_integer(short a) {
  *this = big_integer(static_cast<long long>(a));
}

big_integer::big_integer(unsigned short a) {
  *this = big_integer(static_cast<unsigned long long>(a));
}

uint32_t big_integer::getInt(const std::string& str) {
  uint32_t res = 0;
  for (char i : str) {
    res *= 10;
    res += static_cast<uint32_t>(i - '0');
  }
  return res;
}

big_integer::big_integer(const std::string& str) {
  if (str.empty()) {
    throw std::invalid_argument("Your string is 0 size");
  }
  size_t begin = (str[0] == '-' || str[0] == '+' ? 1 : 0);
  if (str.size() == begin) {
    throw std::invalid_argument("Invalid string given: {}");
  }
  for (size_t i = begin; i < str.size(); i++) {
    if ('0' > str[i] || str[i] > '9') {
      throw std::invalid_argument("Invalid string given(It is not a number)");
    }
  }
  size_t size = str.size();
  for (size_t i = begin; i < size; i += 9) {
    size_t right = std::min(i + 8, size - 1);
    size_t dif = right - i + 1;
    uint32_t number = getInt(str.substr(i, dif));
    *this *= (static_cast<int64_t>(std::pow(10, dif)));
    *this += (static_cast<int64_t>(number));
  }
  sign = str[0] == '-';
  clear_zeros();
}

big_integer::~big_integer() {
  digits.clear();
  sign = false;
}

void big_integer::swap(big_integer& other) {
  std::swap(digits, other.digits);
  std::swap(sign, other.sign);
}

big_integer& big_integer::operator=(const big_integer& other) {
  if (this != &other) {
    big_integer temp = other;
    swap(temp);
  }
  return *this;
}

big_integer big_integer::abs(const big_integer& other) {
  big_integer temp(other);
  temp.sign = false;
  return temp;
}

big_integer& big_integer::operator+=(const big_integer& rhs) {
  if (sign == rhs.sign) {
    bool resultedSign = sign;
    digits.resize(std::max(digits.size(), rhs.digits.size()) + 1);
    uint64_t second_carry = 0;
    for (size_t i = 0; i < digits.size(); i++) {
      uint64_t first_carry = digits[i];
      first_carry += (i < rhs.digits.size() ? rhs.digits[i] : 0);
      first_carry += second_carry;
      digits[i] = first_carry % systemBase;
      second_carry = first_carry / systemBase;
    }
    sign = resultedSign;
    clear_zeros();
    return *this;
  }
  if (sign) {
    *this = rhs - abs(*this);
  } else {
    *this -= abs(rhs);
  }
  return *this;
}

big_integer& big_integer::operator-=(const big_integer& rhs) {
  if (sign != rhs.sign) {
    if (sign) {
      *this = -(abs(*this) + rhs);
    } else {
      *this += abs(rhs);
    }
    return *this;
  }
  bool resultedSign = (*this < rhs);
  bool absCompare = (abs(*this) < abs(rhs));
  digits.resize(std::max(digits.size(), rhs.digits.size()));
  uint64_t carry = 0;
  for (size_t i = 0; i < digits.size(); i++) {
    uint64_t first = digits[i], second = (i < rhs.digits.size() ? rhs.digits[i] : 0);
    if (absCompare) {
      std::swap(first, second);
    }
    uint64_t d = first;
    if (first >= second + carry) {
      d -= second;
      d -= carry;
      carry = 0;
    } else {
      d += systemBase - second - carry;
      carry = 1;
    }
    digits[i] = d % systemBase;
  }
  sign = resultedSign;
  clear_zeros();
  return *this;
}

big_integer& big_integer::operator*=(const big_integer& rhs) {
  bool resultedSign = (sign ^ rhs.sign);
  uint64_t carry = 0;
  size_t size = digits.size();
  digits.resize(digits.size() + rhs.digits.size() + 1);
  for (ptrdiff_t i = size - 1; i >= 0; i--) {
    uint64_t previousDigit = digits[i];
    digits[i] = 0;
    for (size_t j = 0; j < rhs.digits.size(); j++) {
      uint64_t tempDigit = digits[i + j];
      tempDigit += static_cast<uint64_t>(previousDigit * rhs.digits[j]);
      tempDigit += carry;
      digits[i + j] = tempDigit % systemBase;
      carry = tempDigit / systemBase;
    }
    size_t pos = i + rhs.digits.size();
    for (; carry; pos++) {
      uint64_t temp_carry = digits[pos] + carry;
      digits[pos] = temp_carry % systemBase;
      carry = temp_carry / systemBase;
    }
  }
  sign = resultedSign;
  clear_zeros();
  return *this;
}

int64_t big_integer::quotient(big_integer& x, uint32_t k) {
  size_t m = x.digits.size();
  uint64_t carry = 0;
  uint64_t b = (1LL << intSize);
  for (ptrdiff_t i = m - 1; i >= 0; i--) {
    uint64_t temp = carry * b + x.digits[i];
    x.digits[i] = temp / k;
    carry = temp % k;
  }
  x.clear_zeros();
  return carry;
}

uint32_t big_integer::trial(big_integer& r, const big_integer& d, int k, int m) {
  uint64_t d2 = 0, km = 0;
  km = k + m;
  uint64_t b = (1LL << intSize);
  uint64_t r3 = 0;
  if (km < r.digits.size()) {
    r3 += r.digits[km] * b;
  }
  if (km - 1 < r.digits.size()) {
    r3 += r.digits[km - 1];
  }

  d2 = d.digits[m - 1];
  return std::min(r3 / d2, static_cast<uint64_t>(b - 1));
}

void big_integer::difference(big_integer& r, const big_integer& dq, int k, int m) {
  r.digits.resize(std::max(static_cast<size_t>(m + k + 1), r.digits.size()));
  uint64_t borrow = 0;
  uint64_t b = (1LL << intSize);

  for (size_t i = 0; i <= m; i++) {
    uint64_t val = 0;
    if (i < dq.digits.size()) {
      val = dq.digits[i];
    }
    if (r.digits[i + k] >= val + borrow) {
      r.digits[i + k] -= val + borrow;
      borrow = 0;
    } else {
      r.digits[i + k] += b - val - borrow;
      borrow = 1;
    }
  }
  r.clear_zeros();
}

bool big_integer::smaller(big_integer& r, const big_integer& dq, int k, int m) {
  int i = m;
  while (i != 0) {
    uint32_t val1 = 0, val2 = 0;
    if (i + k < r.digits.size()) {
      val1 = r.digits[i + k];
    }
    if (i < dq.digits.size()) {
      val2 = dq.digits[i];
    }
    if (val1 != val2) {
      return val1 < val2;
    } else {
      i = i - 1;
    }
  }
  return false;
}

std::pair<big_integer, big_integer> big_integer::long_divide(big_integer& x, const big_integer& y) {
  if (abs(x) < abs(y)) {
    return {0, x};
  }
  if (y == 0) {
    throw std::invalid_argument("You cant divide by zero");
  }
  bool resultedSign = x.sign ^ y.sign;
  bool prevSign = x.sign;
  if (y.digits.size() == 1) {
    x.sign = resultedSign;
    big_integer rem = quotient(x, y.digits[0]);
    rem.sign = prevSign;
    rem.clear_zeros();
    return {x, rem};
  }
  x.sign = false;
  big_integer z = y;
  z.sign = false;
  big_integer d, dq;
  uint64_t f = 0, qt = 0;
  size_t m = y.digits.size();
  size_t n = x.digits.size();
  int64_t b = (1LL << intSize);
  f = b / (z.digits[m - 1] + 1);
  big_integer r = x, q = 0;
  r = x * f;
  d = z * f;
  q.digits.resize(n - m + 1);
  for (ptrdiff_t k = n - m; k >= 0; k--) {
    qt = trial(r, d, k, m);
    dq = d * qt;
    if (smaller(r, dq, k, m)) {
      qt--;
      dq = d * qt;
    }
    q.digits[k] = qt;
    difference(r, dq, k, m);
  }

  q.sign = resultedSign;
  r.sign = prevSign;
  q.clear_zeros();
  quotient(r, f);
  r.clear_zeros();
  return {q, r};
}

big_integer& big_integer::operator/=(const big_integer& rhs) {
  *this = long_divide(*this, rhs).first;
  return *this;
}

big_integer& big_integer::operator%=(const big_integer& rhs) {
  *this = long_divide(*this, rhs).second;
  return *this;
}

big_integer& big_integer::binaryOperation(const big_integer& rhs, const std::function<int(int, int)>& op) {
  big_integer temp2(rhs);
  bool oldSign = sign;
  bool oldTemp2Sign = temp2.sign;
  if (temp2.sign) {
    temp2++;
  }
  if (sign) {
    operator++();
  }
  digits.resize(std::max(digits.size(), temp2.digits.size()));
  sign = oldSign;
  temp2.sign = oldTemp2Sign;
  bool resultedSign = op(sign, temp2.sign);
  for (size_t i = 0; i < digits.size(); i++) {
    if (sign) {
      digits[i] = ~digits[i];
    }
    uint32_t rhsDigit = (i < temp2.digits.size() ? temp2.digits[i] : 0);
    if (temp2.sign) {
      rhsDigit = ~rhsDigit;
    }
    digits[i] = op(digits[i], rhsDigit);
    if (resultedSign) {
      digits[i] = ~digits[i];
    }
  }
  sign = resultedSign;
  if (sign) {
    (*this)--;
  }
  clear_zeros();
  return *this;
}

big_integer& big_integer::operator&=(const big_integer& rhs) {
  return binaryOperation(rhs, [](uint32_t x, uint32_t y) { return x & y; });
}

big_integer& big_integer::operator|=(const big_integer& rhs) {
  return binaryOperation(rhs, [](uint32_t x, uint32_t y) { return x | y; });
}

big_integer& big_integer::operator^=(const big_integer& rhs) {
  return binaryOperation(rhs, [](uint32_t x, uint32_t y) { return x ^ y; });
}

big_integer& big_integer::operator<<=(int rhs) {
  if (rhs < 0) {
    return operator>>=(-rhs);
  }
  digits.insert(digits.begin(), rhs / intSize, 0);
  rhs %= intSize;
  uint64_t d = 1;
  d <<= rhs;
  *this *= static_cast<int64_t>(d);
  return *this;
}

big_integer& big_integer::operator>>=(int rhs) {
  if (rhs < 0) {
    return operator<<=(-rhs);
  }
  digits.erase(digits.begin(), digits.begin() + rhs / intSize);
  rhs %= intSize;
  uint64_t d = (1ull << rhs);
  bool sign_ = sign;
  sign = false;
  uint32_t rem = quotient(*this, d);
  if (sign_ && rem > 0) {
    *this += 1;
  }
  sign = sign_;
  return *this;
}

big_integer big_integer::operator+() const {
  return *this;
}

big_integer big_integer::operator-() const {
  return ((*this != 0) ? big_integer(0) - *this : big_integer(0));
}

big_integer big_integer::operator~() const {
  big_integer res = *this;
  res++;
  res.sign ^= 1;
  res.sign &= (!res.digits.empty());
  return res;
}

big_integer& big_integer::operator++() {
  return *this += 1;
}

big_integer big_integer::operator++(int) {
  big_integer temp = *this;
  ++*this;
  return temp;
}

big_integer& big_integer::operator--() {
  return *this -= 1;
}

big_integer big_integer::operator--(int) {
  big_integer temp = *this;
  --*this;
  return temp;
}

big_integer operator+(big_integer a, const big_integer& b) {
  return a += b;
}

big_integer operator-(big_integer a, const big_integer& b) {
  return a -= b;
}

big_integer operator*(big_integer a, const big_integer& b) {
  return a *= b;
}

big_integer operator/(big_integer a, const big_integer& b) {
  return a /= b;
}

big_integer operator%(big_integer a, const big_integer& b) {
  return a %= b;
}

big_integer operator&(big_integer a, const big_integer& b) {
  return a &= b;
}

big_integer operator|(big_integer a, const big_integer& b) {
  return a |= b;
}

big_integer operator^(big_integer a, const big_integer& b) {
  return a ^= b;
}

big_integer operator<<(big_integer a, int b) {
  return a <<= b;
}

big_integer operator>>(big_integer a, int b) {
  return a >>= b;
}

bool operator==(const big_integer& a, const big_integer& b) {
  return (a.digits == b.digits && a.sign == b.sign);
}

bool operator!=(const big_integer& a, const big_integer& b) {
  return !(a == b);
}

bool operator<(const big_integer& a, const big_integer& b) {
  if (a.sign != b.sign) {
    return a.sign;
  }
  if (a.digits.size() > b.digits.size()) {
    return a.sign;
  }
  if (a.digits.size() < b.digits.size()) {
    return !a.sign;
  }
  for (size_t i = a.digits.size(); i != 0; i--) {
    if (a.digits[i - 1] < b.digits[i - 1]) {
      return !a.sign;
    }
    if (a.digits[i - 1] > b.digits[i - 1]) {
      return a.sign;
    }
  }
  return false;
}

bool operator>(const big_integer& a, const big_integer& b) {
  return !(a < b || a == b);
}

bool operator<=(const big_integer& a, const big_integer& b) {
  return !(a > b);
}

bool operator>=(const big_integer& a, const big_integer& b) {
  return !(a < b);
}

std::string getReversedString(uint32_t x) {
  std::string res = "";
  while (x) {
    res += static_cast<char>('0' + x % 10);
    x /= 10;
  }
  while (res.size() < 9) {
    res.push_back('0');
  }
  return res;
}

std::string to_string(const big_integer& a) {
  big_integer b = a;
  bool resultedSign = (b < 0);
  b.sign = false;
  const uint32_t step = 1000 * 1000 * 1000;
  std::string result = (a == 0 ? "0" : "");
  while (b != 0) {
    big_integer rem = b % step;
    uint32_t digit = (!rem.digits.empty() ? rem.digits[0] : 0);
    result += getReversedString(digit);
    b /= step;
  }
  while (result.back() == '0' && result.size() > 1) {
    result.pop_back();
  }
  if (resultedSign) {
    result += '-';
  }
  std::reverse(result.begin(), result.end());
  return result;
}

std::ostream& operator<<(std::ostream& s, const big_integer& a) {
  return s << to_string(a);
}

void big_integer::clear_zeros() {
  while (!digits.empty() && digits.back() == 0) {
    digits.pop_back();
  }
  if (digits.empty()) {
    sign = false;
  }
}
