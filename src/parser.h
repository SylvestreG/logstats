#ifndef __PARSER_H__
#define __PARSER_H__

#include <chrono>
#include <filesystem>
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
  optional<std::filesystem::path> path;
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

#endif /* __PARSER_H__ */