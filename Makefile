# for debug
# CFLAGS= -std=gnu99 -Wall -Wextra -Winline -g3 -ggdb -gdwarf-2 -fstack-protector-all -fno-omit-frame-pointer

# for release
CFLAGS= -std=gnu99 -Wall -Wextra -Winline -O2 -DNDEBUG

GENCONST_OBJ= genconst.o field.o
GENCONST_BIN= genconst

TEST_OBJ= test-api.o scalar.o field.o group.o api.o sha512.o
TEST_BIN= test-api

.PHONY: all clean test

all: test

clean:
	$(RM) $(GENCONST_BIN) $(TEST_BIN) *.o constants.h

$(GENCONST_BIN): $(GENCONST_OBJ)

constants.h: $(GENCONST_BIN)
	./$<

$(TEST_BIN): $(TEST_OBJ)

test: $(TEST_BIN)
	./$<

genconst.o: genconst.c field.h types.h
field.o: field.c field64.c field32.c field.h types.h
scalar.o: scalar.c scalar64.c scalar32.c scalar.h types.h
group.o: group.c types.h group.h constants.h field.h
api.o: api.c api.h types.h group.h constants.h field.h sha512.h
sha512.o: sha512.c sha512.h
test-api.o: test-api.c scalar.h field.h types.h group.h api.h
