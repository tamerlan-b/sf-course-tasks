TARGET = final_project
MSG_LIB = message
DT_LIB = datetime
APP_LIB = app
all: final_project client server
client: chat_client.hpp chat_client.cpp tcp_library.hpp tcp_library.cpp datetime_lib message_lib
	g++ chat_client.cpp tcp_library.cpp -std=c++17 -o chat_client -L. -l$(MSG_LIB) -l$(DT_LIB)
server: chat_server.hpp chat_server.cpp tcp_library.hpp tcp_library.cpp datetime_lib message_lib
	g++ chat_server.cpp tcp_library.cpp -std=c++17 -o chat_server -L. -l$(MSG_LIB) -l$(DT_LIB)
final_project: main.cpp app_lib datetime_lib message_lib file_data_manager.hpp data_manager_interface.hpp
	g++ main.cpp -std=c++17 -o $(TARGET) -L. -l$(APP_LIB) -l$(MSG_LIB) -l$(DT_LIB)
app_lib: $(APP_LIB).cpp $(APP_LIB).hpp message_lib datetime_lib
	g++ -o $(APP_LIB).o $(APP_LIB).cpp -c -L. -l$(MSG_LIB) -l$(DT_LIB)
	ar rc lib$(APP_LIB).a $(APP_LIB).o
message_lib: $(MSG_LIB).cpp $(MSG_LIB).hpp
	g++ -o $(MSG_LIB).o $(MSG_LIB).cpp -c -L.
	ar rc lib$(MSG_LIB).a $(MSG_LIB).o
datetime_lib: $(DT_LIB).cpp $(DT_LIB).hpp
	g++ -o $(DT_LIB).o $(DT_LIB).cpp -c
	ar rc lib$(DT_LIB).a $(DT_LIB).o
clean:
	rm *.o *.a
install:
	install ./$(TARGET) /usr/local/bin
uninstall:
	rm -rf /usr/local/bin/$(TARGET)