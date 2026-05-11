CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic
MODULE = OrderedList.o
DEMO = demo.exe
TEST = test.exe

.PHONY: all module demo run-demo test run-test clean rebuild

all: $(DEMO) $(TEST)

module: $(MODULE)

$(MODULE): OrderedList.cpp OrderedList.h
	$(CXX) $(CXXFLAGS) -c OrderedList.cpp -o $(MODULE)

$(DEMO): $(MODULE) demo.cpp
	$(CXX) $(CXXFLAGS) demo.cpp $(MODULE) -o $(DEMO)

demo: $(DEMO)

run-demo: demo
	./$(DEMO)

$(TEST): $(MODULE) test.cpp
	$(CXX) $(CXXFLAGS) test.cpp $(MODULE) -o $(TEST)

test: $(TEST)

run-test: test
	./$(TEST)

clean:
	cmd /C "if exist $(MODULE) del /Q $(MODULE)"
	cmd /C "if exist $(DEMO) del /Q $(DEMO)"
	cmd /C "if exist $(TEST) del /Q $(TEST)"
	cmd /C "if exist log.txt del /Q log.txt"

rebuild: clean all
