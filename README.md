# Python Interpreter

A simple python interpreter written in C++ that mimics Python 2.7

## Instructions:
  - Compile with ```make``` or ```g++ -std=c++17 *.cpp```
  - Run with ```./python.exe [-v] <filepath>```
  - ```-v``` is optional, include it to enable verbose mode, mainly for debugging

## Working features:
  - print()
  - \# Comments
  - Allows for blank lines
  - Assignment statements (variable definitions)
  - Implements the following data types: int, float, str, list
  - Handles arithmetic expressions and operand TypeErrors
  - For-loops and nested for-loops
  - If/Elif/Else statements and Relational/Logical operations
  - List operations: append(), push(), pop(), len() with TypeError/IndexError handling
  - Function definitions/calls/returns/symbol-table scoping

## Planned features:
  - While loops
  - Control flow statements: break, continue, pass
  - input()
  - Dictionaries
  - Classes
  - Import statements
  - Lambda functions
  - List slicing
  - List operations: min()/max()/sorted()/sum()
  - round()
  - pow()
  - format()
  - open()

Thanks to Dr. Ali Kooshesh for the starter code + help along the way
