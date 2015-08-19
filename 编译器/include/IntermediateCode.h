#ifndef INTERMEDIATECODE_H
#define INTERMEDIATECODE_H
#include "GrammarAnalyzer.h"

/*�������*/
enum class SemanticErrorType {
    UndeclaredSymbols,  // ����δ����
    TypeMismatch,       // ���Ͳ�ƥ��
};
struct SemanticError {
    SemanticErrorType Type;
    int line = -1;
    string value;
    SemanticError(SemanticErrorType S, int l, string v): Type(S), line(l), value(v) {}
    void output() {
        printf("�������in line[%d]��%s��\n", line, value.c_str());
    }
};
/* ��Ԫʽ���� */
struct Quadruple {
    string op;
    string name1, name2, name_res;
    Quadruple(string o, string n1,  string n2, string ns) {
        op = o, name1 = n1, name2 = n2, name_res = ns;
    }
    friend ostream& operator << (ostream &out, Quadruple &Q) {
        out << "(" << Q.op << ", " << Q.name1 << ", " << Q.name2 << ", " << Q.name_res << ")";
        return out;
    }
};
/* �������� */
struct Var {
    string Type;
    string Name;
    string value;
    int idx;
    Var() {}
    Var(string T, string N, int id, string v = ""): Type(T), Name(N), idx(id), value(v) {}
};
/* �������� */
class CodeGenerator {
public:
    vector <SemanticError>  Errors;
    vector <Quadruple> QuadList;
    map<string, Var> VarTable;
    TokenList& Tokens;
    int CurLabel = 0;
    int CurToken = 0;
    int VarNum = 0;
    bool isError = false;
    void OutputAsm(char * FileName);
    void OutputQuadList(char *FileName);
    void OutputVarTable(char *FileName);
    CodeGenerator(TokenList &L);
private:
    void Main();
    void Decla();
    void Block();
    void For();
    void While();
    void Assign();
    void If();
    string Expr();
    string itos(double num);
    string newConst(string Type, string value);
    string newTemp(string Type);
    int CurLine();
    CodeToken& getToken();
    string CodeToString(CodeToken & T);
    bool isNext(string sym);
    void match(string sym);
    void addVar(string name, string type, string value = "");
    bool isDefined(string name);
    int addQuad(string op, string name1, string name2, string res);
};

#endif // INTERMEDIATECODE_H
