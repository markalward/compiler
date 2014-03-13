
# set this to the name of a test file
PROFTEST =

# OBJS get built with built-in make rules
OBJS = lexer/lexer.o lexer/reader.o \
	parser/newparser.o \
    generator/generator.o \
	compiler.o

INCS = -I./include

LD = g++
CXXFLAGS = -std=c++0x $(INCS) -DENABLE_FUNCTIONS
LDFLAGS = -std=c++0x

# output file is called a.out 
RUNFLAGS = -o a.out

export TESTDIR=./tests/parser/

compiler: $(OBJS)
	$(LD) $(LDFLAGS) -o compiler $(OBJS) 

clean:
	rm -f *.o lexer/*.o parser/*.o generator/*.o core *.out
	ls

stutest.out: compiler
	@./codegen_tests.sh


proftest.out: compiler
	cat $(PROFTEST)
	-./compiler $(RUNFLAGS) $(PROFTEST) > proftest.out
	cat proftest.out

