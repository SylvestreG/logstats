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

void clf::Parser::reset() {
  _currentLine.ipAddr = std::nullopt;
  _currentLine.userIdentifier = std::nullopt;
  _currentLine.userId = std::nullopt;
  _currentLine.timestamp = std::nullopt;
  _currentLine.verb = std::nullopt;
  _currentLine.path = std::nullopt;
  _currentLine.version = std::nullopt;
  _currentLine.statusCode = std::nullopt;
  _currentLine.objectSize = std::nullopt;
}
