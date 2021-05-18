all: control

control: main.o controlSystem.o
	gcc main.o controlSystem.o -o control

main.o: main.c controlSystem.h
	gcc -c main.c

controlSystem.o: controlSystem.c controlSystem.h
	gcc -c controlSystem.c

clean:
	rm *.o control