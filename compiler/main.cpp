#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>

#include "antlr4-runtime.h"
#include "generated/ifccLexer.h"
#include "generated/ifccParser.h"
#include "generated/ifccBaseVisitor.h"
#include "asm_generator/Gen_x86.h"

#include "ASTVisitor.h"
#include "DeclarationVisitor.h"

using namespace antlr4;
using namespace std;

int main(int argn, const char **argv)
{
  stringstream in;
  if (argn == 2)
  {
    ifstream lecture(argv[1]);
    in << lecture.rdbuf();
  }
  else
  {
    cerr << "usage: ifcc path/to/file.c" << endl;
    exit(1);
  }

  ANTLRInputStream input(in.str());

  ifccLexer lexer(&input);
  CommonTokenStream tokens(&lexer);

  tokens.fill();

  ifccParser parser(&tokens);
  tree::ParseTree *tree = parser.axiom();

  if (parser.getNumberOfSyntaxErrors() != 0)
  {
    cerr << "error: syntax error during parsing" << endl;
    exit(1);
  }

  // use another visitor declarationVisitor
  /*
  use hashmap <string, long> = <variableName, offsetInStack>
  if newDeclaration in hashmap => error (even if different type)
  */

  // create symbolTable for the scope
  cerr << "DeclarationVisitor" << endl;
  DeclarationVisitor d;
  d.visit(tree);
  cerr << "ASTVisitor" << endl;
  ASTVisitor v;
  v.visit(tree);

  CodeGen *codegen = new Gen_x86(v.getCFGs());

  codegen->gen_asm(std::cout);

  return 0;
}
