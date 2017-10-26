BOOST_ROOT = ../utils/boost_1_65_1

CXXFLAGS =	-I$(BOOST_ROOT) -std=c++11 -pthread -O3
OBJS =		TestQueue.o 	

LIBS =

TARGET =	testqueue

$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS) $(CXXFLAGS)

all:	clean	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
