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
#ifndef __PARSER_H__
#define __PARSER_H__

#include <chrono>
#include <memory>
#include <optional>
#include <string_view>
#include <variant>

#include <boost/beast.hpp>

using namespace std;
using namespace std::chrono;

namespace clf {

using address =
    std::variant<uint32_t, std::tuple<uint64_t, uint64_t>, string_view>;

enum class httpVersion { httpV10, httpV11, httpVUnknown };

constexpr const char *to_string(httpVersion const v) {
  if (v == httpVersion::httpV10)
    return "HTTP 1/0";
  else if (v == httpVersion::httpV11)
    return "HTTP 1/1";
  else
    return "unknown";
}

struct ClfLine {
  optional<clf::address> ipAddr;
  optional<string> userIdentifier;
  optional<string> userId;
  optional<string> timestamp;
  optional<boost::beast::http::verb> verb;
  optional<std::string> path;
  optional<httpVersion> version;
  optional<boost::beast::http::status> statusCode;
  optional<std::size_t> objectSize;
};

class Parser {
public:
  Parser() = default;
  ~Parser() = default;
  Parser(Parser const &) = delete;
  Parser &operator=(Parser const &) = delete;

  void reset();
  std::optional<ClfLine> parseLine(std::string_view str);

  // interfaces called by yacc
  void onIp(clf::address) noexcept;
  void onUserIdentifier(std::string *) noexcept;
  void onUserId(std::string *) noexcept;
  void onTimeStamp(std::string *) noexcept;
  void onRequest(boost::beast::http::verb, std::string *path,
                 httpVersion version) noexcept;
  void onObjectSize(size_t) noexcept;
  void onErrorCode(boost::beast::http::status) noexcept;
  void onParseDone() noexcept;

private:
  ClfLine _currentLine;
};
} // namespace clf

#endif // __PARSER_H__