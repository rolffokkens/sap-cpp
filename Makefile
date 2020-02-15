all: sap1 sap2

sap1: sap1.o cpu1.o
	g++ -g sap1.o cpu1.o -o sap1

sap2: sap2.o cpu2.o
	g++ -g sap2.o cpu2.o -o sap2

sap.o: sap.cpp cpu1.h cpu2.h
	g++ -g sap.cpp -c -o sap.o

cpu1.o: cpu1.cpp cpu1.h cpu.h ram.h register.h
	g++ -g cpu1.cpp -c -o cpu1.o

cpu2.o: cpu2.cpp cpu2.h cpu.h ram.h register.h
	g++ -g cpu2.cpp -c -o cpu2.o
