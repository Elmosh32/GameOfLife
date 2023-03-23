CXX?=g++
CC=$(CXX)

CPPFLAGS += -I$(INC_DIR)
CXXFLAGS += -std=c++20
CXXFLAGS += -pedantic -Wall -g
CPPFLAGS += -MMD -MP

SRC_DIR = ../../src
INC_DIR = ../../inc

TARGET = utest
OBJECTS = utest.o

check: $(TARGET) 
	./$(TARGET)

$(TARGET): $(OBJECTS) $(SRC_DIR)/gameOfLife.o -lpthread

all:  $(TARGET)   

video: 
	ffmpeg -framerate 20 -i images/life.%4d.ppm -c:v libx264 -pix_fmt yuv420p game_of_life.mp4

valgrind:$(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --log-file=logV1.txt ./$(TARGET)

clean:
	@$(RM) $(OBJECTS) $(DEPENDS) $(TARGET) images/life* game_of_life*

DEPENDS = $(OBJECTS:%.o=%.d)

-include $(DEPENDS) 