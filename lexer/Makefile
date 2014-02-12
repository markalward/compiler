
OBJS = lexer.o reader.o

INCS = -I/home/alwardm/cs480/compiler/include


LD = g++
CXXFLAGS = -std=c++0x $(INCS)
LDFLAGS = -std=c++0x


lexer: $(OBJS)
	$(LD) $(LDFLAGS) -o lexer $(OBJS)

