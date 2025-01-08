TARGET = final_project
MSG_LIB = message
DT_LIB = datetime
TCP_LIB = tcp_library
all: client server
client: chat_client.hpp chat_client.cpp tcp_lib datetime_lib message_lib
	g++ chat_client.cpp tcp_library.cpp -std=c++17 -o chat_client -L. -l$(MSG_LIB) -l$(DT_LIB) -l$(TCP_LIB)
server: chat_server.hpp chat_server.cpp tcp_lib datetime_lib message_lib
	g++ chat_server.cpp tcp_library.cpp -std=c++17 -o chat_server -L. -l$(MSG_LIB) -l$(DT_LIB) -l$(TCP_LIB)
tcp_lib: $(TCP_LIB).cpp $(TCP_LIB).hpp
	g++ -o $(TCP_LIB).o $(TCP_LIB).cpp -c
	ar rc lib$(TCP_LIB).a $(TCP_LIB).o
message_lib: $(MSG_LIB).cpp $(MSG_LIB).hpp
	g++ -o $(MSG_LIB).o $(MSG_LIB).cpp -c
	ar rc lib$(MSG_LIB).a $(MSG_LIB).o
datetime_lib: $(DT_LIB).cpp $(DT_LIB).hpp
	g++ -o $(DT_LIB).o $(DT_LIB).cpp -c
	ar rc lib$(DT_LIB).a $(DT_LIB).o
clean:
	rm *.o *.a
# install:
# 	install ./$(TARGET) /usr/local/bin
# uninstall:
# 	rm -rf /usr/local/bin/$(TARGET)