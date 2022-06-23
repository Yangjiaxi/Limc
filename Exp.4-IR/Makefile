target = build/GenIR
# target = cmake-build-debug/GenIR
flags = --tree


init:
	ln -s -f ./$(target)	./limc

ss:
	@./$(target) ./test/simple.lc

fact:
	@./$(target) ./test/fact.lc

qs:
	@./$(target) ./test/qs.lc

arr:
	@./$(target) ./test/arr.lc

loop:
	@./$(target) ./test/loop.lc

fp:
	@./$(target) ./test/fp.lc

clean:
	rm -rf ./build