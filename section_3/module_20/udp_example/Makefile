SERVER_CPP = udp_server.cpp
CLIENT_CPP = udp_client.cpp
all: udp_server udp_client
udp_server: $(SERVER_CPP)
	g++ $(SERVER_CPP) -o udp_server.a
udp_client: $(CLIENT_CPP)
	g++ $(CLIENT_CPP) -o udp_client.a
clean:
	rm *.a