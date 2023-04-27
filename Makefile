CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -I$(INCLUDES)

TARGET = pcfg_tool
SRCS = main.cpp utils.cpp
OBJS = $(SRCS:.cpp=.o)
INCLUDES = .

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

%.o: %.ccp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJS) $(TARGET)