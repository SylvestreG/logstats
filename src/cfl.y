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

static void yyerror(YYLTYPE *yylloc, yyscan_t scanner, clf::Parser *parser, const char * msg);
std::shared_ptr<spdlog::logger> parserLogger =
    spdlog::basic_logger_mt("parser", "parser.log");
%}

%define api.pure full
%lex-param {yyscan_t scanner}
%parse-param {yyscan_t scanner}
%parse-param {clf::Parser *parser}
%locations

%union {
  std::string *str;
  boost::beast::http::verb requestType;
  clf::httpVersion version;
}

%token T_SPACE T_DOT T_DIGIT T_ALPHANUM T_COLON T_DASH T_ALPHA T_PUNCT
%token T_OPEN_TAB T_CLOSE_TAB T_PLUS T_SLASH T_QUOTE T_EQUAL T_QUESTION T_TILDE
%token T_HTTP_DELETE T_HTTP_GET T_HTTP_HEAD T_HTTP_POST T_HTTP_PUT T_HTTP_CONNECT T_HTTP_OPTIONS
%token T_HTTP_TRACE T_HTTP_COPY T_HTTP_LOCK T_HTTP_MKCOL T_HTTP_MOVE T_HTTP_PROPFIND T_HTTP_PROPPATCH
%token T_HTTP_SEARCH T_HTTP_UNLOCK  T_HTTP_BIND T_HTTP_REBIND T_HTTP_UNBIND T_HTTP_ACL T_HTTP_REPORT
%token T_HTTP_MKACTIVITY T_HTTP_CHECKOUT T_HTTP_MERGE T_HTTP_MSEARCH T_HTTP_NOTIFY T_HTTP_SUBSCRIBE
%token T_HTTP_UNSUBSCRIBE T_HTTP_PATCH T_HTTP_PURGE T_HTTP_MKCALENDAR T_HTTP_LINK T_HTTP_UNLINK
%token T_HTTP_1_0 T_HTTP_1_1
%token T_NEW_LINE T_EOF
%start main_rule

%type<str> T_ALPHA T_PUNCT T_DIGIT T_ALPHANUM anything anythingList
%type<str> url_valid url_elem url anythingListWithSpace
%type<requestType> http_request_type
%%

main_rule:
	cfl_rule
	;

cfl_rule:
	| ip userIdentifier userId timestamp request error_code size T_NEW_LINE { parserLogger->info("parse succeed"); }
	| ip userIdentifier userId timestamp request error_code size T_NEW_LINE T_EOF{ parserLogger->info("parse succeed"); }
	| ip userIdentifier userId timestamp request error_code size T_EOF { parserLogger->info("parse succeed"); }
	| ip userIdentifier userId timestamp request error_code size T_SPACE T_NEW_LINE { parserLogger->info("parse succeed"); }
	| ip userIdentifier userId timestamp request error_code size T_SPACE T_NEW_LINE T_EOF { parserLogger->info("parse succeed"); }
	| ip userIdentifier userId timestamp request error_code size T_SPACE T_EOF { parserLogger->info("parse succeed"); }
	| ip userIdentifier userId timestamp request error_code size anythingListWithSpace T_NEW_LINE { parserLogger->info("parse succeed"); }
	| ip userIdentifier userId timestamp request error_code size anythingListWithSpace T_NEW_LINE T_EOF{ parserLogger->info("parse succeed"); }
	| error T_EOF {  parserLogger->info("parse error"); }
	;

ip:
	T_DASH T_SPACE
	| ipv4 T_SPACE
	| ipv6 T_SPACE
	| error T_SPACE { parserLogger->info("bad ip"); }
	;

ipv4:
	T_DIGIT T_DOT T_DIGIT T_DOT T_DIGIT T_DOT T_DIGIT { parserLogger->info("IPV4"); delete $1; delete $3; delete $5; delete $7; }
	;

ipv6:
	hex T_COLON hex T_COLON hex T_COLON hex T_COLON hex T_COLON hex T_COLON hex T_COLON hex { parserLogger->info("IPV6"); }
	;

userIdentifier:
	anythingList T_SPACE { parserLogger->info("userIdentifier"); parser->onUserIdentifier($1); }
	;

userId:
	anythingList T_SPACE { parserLogger->info("userIdentifier"); parser->onUserId($1); }
	;

timestamp:
	T_DASH T_SPACE { parserLogger->info("timestamp"); }
	| T_OPEN_TAB T_DIGIT T_SLASH T_ALPHA T_SLASH T_DIGIT T_COLON T_DIGIT T_COLON T_DIGIT T_COLON T_DIGIT T_SPACE T_PLUS T_DIGIT T_CLOSE_TAB T_SPACE { parserLogger->info("TIME"); delete $2; delete $4; delete $6; delete $8; delete $10; delete $15;}
	| T_OPEN_TAB T_DIGIT T_SLASH T_ALPHA T_SLASH T_DIGIT T_COLON T_DIGIT T_COLON T_DIGIT T_COLON T_DIGIT T_SPACE T_DASH T_DIGIT T_CLOSE_TAB T_SPACE { parserLogger->info("TIME"); delete $2; delete $4; delete $6; delete $8; delete $10; delete $15;}
	| error T_SPACE  { parserLogger->info("bad timestamp"); }
	;

