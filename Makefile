all: pong

pong: pong.cpp gui.cpp
	g++ -std=c++17 $^ -o $@ -O0 -ggdb3 `pkg-config --libs --cflags gtk+-3.0`

dist:
	mkdir pong1
	cp pong.cpp gui.cpp gui.h Makefile pong1/
	unifdef -x2 -m -U REF pong1/pong.cpp
	tar cfj pong1.tar.bz2 pong1
	rm pong1/*
	rmdir pong1
