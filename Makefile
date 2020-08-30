all: sap1 sap2 stack1 stack2 stack2a stack2b stack2c stack2d

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

cpu16.o: cpu16.cpp cpu16.h ram8.h register16.h internalbus16.h
	g++ -g cpu16.cpp -c -o cpu16.o

cpu16a.o: cpu16a.cpp cpu16a.h ram8.h register16.h internalbus16.h
	g++ -g cpu16a.cpp -c -o cpu16a.o

cpu16b.o: cpu16b.cpp cpu16b.h ram8.h register16.h internalbus16.h
	g++ -g cpu16b.cpp -c -o cpu16b.o

cpu16c.o: cpu16c.cpp cpu16c.h ram8.h register16.h internalbus16.h
	g++ -g cpu16c.cpp -c -o cpu16c.o

cpu16d.o: cpu16d.cpp cpu16d.h ram8.h register16.h internalbus16.h
	g++ -g cpu16d.cpp -c -o cpu16d.o

register16.o: register16.cpp register16.h internalbus16.h
	g++ -g register16.cpp -c -o register16.o

ram8.o: ram8.cpp ram8.h register16.h internalbus16.h
	g++ -g ram8.cpp -c -o ram8.o

stack1: stack1.o cpu16.o register16.o ram8.o
	g++ -g stack1.o cpu16.o register16.o ram8.o -o stack1

stack2: stack2.o cpu16.o register16.o ram8.o
	g++ -g stack2.o cpu16.o register16.o ram8.o -o stack2

stack2a: stack2a.o cpu16a.o register16.o ram8.o
	g++ -g stack2a.o cpu16a.o register16.o ram8.o -o stack2a

stack2b: stack2b.o cpu16b.o register16.o ram8.o
	g++ -g stack2b.o cpu16b.o register16.o ram8.o -o stack2b

stack2c: stack2c.o cpu16c.o register16.o ram8.o
	g++ -g stack2c.o cpu16c.o register16.o ram8.o -o stack2c

stack2d: main16d.o cpu16d.o register16.o ram8.o
	g++ -g main16d.o cpu16d.o register16.o ram8.o -lboost_program_options -o stack2d
