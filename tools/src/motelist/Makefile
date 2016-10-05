# ===========================================================

bin = $(NOS_HOME)/tools
CPP = g++

all: motelist

motelist: $(bin)/motelist.exe

$(bin)/motelist.exe: motelist-win32.cpp
	$(CPP) -o $@ $<

clean:
	-rm -f *.o
