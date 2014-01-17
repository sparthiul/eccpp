header_files = \
    inc/ec_config.h \
    inc/ec_iconfig.h \
    inc/ec_iregex.h \
    inc/ec_log.h \
    inc/ec_logger.h \
    inc/ec_regex.h \
    inc/ec_utils.h \

src_files = \
    src/ec_config.cpp \
    src/ec_ctype.h \
    src/ec_log.cpp \
    src/ec_logger.cpp \
    src/ec_regex.cpp \
    src/ec_utils.cpp \

src_json_files = \
    src/json/autolink.h \
    src/json/config.h \
    src/json/features.h \
    src/json/forwards.h \
    src/json/json.h \
    src/json/reader.h \
    src/json/value.h \
    src/json/writer.h \
    src/json/json_batchallocator.h \
    src/json/json_reader.cpp \
    src/json/json_value.cpp \
    src/json/json_writer.cpp \

obj_files = $(src_files:src/%.cpp=%.o)

obj_json_files = $(src_json_files:src/json/%.cpp=%.o)

all: basics.a

basics.a: $(src_files) $(src_json_files) $(header_files)
	g++ -g -O0 -Wall -c $(src_files) $(src_json_files) -Isrc/ -Iinc/
	ar cq $@ $(obj_files) $(obj_json_files)

clean:
	rm -rf *.o basics.a
