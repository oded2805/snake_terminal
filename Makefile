all:
	gcc -Wall -Wextra -std=c17 snake.c -o snake

clean:
	rm -f snake