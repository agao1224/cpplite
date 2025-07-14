parser:
	bison -d -o lib/parse.tab.c lib/parse.y	

lexer:
	flex -o lib/lex.yy.c lib/lex.y

test:
	cmake -S . -B build
	cmake --build build
	ctest --test-dir build

test_verbose:
	cmake -S . -B build
	cmake --build build
	ctest --test-dir build --output-on-failure

all:
	$(MAKE) parser
	$(MAKE) lexer
	g++ -o sql_parser main.cpp lib/parse.tab.c lib/lex.yy.c -I/opt/homebrew/opt/flex/include