main: processors.o
	g++ -std=c++11 -o wave src/Main.o src/WaveFileController.o src/WaveFileController.o src/ConsoleManager.o

processors.o: src/WaveFileController.cpp src/WaveFileController.cpp src/ConsoleManager.cpp src/ConsoleManager.hpp src/Utils.hpp src/Main.cpp
	g++ -std=c++11 -c src/WaveFileController.cpp src/WaveFileController.cpp src/ConsoleManager.cpp src/ConsoleManager.hpp src/Utils.hpp src/Main.cpp

clean:
	rm *.o *.gch wave