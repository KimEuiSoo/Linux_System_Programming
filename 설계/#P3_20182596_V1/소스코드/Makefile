ssu_monitor: ssu_monitor.o help.o add.o
	gcc ssu_monitor.o -o ssu_monitor
	gcc help.o -o help
	gcc add.o -o add

ssu_monitor.o: ssu_monitor.c ssu_monitor.h
	gcc -c ssu_monitor.c

help.o: help.c
	gcc -c help.c

add.o: add.c
	gcc -c add.c

clean :
	rm *.o
	rm ssu_score
