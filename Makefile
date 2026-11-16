CC ?= cc
CFLAGS ?= -std=c99 -Wall -Wextra -Werror -pedantic
.PHONY: test clean

test: game_logic_test
	./game_logic_test

game_logic_test: src/game_logic.c src/game_logic.h tests/test_game_logic.c
	$(CC) $(CFLAGS) -Isrc src/game_logic.c tests/test_game_logic.c -o $@

clean:
	rm -f game_logic_test
