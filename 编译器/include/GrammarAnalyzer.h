#ifndef GRAMMARANALYZER_H
#define GRAMMARANALYZER_H
#include "LexicalAnalyzer.h"
using namespace std;

enum ErrorType {
    MissTerminalSymbol,
    MissPartSymbol,
    ExcessTerminalSymbol
};
struct Symbol {
    Symbol(string v, bool T) {
        value = v;
        isTerminal = T;
    }
    string value;
    bool isTerminal;
    bool operator < (const Symbol& B) const {
        return value < B.value;
    }
};
typedef vector<Symbol> Express;

struct CompileError {
    ErrorType Type;
    int line = -1;
    string value;
};
const set <string> isType = {
    "bool", "int", "double", "string", "void", "char"
};

class GrammarAnalyzer {
public:
    bool isOK;          // �﷨�������
    string start;       // ��ʼ��
    TokenList& List;    // ��������
    GrammarAnalyzer(TokenList& L);  // Ĭ�Ϲ��캯��
    vector <CompileError> ErrorList;    // ������Ϣ����
    map <string, map<string, Express> > AnalyTable; // Ԥ�������
private:
    void readAnalyTable(char *fileName); // ����Ԥ�������
    void readTokenList(char* fileName);  // ���뵥������
    string TokenTypeToString(CodeToken T); // ����������תΪstring
    bool ErrorRecover(Symbol& top, CodeToken& token, stack<Symbol>& S, TokenList::iterator& i); // ����ָ�
    bool doAnalyse(char *fileName); // Ԥ���������
    void outputErrorList(char *fileName); // ���������Ϣ
};

#endif // GRAMMARANALYZER_H
