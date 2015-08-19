#include "IntermediateCode.h"

CodeGenerator::CodeGenerator(TokenList &L): Tokens(L) {
    Main();
    if(isError) {
        cout << "存在语义错误！" << endl;
        for(SemanticError E : Errors) {
            E.output();
        }
    } else {
        cout << "语义分析结果 ：成功。" << endl;
        OutputVarTable("VarTable.txt");
        OutputQuadList("QuadList.txt");
        OutputAsm("Asm.txt");
    }
}
void CodeGenerator::OutputAsm(char * FileName) {
    ofstream out(FileName, ios::out);
    vector <int> newLabel;
    int idx = 0;
    set <string> TWO = {"J>=", "J<=", "J==", "J!=", "J>", "J<", "+", "-" , "*", "/", "%"};
    for(Quadruple Q : QuadList) {
        newLabel.push_back(idx);
        string &op = Q.op;
        if(TWO.count(op)) idx += 2;
        else idx += 1;
    }
    int addr = 0;
    for(Quadruple Q : QuadList) {
        string &op = Q.op;
        string handle;
        if(TWO.count(op)) {
            if(op[0] == 'J') {
                int num = atoi(Q.name_res.c_str());
                handle =
                    op == "J>" ? "JG" :
                    op == "J<" ? "JL" :
                    op == "J<=" ? "JGE" :
                    op == "J>=" ? "JLE" :
                    op == "J!=" ? "JNZ" :
                    op == "J==" ? "JZ" : "UNKNOW";
                out << addr++ << ":\t" << "CMP " << Q.name1 << ", " << Q.name2 << endl;
                out << addr++ << ":\t" << handle << " " << newLabel[num] << endl;
            } else {
                handle =
                    op == "+" ? "ADD" :
                    op == "-" ? "SUB" :
                    op == "*" ? "MUL" :
                    op == "/" ? "DIV" :
                    op == "%" ? "MOD" :
                    op == "&&" ? "AND" :
                    op == "||" ? "OR" : "UNKNOW";
                out << addr++ << ":\t" << "MOV " << Q.name_res << ", " << Q.name1 << endl;
                out << addr++ << ":\t" << handle << " " << Q.name_res << ", " << Q.name2 << endl;
            }
        } else {
            if(op == "J") {
                int num = atoi(Q.name_res.c_str());
                out << addr++ << ":\t" << "JUMP " << newLabel[num] << endl;
            } else if(op == "=") {
                out << addr++ << ":\t" << "MOV " << Q.name1 << ", " << Q.name2 << endl;
            } else if(op == "return") {
                out << addr++ << ":\t" << "RET " << Q.name1 << endl;
            } else if(op == "write") {
                out << addr++ << ":\t" << "OUTPUT " << Q.name1 << endl;
            } else if(op == "read") {
                out << addr++ << ":\t" << "INPUT " << Q.name1 << endl;
            } else if(op == "++") {
                out << addr++ << ":\t" << "INC " << Q.name_res << endl;
            } else if(op == "--") {
                out << addr++ << ":\t" << "DEC " << Q.name_res << endl;
            } else if(op == "!") {
                out << addr++ << ":\t" << "NOT " << Q.name_res << ", " << Q.name1 << endl;
            }
        }
    }
}
void CodeGenerator::OutputQuadList(char *FileName) {
    ofstream out(FileName, ios::out);
    int id = 0;
    for(Quadruple Q : QuadList) {
        out << "[" << id++ << "]:\t" << Q << endl;
    }
}
void CodeGenerator::OutputVarTable(char *FileName) {
    ofstream out(FileName, ios::out);
    out << setw(10) << "属性" << setw(10) << "名称" << setw(10) << "类型" << setw(10) << "值" << endl;
    out << "================================================" << endl;
    for(auto P : VarTable) {
        string name = P.first;
        Var v = P.second;
        if(name[0] == '*') {
            out << setw(10) << "const" << setw(10) << name << setw(10) << v.Type << setw(10) << v.value << endl;
        } else if(name[0] == '#') {
            out << setw(10) << "temp" << setw(10) <<  name << setw(10) << v.Type << endl;
        } else {
            out << setw(10) << "var" << setw(10) << name << setw(10)  << v.Type << endl;
        }
    }
}
void CodeGenerator::Main() {
    match("type"); match("id"); match("("); match(")"); match("{");
    Decla();
    Block();
    match("}");
}
// 声明
void CodeGenerator::Decla() {
    while(isNext("type")) {
        string Type = getToken().value;
        string var = "";
        while(!isNext(";")) {
            var = getToken().value;
            addVar(var, Type);
            if(isNext(";")) break;
            match(",");
        }
        if(var != "") addVar(var, Type);
        match(";");
    }
}
// 函数块
void CodeGenerator::Block() {
    while(!isNext("}")) {
        CodeToken T = getToken();
        string value = T.value;
        if(value == "if") {
            If();
        } else if(value == "while") {
            While();
        } else if(value == "for") {
            For();
        } else if(value == "read") {
            match("("); addQuad("read", getToken().value, "$", "$"); match(")"); match(";");
        } else if(value == "write") {
            match("("); addQuad("write", getToken().value, "$", "$"); match(")"); match(";");
        } else if(value == "return") {
            addQuad("return", itos(getToken().num), "$", "$");   match(";");
        } else if(CodeToString(T) == "id") {
            CurToken--;
            Assign();
        } else {
            puts("函数块错误！！"); printf("in %d", CurLine()); exit(0);
        }
    }
}
// For循环
void CodeGenerator::For() {
    match("(");
    // init
    Assign();
    // cond
    int Begin = CurLabel;
    string res = Expr();
    addQuad("J==", res, "1", itos(CurLabel + 2));
    int OUT = addQuad("J", "$", "$", "Unknow");
    match(";");
    string inc = getToken().value;
    string op = getToken().value;
    match(")");
    match("{");
    Block();
    // step
    addQuad(op, inc, "$", inc);
    addQuad("J", "$", "$", itos(Begin));
    QuadList[OUT].name_res = itos(CurLabel);
    match("}");
}
// while 循环
void CodeGenerator::While() {
    match("(");
    int Begin = CurLabel;
    string res = Expr();
    match(")");

    addQuad("J==", res, "1", itos(CurLabel + 2));
    int OUT = addQuad("J", "$", "$", "Unknow");
    match("{");
    Block();
    addQuad("J", "$", "$", itos(Begin));
    QuadList[OUT].name_res = itos(CurLabel);
    match("}");
}
void CodeGenerator::Assign() {
    string arg1 = getToken().value; match("=");
    string arg2 = Expr();
    addQuad("=", arg1, arg2, arg1);
    match(";");
}
void CodeGenerator::If() {
    match("("); string res = Expr(); match(")");

    match("{");
    int ET = addQuad("J==", res, "1", itos(CurLabel + 2));
    int EF = addQuad("J", "$", "$", "Unknow");
    Block();
    int OUT = addQuad("J", "$", "$", "Unknow");
    QuadList[EF].name_res = itos(CurLabel);
    match("}");
    if(isNext("else")) {
        match("else"); match("{");
        Block();
        match("}");
    }
    QuadList[OUT].name_res = itos(CurLabel);
}
// 表达式
string CodeGenerator::Expr() {
    if(Tokens[CurToken].Type == String) {
        string res = newTemp("string");
        newConst("string", Tokens[CurToken].value);
        CurToken++;
        return res;
    }
    int L = CurToken, R;
    int cntBrac = 0;
    for(R = L; R < Tokens.size(); R++) {
        string val = Tokens[R].value;
        if(val == ";" || val == ")" && cntBrac == 0) break;
        if(val == "(") cntBrac++;
        if(val == ")") cntBrac--;
    }
    CurToken = R;

    map<string, int> Prior;
    Prior["("] = 10, Prior[")"] = 0;
    Prior["!"] = 1;
    Prior["%"] = Prior["*"] = Prior["/"] = 2;
    Prior["+"] = Prior["-"] = 3;
    Prior[">="] = Prior["<="] = Prior[">"] = Prior["<"] = 4;
    Prior["=="] = Prior["!="] = 5;
    Prior["&&"] = 6;
    Prior["||"] = 7;

    stack<string> Num, Op;
    for(int i = L; i < R; i++) {
        string Type = CodeToString(Tokens[i]);
        string val = Tokens[i].value;
        if(Type == "number" || Type == "id") {
            if(Type == "id") Num.push(val);
            else Num.push(newConst((string)(Tokens[i].Type == Integer ? "int" : "double"), itos(Tokens[i].num)));
        } else {
            if(val == "(") {
                Op.push(val);
            } else if(val == ")") {
                while(Op.top() != "(") {
                    string op = Op.top(); Op.pop();
                    string y = Num.top(); Num.pop();
                    if(op == "!") {
                        string res = newTemp(VarTable[y].Type); Num.push(res);
                        addQuad(op, y, "$", res);
                        continue;
                    }
                    string x = Num.top(); Num.pop();
                    string res = newTemp(VarTable[x].Type); Num.push(res);
                    if(op == ">=" || op == "<=" || op == "==" || op == "!=" || op == ">" || op == "<") {
                        addQuad("J" + op, x, y, itos(CurLabel + 3));
                        addQuad("=", res, "0", res);
                        addQuad("J", "$", "$", itos(CurLabel + 2));
                        addQuad("=", res, "1", res);
                    } else {
                        addQuad(op, x, y, res);
                    }
                }
                Op.pop();
            } else {
                while(!Op.empty() && Prior[val] >= Prior[Op.top()]) {
                    string op = Op.top(); Op.pop();
                    string y = Num.top(); Num.pop();
                    if(op == "!") {
                        string res = newTemp(VarTable[y].Type); Num.push(res);
                        addQuad(op, y, "$", res);
                        continue;
                    }
                    string x = Num.top(); Num.pop();
                    string res = newTemp(VarTable[x].Type); Num.push(res);
                    if(op == ">=" || op == "<=" || op == "==" || op == "!=" || op == ">" || op == "<") {
                        addQuad("J" + op, x, y, itos(CurLabel + 3));
                        addQuad("=", res, "0", res);
                        addQuad("J", "$", "$", itos(CurLabel + 2));
                        addQuad("=", res, "1", res);
                    } else {
                        addQuad(op, x, y, res);
                    }
                }
                Op.push(val);
            }
        }
    }
    while(!Op.empty()) {
        string op = Op.top(); Op.pop();
        string y = Num.top(); Num.pop();
        if(op == "!") {
            string res = newTemp(VarTable[y].Type); Num.push(res);
            addQuad(op, y, "$", res);
            continue;
        }
        string x = Num.top(); Num.pop();
        string res = newTemp(VarTable[x].Type); Num.push(res);
        if(op == ">=" || op == "<=" || op == "==" || op == "!=" || op == ">" || op == "<") {
            addQuad("J" + op, x, y, itos(CurLabel + 3));
            addQuad("=", res, "0", res);
            addQuad("J", "$", "$", itos(CurLabel + 2));
            addQuad("=", res, "1", res);
        } else {
            addQuad(op, x, y, res);
        }
    }
    return Num.top();
}

