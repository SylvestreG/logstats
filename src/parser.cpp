//
// Created by syl on 3/2/21.
//

#include "parser.h"
#include <spdlog/spdlog.h>

extern std::shared_ptr<spdlog::logger> parserLogger;

void clf::Parser::onIp(clf::address) noexcept {}

void clf::Parser::onUserIdentifier(std::string *uId) noexcept {
  if (*uId != "-")
    _currentLine.userIdentifier = *uId;
  delete uId;
}

void clf::Parser::onUserId(std::string *userId) noexcept {
  if (*userId != "-")
    _currentLine.userId = *userId;
  delete userId;
}

void clf::Parser::onTimeStamp(std::string *timestamp) noexcept {
  _currentLine.timestamp = *timestamp;
  delete timestamp;
}

void clf::Parser::onRequest(boost::beast::http::verb request, std::string *path,
                            httpVersion version) noexcept {
  _currentLine.verb = request;
  _currentLine.path = *path;
  _currentLine.version = version;
  delete path;
}

void clf::Parser::onObjectSize(size_t size) noexcept {
  _currentLine.objectSize = size;
}

void clf::Parser::onErrorCode(boost::beast::http::status st) noexcept {
  _currentLine.statusCode = st;
}
