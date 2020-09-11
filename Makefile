.SUFFIXES: .o .cpp .exe

CFLAGS = -ggdb -std=c++17
objects = Token.o Tokenizer.o Parser.o ArithExpr.o SymTab.o Statements.o TypeDescriptor.o Range.o Functions.o main.o

python.exe: $(objects)
	g++ $(CFLAGS) -o python.exe $(objects)

.cpp.o:
	g++ $(CFLAGS) -c $< -o $@


Token.o: Token.cpp Token.hpp
Tokenizer.o: Tokenizer.cpp Tokenizer.hpp
ArithExpr.o: ArithExpr.cpp ArithExpr.hpp Token.hpp SymTab.hpp Functions.hpp TypeDescriptor.hpp
SymTab.o: SymTab.cpp SymTab.hpp TypeDescriptor.hpp
Parser.o: Parser.cpp Token.hpp Parser.hpp Tokenizer.hpp SymTab.hpp ArithExpr.hpp Statements.hpp Range.hpp Functions.hpp
Statements.o: Statements.cpp Statements.hpp ArithExpr.hpp SymTab.hpp Range.hpp TypeDescriptor.hpp
TypeDescriptor.o: TypeDescriptor.cpp TypeDescriptor.hpp
Range.o: Range.cpp Range.hpp ArithExpr.hpp
Functions.o: Functions.cpp Functions.hpp SymTab.hpp Statements.hpp
main.o: main.cpp Token.hpp Tokenizer.hpp Parser.hpp

clean:
	rm -fr *.o *~ *.exe
