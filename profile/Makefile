CODE_FILES=$(wildcard *.c *.h Makefile)
FILTER_FILES=Makefile %.h

all: profile

profile: $(CODE_FILES)
	$(CC) -D_GNU_SOURCE -Wall -lpthread -g -O2 $(filter-out $(FILTER_FILES), $^) -o $@

clean:
	rm profile

.PHONY: all clean
