build:
	g++ -std=c++17 main.cpp -Wall -Wpedantic -Wextra

run:
	./a.out $(file) < $(file)
