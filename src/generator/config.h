/*
 * Copyright (c) 2021 Sylvestre Gallon
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#ifndef __GENERATOR_CONFIG_H__
#define __GENERATOR_CONFIG_H__

#include <boost/beast.hpp>
#include <chrono>
#include <cstdint>
#include <filesystem>

using namespace std::chrono_literals;

class Config {
public:
  Config() = default;
  explicit Config(const std::filesystem::path &file);
  ~Config() = default;
  Config(Config const &conf) = delete;
  Config &operator=(Config const &conf) = delete;

  uint16_t requestNumber() const noexcept;
  std::chrono::milliseconds delayBetweenRequestMs() const noexcept;

  // requestStuff
  std::string ipAddr() const noexcept;
  std::string userIdentifier() const noexcept;
  std::string userId() const noexcept;
  boost::beast::http::verb httpVerb() const noexcept;
  std::filesystem::path path() const noexcept;
  std::string httpVersion() const noexcept;
  boost::beast::http::status httpStatus() const noexcept;
  uint32_t size() const noexcept;

private:
  uint16_t _requestNumber{100};
  std::chrono::milliseconds _delayBetweenRequestMs{100};

  std::string _ip{"-"};
  std::string _userIdentifer{"-"};
  std::string _userId{"-"};

  boost::beast::http::verb _httpVerb{boost::beast::http::verb::get};
  std::filesystem::path _path{"/index.html"};
  std::string _httpVersion{"HTTP/1.1"};

  boost::beast::http::status _status{boost::beast::http::status::ok};
  uint32_t _size{420};
};

#endif // __GENERATOR_CONFIG_H__