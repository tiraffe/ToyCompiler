#include "LexicalAnalyzer.h"

LexicalAnalyzer::LexicalAnalyzer(char *fileName) {
    if(!doAnalyzer(fileName)) {
        isOK = false;
        printf("词法错误 in Line : %d\n", Line);
    } else {
        isOK = true;
        printf("词法分析结果 ：成功。\n");
        OutputToFile("TokenList.txt", "NumberTable.txt", "IdentTable.txt");
    }
}
bool LexicalAnalyzer::isKeywords(string& str) {
    for(auto it : KeywordTable) {
        if(it == str) return true;
    }
    return false;
}
bool LexicalAnalyzer::readNum(ifstream& in, CodeToken& t, char& ch) {
    t.Type = Integer;
    while(isdigit(ch) && !in.eof()) {
        t.num = t.num * 10 + ch - '0';
        in.get(ch);
    }
    if(ch == '.') {
        t.Type = Float;
        in.get(ch);

        double k = 1;
        while(isdigit(ch) && !in.eof()) {
            k /= 10;
            t.num += k * (ch - '0');
            in.get(ch);
        }
    } else if(isalpha(ch)){
        return false;
    }
    if(ch == 'e' || ch == 'E') {
        t.Type = Float;
        in.get(ch);
        bool flag = false;
        double k = 1;

        if(ch == '+' || ch == '-') {
            if(ch == '-') flag = true;
            in.get(ch);
        }
        if(isdigit(ch)) {
            int x = 0;
            while(isdigit(ch) && !in.eof()) {
                x = x * 10 + ch - '0';
                in.get(ch);
            }
            if(flag) x *= -1;
            t.num *= powl(10, x);
        } else { // e后不是数字和+-
            return false;
        }
    } else if(isalpha(ch)){
        return false;
    }
    return true;
}
bool LexicalAnalyzer::readDelimiter(ifstream& in, CodeToken& t, char& ch) {
    t.value += ch;
    char ch2;
    in.get(ch2);
    string op;
    op += ch;
    if(OperatorTable.count(op + ch2)) {
        t.Type = Operator;
        t.value += ch2;
        in.get(ch);
        return true;
    }
    if(OperatorTable.count(op)) {
        t.Type = Operator;
        ch = ch2;
        return true;
    }
    if(DelimiterTable.count(ch)) {
        t.Type = Delimiter;
        ch = ch2;
        return true;
    }
    return false;
}
bool LexicalAnalyzer::readString(ifstream& in, CodeToken& t, char& ch) {
    t.Type = String;
    in.get(ch);
    while(ch != '\"' && !in.eof()) {
        if(ch == '\n') return false;
        if(ch == '\\') {
            char ch2;
            in.get(ch2);
            switch(ch2) {
            case 'n':
                t.value += '\n';
                in.get(ch);
                break;
            case 't':
                t.value += '\t';
                in.get(ch);
                break;
            case '\\':
                t.value += '\\';
                in.get(ch);
                break;
            default:
                ch = ch2;
                break;
            }
        } else {
            t.value += ch;
            in.get(ch);
        }
    }
    in.get(ch);
    return true;
}
bool LexicalAnalyzer::doAnalyzer(char* input) {
    ifstream in(input, ios::in);

    char ch, ch2;
    bool isLastNum = false;
    Line = 1;
    in.get(ch);
    while(!in.eof()) {
        CodeToken t;

        if(isspace(ch)) { // 跳过空格回车
            while(isspace(ch) && !in.eof()) {
                if(ch == '\n') Line++;
                in.get(ch);
            }
        } else if(ch == '+' || ch == '-') {
            in.get(ch2);
            bool flag = ch == '-';
            if(isdigit(ch2) && !isLastNum) {
                ch = ch2;
                if(!readNum(in, t, ch)) return false;
            } else {
                t.value += ch;
                t.Type = Operator;
                if(ch == ch2) {
                    t.value += ch;
                    in.get(ch);
                } else {
                    ch = ch2;
                }
            }
            if(flag) t.num *= -1;
        } else if(isdigit(ch)) {
            if(!readNum(in, t, ch)) return false;
        } else if(isalpha(ch) || ch == '_') { // 标识符
            while(!in.eof()) {
                if(!isalnum(ch) && ch != '_') break;
                t.value += ch;
                in.get(ch);
            }
            if(isKeywords(t.value)) {
                t.Type = Keywords;
            } else {
                t.Type = Identifier;
            }
        } else if(ch == '\"') { // 字符串
            if(!readString(in, t, ch)) return false;
        } else { // 运算符 或 界符
            if(!readDelimiter(in, t, ch)) return false;
        }

        t.line = Line;
        if(t.Type != Unknown) {
            List.push_back(t);
        }
        if(t.Type == Float || t.Type == Integer || t.Type == Identifier) {
            isLastNum = true;
        } else {
            isLastNum = false;
        }
    }
    return true;
}
void LexicalAnalyzer::OutputToFile(char *ListFile, char* NumTable, char *IdTable) {
    ofstream Lout(ListFile, ios::out);
    ofstream Nout(NumTable, ios::out);
    ofstream Iout(IdTable, ios::out);
    set <string> vis;

    for(int i = 0; i < List.size(); i++) {
        Lout <<  List[i].line << " ";
        Lout << List[i].Type << " ";
        if(List[i].Type == Integer || List[i].Type == Float) {
            Lout << List[i].num << endl;
            Nout << List[i].num << endl;
        } else {
            if(List[i].Type == Identifier) {
                if(vis.count(List[i].value) == 0) {
                    Iout << List[i].value << endl;
                    vis.insert(List[i].value);
                }
            }
            Lout << List[i].value << endl;
        }
    }
}
