
PROG = compiler
# OBJS get built with built-in make rules
OBJS = lexer/lexer.o lexer/reader.o \
	parser/parser.o parser/tokenstream.o \
	compiler.o

INCS = -Iinclude

LD = g++
CXXFLAGS = -std=c++0x $(INCS)
LDFLAGS = -std=c++0x

$(PROG): $(OBJS)
	$(LD) $(LDFLAGS) -o $(PROG) $(OBJS) 


clean:
	rm lexer/*.o
	rm parser/*.o

