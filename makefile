main: main.cpp processors.o
	g++ -std=c++11 -o wave processors.o main.cpp

processors.o: Echo.cpp Echo.h Reverser.cpp Reverser.h Normalizer.cpp Normalizer.h
	g++ -std=c++11 -c Echo.h Reverser.cpp Reverser.h Normalizer.cpp Normalizer.h

clean:
	rm *.o *.gch wave