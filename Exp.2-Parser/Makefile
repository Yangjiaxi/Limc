all:
	flex -o src/scanner.cpp generate/scanner.l
	bison -v -o src/parser.cpp generate/parser.y
	g++ -std=c++11 -w -pipe -g src/main.cpp src/scanner.cpp src/parser.cpp src/driver.cpp src/token.cpp -o output/exec.out

gen:
	flex -o src/scanner.cpp generate/scanner.l
	bison -v -o src/parser.cpp generate/parser.y

gcc:
	g++ -std=c++11 -w -pipe -g src/main.cpp \
		src/scanner.cpp \
		src/parser.cpp \
		src/driver.cpp \
		src/token.cpp \
		-o output/exec.out

clean:
	rm -rf src/scanner.cpp \
		src/parser.cpp src/parser.hpp \
		src/parser.output \
		src/location.hh src/position.hh src/stack.hh \
		output/exec.out

fibo:
	./output/exec.out  ./test/fibo.c

ss:
	./output/exec.out  ./test/simple.c

cmt:
	./output/exec.out  ./test/comment.c

copy:
	./output/compiler  ./test/copy.c