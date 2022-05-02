main: Files.o
	g++ -std=c++11 -o wave Main.o WaveFileController.o ConsoleManager.o

Files.o: src/Main.cpp src/WaveFileController.hpp src/ConsoleManager.hpp WaveFileController.o 
	g++ -std=c++11 -c src/Main.cpp src/WaveFileController.cpp src/ConsoleManager.cpp

clean:
	rm *.o *.gch wave