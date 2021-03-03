#ifndef __PARSER_H__
#define __PARSER_H__

#include <chrono>
#include <memory>
#include <optional>
#include <string_view>
#include <variant>

#include "http.h"

using namespace std;
using namespace std::chrono;

namespace clf {

using address =
    std::variant<uint32_t, std::tuple<uint64_t, uint64_t>, string_view>;

struct ClfLine {
  optional<string_view> ipAddr;
  optional<string_view> userIdentifier;
  optional<string_view> userId;
  optional<time_point<system_clock, seconds>> timestamp;
  optional<http::status> statusCode;
  optional<std::size_t> objectSize;
};

class parser {
public:
  parser() = default;
  ~parser() = default;
  parser(parser const &) = delete;
  parser &operator=(parser const &) = delete;

  std::optional<ClfLine> parseLine(std::string_view str);

  // interfaces called by yacc
  void onIp(std::string_view) noexcept;
  void onUserIdentifier(std::string_view) noexcept;
  void onUserId(std::string_view) noexcept;
  void onTimeStamp(std::string_view) noexcept;
  void onRequest(std::string_view) noexcept;
  void onObjectSize(std::string_view) noexcept;
  void onParseDone() noexcept;

private:
  ClfLine _currentLine;
};
} // namespace clf

#endif /* __PARSER_H__ */