all: client.c serverM.c serverCred.c serverCS.c serverEE.c
		gcc -o client client.c
		gcc -o server serverM.c
		gcc -o serverCred serverCred.c
		gcc -o serverCS serverCS.c
		gcc -o serverEE serverEE.c -lm  #-lm is to compile math.h used for rounding

server:
		./server

serverCred:
		./serverCred

serverCS:
		./serverCS

serverEE:
		./serverEE

.PHONY: server serverCred serverCS serverEE
