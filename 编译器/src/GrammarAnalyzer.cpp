#include "GrammarAnalyzer.h"

GrammarAnalyzer::GrammarAnalyzer(TokenList& L): List(L) {
    readAnalyTable("table.txt");
    if(doAnalyse("resault.txt")) {
        isOK = true;
        printf("\n语法分析结果 ：成功。\n\n");
    } else {
        isOK = false;
        outputErrorList("ErrorList.txt");
        printf("\n语法分析结果 ：存在语法错误\n");
    }
}
void GrammarAnalyzer::readAnalyTable(char *fileName) {
    ifstream in(fileName, ios::in);
    string str;
    in >> start;
    while(!in.eof()) {
        getline(in, str);
        stringstream line(str);
        string X, Y, sym;
        line >> X >> Y;
        Express& A = AnalyTable[X][Y];
        while(line >> sym) {
            int len = sym.size();
            if(sym[0] == '<' && sym[len - 1] == '>') {
                sym = sym.substr(1, len - 2);
                A.push_back({sym, 0});
            } else {
                A.push_back({sym, 1});
            }
        }
        reverse(A.begin(), A.end());
    }
}
void GrammarAnalyzer::readTokenList(char* fileName) {
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
string GrammarAnalyzer::TokenTypeToString(CodeToken T) {
    if(T.Type == Integer || T.Type == Float) return "number";
    if(T.Type == Identifier) return "id";
    if(T.Type == String) return "string";
    if(T.Type == Keywords && isType.count(T.value)) return "type";
    return T.value;
}
bool GrammarAnalyzer::ErrorRecover(Symbol& top, CodeToken& token, stack<Symbol>& S, TokenList::iterator& i) {
    CompileError E;
    E.line = token.line;
    string a = TokenTypeToString(token), A = top.value;

    if(top.isTerminal == true) {
        E.Type = MissTerminalSymbol;
        E.value = top.value;
        ErrorList.push_back(E);
    } else if(AnalyTable[A][";"].size() > 0) {
        E.Type = MissTerminalSymbol;
        E.value = top.value;
        ErrorList.push_back(E);
    } else if(a == ")" || a == "}" || a == "]" || a == ";") {
        E.Type = MissPartSymbol;
        E.value = A;
        ErrorList.push_back(E);
    } else {
        S.push(top);
        E.Type = ExcessTerminalSymbol;
        E.value = token.value;
        ErrorList.push_back(E);
        i++;
    }
    return true;
}
bool GrammarAnalyzer::doAnalyse(char *fileName) {
    ofstream out(fileName, ios::out);

    out << setw(14) << "当前栈顶符号" << setw(14) << "当前输入符号" << "\t\t\t推导所用产生式或者匹配 " << endl;
    out << "================================================================================" << endl;

    bool isError = false;
    stack <Symbol> AnalysisStack;
    AnalysisStack.push({"#", 1});
    AnalysisStack.push({start, 0});

    TokenList::iterator i = List.begin();
    while(i != List.end()) {
        CodeToken token = *i;
        Symbol top = AnalysisStack.top();
        string a = top.value, b = TokenTypeToString(token);
        Express& Ay = AnalyTable[a][b];
        if(a == "#" || b == "#") {
            if(b != "#") isError = true;
            break;
        }
        out << setw(14) << a << setw(14) << b << "\t----->\t";
        AnalysisStack.pop();
        if(a == b) {
            out << "'" << a << "' 匹配" << endl;
            i++;
        } else if(Ay.size() == 0) {
            out << "错误" << endl;
            if(!ErrorRecover(top, token, AnalysisStack, i)) return false;
            isError = true;
        } else {
            out << a << " -> ";
            for(auto x = Ay.rbegin(); x != Ay.rend(); x++) {
                out << (*x).value << " " ;
            }
            out << endl;
            if(Ay[0].value == "$") continue;
            for(auto x : Ay) {
                AnalysisStack.push(x);
            }
        }
    }
    if(AnalysisStack.top().value != "#") { // 检查是否正常结束
        isError = true;
        CompileError E;
        while(AnalysisStack.top().value != "#") {
            E.line = (*List.rbegin()++).line;
            E.Type = MissTerminalSymbol;
            E.value = AnalysisStack.top().value;
            ErrorList.push_back(E);
            AnalysisStack.pop();
        }
    }
    out << "================================================================================" << endl;

    return !isError;
}
void GrammarAnalyzer::outputErrorList(char *fileName) {
    ofstream out(fileName, ios::out);

    for(CompileError E : ErrorList) {
        ErrorType T = E.Type;
        string str = E.value;
        cout << "Error in Line [" << E.line << "] : ";
        switch(T) {
        case MissPartSymbol:
            cout << "语句成分不完整，缺少成分[" << str << "]\n";
            break;
        case MissTerminalSymbol:
            cout << "语句成分不完整：缺少\"" << str << "\"\n";
            break;
        case ExcessTerminalSymbol:
            cout << "语句含有多余成分：多余的\"" << str << "\"\n";
            break;
        }
    }
}
