SERVER_CPP = tcp_server.cpp
CLIENT_CPP = tcp_client.cpp
all: tcp_server tcp_client
tcp_server: $(SERVER_CPP)
	g++ $(SERVER_CPP) -o tcp_server.a
tcp_client: $(CLIENT_CPP)
	g++ $(CLIENT_CPP) -o tcp_client.a
clean:
	rm *.a