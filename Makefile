
# OBJS get built with built-in make rules
OBJS = lexer/lexer.o lexer/reader.o \
	parser/parser.o parser/tokenstream.o \
	compiler.o

INCS = -Iinclude

LD = g++
CXXFLAGS = -std=c++0x $(INCS)
LDFLAGS = -std=c++0x

# tells compiler to tokenize only 
RUNFLAGS = -t

compiler: $(OBJS)
	$(LD) $(LDFLAGS) -o compiler $(OBJS) 

clean:
	rm -f *.o lexer/*.o parser/*.o core *.out
	ls

stutest.out: compiler
	cat tests/good1.in
	-./compiler $(RUNFLAGS) tests/good1.in > good1.out
	cat good1.out

	cat tests/good2.in
	-./compiler $(RUNFLAGS) tests/good2.in > good2.out
	cat good2.out

	cat tests/fail1.in
	-./compiler $(RUNFLAGS) tests/fail1.in > fail1.out
	cat fail1.out

	cat tests/fail2.in
	-./compiler $(RUNFLAGS) tests/fail2.in > fail2.out
	cat fail2.out

proftest.out: compiler
	cat $(PROFTEST)
	./compiler $(RUNFLAGS) $(PROFTEST) > proftest.out
	cat proftest.out

