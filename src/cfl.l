%{
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include "parser.h"
#include "cflParser.hpp"

std::shared_ptr<spdlog::logger> lexerLogger =
    spdlog::basic_logger_mt("lexer", "lexer.log");
%}

%option reentrant bison-bridge bison-locations
%option fast
%option ecs
%option stack
%option case-insensitive

%x IN_EOF

%%

\.                          {lexerLogger->warn("T_DOT"); return T_DOT; }
\[                          {lexerLogger->warn("T_OPEN_TAB"); return T_OPEN_TAB;}
\]                          {lexerLogger->warn("T_CLOSE_TAB"); return T_CLOSE_TAB;}
\+                          {lexerLogger->warn("T_PLUS"); return T_PLUS;}
\=                          {lexerLogger->warn("T_EQUAL"); return T_EQUAL;}
-                           {lexerLogger->warn("T_DASH"); return T_DASH;}
\/                          {lexerLogger->warn("T_SLASH"); return T_SLASH;}
:                           {lexerLogger->warn("T_COLON"); return T_COLON;}
\"                          {lexerLogger->warn("T_QUOTE"); return T_QUOTE;}
\?                          {lexerLogger->warn("T_QUESTION"); return T_QUESTION;}
\~                          {lexerLogger->warn("T_TILDE"); return T_TILDE;}
[[:space:]]HTTP\/1.0\"      {lexerLogger->warn("T_HTTP_1_0"); return T_HTTP_1_0;}
[[:space:]]HTTP\/1.1\"      {lexerLogger->warn("T_HTTP_1_1"); return T_HTTP_1_1;}
\"DELETE                    {lexerLogger->warn("T_HTTP_DELETE"); yylval->requestType = boost::beast::http::verb::delete_; return T_HTTP_DELETE;}
\"GET                       {lexerLogger->warn("T_HTTP_GET"); yylval->requestType = boost::beast::http::verb::get; return T_HTTP_GET;}
\"HEAD                      {lexerLogger->warn("T_HTTP_HEAD"); yylval->requestType = boost::beast::http::verb::head; return T_HTTP_HEAD;}
\"POST                      {lexerLogger->warn("T_HTTP_POST"); yylval->requestType = boost::beast::http::verb::post; return T_HTTP_POST;}
\"PUT                       {lexerLogger->warn("T_HTTP_PUT"); yylval->requestType = boost::beast::http::verb::put; return T_HTTP_PUT;}
\"CONNECT                   {lexerLogger->warn("T_HTTP_CONNECT"); yylval->requestType = boost::beast::http::verb::connect; return T_HTTP_CONNECT;}
\"OPTIONS                   {lexerLogger->warn("T_HTTP_OPTIONS"); yylval->requestType = boost::beast::http::verb::options; return T_HTTP_OPTIONS;}
\"TRACE                     {lexerLogger->warn("T_HTTP_TRACE"); yylval->requestType = boost::beast::http::verb::trace; return T_HTTP_TRACE;}
\"COPY                      {lexerLogger->warn("T_HTTP_COPY"); yylval->requestType = boost::beast::http::verb::copy; return T_HTTP_COPY;}
\"LOCK                      {lexerLogger->warn("T_HTTP_LOCK"); yylval->requestType = boost::beast::http::verb::lock; return T_HTTP_LOCK;}
\"MKCOL                     {lexerLogger->warn("T_HTTP_MKCOL"); yylval->requestType = boost::beast::http::verb::mkcol; return T_HTTP_MKCOL;}
\"MOVE                      {lexerLogger->warn("T_HTTP_MOVE"); yylval->requestType = boost::beast::http::verb::move; return T_HTTP_MOVE;}
\"PROPFIND                  {lexerLogger->warn("T_HTTP_PROPFIND"); yylval->requestType = boost::beast::http::verb::propfind; return T_HTTP_PROPFIND;}
\"PROPPATCH                 {lexerLogger->warn("T_HTTP_PROPPATCH"); yylval->requestType = boost::beast::http::verb::proppatch; return T_HTTP_PROPPATCH;}
\"SEARCH                    {lexerLogger->warn("T_HTTP_SEARCH"); yylval->requestType = boost::beast::http::verb::search; return T_HTTP_SEARCH;}
\"UNLOCK                    {lexerLogger->warn("T_HTTP_UNLOCK"); yylval->requestType = boost::beast::http::verb::unlock; return T_HTTP_UNLOCK;}
\"BIND                      {lexerLogger->warn("T_HTTP_BIND"); yylval->requestType = boost::beast::http::verb::bind; return T_HTTP_BIND;}
\"REBIND                    {lexerLogger->warn("T_HTTP_REBIND"); yylval->requestType = boost::beast::http::verb::rebind; return T_HTTP_REBIND;}
\"UNBIND                    {lexerLogger->warn("T_HTTP_UNBIND"); yylval->requestType = boost::beast::http::verb::unbind; return T_HTTP_UNBIND;}
\"ACL                       {lexerLogger->warn("T_HTTP_ACL"); yylval->requestType = boost::beast::http::verb::acl; return T_HTTP_ACL;}
\"REPORT                    {lexerLogger->warn("T_HTTP_REPORT"); yylval->requestType = boost::beast::http::verb::report; return T_HTTP_REPORT;}
\"MKACTIVITY                {lexerLogger->warn("T_HTTP_MKACTIVITY"); yylval->requestType = boost::beast::http::verb::mkactivity; return T_HTTP_MKACTIVITY;}
\"CHECKOUT                  {lexerLogger->warn("T_HTTP_CHECKOUT"); yylval->requestType = boost::beast::http::verb::checkout; return T_HTTP_CHECKOUT;}
\"MERGE                     {lexerLogger->warn("T_HTTP_MERGE"); yylval->requestType = boost::beast::http::verb::merge; return T_HTTP_MERGE;}
\"MSEARCH                   {lexerLogger->warn("T_HTTP_MSEARCH"); yylval->requestType = boost::beast::http::verb::msearch; return T_HTTP_MSEARCH;}
\"NOTIFY                    {lexerLogger->warn("T_HTTP_NOTIFY"); yylval->requestType = boost::beast::http::verb::notify; return T_HTTP_NOTIFY;}
\"SUBSCRIBE                 {lexerLogger->warn("T_HTTP_SUBSCRIBE"); yylval->requestType = boost::beast::http::verb::subscribe; return T_HTTP_SUBSCRIBE;}
\"UNSUBSCRIBE               {lexerLogger->warn("T_HTTP_UNSUBSCRIBE"); yylval->requestType = boost::beast::http::verb::unsubscribe; return T_HTTP_UNSUBSCRIBE;}
\"PATCH                     {lexerLogger->warn("T_HTTP_PATCH"); yylval->requestType = boost::beast::http::verb::patch; return T_HTTP_PATCH;}
\"PURGE                     {lexerLogger->warn("T_HTTP_PURGE"); yylval->requestType = boost::beast::http::verb::purge; return T_HTTP_PURGE;}
\"MKCALENDAR                {lexerLogger->warn("T_HTTP_MKCALENDAR"); yylval->requestType = boost::beast::http::verb::link; return T_HTTP_MKCALENDAR;}
\"LINK                      {lexerLogger->warn("T_HTTP_LINK"); yylval->requestType = boost::beast::http::verb::link; return T_HTTP_LINK;}
\"UNLINK                    {lexerLogger->warn("T_HTTP_UNLINK"); yylval->requestType = boost::beast::http::verb::unlink; return T_HTTP_UNLINK;}
[ \t]+                      {lexerLogger->warn("T_SPACE"); return T_SPACE;};
[[:digit:]]+                {lexerLogger->warn("T_DIGIT"); yylval->str = new std::string(yytext); return T_DIGIT;}
[[:xdigit:]]+               {lexerLogger->warn("T_ALPHANUM"); yylval->str = new std::string(yytext); return T_ALPHANUM;}
[[:alpha:]]+                {lexerLogger->warn("T_ALPHA"); yylval->str = new std::string(yytext); return T_ALPHA;}
[[:punct:]]+                {lexerLogger->warn("T_PUNCT"); yylval->str = new std::string(yytext); return T_PUNCT;}
\n                          {lexerLogger->warn("T_NEW_LINE"); return T_NEW_LINE;}
<IN_EOF><<EOF>>             {return 0;}
<<EOF>>                     {lexerLogger->warn("T_EOF"); BEGIN(IN_EOF); return T_EOF;}
%%