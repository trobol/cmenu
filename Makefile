
build:
	gcc main.c ttf.c -Wpadded

build_dump:
	gcc ttfdump.c -o ttfdump.out

help: ## help
	@grep -E '(^##)|(\s##)' $(MAKEFILE_LIST) | awk 'BEGIN {FS = "(:.*?)?## "}; {if ($$1) { printf "  %-30s %s\n", $$1, $$2 } else { printf "\n%s\n", $$2 }}'
