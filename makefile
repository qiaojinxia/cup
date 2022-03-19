program:
	cd ./cmake-build-debug/ && make
	@echo "compailer code $(code)"
	cd ./cmake-build-debug/ && ./boddy "code" "$(code)" > tmp.s
	clang ./cmake-build-debug/tmp.s ./src/driver.c -o tmp.out
	@echo ">>>>>>>>>" && tmp.out
	rm -rf tmp.out ./cmake-build-debug/tmp.s
programPath:
	cd ./cmake-build-debug/ && make
	@echo "compailer code $(code)"
	cd ./cmake-build-debug/ && ./boddy "path" "$(code)" > tmp.s
	clang ./cmake-build-debug/tmp.s ./src/driver.c -o tmp.out
	@echo ">>>>>>>>>" && tmp.out
	rm -rf tmp.out ./cmake-build-debug/tmp.s
outsource:
	cd ./cmake-build-debug/ && make
	@echo "compailer code $(code)"
	cd ./cmake-build-debug/ && ./boddy "$(code)" > tmp.s
	clang ./cmake-build-debug/tmp.s ./src/driver.c -o tmp.out