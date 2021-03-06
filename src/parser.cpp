//
// Created by syl on 3/2/21.
//

#include "parser.h"
#include <spdlog/spdlog.h>

void clf::parser::onIp(clf::address) noexcept {}

void clf::parser::onUserIdentifier(std::string *uId) noexcept {
  if (*uId != "-")
    _currentLine.userIdentifier = *uId;
  delete uId;
}

void clf::parser::onUserId(std::string *userId) noexcept {
  if (*userId != "-")
    _currentLine.userId = *userId;
  delete userId;
}

void clf::parser::onTimeStamp(std::string *) noexcept {}

void clf::parser::onRequest(boost::beast::http::verb request,
                            std::filesystem::path path,
                            httpVersion version) noexcept {
  spdlog::info("userId found:{}",
               boost::beast::http::to_string(request).to_string(),
               path.string(), clf::to_string(version));
}

void clf::parser::onObjectSize(size_t size) noexcept {
  _currentLine.objectSize = size;
}

void clf::parser::onErrorCode(boost::beast::http::status st) noexcept {
  spdlog::info("error_code:{}",
               boost::beast::http::obsolete_reason(st).to_string());
}
