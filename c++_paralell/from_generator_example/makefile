CC=g++
CFLAGS= -c -std=c++17 -Wall -O3
LDFLAGS= -O3
SOURCES=mem_from_gen_example.cpp ../memory_estimators_with_co_order_coll.cpp ../Synthetic_generators.cpp
DEPS=../memory_estimators_with_co_order_coll_head.h ../Synthetic_generators_head.h
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=mem_from_gen_example

all: $(SOURCES) $(EXECUTABLE)


%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

    
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@