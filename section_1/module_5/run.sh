#! /bin/bash

EXEC_NAME=module_5
g++ *.cpp -o $EXEC_NAME;
cat input.txt | ./$EXEC_NAME