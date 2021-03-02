%{
typedef void * yyscan_t;

#include <cstdint>
#include <string_view>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "parser.h"
#include "cflParser.hpp"
#include "cflLexer.h"

static void yyerror(YYLTYPE *yylloc, yyscan_t scanner, parser *parser, const char * msg);
std::shared_ptr<spdlog::logger> parserLogger = spdlog::stderr_color_mt("parser");
%}

%define api.pure full
%lex-param {yyscan_t scanner}
%parse-param {yyscan_t scanner}
%parse-param {parser *parser}
%locations

%union {
  uint32_t ip;
}

%token T_SPACE T_DOT T_DIGIT T_ALPHANUM T_COLON T_DASH T_ALPHA T_PUNCT
%token T_OPEN_TAB T_CLOSE_TAB T_PLUS T_SLASH T_QUOTE
%token T_HTTP_DELETE T_HTTP_GET T_HTTP_HEAD T_HTTP_POST T_HTTP_PUT T_HTTP_CONNECT T_HTTP_OPTIONS
%token T_HTTP_TRACE T_HTTP_COPY T_HTTP_LOCK T_HTTP_MKCOL T_HTTP_MOVE T_HTTP_PROPFIND T_HTTP_PROPPATCH
%token T_HTTP_SEARCH T_HTTP_UNLOCK  T_HTTP_BIND T_HTTP_REBIND T_HTTP_UNBIND T_HTTP_ACL T_HTTP_REPORT
%token T_HTTP_MKACTIVITY T_HTTP_CHECKOUT T_HTTP_MERGE T_HTTP_MSEARCH T_HTTP_NOTIFY T_HTTP_SUBSCRIBE
%token T_HTTP_UNSUBSCRIBE T_HTTP_PATCH T_HTTP_URGE T_HTTP_MKCALENDAR T_HTTP_LINK T_HTTP_UNLINK
%token T_HTTP_1_0 T_HTTP_1_1
%start main_rule

%%

main_rule:
	cfl_rule
	;

cfl_rule:
	| ip userIdentifier userId timestamp request error_code size { parserLogger->info("parse succeed"); }
	;

ip:
	T_DASH T_SPACE
	| ipv4 T_SPACE
	| ipv6 T_SPACE
	| error T_SPACE { parserLogger->info("bad ip"); }
	;

ipv4:
	T_DIGIT T_DOT T_DIGIT T_DOT T_DIGIT T_DOT T_DIGIT { parserLogger->info("IPV4"); }
	;

ipv6:
	hex T_COLON hex T_COLON hex T_COLON hex T_COLON hex T_COLON hex T_COLON hex T_COLON hex { parserLogger->info("IPV6"); }
	;

userIdentifier:
	anythingList T_SPACE { parserLogger->info("userIdentifier"); }
	;

userId:
	anythingList T_SPACE { parserLogger->info("userId"); }
	;

timestamp:
	T_OPEN_TAB T_DIGIT T_SLASH T_ALPHA T_SLASH T_DIGIT T_COLON T_DIGIT T_COLON T_DIGIT T_COLON T_DIGIT T_SPACE T_PLUS T_DIGIT T_CLOSE_TAB T_SPACE { parserLogger->info("TIME"); }
	| T_OPEN_TAB T_DIGIT T_SLASH T_ALPHA T_SLASH T_DIGIT T_COLON T_DIGIT T_COLON T_DIGIT T_COLON T_DIGIT T_SPACE T_DASH T_DIGIT T_CLOSE_TAB T_SPACE { parserLogger->info("TIME"); }
	| error T_SPACE  { parserLogger->info("bad timestamp"); }
	;

request:
	http_request_type pathList T_HTTP_1_0 T_SPACE {parserLogger->info("request");}
	| http_request_type pathList T_HTTP_1_1 T_SPACE {parserLogger->info("request");}
	;


error_code:
	T_DIGIT T_SPACE { parserLogger->info("error_code");};

size:
	T_DIGIT { parserLogger->info("size");};

hex:
	T_ALPHANUM
	| T_DIGIT
	;

anythingList:
	anything
	| anythingList anything
	;

pathList:
	path
	| pathList path
	;

path:
	T_SLASH url
	;

url:
	T_ALPHA
	| T_ALPHANUM
	| T_DIGIT
	| T_DOT
	| T_PUNCT
	| url T_ALPHA
	| url T_ALPHANUM
	| url T_DIGIT
	| url T_DOT
	| url T_PUNCT
	;

http_request_type:
	T_HTTP_DELETE
	| T_HTTP_GET
	| T_HTTP_HEAD
	| T_HTTP_POST
	| T_HTTP_PUT
	| T_HTTP_CONNECT
	| T_HTTP_OPTIONS
	| T_HTTP_TRACE
	| T_HTTP_COPY
	| T_HTTP_LOCK
	| T_HTTP_MKCOL
	| T_HTTP_MOVE
	| T_HTTP_PROPFIND
	| T_HTTP_PROPPATCH
	| T_HTTP_SEARCH
	| T_HTTP_UNLOCK
	| T_HTTP_BIND
	| T_HTTP_REBIND
	| T_HTTP_UNBIND
	| T_HTTP_ACL
	| T_HTTP_REPORT
	| T_HTTP_MKACTIVITY
	| T_HTTP_CHECKOUT
	| T_HTTP_MERGE
	| T_HTTP_MSEARCH
	| T_HTTP_NOTIFY
	| T_HTTP_SUBSCRIBE
	| T_HTTP_UNSUBSCRIBE
	| T_HTTP_PATCH
	| T_HTTP_URGE
	| T_HTTP_MKCALENDAR
	| T_HTTP_LINK
	| T_HTTP_UNLINK

anything:
	T_DOT
	| T_DIGIT
	| T_ALPHANUM
	| T_COLON
	| T_DASH
	| T_ALPHA
	| T_PUNCT
	| T_OPEN_TAB
	| T_CLOSE_TAB
	| T_PLUS
	| T_SLASH
	| T_QUOTE
	| http_request_type

%%

int yywrap ( yyscan_t yyscanner ) {
	return 1;
}

static void yyerror(YYLTYPE *yy, yyscan_t scanner, parser *parser, const char * msg) {
}

std::optional<ClfLine> parser::parseLine(std::string_view str) {
	yyscan_t scanner;
	yylex_init(&scanner);
	yylex_init_extra(this, &scanner);

	yy_scan_bytes(str.data(), str.size(), scanner);

	int val = yyparse(scanner, this);
	yylex_destroy (scanner) ;

	if (val)
		return nullopt;

	return _currentLine;
}