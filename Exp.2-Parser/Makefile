all:
	flex -o src/scanner.cpp generate/scanner.l
	bison -v -o src/parser.cpp generate/parser.y
	g++ -std=c++11 -w -pipe -g src/main.cpp src/scanner.cpp src/parser.cpp src/driver.cpp src/token.cpp -o output/compiler

gen:
	flex -o src/scanner.cpp generate/scanner.l
	bison -v -o src/parser.cpp generate/parser.y

gcc:
	g++ -std=c++11 -w -pipe -g src/main.cpp \
		src/scanner.cpp \
		src/parser.cpp \
		src/driver.cpp \
		src/token.cpp \
		-o output/compiler

clean:
	rm -rf src/scanner.cpp \
		src/parser.cpp src/parser.hpp \
		src/parser.output \
		src/location.hh src/position.hh src/stack.hh \
		output/compiler

fibo:
	./output/compiler  ./test/fibo.c

ss:
	./output/compiler  ./test/simple.c

cmt:
	./output/compiler  ./test/comment.c

copy:
	./output/compiler  ./test/copy.c