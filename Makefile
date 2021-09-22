all: *.c
	gcc -o dummy_ai *.c
clean:
	rm dummy_ai
