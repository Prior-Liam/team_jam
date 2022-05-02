CC = g++
CFLAGS = -std=c++11

main: Main.o WaveFileController.o ConsoleManager.o
	$(CC) $(CFLAGS) -o wave -c src/Main.o src/WaveFileController.o src/ConsoleManager.o

Main.o: src/Main.cpp src/WaveFileController.hpp src/ConsoleManager.hpp
	$(CC) $(CFLAGS) -c src/Main.cpp src/WaveFileController.cpp src/ConsoleManager.cpp
 
WaveFileController.o: src/WaveFileController.hpp
 
ConsoleManager.o: src/ConsoleManager.hpp src/WaveFileController.hpp

clean:
	rm *.o *.gch wave