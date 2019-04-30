CC = arm-none-linux-gnueabi-gcc -pthread -static

app.out : main.o input.o output.o
	$(CC) -o app.out main.o input.o output.o

main.o : main.c
	$(CC) -c -o main.o main.c

input.o : input.c
	$(CC) -c -o input.o input.c

output.o : output.c
	$(CC) -c -o output.o output.c


clean :
	rm -f main.o input.o output.o app.out
