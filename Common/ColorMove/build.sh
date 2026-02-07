#!/bin/bash
clang++ -m64 -fPIC -static -std=c++23 -stdlib=libc++ -O3 ColorMove.cpp -o colorMove.elf
strip colorMove.elf
