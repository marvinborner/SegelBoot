CC = xelatex
BIB = biber
VIEW = zathura

all:
	@mkdir -p build/
	@$(CC) --output-directory=build/ --shell-escape main

full: clean
	@mkdir -p build/
	@$(CC) --output-directory=build/ --shell-escape main
	@$(BIB) --input-directory=build/ main
	@$(CC) --output-directory=build/ --shell-escape main

clean:
	@$(RM) -rf build

run: clean all
	@$(VIEW) build/main.pdf
