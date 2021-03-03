# building and launch

# configuration

# architecture

## parsing

For parsing I am using a flex/bison parser. I try to produce a grammar for
the common log format.

This parser is intended to parser only one line at a time.

The lex/yacc parser backend create a push parser using the cfl::parser object.

### Limitations

When some part of the message are not handled by the grammar we try to drop just this
part of the data.

For ip address we only handle for now this kind of addr :
- valid ipv4 (xxx.xxx.xxx.xxx)
- valid ipv6 of full length
