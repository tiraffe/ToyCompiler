#ifndef LEXICALANALYZER_H
#define LEXICALANALYZER_H

#include <bits/stdc++.h>
using namespace std;

enum CodeTokenType {
    Integer,        //整数
    Float,          //浮点数
    String,         //字符串
    Operator,       //运算符
    Identifier,     //标识符
    Delimiter,      //分界符
    Keywords,       //保留字

    Unknown
};

struct CodeToken {
    CodeTokenType Type = Unknown;
    string value;
    double num = 0;
    int line = -1;
};
typedef vector<CodeToken> TokenList;

const vector<string> KeywordTable = {
    "int", "double", "bool", "string", "if", "else", "while", "for", "include", "read", "write",
    "return", "true", "false"
};

const set <string> OperatorTable = {
    "!", "!=", "%", "*", "+", "++", "-", "--", "/", "<",
    "<=", "=", "==", ">", ">=", "+=", "-=", "*=", "/=",
    "&&", "||"
};
const set <char> DelimiterTable = {
    '(', ')', ',', ';', '[', ']', '{', '}',
};

class LexicalAnalyzer {
public:
    bool isOK;          // 词法分析结果
    TokenList List;     // 单词序列
    LexicalAnalyzer(char *fileName);    // 主程序
    bool doAnalyzer(char* input);       // 分析函数
    void OutputToFile(char *ListFile, char* NumTable, char *IdTable); //输出结果到文件
    bool isKeywords(string& str);   // 判断是否为关键字
private:
    int Line;   // 当前单词的行数
    bool readString(ifstream& in, CodeToken& t, char& ch); // 读入字符串
    bool readNum(ifstream& in, CodeToken& t, char& ch); // 读入数字
    bool readDelimiter(ifstream& in, CodeToken& t, char& ch); // 读入标识符
};

#endif // LEXICALANALYZER_H
