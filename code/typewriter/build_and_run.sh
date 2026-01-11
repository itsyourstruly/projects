#!/bin/zsh

echo "Compiling file manager..."
g++ -std=c++17 main.cpp -o filemanager -lncurses

if [ $? -eq 0 ]; then
    echo "Compilation successful!"
    echo "Running file manager..."
    echo ""
    ./filemanager
else
    echo "Compilation failed!"
    exit 1
fi

