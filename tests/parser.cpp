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
#include <gtest/gtest.h>

TEST(parser, invalidLineTooShort) {
  auto result = clf::Parser().parseLine("- - - - - -");

  ASSERT_FALSE(result);
}

TEST(parser, invalidLineTooLong) {
  auto result = clf::Parser().parseLine("- - - - - - - -");

  ASSERT_FALSE(result);
}

TEST(parser, emptyLine) {
  auto result = clf::Parser().parseLine("- - - - - - -");

  ASSERT_TRUE(result);
  ASSERT_FALSE(result->ipAddr);
  ASSERT_FALSE(result->userIdentifier);
  ASSERT_FALSE(result->userId);
  ASSERT_FALSE(result->timestamp);
  ASSERT_FALSE(result->objectSize);
  ASSERT_FALSE(result->statusCode);
}

TEST(parser, simpleLineIpv4) {
  auto result = clf::Parser().parseLine(
      "127.0.0.1 - frank [10/Oct/2000:13:55:36 "
      "-0700] \"GET /apache_pb.gif HTTP/1.0\" 200 2326");

  ASSERT_TRUE(result);
}

TEST(parser, simpleLineIpv6) {
  auto result = clf::Parser().parseLine(
      "cafe:dede:dead:beef:1337:4242:1010:aaac - frank [10/Oct/2000:13:55:36 "
      "-0700] \"GET /apache_pb.gif HTTP/1.0\" 200 2326");

  ASSERT_TRUE(result);
}

TEST(parser, badIp) {
  auto result = clf::Parser().parseLine(
      "datadog.com toto-13134@!!411 frank [10/Oct/2000:13:55:36 "
      "-0700] \"GET /apache_pb.gif HTTP/1.0\" 200 2326");

  ASSERT_TRUE(result);
}