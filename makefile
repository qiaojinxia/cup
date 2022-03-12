build:
	cd ./cmake-build-debug/ && make
	@echo "calc express $(express)"
	cd ./cmake-build-debug/ && ./boddy "$(express)" > tmp.s
	clang ./cmake-build-debug/tmp.s ./src/driver.c -o tmp.out
	tmp.out
	rm -rf tmp.out tmp.s
