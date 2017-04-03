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
	strip $@

.PHONY:clean
clean:
	rm -f *.[od] checkers *~
	rm -f wallingford_shumaker_proj1.tar.gz
	rm -rf wallingford_shumaker_proj1

wallingford_shumaker_proj1.tar.gz: checkers ${SRC} README.md Makefile $(wildcard *.hpp)
	rm -rf wallingford_shumaker_proj1
	mkdir wallingford_shumaker_proj1
	cp $^ wallingford_shumaker_proj1/
	tar -czvf $@ wallingford_shumaker_proj1

DEPS := $(OBJ:.o=.d)
-include $(DEPS)