request:
	T_DASH T_SPACE { parserLogger->info("request"); }
	| http_request_type url T_HTTP_1_0 T_SPACE {parserLogger->info("request"); parser->onRequest($1, $2, clf::httpVersion::httpV10);}
	| http_request_type url T_SLASH T_HTTP_1_0 T_SPACE {parserLogger->info("request"); parser->onRequest($1, $2, clf::httpVersion::httpV10);}
	| http_request_type T_SLASH T_HTTP_1_0 T_SPACE {parserLogger->info("request"); parser->onRequest($1, new std::string("/"), clf::httpVersion::httpV10);}
	| http_request_type url T_HTTP_1_1 T_SPACE {parserLogger->info("request"); parser->onRequest($1, $2, clf::httpVersion::httpV11);}
	| http_request_type url T_SLASH T_HTTP_1_1 T_SPACE {parserLogger->info("request"); parser->onRequest($1, $2, clf::httpVersion::httpV11);}
	| http_request_type T_SLASH T_HTTP_1_1 T_SPACE {parserLogger->info("request"); parser->onRequest($1, new std::string("/"), clf::httpVersion::httpV11);}
	;


error_code:
	T_DASH T_SPACE { parserLogger->info("error_code"); }
	| T_DIGIT T_SPACE { parserLogger->info("error_code"); parser->onErrorCode(boost::beast::http::int_to_status(std::stoul(*$1))); delete $1;};

size:
	T_DASH { parserLogger->info("no size"); }
	| T_DIGIT { parserLogger->info("size"); parser->onObjectSize(std::stoul(*$1)); delete $1;};

hex:
	T_ALPHANUM  { delete $1; }
	| T_DIGIT  { delete $1; }
	;

anythingListWithSpace:
	T_SPACE free_anything { }
	| anythingListWithSpace free_anything { }
	;

anythingList:
	anything { $$ = $1; }
	| anythingList anything { $1->append(*$2); delete $2; $$ = $1;}
	;


url:
	url_elem { $$ = $1;}
	| url url_elem { delete $2; $$ = $1;}
	;


url_elem:
	T_SLASH url_valid { $$ = $2;}
	;

url_valid:
	T_DASH { $$ = new std::string("-"); }
	| T_ALPHA {  $$ = $1; }
	| T_ALPHANUM  { $$ = $1; }
	| T_DIGIT  { $$ = $1; }
	| T_DOT	{ $$ = new std::string("."); }
	| T_PUNCT  { $$ = $1; }
	| T_QUOTE  { $$ = new std::string("\""); }
	| T_QUESTION  { $$ = new std::string("?"); }
	| T_TILDE  { $$ = new std::string("~"); }
	| T_PLUS  { $$ = new std::string("+"); }
	| T_EQUAL  { $$ = new std::string("="); }
	| url_valid T_DASH  { $1->append("-"); $$ = $1; }
	| url_valid T_ALPHA  { $1->append(*$2); delete $2; $$ = $1; }
	| url_valid T_ALPHANUM  { $1->append(*$2); delete $2; $$ = $1; }
	| url_valid T_DIGIT { $1->append(*$2); delete $2; $$ = $1; }
	| url_valid T_DOT { $1->append("."); $$ = $1;}
	| url_valid T_PUNCT { $1->append(*$2); delete $2; $$ = $1; }
	| url_valid T_QUOTE  { $1->append("\""); $$ = $1; }
	| url_valid T_QUESTION  { $1->append("?"); $$ = $1; }
	| url_valid T_TILDE  { $1->append("~"); $$ = $1; }
	| url_valid T_PLUS  { $1->append("+"); $$ = $1; }
	| url_valid T_EQUAL  { $1->append("="); $$ = $1; }
	;

