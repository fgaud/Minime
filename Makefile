CC = gcc
CFLAGS = -g -Wall -Werror -O3
LDLIBS = -lpthread -lgsl -lgslcblas -lnuma
QUIET_STDERR = " >/dev/null 2>&1"

.phony: all clean tst

PLUGINS = plugin_throughput.o plugin_sequential_read.o plugin_random_read.o

TARGETS = memory_test 
all : tst makefile.dep $(TARGETS)
	
tst:
err:= ""
has_nasm := $(shell sh -c "which nasm > /dev/null && echo 1")
ifneq ($(has_nasm),1)
err +=  "2"
$(warning nasm not found. Try doing sudo apt-get install nasm.)
endif
has_gsl := $(shell sh -c "(echo 'int main(void) { return 0; }') | $(CC) -x c - -lgsl "$(QUIET_STDERR)" && rm a.out && echo y")
ifneq ($(has_gsl),y)
err +=  "2"
$(warning libgsl not found. Try doing sudo apt-get install libgsl0-dev)
endif
ifneq ($(err),"")
$(error Exiting from previous errors)
endif

makefile.dep : *.[Cch]
	    for i in *.[Cc]; do gcc -MM "$${i}"; done > $@

-include makefile.dep

memory_test : memory_test.o routines64.o ${PLUGINS}

routines64.o: routines64.s
	nasm -f elf64 routines64.s -o routines64.o

clean :
	rm -f $(TARGETS)
	rm -f *.o
	rm -f core*
	rm -f makefile.dep
