SRC := $(wildcard *.cpp)
OBJ := $(SRC:.cpp=.o)

LDFLAGS += $(CXXFLAGS)
CXXFLAGS += -std=gnu++11 -pipe -march=native -Wall -Wextra -MMD -D_GNU_SOURCE
LDLIBS += -lpthread -lncursesw

release: CXXFLAGS += -O3 -DNDEBUG
release: checkers

debug: CXXFLAGS += -O0 -ggdb3
debug: checkers

checkers: $(OBJ)
	$(CXX) $(LDFLAGS) $(LDLIBS) -o $@ $^

.PHONY:clean
clean:
	rm -rf *.[od] checkers *~

DEPS := $(OBJ:.o=.d)
-include $(DEPS)
