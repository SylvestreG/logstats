//
// Created by syl on 3/2/21.
//

#include "parser.h"
#include <gtest/gtest.h>


TEST(parser, invalidLineTooShort) {
  auto result =
      clf::parser().parseLine("- - - - - -");

  ASSERT_FALSE(result);
}

TEST(parser, invalidLineTooLong) {
  auto result =
      clf::parser().parseLine("- - - - - - - -");

  ASSERT_FALSE(result);
}

TEST(parser, emptyLine) {
  auto result =
      clf::parser().parseLine("- - - - - - -");

  ASSERT_TRUE(result);
  ASSERT_FALSE(result->ipAddr);
  ASSERT_FALSE(result->userIdentifier);
  ASSERT_FALSE(result->userId);
  ASSERT_FALSE(result->timestamp);
  ASSERT_FALSE(result->objectSize);
  ASSERT_FALSE(result->statusCode);
}

TEST(parser, simpleLineIpv4) {
  auto result =
      clf::parser().parseLine("127.0.0.1 - frank [10/Oct/2000:13:55:36 "
                  "-0700] \"GET /apache_pb.gif HTTP/1.0\" 200 2326");

  ASSERT_TRUE(result);
}

TEST(parser, simpleLineIpv6) {
  auto result =
      clf::parser().parseLine("cafe:dede:dead:beef:1337:4242:1010:aaac - frank [10/Oct/2000:13:55:36 "
                  "-0700] \"GET /apache_pb.gif HTTP/1.0\" 200 2326");

  ASSERT_TRUE(result);
}

TEST(parser, badIp) {
  auto result =
      clf::parser().parseLine("datadog.com toto-13134@!!411 frank [10/Oct/2000:13:55:36 "
                  "-0700] \"GET /apache_pb.gif HTTP/1.0\" 200 2326");

  ASSERT_TRUE(result);
}