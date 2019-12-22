target = build/Gen-x86_64
# target = cmake-build-debug/Semantic
flags = --tree


init:
	ln -s -f ./$(target)	./limc

ss:
	@./$(target) ./test/ss.lc ./tmp/a.s
	@make runasm

fact:
	@./$(target) ./test/fact.lc ./tmp/a.s
	@make runasm

qs:
	@./$(target) ./test/qs.lc ./tmp/a.s
	@make runasm

arr:
	@./$(target) ./test/arr.lc ./tmp/a.s
	@make runasm

loop:
	@./$(target) ./test/loop.lc ./tmp/a.s
	@make runasm

fp:
	@./$(target) ./test/fp.lc ./tmp/a.s
	@make runasm

runasm:
	@gcc -o ./tmp/a.out ./tmp/a.s
	@./tmp/a.out

clean:
	rm -rf ./build ./tmp/*