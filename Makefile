
# set this to the name of a test file
PROFTEST = tests/milestone5/good1.in

# OBJS get built with built-in make rules
OBJS = lexer/lexer.o lexer/reader.o \
	parser/newparser.o \
    generator/generator.o \
	compiler.o

INCS = -I./include

LD = g++
CXXFLAGS = -std=c++0x $(INCS)
LDFLAGS = -std=c++0x

ifdef FUNCTIONS
CXXFLAGS += -DENABLE_FUNCTIONS
endif

# output file is called proftest.out 
RUNFLAGS = -o proftest.out

export TESTDIR=./tests/milestone5/

compiler: $(OBJS)
	$(LD) $(LDFLAGS) -o compiler $(OBJS) 

clean:
	rm -f *.o lexer/*.o parser/*.o generator/*.o compiler.o core *.out
	ls

stutest.out: compiler
	@./runtests.sh


proftest.out: compiler
	cat $(PROFTEST)
	-./compiler $(RUNFLAGS) $(PROFTEST)
	cat proftest.out

