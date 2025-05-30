parser:
	bison -d -o lib/parse.tab.c lib/parse.y	

lexer:
	flex -o lib/lex.yy.c lib/lex.y

all:
	$(MAKE) parser
	$(MAKE) lexer
	g++ -o sql_parser main.cpp lib/parse.tab.c lib/lex.yy.c -I/opt/homebrew/opt/flex/include