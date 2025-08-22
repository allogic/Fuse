# fuse-engine

```
flex --outfile="lexer.c" --yylineno --nounistd "lexer.l"
bison --header="parser.h" --output="parser.c" --locations "parser.y"
```