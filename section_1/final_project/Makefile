TARGET = final_project
USER_LIB = user
MSG_LIB = message
DT_LIB = datetime
APP_LIB = app
final_project: main.cpp app_lib datetime_lib message_lib user_lib
	g++ main.cpp -std=c++17 -o $(TARGET) -L. -l$(APP_LIB) -l$(USER_LIB) -l$(MSG_LIB) -l$(DT_LIB)
app_lib: $(APP_LIB).cpp $(APP_LIB).hpp message_lib user_lib datetime_lib
	g++ -o $(APP_LIB).o $(APP_LIB).cpp -c -L. -l$(USER_LIB) -l$(MSG_LIB) -l$(DT_LIB)
	ar rc lib$(APP_LIB).a $(APP_LIB).o
message_lib: $(MSG_LIB).cpp $(MSG_LIB).hpp user_lib
	g++ -o $(MSG_LIB).o $(MSG_LIB).cpp -c -L. -l$(USER_LIB)
	ar rc lib$(MSG_LIB).a $(MSG_LIB).o
user_lib: $(USER_LIB).cpp $(USER_LIB).hpp
	g++ -o $(USER_LIB).o $(USER_LIB).cpp -c
	ar rc lib$(USER_LIB).a $(USER_LIB).o
datetime_lib: $(DT_LIB).cpp $(DT_LIB).hpp
	g++ -o $(DT_LIB).o $(DT_LIB).cpp -c
	ar rc lib$(DT_LIB).a $(DT_LIB).o
clean:
	rm *.o *.a
install:
	install ./$(TARGET) /usr/local/bin
uninstall:
	rm -rf /usr/local/bin/$(TARGET)