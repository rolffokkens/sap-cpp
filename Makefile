all: sap1 sap2 stack1 stack2

sap1: sap1.o cpu1.o
	g++ -g sap1.o cpu1.o -o sap1

sap2: sap2.o cpu2.o
	g++ -g sap2.o cpu2.o -o sap2

sap1.o: sap1.cpp cpu1.h cpu.h ram.h register.h
	g++ -g sap1.cpp -c -o sap1.o

sap2.o: sap2.cpp cpu1.h cpu.h ram.h register.h
	g++ -g sap2.cpp -c -o sap2.o

cpu1.o: cpu1.cpp cpu1.h cpu.h ram.h register.h
	g++ -g cpu1.cpp -c -o cpu1.o

cpu2.o: cpu2.cpp cpu2.h cpu.h ram.h register.h
	g++ -g cpu2.cpp -c -o cpu2.o

stack1: stack1.o cpu16.o register16.o ram8.o
	g++ -g stack1.o cpu16.o register16.o ram8.o -o stack1

stack2: stack2.o cpu16.o register16.o ram8.o
	g++ -g stack2.o cpu16.o register16.o ram8.o -o stack2

cpu16.o: cpu16.cpp cpu16.h ram8.h register16.h internalbus16.h
	g++ -g cpu16.cpp -c -o cpu16.o

register16.o: register16.cpp register16.h internalbus16.h
	g++ -g register16.cpp -c -o register16.o

ram8.o: ram8.cpp ram8.h register16.h internalbus16.h
	g++ -g ram8.cpp -c -o ram8.o
