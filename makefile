program:
	cd ./cmake-build-debug/ && make
	@echo "compailer code $(code)"
	cd ./cmake-build-debug/ && ./boddy "code" "$(code)" > tmp.s
	clang ./cmake-build-debug/tmp.s ./src/driver.c -o tmp.out
	@echo ">>>>>>>>>" && tmp.out
	rm -rf tmp.out ./cmake-build-debug/tmp.s
programPath:
	cd ./cmake-build-debug/ && make
	@echo "compailer code path:$(path)"
	cd ./cmake-build-debug/ && ./boddy "path" "$(path)" > tmp.s
	clang ./cmake-build-debug/tmp.s ./src/driver.c -o tmp.out
	@echo ">>>>>>>>>" && tmp.out
	rm -rf tmp.out ./cmake-build-debug/tmp.s
testCode:
	cd ./cmake-build-debug/ && make
	cd ./cmake-build-debug/ && ./boddy "path" "../test/testCodes.c" > tmp.s
	clang ./cmake-build-debug/tmp.s ./test/main.c ./test/assert.c  -o tmp.out
	./tmp.out