/*
 *  Copyright (c) 2015, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FATAL_INCLUDE_fatal_string_string_ref_h
#define FATAL_INCLUDE_fatal_string_string_ref_h

#include <fatal/math/hash.h>

#include <algorithm>
#include <iterator>

#include <cassert>
#include <cstring>

namespace fatal {

struct string_ref {
  using value_type = char;
  using size_type = std::size_t;
  using const_iterator = char const *;

  string_ref(): begin_(nullptr), end_(nullptr) {}

  string_ref(const_iterator begin, const_iterator end):
    begin_(begin),
    end_(end)
  {
    assert(begin_ <= end_);
  }

  string_ref(const_iterator s, std::size_t size):
    begin_(s),
    end_(std::next(s, size))
  {
    assert(begin_ <= end_);
  }

  explicit string_ref(value_type *s):
    begin_(s),
    end_(std::next(s, std::strlen(s)))
  {
    assert(begin_ <= end_);
  }

  explicit string_ref(value_type const *s):
    begin_(s),
    end_(std::next(s, std::strlen(s)))
  {
    assert(begin_ <= end_);
  }

  template <std::size_t N>
  explicit string_ref(value_type const (&s)[N]):
    begin_(s),
    end_(std::strlen(s))
  {
    assert(begin_ <= end_);
  }

  explicit string_ref(value_type const &c):
    begin_(std::addressof(c)),
    end_(std::next(std::addressof(c)))
  {
    assert(begin_ <= end_);
  }

  template <typename U, typename = safe_overload_t<string_ref, U>>
  explicit string_ref(U &&s):
    begin_(s.data()),
    end_(std::next(s.data(), s.size()))
  {
    assert(begin_ <= end_);
  }

  string_ref slice(size_type offset, size_type end) const {
    assert(offset <= size());
    assert(end <= size());
    return string_ref(std::next(begin_, offset), std::next(begin_, end));
  }

  const_iterator find(value_type needle) const {
    return find(needle, begin_);
  }

  const_iterator find(value_type needle, const_iterator offset) const {
    assert(begin_ <= offset);
    assert(offset <= end_);
    return std::find(offset, end_, needle);
  }

  template <typename U>
  string_ref split_step(U &&delimiter) {
    string_ref result(begin_, find(std::forward<U>(delimiter)));
    begin_ = result.end() == end_ ? end_ : std::next(result.end());
    assert(begin_ <= end_);
    return result;
  }

  void advance_to(const_iterator i) {
    assert(begin_ <= i);
    begin_ = i;
    assert(begin_ <= end_);
  }

  void limit(size_type size) {
    assert(size >= 0);

    if (size < this->size()) {
      end_ = std::next(begin_, size);
    }

    assert(begin_ <= end_);
  }

  const_iterator data() const { return begin_; }

  void clear() { begin_ = end_; }

  size_type size() const {
    assert(begin_ <= end_);
    return std::distance(begin_, end_);
  }

  bool empty() const {
    assert(begin_ <= end_);
    return begin_ == end_;
  }

  const_iterator cbegin() const { return begin_; }
  const_iterator begin() const { return begin_; }
  const_iterator cend() const { return end_; }
  const_iterator end() const { return end_; }

  string_ref &operator +=(size_type i) {
    assert(i <= size());
    std::advance(begin_, i);
    assert(begin_ <= end_);
    return *this;
  }

  string_ref &operator -=(size_type i) {
    begin_ -= i;
    assert(begin_ <= end_);
    return *this;
  }

  string_ref operator +(size_type i) const {
    auto copy(*this);
    copy += i;
    return copy;
  }

  string_ref operator -(size_type i) const {
    auto copy(*this);
    copy -= i;
    return copy;
  }

  string_ref &operator ++() {
    assert(begin_ < end_);
    std::advance(begin_, 1);
    assert(begin_ <= end_);
    return *this;
  }

  string_ref &operator --() {
    --begin_;
    assert(begin_ <= end_);
    return *this;
  }

  string_ref operator ++(int) {
    auto copy(*this);
    ++*this;
    return copy;
  }

  string_ref operator --(int) {
    auto copy(*this);
    --*this;
    return copy;
  }

  value_type operator *() const {
    assert(begin_ <= end_);
    return *begin_;
  }

  bool operator ==(string_ref rhs) const {
    return size() == rhs.size() && !std::strncmp(begin_, rhs.begin_, size());
  }

  template <typename U, typename = safe_overload_t<string_ref, U>>
  bool operator ==(U &&rhs) const {
    return *this == string_ref(std::forward<U>(rhs));
  }

  template <typename U>
  bool operator !=(U &&rhs) const { return !(*this == std::forward<U>(rhs)); }

  bool operator <(string_ref rhs) const {
    auto const result = std::strncmp(
      begin_, rhs.begin_, std::min(size(), rhs.size())
    );

    return result < 0 || (!result && size() < rhs.size());
  }

  template <typename U, typename = safe_overload_t<string_ref, U>>
  bool operator <(U &&rhs) const {
    return *this < string_ref(std::forward<U>(rhs));
  }

  bool operator >(string_ref rhs) const { return rhs < *this; }

  template <typename U, typename = safe_overload_t<string_ref, U>>
  bool operator >(U &&rhs) const {
    return *this > string_ref(std::forward<U>(rhs));
  }

  explicit operator bool() const { return begin_ < end_; }

  bool operator !() const { return empty(); }

private:
  const_iterator begin_;
  const_iterator end_;
};

/////////////////
// operator == //
/////////////////

template <typename T, typename = safe_overload_t<string_ref, T>>
bool operator ==(T const &lhs, string_ref rhs) { return rhs == lhs; }

////////////////
// operator < //
////////////////

template <typename T, typename = safe_overload_t<string_ref, T>>
bool operator <(T const &lhs, string_ref rhs) { return rhs > lhs; }

////////////////
// operator > //
////////////////

template <typename T, typename = safe_overload_t<string_ref, T>>
bool operator >(T const &lhs, string_ref rhs) { return rhs < lhs; }

/////////////////
// operator != //
/////////////////

template <typename T>
bool operator !=(string_ref lhs, T const &rhs) { return !(lhs == rhs); }

template <typename T, typename = safe_overload_t<string_ref, T>>
bool operator !=(T const &lhs, string_ref rhs) { return !(rhs == lhs); }

/////////////////
// operator <= //
/////////////////

template <typename T>
bool operator <=(string_ref lhs, T const &rhs) { return !(lhs > rhs); }

template <typename T, typename = safe_overload_t<string_ref, T>>
bool operator <=(T const &lhs, string_ref rhs) { return !(rhs < lhs); }

/////////////////
// operator >= //
/////////////////

template <typename T>
bool operator >=(string_ref lhs, T const &rhs) { return !(lhs < rhs); }

template <typename T, typename = safe_overload_t<string_ref, T>>
bool operator >=(T const &lhs, string_ref rhs) { return !(rhs > lhs); }

///////////////////////////////
// operator <<(std::ostream) //
///////////////////////////////

std::ostream &operator <<(std::ostream &out, string_ref rhs) {
  return out.write(rhs.data(), rhs.size());
}

} // namespace fatal {

#include <functional>

namespace std {

template <>
struct hash<fatal::string_ref> {
  using argument = fatal::string_ref;
  using result_type = std::size_t;

  result_type operator ()(fatal::string_ref s) const {
    return *fatal::bytes_hasher<result_type>()(s.data(), s.size());
  }
};

} // namespace std {

#endif // FATAL_INCLUDE_fatal_string_string_ref_h
