//
// Created by syl on 3/2/21.
//

#include "parser.h"
#include <gtest/gtest.h>

TEST(parser, simpleLineIpv4) {
  parser p;

  auto result =
      p.parseLine("127.0.0.1 - frank [10/Oct/2000:13:55:36 "
                  "-0700] \"GET /apache_pb.gif HTTP/1.0\" 200 2326");

  ASSERT_TRUE(result);
}

TEST(parser, simpleLineIpv6) {
  parser p;

  auto result =
      p.parseLine("cafe:dede:dead:beef:1337:4242:1010:aaac - frank [10/Oct/2000:13:55:36 "
                  "-0700] \"GET /apache_pb.gif HTTP/1.0\" 200 2326");

  ASSERT_TRUE(result);
}

TEST(parser, badIp) {
  parser p;

  auto result =
      p.parseLine("datadog.com - frank [10/Oct/2000:13:55:36 "
                  "-0700] \"GET /apache_pb.gif HTTP/1.0\" 200 2326");

  ASSERT_TRUE(result);
}