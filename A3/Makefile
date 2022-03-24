mysh: shell.c interpreter.c shellmemory.c scheduler.c helper.c
	gcc -g -c shell.c interpreter.c shellmemory.c scheduler.c helper.c
	gcc -g -o mysh shell.o interpreter.o shellmemory.o scheduler.o helper.o

clean: 
	rm mysh; rm *.o
