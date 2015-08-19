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
    bool isOK;          // 语法分析结果
    string start;       // 开始符
    TokenList& List;    // 单词序列
    GrammarAnalyzer(TokenList& L);  // 默认构造函数
    vector <CompileError> ErrorList;    // 错误信息序列
    map <string, map<string, Express> > AnalyTable; // 预测分析表
private:
    void readAnalyTable(char *fileName); // 读入预测分析表
    void readTokenList(char* fileName);  // 读入单词序列
    string TokenTypeToString(CodeToken T); // 将单词类型转为string
    bool ErrorRecover(Symbol& top, CodeToken& token, stack<Symbol>& S, TokenList::iterator& i); // 错误恢复
    bool doAnalyse(char *fileName); // 预测分析函数
    void outputErrorList(char *fileName); // 输出错误信息
};

#endif // GRAMMARANALYZER_H
