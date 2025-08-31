# Fuse

Fuse is a lightweight, dependency-free engine built in pure C11. It features a custom shader parser and can automatically generate Vulkan-compatible C code from shader definitions, streamlining graphics pipeline integration. Designed with simplicity and portability in mind, it provides a solid foundation for experimenting with Vulkan while remaining minimal and easy to extend.

```
flex --header-file="lexer.h" --outfile="lexer.c" --nounistd --noyywrap --yylineno "lexer.l"
bison --header="parser.h" --output="parser.c" --locations "parser.y"
```

```
https://www.shadertoy.com/view/wt2Gzc
```

```
 TODO: reformat GLSL code
```