test_files = \
    src/ec_test_common.h \
    src/ec_test_common.cpp \
    src/ec_test_main.cpp \
    src/ec_boolean_classifier_test.cpp \
    src/ec_config_test.cpp \
    src/ec_logger_test.cpp \
    src/ec_regex_test.cpp \
    src/ec_svm_classifier_test.cpp \
    src/ec_utils_test.cpp \
    src/ec_ml_basics_test.cpp \

all: unit_tests

unit_tests: $(test_files) ../basics/basics.a ../mll/mll.a
	g++ -Wall -o $@ $(test_files) ../basics/basics.a ../mll/mll.a -Isrc/ -I../basics/inc/ -I../mll/inc/ -I../mll/ ../../gtest-1.6.0/make/gtest_main.a -I../../gtest-1.6.0/include/ -lpthread

clean:
	rm -rf *.o unit_tests

