#ifndef LEXICALANALYZER_H
#define LEXICALANALYZER_H

#include <bits/stdc++.h>
using namespace std;

enum CodeTokenType {
    Integer,        //����
    Float,          //������
    String,         //�ַ���
    Operator,       //�����
    Identifier,     //��ʶ��
    Delimiter,      //�ֽ��
    Keywords,       //������

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
    bool isOK;          // �ʷ��������
    TokenList List;     // ��������
    LexicalAnalyzer(char *fileName);    // ������
    bool doAnalyzer(char* input);       // ��������
    void OutputToFile(char *ListFile, char* NumTable, char *IdTable); //���������ļ�
    bool isKeywords(string& str);   // �ж��Ƿ�Ϊ�ؼ���
private:
    int Line;   // ��ǰ���ʵ�����
    bool readString(ifstream& in, CodeToken& t, char& ch); // �����ַ���
    bool readNum(ifstream& in, CodeToken& t, char& ch); // ��������
    bool readDelimiter(ifstream& in, CodeToken& t, char& ch); // �����ʶ��
};

#endif // LEXICALANALYZER_H
