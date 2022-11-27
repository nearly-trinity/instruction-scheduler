CFLAGS=-Wall -O2 -std=c++11
OBJS    = main.o scanner.o display.o tables.o allocator.o
SOURCE    = main.cpp scanner.cpp display.cpp tables.cpp allocator.cpp
HEADER    = scanner.h printer.h allocator.h
OUT    = allocator 
CC     = g++ -std=c++11
FLAGS     = -g -c -Wall
LFLAGS     = 

all: $(OBJS)
	    $(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

main.o: main.cpp
	    $(CC) $(FLAGS) main.cpp 

scanner.o: scanner.cpp
	    $(CC) $(FLAGS) scanner.cpp 

display.o: display.cpp
	    $(CC) $(FLAGS) display.cpp 

tables.o: tables.cpp
	    $(CC) $(FLAGS) tables.cpp 

allocator.o: allocator.cpp
		$(CC) $(FLAGS) allocator.cpp

clean:
	    rm -f $(OBJS) $(OUT)
