CXX = g++
CXXFLAGS = -O2 -MMD -Wall
OBJECTS = main.o
DEPENDS = $(OBJECTS:.o=.d)
EXEC = sim

all: $(EXEC)

$(EXEC) : $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(EXEC)

clean :
	rm -rf $(DEPENDS) $(OBJECTS)

cleanall :
	rm -rf $(DEPENDS) $(OBJECTS) $(EXEC)

-include $(DEPENDS)
