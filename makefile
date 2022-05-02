main: Main.o WaveFileController.o ConsoleManager.o
	g++ -std=c++11 -o wave Main.o WaveFileController.o ConsoleManager.o

Main.o: src/Main.cpp src/WaveFileController.hpp src/ConsoleManager.hpp
	g++ -std=c++11 -c src/Main.cpp src/WaveFileController.cpp src/ConsoleManager.cpp
 
WaveFileController.o: src/WaveFileController.hpp
 
ConsoleManager.o: src/ConsoleManager.hpp 

clean:
	rm *.o *.gch wave