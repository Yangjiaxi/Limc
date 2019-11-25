target = output/exec.out

s0 = src/S.0-Utils
s1 = src/S.1-Scanner
s2 = src/S.2-Parser

all:
	flex -o $(s1)/scanner.cpp $(s1)/scanner.l
	bison -v -o $(s2)/parser.cpp $(s2)/parser.y
	g++ -std=c++11 -w -pipe -g \
		src/main.cpp \
		$(s1)/scanner.cpp \
		$(s2)/parser.cpp \
		$(s0)/driver.cpp $(s0)/token.cpp \
		-o $(target)

gen:
	flex -o $(s1)/scanner.cpp $(s1)/scanner.l
	bison -v -o $(s2)/parser.cpp $(s2)/parser.y

gcc:
	g++ -std=c++11 -w -pipe -g \
		src/main.cpp \
		$(s1)/scanner.cpp \
		$(s2)/parser.cpp \
		$(s0)/driver.cpp $(s0)/token.cpp \
		-o $(target)

clean:
	rm -rf $(s1)/scanner.cpp \
		$(s2)/parser.cpp $(s2)/parser.hpp $(s2)/parser.output\
		$(s2)/location.hh $(s2)/position.hh $(s2)/stack.hh \
		$(target)

fibo:
	./$(target)  ./test/fibo.c

ss:
	./$(target)  ./test/simple.c

cmt:
	./$(target)  ./test/comment.c

copy:
	./$(target)  ./test/copy.c