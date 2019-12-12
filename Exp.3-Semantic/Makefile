# target = build/Semantic
target = cmake-build-debug/Semantic
flags = --tree


init:
	ln -s -f ./$(target)	./entry

fibo:
	./$(target)	-f ./test/fibo.c	$(flags)

ss:
	./$(target)	-f ./test/simple.c	$(flags)

cmt:
	./$(target) -f ./test/comment.c	$(flags)

copy:
	./$(target) -f ./test/copy.c	$(flags)

clean:
	rm -rf ./build