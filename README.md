# Fuse

Fuse is a lightweight, dependency-free engine built in pure C11. It features a custom shader parser and can automatically generate Vulkan-compatible C code from shader definitions, streamlining graphics pipeline integration. Designed with simplicity and portability in mind, it provides a solid foundation for experimenting with Vulkan while remaining minimal and easy to extend.

```
flex --outfile="lexer.c" --yylineno --nounistd "lexer.l"
bison --header="parser.h" --output="parser.c" --locations "parser.y"
```
