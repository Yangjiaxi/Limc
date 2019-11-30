target = build/Semantic

init:
	ln -s -f ./$(target)	./entry

fibo:
	./$(target)	-f ./test/fibo.c	-t

ss:
	./$(target)	-f ./test/simple.c	-t

cmt:
	./$(target) -f ./test/comment.c	-t

copy:
	./$(target) -f ./test/copy.c	-t

clean:
	rm -rf ./build