header_files = \
    inc/pa_dom_common.h \
    inc/pa_dom_node_visitor.h \
    inc/pa_page_analysis_facade.h \
    inc/pa_page_analysis_factory.h \
    inc/pa_python_wrapper.h \
    inc/pa_server_dom_node_visitor.h \
    inc/pa_server_page_analysis_factory.h \

src_files = \
    src/pa_dom_node.h \
    src/pa_dom_tree_parser.h \
    src/pa_python_dom_helper.h \
    src/pa_server_config_store.h \
    src/pa_dom_node.cpp \
    src/pa_dom_tree_parser.cpp \
    src/pa_python_dom_helper.cpp \
    src/pa_python_wrapper.cpp \
    src/pa_server_config_store.cpp \
    src/pa_server_dom_node_visitor.cpp \
    src/pa_server_page_analysis_factory.cpp \

test_files = \
    ../basics/test_common/ec_test_common.h \
    ../basics/test_common/ec_test_common.cpp \
    ../basics/test_common/ec_test_main.cpp \

obj_files = $(src_files:src/%.cpp=%.o)

all: python_dom.a python_dom_test

python_dom.a: $(src_files) $(header_files)
	g++ -Wall -c $(src_files) -Isrc/ -Iinc/ -I.. -I../basics/inc/ -I/usr/include/python2.7/
	ar cq $@ $(obj_files)

python_dom_test: $(test_files) python_dom.a basics.a
	g++ -Wall -o $@ $(test_files) python_dom.a basics.a ../../gtest-1.6.0/make/gtest_main.a -Isrc/ -Itests/ -Iinc/ -I../basics/test_common/ -I.. -I../basics/inc/ -I../../gtest-1.6.0/include/ -lpthread -I/usr/include/python2.7/


clean:
	rm -rf *.o python_dom.a python_dom.test
