main: src/Main.cpp processors.o
	g++ -std=c++11 -o wave src/Main.cpp src/*.o

processors.o: src/WaveFileController.cpp src/WaveFileController.cpp src/ConsoleManager.cpp src/ConsoleManager.hpp src/Utils.hpp
	g++ -std=c++11 -c src/WaveFileController.cpp src/WaveFileController.cpp src/ConsoleManager.cpp src/ConsoleManager.hpp src/Utils.hpp

clean:
	rm *.o *.gch wave