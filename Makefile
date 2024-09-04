all:
	g++ -Wall -Wextra -o client client.cpp
	g++ -Wall -Wextra -o server chatServer.cpp
clean:
	rm -f ./client ./server