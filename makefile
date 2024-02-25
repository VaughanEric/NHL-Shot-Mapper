CFLAGS ?= -std=c++11 -Wall -Wextra -Iinclude

all: nsm

main.o: main.cpp nsm.h
	g++ $(CFLAGS) -c -o main.o main.cpp

nsm.o: nsm.cpp nsm.h
	g++ $(CFLAGS) -c -o nsm.o nsm.cpp

nsm: main.o nsm.o
	g++ $(CFLAGS) -o nsm main.o nsm.o

clean:
	rm -f nsm *.o
