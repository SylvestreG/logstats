#ifndef __CONFIG_H__
#define __CONFIG_H__

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

#endif /* __CONFIG_H__ */