CC = gcc -Wall -Werror -Wextra
SRC_FILES = s21_decimal.c helpers.c
TEST_FILES = tests.c

OBJ_FILES = $(SRC_FILES:.c=.o)
UNAME := $(shell uname)
CFLAGS = -std=c11

ifeq ($(UNAME),Darwin)
CHECK_INCLUDE_PATH = $(shell brew --prefix check 2>/dev/null)/include
CHECK_LIB_PATH = $(shell brew --prefix check 2>/dev/null)/lib
OPEN_CMD = open
CHECK_LIB = -lcheck
else
CHECK_INCLUDE_PATH = /usr/include
CHECK_LIB_PATH = /usr/lib
OPEN_CMD = xdg-open
CHECK_LIB = -lcheck -pthread -lrt -lm -lsubunit
endif

LIB = s21_decimal.a
TARGET = decimal

all: $(LIB)

$(LIB): $(OBJ_FILES)
	ar rcs $(LIB) $(OBJ_FILES)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

ifeq ($(UNAME),Darwin)
test: $(LIB) $(TEST_FILES)
	$(CC) $(CFLAGS) -ftest-coverage -fprofile-arcs -I$(CHECK_INCLUDE_PATH) $(TEST_FILES) -o $(TARGET) -L$(CHECK_LIB_PATH) $(CHECK_LIB) $(LIB)
	./$(TARGET)
else
test: $(LIB) $(TEST_FILES)
	$(CC) $(CFLAGS) -ftest-coverage -fprofile-arcs -I$(CHECK_INCLUDE_PATH) $(TEST_FILES) -o $(TARGET) -L$(CHECK_LIB_PATH) $(CHECK_LIB) -L. -l:s21_decimal.a
	./$(TARGET)
endif

clean:
	rm -f $(OBJ_FILES) $(TARGET) *.gcda *.gcno $(LIB) *.info
	rm -rf report

gcov_report: test
	lcov --directory . --capture --exclude '/usr/*' -o coverage.info > /dev/null 2>&1
	genhtml --output-directory report --legend coverage.info > /dev/null 2>&1
	@$(OPEN_CMD) report/index.html
	@echo "Отчет gcov сгенерирован в папке report/"
