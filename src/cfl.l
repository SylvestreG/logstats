%{
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include "parser.h"
#include "cflParser.hpp"

/*std::shared_ptr<spdlog::logger> lexerLogger =
    spdlog::basic_logger_mt("lexer", "lexer.log");*/
std::shared_ptr<spdlog::logger> lexerLogger =
    spdlog::stderr_color_mt("lexer");
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
-                           {lexerLogger->warn("T_DASH"); return T_DASH;}
\/                          {lexerLogger->warn("T_SLASH"); return T_SLASH;}
:                           {lexerLogger->warn("T_COLON"); return T_COLON;}
\"                          {lexerLogger->warn("T_QUOTE"); return T_QUOTE;}
[[:space:]]HTTP\/1.0\"       {lexerLogger->warn("T_HTTP_1_0"); return T_HTTP_1_0;}
[[:space:]]HTTP\/1.1\"       {lexerLogger->warn("T_HTTP_1_1"); return T_HTTP_1_1;}
\"DELETE[[:space:]]         {lexerLogger->warn("T_HTTP_DELETE"); return T_HTTP_DELETE;}
\"GET[[:space:]]            {lexerLogger->warn("T_HTTP_GET"); return T_HTTP_GET;}
\"HEAD[[:space:]]           {lexerLogger->warn("T_HTTP_HEAD"); return T_HTTP_HEAD;}
\"POST[[:space:]]           {lexerLogger->warn("T_HTTP_POST"); return T_HTTP_POST;}
\"PUT[[:space:]]            {lexerLogger->warn("T_HTTP_PUT"); return T_HTTP_PUT;}
\"CONNECT[[:space:]]        {lexerLogger->warn("T_HTTP_CONNECT"); return T_HTTP_CONNECT;}
\"OPTIONS[[:space:]]        {lexerLogger->warn("T_HTTP_OPTIONS"); return T_HTTP_OPTIONS;}
\"TRACE[[:space:]]          {lexerLogger->warn("T_HTTP_TRACE"); return T_HTTP_TRACE;}
\"COPY[[:space:]]           {lexerLogger->warn("T_HTTP_COPY"); return T_HTTP_COPY;}
\"LOCK[[:space:]]           {lexerLogger->warn("T_HTTP_LOCK"); return T_HTTP_LOCK;}
\"MKCOL[[:space:]]          {lexerLogger->warn("T_HTTP_MKCOL"); return T_HTTP_MKCOL;}
\"MOVE[[:space:]]           {lexerLogger->warn("T_HTTP_MOVE"); return T_HTTP_MOVE;}
\"PROPFIND[[:space:]]       {lexerLogger->warn("T_HTTP_PROPFIND"); return T_HTTP_PROPFIND;}
\"PROPPATCH[[:space:]]      {lexerLogger->warn("T_HTTP_PROPPATCH"); return T_HTTP_PROPPATCH;}
\"SEARCH[[:space:]]         {lexerLogger->warn("T_HTTP_SEARCH"); return T_HTTP_SEARCH;}
\"UNLOCK[[:space:]]         {lexerLogger->warn("T_HTTP_UNLOCK"); return T_HTTP_UNLOCK;}
\"BIND[[:space:]]           {lexerLogger->warn("T_HTTP_BIND"); return T_HTTP_BIND;}
\"REBIND[[:space:]]         {lexerLogger->warn("T_HTTP_REBIND"); return T_HTTP_REBIND;}
\"UNBIND[[:space:]]         {lexerLogger->warn("T_HTTP_UNBIND"); return T_HTTP_UNBIND;}
\"ACL[[:space:]]            {lexerLogger->warn("T_HTTP_ACL"); return T_HTTP_ACL;}
\"REPORT[[:space:]]         {lexerLogger->warn("T_HTTP_REPORT"); return T_HTTP_REPORT;}
\"MKACTIVITY[[:space:]]     {lexerLogger->warn("T_HTTP_MKACTIVITY"); return T_HTTP_MKACTIVITY;}
\"CHECKOUT[[:space:]]       {lexerLogger->warn("T_HTTP_CHECKOUT"); return T_HTTP_CHECKOUT;}
\"MERGE[[:space:]]          {lexerLogger->warn("T_HTTP_MERGE"); return T_HTTP_MERGE;}
\"MSEARCH[[:space:]]        {lexerLogger->warn("T_HTTP_MSEARCH"); return T_HTTP_MSEARCH;}
\"NOTIFY[[:space:]]         {lexerLogger->warn("T_HTTP_NOTIFY"); return T_HTTP_NOTIFY;}
\"SUBSCRIBE[[:space:]]      {lexerLogger->warn("T_HTTP_SUBSCRIBE"); return T_HTTP_SUBSCRIBE;}
\"UNSUBSCRIBE[[:space:]]    {lexerLogger->warn("T_HTTP_UNSUBSCRIBE"); return T_HTTP_UNSUBSCRIBE;}
\"PATCH[[:space:]]          {lexerLogger->warn("T_HTTP_PATCH"); return T_HTTP_PATCH;}
\"URGE[[:space:]]           {lexerLogger->warn("T_HTTP_URGE"); return T_HTTP_URGE;}
\"MKCALENDAR[[:space:]]     {lexerLogger->warn("T_HTTP_MKCALENDAR"); return T_HTTP_MKCALENDAR;}
\"LINK[[:space:]]           {lexerLogger->warn("T_HTTP_LINK"); return T_HTTP_LINK;}
\"UNLINK[[:space:]]         {lexerLogger->warn("T_HTTP_UNLINK"); return T_HTTP_UNLINK;}
[ \t]+                      {lexerLogger->warn("T_SPACE"); return T_SPACE;};
[[:digit:]]+                {lexerLogger->warn("T_DIGIT"); return T_DIGIT;}
[[:xdigit:]]+               {lexerLogger->warn("T_ALPHANUM"); return T_ALPHANUM;}
[[:alpha:]]+                {lexerLogger->warn("T_ALPHA"); return T_ALPHA;}
[[:punct:]]+                {lexerLogger->warn("T_PUNCT"); return T_PUNCT;}
\n                          {lexerLogger->warn("T_NEW_LINE"); return T_NEW_LINE;}
<IN_EOF><<EOF>>             {return 0;}
<<EOF>>                     {lexerLogger->warn("T_EOF"); BEGIN(IN_EOF); return T_EOF;}
%%