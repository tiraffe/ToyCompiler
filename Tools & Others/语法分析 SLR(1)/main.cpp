#include <bits/stdc++.h>
using namespace std;

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
TokenList List;

enum ActionType {
    Reduction,  //规约
    Move,       //移近
    Accept      //接受
};
struct Action {
    ActionType Type;
    int value;
    Action() {}
    Action(ActionType T, int v): Type(T), value(v) {}
};
vector<pair<string, int> > Index;
map<int, map<string, Action> > AnalysisTable;   //分析表

void readTokenList(char* fileName) {
    ifstream in(fileName, ios::in);
    int type, line;
    while(in >> line) {
        in >> type;
        CodeToken C;
        C.line = line;
        C.Type = (CodeTokenType)type;
        in >> C.value;
        if(C.Type == Float || C.Type == Integer) {
            C.num = strtod(C.value.c_str(), NULL);
        }
        List.push_back(C);
    }
    CodeToken E;
    E.Type = Unknown;
    E.value = "#";
    List.push_back(E);
}
void readAnalysisTable(char* fileName) {
    ifstream in(fileName, ios::in);
    int state, type, idx;
    string str;
    while(in >> state) {
        in >> str >> type >> idx;
        AnalysisTable[state][str] = (Action) {
            (ActionType)type, idx
        };
    }
}
void readIndex(char* fileName) {
    ifstream in(fileName, ios::in);
    string str;
    while(getline(in, str)) {
        stringstream line(str);
        string origin, sym;
        Express Ex;
        line >> origin;
        origin = origin.substr(1, origin.size() - 2);
        line >> sym;
        int sz = 0;
        while(line >> sym) {
            if(sym == "$") sz = 0;
            else sz++;
        }
        Index.push_back(make_pair(origin, sz));
    }
}
/********************************************* 准备工作完毕 ********************************************************/

const set <string> isType = {
    "bool", "int", "double", "string", "void", "char"
};
string TokenTypeToString(CodeToken T) {
    if(T.Type == Integer || T.Type == Float) return "number";
    if(T.Type == Identifier) return "id";
    if(T.Type == String) return "string";
    if(T.Type == Keywords && isType.count(T.value)) return "type";
    return T.value;
}

bool doAnalysis(char* fileName) {
    ofstream out("out.txt", ios::out);
    bool isError = false;
    vector <int> StateStack;
    vector <string> SymbolStack;
    StateStack.push_back(0);
    SymbolStack.push_back("#");
    cout << "状态栈栈顶\t当前符号\t相应操作" << endl;
    cout << "============================================================" << endl;
    auto it = List.begin();
    while(it != List.end()) {
        CodeToken T = *it;
        int top = StateStack[StateStack.size() - 1];
        string token = TokenTypeToString(T);

        cout << top << "\t\t" << token << "\t\t";

        if(AnalysisTable[top].find(token) == AnalysisTable[top].end()) {
            printf("第%d行存在语法错误\n", T.line);
            isError = true;
            break;
        }
        Action& A = AnalysisTable[top][token];
        if(A.Type == Move) {
            cout << "移近-->" << A.value << endl;
            SymbolStack.push_back(token);
            StateStack.push_back(A.value);
            it++;
        } else if(A.Type == Reduction) {
            int id = A.value;
            cout << "用产生式[" << id << "]规约 \t" ;
            for(int i = 0; i < Index[id].second; i++) {
                StateStack.pop_back();
                SymbolStack.pop_back();
            }
            SymbolStack.push_back(Index[id].first);
            int newstate = AnalysisTable[StateStack[StateStack.size() - 1]][Index[id].first].value;
            cout << " --> GOTO " << newstate << endl;
            StateStack.push_back(newstate);
        } else if(A.Type == Accept) {
            cout << "接受" << endl;
            break;
        }
    }
    cout << "==============================================================\n" << endl;
    return !isError;
}
int main() {
    readAnalysisTable("table.txt");
    readIndex("index.txt");
    readTokenList("list.txt");

    if(doAnalysis("resault.txt")) {
        puts("语法分析成功！");
    } else {
        puts("存在语法错误！");
    }

    return 0;
}
