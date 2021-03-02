//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

#ifndef __HTTP_H__
#define __HTTP_H__

#include <cstdint>
#include <string_view>

namespace http {
enum class status : unsigned {
  /** An unknown status-code.

      This value indicates that the value for the status code
      is not in the list of commonly recognized status codes.
      Callers interested in the exactly value should use the
      interface which provides the raw integer.
  */
  unknown = 0,

  continue_ = 100,

  /** Switching Protocols

      This status indicates that a request to switch to a new
      protocol was accepted and applied by the server. A successful
      response to a WebSocket Upgrade HTTP request will have this
      code.
  */
  switching_protocols = 101,

  processing = 102,

  ok = 200,
  created = 201,
  accepted = 202,
  non_authoritative_information = 203,
  no_content = 204,
  reset_content = 205,
  partial_content = 206,
  multi_status = 207,
  already_reported = 208,
  im_used = 226,

  multiple_choices = 300,
  moved_permanently = 301,
  found = 302,
  see_other = 303,
  not_modified = 304,
  use_proxy = 305,
  temporary_redirect = 307,
  permanent_redirect = 308,

  bad_request = 400,
  unauthorized = 401,
  payment_required = 402,
  forbidden = 403,
  not_found = 404,
  method_not_allowed = 405,
  not_acceptable = 406,
  proxy_authentication_required = 407,
  request_timeout = 408,
  conflict = 409,
  gone = 410,
  length_required = 411,
  precondition_failed = 412,
  payload_too_large = 413,
  uri_too_long = 414,
  unsupported_media_type = 415,
  range_not_satisfiable = 416,
  expectation_failed = 417,
  misdirected_request = 421,
  unprocessable_entity = 422,
  locked = 423,
  failed_dependency = 424,
  upgrade_required = 426,
  precondition_required = 428,
  too_many_requests = 429,
  request_header_fields_too_large = 431,
  connection_closed_without_response = 444,
  unavailable_for_legal_reasons = 451,
  client_closed_request = 499,

  internal_server_error = 500,
  not_implemented = 501,
  bad_gateway = 502,
  service_unavailable = 503,
  gateway_timeout = 504,
  http_version_not_supported = 505,
  variant_also_negotiates = 506,
  insufficient_storage = 507,
  loop_detected = 508,
  not_extended = 510,
  network_authentication_required = 511,
  network_connect_timeout_error = 599
};

constexpr status int_to_status(unsigned v) {
  switch (static_cast<status>(v)) {
    // 1xx
  case status::continue_:
  case status::switching_protocols:
  case status::processing:

    // 2xx
  case status::ok:
  case status::created:
  case status::accepted:
  case status::non_authoritative_information:
  case status::no_content:
  case status::reset_content:
  case status::partial_content:
  case status::multi_status:
  case status::already_reported:
  case status::im_used:

    // 3xx
  case status::multiple_choices:
  case status::moved_permanently:
  case status::found:
  case status::see_other:
  case status::not_modified:
  case status::use_proxy:
  case status::temporary_redirect:
  case status::permanent_redirect:

    // 4xx
  case status::bad_request:
  case status::unauthorized:
  case status::payment_required:
  case status::forbidden:
  case status::not_found:
  case status::method_not_allowed:
  case status::not_acceptable:
  case status::proxy_authentication_required:
  case status::request_timeout:
  case status::conflict:
  case status::gone:
  case status::length_required:
  case status::precondition_failed:
  case status::payload_too_large:
  case status::uri_too_long:
  case status::unsupported_media_type:
  case status::range_not_satisfiable:
  case status::expectation_failed:
  case status::misdirected_request:
  case status::unprocessable_entity:
  case status::locked:
  case status::failed_dependency:
  case status::upgrade_required:
  case status::precondition_required:
  case status::too_many_requests:
  case status::request_header_fields_too_large:
  case status::connection_closed_without_response:
  case status::unavailable_for_legal_reasons:
  case status::client_closed_request:

    // 5xx
  case status::internal_server_error:
  case status::not_implemented:
  case status::bad_gateway:
  case status::service_unavailable:
  case status::gateway_timeout:
  case status::http_version_not_supported:
  case status::variant_also_negotiates:
  case status::insufficient_storage:
  case status::loop_detected:
  case status::not_extended:
  case status::network_authentication_required:
  case status::network_connect_timeout_error:
    return static_cast<status>(v);

  default:
    break;
  }
  return status::unknown;
}

std::string_view obsolete_reason(status v);
}; // namespace http

#endif // __HTTP_H__