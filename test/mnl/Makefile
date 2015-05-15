BASEDIR = ../../
include $(BASEDIR)Make.env

INCS	= $(INC_MOON)
LIBS	= $(LIB_MOON) -levent

SOURCES = $(wildcard *_test.c)
BINARY = $(patsubst %.c, %, $(SOURCES))
DEPEND = .depend

all: preload $(BINARY)

$(DEPEND): $(SOURCES)
	@$(CC) $(CFLAGS) -MM $^ $(INCS) > $@

preload:
	@$(MAKE) -C $(LIBBASE)

-include $(DEPEND)
%:%.c ptest.c
	@if [ "$<" = `ls *.c|awk '{print $1}'|sed -n '1p'` ]; then \
		echo "=== "$(CFLAGS); \
		echo "=== "$(INCS); \
		echo "=== "$(LIBS); fi
	@echo "$(CC) -o $@"
	$(CC) $(CFLAGS) $^ -o $@ $(INCS) ${LIBS}

clean:
	@rm -rf ${BINARY}
