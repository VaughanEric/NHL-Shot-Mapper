CFLAGS ?= -std=c++11 -Wall -Wextra -Iinclude

all: nsm

main.o: main.cpp nsm.h
	g++ $(CFLAGS) -c -o main.o main.cpp

nsm.o: nsm.cpp nsm.h
	g++ $(CFLAGS) -c -o nsm.o nsm.cpp

nsm: main.o nsm.o
	g++ $(CFLAGS) -o nsm main.o nsm.o
	mkdir examples
	./nsm NSH 2017-06-03 examples/Nashville-Predators-2017-06-03
	./nsm NSH 2016-05-05 examples/Nashville-Predators-2016-05-05
	./nsm NYR 2024-02-18 examples/New-York-Rangers-2024-02-18
	./nsm SEA 2022-11-29 examples/Seattle-Kraken-2022-11-29
	./nsm VGK 2023-06-13 examples/Vegas-Golden-Knights-2023-06-13

clean:
	rm -rf nsm *.o examples

