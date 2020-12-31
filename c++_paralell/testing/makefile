CC=g++
CFLAGS= -c -std=c++17 -Wall -O3
LDFLAGS= -O3
SOURCES=get_memory_from_file.cpp temp_net_parser.cpp memory_estimators_with_co_order_testing.cpp
DEPS=temp_net_parser_head.h memory_estimators_with_co_order_testing_head.h
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=get_memory_from_file

all: $(SOURCES) $(EXECUTABLE)


%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

    
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@