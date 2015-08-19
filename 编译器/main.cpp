#include "IntermediateCode.h"

int main() {
    LexicalAnalyzer Lex("in.txt");
    if(Lex.isOK) {
        GrammarAnalyzer Gra(Lex.List);
        if(Gra.isOK) CodeGenerator Gen(Lex.List);
    }

    return 0;
}
