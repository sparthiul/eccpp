header_files = \
    inc/ec_boolean_classifier.h \
    inc/ec_linear_classifier.h \
    inc/ec_svm_classifier.h \
    inc/ec_ml_basics.h \
    inc/ec_decision_tree.h \

src_files = \
    src/ec_boolean_classifier.cpp \
    src/ec_linear_classifier.cpp \
    src/ec_svm.cpp \
    src/ec_svm.h \
    src/ec_svm_classifier.cpp \
    src/ec_ml_basics.cpp \
    src/ec_decision_tree.cpp \

obj_files = $(src_files:src/%.cpp=%.o)

all: mll.a

mll.a: $(src_files) $(header_files) ../basics/basics.a
	g++ -g -O0 -Wall -c $(src_files) -Isrc/ -Iinc/ -I../basics/inc/ -D__SVM_EXTENSION_ENABLED__
	ar cq $@ $(obj_files)

clean:
	rm -rf *.o mll.a