string CodeGenerator::itos(double num) {
    string str;
    ostringstream os;
    os << num;
    str.append(os.str());
    return str;
}
string CodeGenerator::newConst(string Type, string value) {
    string con = "*" + itos(VarNum);
    addVar(con, Type, value);
    return con;
}
string CodeGenerator::newTemp(string Type) {
    string temp = "#" + itos(VarNum);
    addVar(temp, Type);
    return temp;
}
int CodeGenerator::CurLine() {
    return Tokens[CurToken].line;
}
CodeToken& CodeGenerator::getToken() {
    return Tokens[CurToken++];
}
string CodeGenerator::CodeToString(CodeToken & T) {
    if(T.Type == Integer || T.Type == Float) return "number";
    if(T.Type == Identifier) return "id";
    if(T.Type == String) return "string";
    if(T.Type == Keywords && isType.count(T.value)) return "type";
    return T.value;
}
bool CodeGenerator::isNext(string sym) {
    string Next = CodeToString(Tokens[CurToken]);
    if(Next == sym) return true;
    else return false;
}
void CodeGenerator::match(string sym) { // 匹配函数
    string Now = CodeToString(Tokens[CurToken]);
    if(Now == sym) {
        CurToken ++;
    } else {
        printf("in line [%d] : expect %s!\n", CurLine(), sym.c_str());
        exit(0);
    }
    //printf("in Line [%d]:%d  Match %s\n", CurLine(), CurToken, sym.c_str());
}
void CodeGenerator::addVar(string name, string type, string value) {
    VarTable.insert(make_pair(name, (Var) {
        type, name, VarNum++, value
    }));
}
bool CodeGenerator::isDefined(string name) {
    return name == "$" || name == "0" || name == "1" || VarTable.count(name) == 1;
}
int CodeGenerator::addQuad(string op, string name1, string name2, string res) {
    if(!isDefined(name1) || !isDefined(name2)) {
        isError = true;
        if(!isDefined(name1)) Errors.push_back({
            SemanticErrorType::UndeclaredSymbols, CurLine(), name1 + "该变量未定义"
        });
        if(!isDefined(name2)) Errors.push_back({
            SemanticErrorType::UndeclaredSymbols, CurLine(), name2 + "该变量未定义"
        });
        return CurLabel;
    }
    if(VarTable.count(name1) && VarTable.count(name2) && VarTable[name1].Type != VarTable[name2].Type) {
        isError = true;
        Errors.push_back({
            SemanticErrorType::TypeMismatch, CurLine(), VarTable[name1].Type + "与" + VarTable[name2].Type + "类型不匹配"
        });
        return CurLabel;
    }
    QuadList.push_back({op, name1, name2, res});
    return CurLabel++;
}

