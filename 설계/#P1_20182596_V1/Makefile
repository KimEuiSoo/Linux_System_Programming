ssu_backup: ssu_backup.o recover.o add.o
	gcc ssu_backup.o -o ssu_backup -lssl -lcrypto
	gcc add.o -o add -lssl -lcrypto
	gcc recover.o -o recover

ssu_backup.o: ssu_backup.c
	gcc -c ssu_backup.c

recover.o: recover.c
	gcc -c recover.c

add.o: add.c
	gcc -c add.c

clean :