http_request_type:
	T_HTTP_DELETE T_SPACE { $$ = boost::beast::http::verb::delete_; }
	| T_HTTP_GET T_SPACE { $$ = boost::beast::http::verb::get; }
	| T_HTTP_HEAD T_SPACE { $$ = boost::beast::http::verb::head; }
	| T_HTTP_POST T_SPACE { $$ = boost::beast::http::verb::post; }
	| T_HTTP_PUT T_SPACE { $$ = boost::beast::http::verb::put; }
	| T_HTTP_CONNECT T_SPACE { $$ = boost::beast::http::verb::connect; }
	| T_HTTP_OPTIONS T_SPACE { $$ = boost::beast::http::verb::options; }
	| T_HTTP_TRACE T_SPACE { $$ = boost::beast::http::verb::trace; }
	| T_HTTP_COPY T_SPACE { $$ = boost::beast::http::verb::copy; }
	| T_HTTP_LOCK T_SPACE { $$ = boost::beast::http::verb::lock; }
	| T_HTTP_MKCOL T_SPACE { $$ = boost::beast::http::verb::mkcol; }
	| T_HTTP_MOVE T_SPACE { $$ = boost::beast::http::verb::move; }
	| T_HTTP_PROPFIND T_SPACE { $$ = boost::beast::http::verb::propfind; }
	| T_HTTP_PROPPATCH T_SPACE { $$ = boost::beast::http::verb::proppatch; }
	| T_HTTP_SEARCH T_SPACE { $$ = boost::beast::http::verb::search; }
	| T_HTTP_UNLOCK T_SPACE { $$ = boost::beast::http::verb::unlock; }
	| T_HTTP_BIND T_SPACE { $$ = boost::beast::http::verb::bind; }
	| T_HTTP_REBIND T_SPACE { $$ = boost::beast::http::verb::rebind; }
	| T_HTTP_UNBIND T_SPACE { $$ = boost::beast::http::verb::unbind; }
	| T_HTTP_ACL T_SPACE { $$ = boost::beast::http::verb::acl; }
	| T_HTTP_REPORT T_SPACE { $$ = boost::beast::http::verb::report; }
	| T_HTTP_MKACTIVITY T_SPACE { $$ = boost::beast::http::verb::mkactivity; }
	| T_HTTP_CHECKOUT T_SPACE { $$ = boost::beast::http::verb::checkout; }
	| T_HTTP_MERGE T_SPACE { $$ = boost::beast::http::verb::merge; }
	| T_HTTP_MSEARCH T_SPACE { $$ = boost::beast::http::verb::msearch; }
	| T_HTTP_NOTIFY T_SPACE { $$ = boost::beast::http::verb::notify; }
	| T_HTTP_SUBSCRIBE T_SPACE { $$ = boost::beast::http::verb::subscribe; }
	| T_HTTP_UNSUBSCRIBE T_SPACE { $$ = boost::beast::http::verb::unsubscribe; }
	| T_HTTP_PATCH T_SPACE { $$ = boost::beast::http::verb::patch; }
	| T_HTTP_PURGE T_SPACE { $$ = boost::beast::http::verb::purge; }
	| T_HTTP_MKCALENDAR T_SPACE { $$ = boost::beast::http::verb::mkcalendar; }
	| T_HTTP_LINK T_SPACE { $$ = boost::beast::http::verb::link; }
	| T_HTTP_UNLINK T_SPACE { $$ = boost::beast::http::verb::unlink; }
	| error T_SPACE  { $$ = boost::beast::http::verb::unknown; }

anything:
	T_DOT { $$ = new std::string("."); }
	| T_DIGIT { $$ = $1; }
	| T_ALPHANUM { $$ = $1; }
	| T_COLON { $$ = new std::string(":"); }
	| T_DASH { $$ = new std::string("-"); }
	| T_ALPHA { $$ = $1; }
	| T_PUNCT { $$ = $1; }
	| T_OPEN_TAB { $$ = new std::string("["); }
	| T_CLOSE_TAB { $$ = new std::string("]"); }
	| T_PLUS { $$ = new std::string("+"); }
	| T_EQUAL { $$ = new std::string("="); }
	| T_SLASH { $$ = new std::string("/"); }
	| T_QUOTE { $$ = new std::string("\""); }
	| T_QUESTION { $$ = new std::string("?"); }
	| T_TILDE { $$ = new std::string("~"); }
	| http_request_type { $$ = new std::string("]"); }

free_anything:
	T_DOT
	| T_DIGIT { delete $1; }
	| T_ALPHANUM { delete $1; }
	| T_COLON
	| T_DASH
	| T_ALPHA { delete $1; }
	| T_PUNCT { delete $1; }
	| T_OPEN_TAB { }
	| T_CLOSE_TAB
	| T_PLUS
	| T_EQUAL
	| T_SLASH
	| T_QUOTE
	| T_QUESTION
	| T_TILDE
	| http_request_type
	| T_HTTP_DELETE
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
        | T_HTTP_PURGE
        | T_HTTP_MKCALENDAR
        | T_HTTP_LINK
        | T_HTTP_UNLINK
        | T_HTTP_1_0
        | T_HTTP_1_1

%%

int yywrap ( yyscan_t yyscanner ) {
	return 1;
}

static void yyerror(YYLTYPE *yy, yyscan_t scanner, clf::Parser *parser, const char * msg) {
}

std::optional<clf::ClfLine> clf::Parser::parseLine(std::string_view str) {
	reset();
	yyscan_t scanner;
	yylex_init(&scanner);
	yylex_init_extra(this, &scanner);

	yy_scan_bytes(str.data(), str.size(), scanner);

	int val = yyparse(scanner, this);
	yylex_destroy (scanner) ;

	if (val) {
		parserLogger->info("cannot parse {}", str);
		return nullopt;
	}


	return _currentLine;
}