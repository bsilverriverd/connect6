all: *.c
	gcc -o dummy_ai *.c
debug: *.c
	gcc -o dummy_ai *.c -DDEBUG
clean:
	rm dummy_ai
