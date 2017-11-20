CXX=gcc
FLAGS=-Wall -pthreads

TARGET=server
SOURCES=$(wildcard *.c)
HEADERS=$(wildcard *.h)
OBJECTS=$(patsubst %.c,%.o,$(SOURCES))

compile: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET) $(FLAGS) 

o/%.o: %.c $(HEADERS)
	$(CXX) -c $< -o $@ $(FLAGS)

clean:
	rm -rf *.o
