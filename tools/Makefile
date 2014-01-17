src_files = \
    src/ec_decision_tree_tool.cpp \

obj_files = $(src_files:src/%.cpp=%.o)

all: decision_tree_tool

decision_tree_tool: $(src_files) ../basics/basics.a ../mll/mll.a
	g++ -g -O0 -Wall -o $@ $(src_files) ../basics/basics.a ../mll/mll.a -Isrc/ -I../basics/inc/ -I../mll/inc/

clean:
	rm -rf *.o decision_tree_tool
