CXX ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wno-unused-variable -Imocks

test: test_sketch
	./test_sketch

test_sketch: test_sketch.cpp ../IoT_SmartTrashCan.ino $(wildcard mocks/*.h)
	$(CXX) $(CXXFLAGS) test_sketch.cpp -o $@

clean:
	rm -f test_sketch test_sketch.exe

.PHONY: test clean
