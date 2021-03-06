
build:
	gcc main.c ttf.c sdf.c -Wpadded -lm -g

build_dump:
	gcc ttfdump.c -o ttfdump.out

run:
	./a.out examples/mem/mem-prop-5x6.ttf

build_run: build run


debug:
	gdb --args ./a.out examples/mem/mem-prop-5x6.ttf

help: ## help
	@grep -E '(^##)|(\s##)' $(MAKEFILE_LIST) | awk 'BEGIN {FS = "(:.*?)?## "}; {if ($$1) { printf "  %-30s %s\n", $$1, $$2 } else { printf "\n%s\n", $$2 }}'
