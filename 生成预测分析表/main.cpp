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
map <string, vector<Express> > Grammar;
string start;

void InputGrammer(char *fileName) {
    ifstream fin(fileName, ios::in);
    string str;
    while(getline(fin, str)) {
        str += " |";
        stringstream line(str);
        string origin, sym;
        Express Ex;
        line >> origin;
        origin = origin.substr(1, origin.size() - 2);
        if(start.size() == 0) start = origin;
        line >> sym;
        while(line >> sym) {
            int len = sym.size();
            bool isTerminal;
            if(sym[0] == '<' && sym[len - 1] == '>') {
                sym = sym.substr(1, sym.size() - 2);
                isTerminal = false;
            } else {
                isTerminal = true;
            }
            if(sym == "|") {
                Grammar[origin].push_back(Ex);
                Ex.clear();
            } else {
                Ex.push_back({sym, isTerminal});
            }
        }
    }
}

void OutputGrammer() {
    cout << "原文法表示：" << endl;
    for(auto Grm : Grammar) {
        cout << Grm.first << " =" << endl;
        for(auto Exp : Grm.second) {
            cout << "  | ";
            for(auto Sym : Exp) {
                cout << Sym.value << " " ;
            }
            cout << endl;
        }
    }
    cout << endl;
}
/**
求First集的步骤：
1.若X->a..，则将终结符ａ加入FIRST(X)中;
2.若X->e ,则将终结符e加入FIRST(X)中(e表示空集)；
3.若 X->BC..D,则将First（B）所有元素（除了空集）加入First（A），然后检测First（B），若First（B）中不存在空集,
即e,则停止，若存在则向B的后面查看，将First（C）中所有元素（除了空集）加入First（A），然后再检测First（C）中
是否有e...直 到最后，若D之前的所有非终结符的First集中都含有e,则检测到D时，将First（D）也加入First（A），若
First（D）中含有e,则将 e加入First（A）。
*/

map <string, set<string> > FIRST, FOLLOW;

void getFIRST() {
    bool isUpdated;
    do {
        isUpdated = false;
        for(auto grm : Grammar) {
            string X = grm.first;
            int sz = FIRST[X].size();
            for(auto ex : grm.second) {
                if(ex[0].isTerminal == true) {
                    FIRST[X].insert(ex[0].value);
                } else {
                    bool didAll = true;
                    for(auto sym : ex) {
                        for(auto it : FIRST[sym.value]) {
                            if(it == "$") continue;
                            FIRST[X].insert(it);
                        }
                        if(sym.isTerminal == true || !FIRST[sym.value].count("$")) {
                            didAll = false;
                            break;
                        }
                    }
                    if(didAll) FIRST[X].insert("$");
                }
            }
            if(sz != FIRST[X].size()) isUpdated = true;
        }
    } while(isUpdated == true);

    cout << "FIRST集合：" << endl;
    for(auto it : FIRST) {
        cout << it.first << ": ";
        for(auto x : it.second) {
            cout << x << ", ";
        }
        cout << endl;
    }
    cout << endl;
}
/**
求Follow集的步骤：
1.对文法开始符号S,置$于FOLLOW(S)中；
2.对于产生式：A->...BC,将除去空集e的First（C）加入Follow（B）中;
3.对于产生式：A->aB或者A->aBC,(其中C可以推导出空串，C=>*e),则将Follow（A）加入Follow（B）中。
*/
void getFOLLOW(string start) {
    FOLLOW[start].insert("#");
    bool isUpdated;
    do {
        isUpdated = false;
        for(auto grm : Grammar) {
            string X = grm.first;
            for(auto ex : grm.second) {
                int n = ex.size();
                bool isEmpty = true;
                for(int i = n - 1; i >= 0; i--) {
                    string A = ex[i].value;
                    if(ex[i].isTerminal) {
                        isEmpty = false;
                        continue;
                    }
                    int sz = FOLLOW[A].size();
                    if(isEmpty) {
                        for(auto it : FOLLOW[X]) {
                            FOLLOW[A].insert(it);
                        }
                    }
                    if(!FIRST[A].count("$")) {
                        isEmpty = false;
                    }
                    if(i == n - 1) continue;
                    string B = ex[i + 1].value;
                    if(ex[i + 1].isTerminal == true) {
                        FOLLOW[A].insert(B);
                    } else {
                        int j = i + 1;
                        for(auto it : FIRST[ex[j].value]) {
                            if(it == "$") continue;
                            FOLLOW[A].insert(it);
                        }
                        while(j + 1 < n && FIRST[ex[j].value].count("$")) {
                            string C = ex[j + 1].value;
                            for(auto it : FIRST[C]) {
                                if(it == "$") continue;
                                FOLLOW[A].insert(it);
                            }
                            j++;
                        }
                    }
                    if(sz != FOLLOW[A].size()) isUpdated = true;
                }
            }
        }
    } while(isUpdated == true);

    cout << "FOLLOW集合：" << endl;
    for(auto it : FOLLOW) {
        cout << it.first << ": ";
        for(auto x : it.second) {
            cout << x << ", ";
        }
        cout << endl;
    }
    cout << endl;
}

map< pair<string, Express>, set<string> > SELECT;

void getSELECT() {
    for(auto grm : Grammar) {
        string X = grm.first;
        for(auto ex : grm.second) {
            pair<string, Express> Y = {X, ex};
            string A = ex[0].value;
            if(ex[0].isTerminal) {
                if(A == "$") {
                    for(auto x : FOLLOW[X]) {
                        SELECT[Y].insert(x);
                    }
                } else {
                    SELECT[Y].insert(A);
                }
            } else {

                bool didAll = true;
                for(auto sym : ex) {
                    string B = sym.value;
                    for(auto it : FIRST[B]) {
                        if(it == "$") continue;
                        SELECT[Y].insert(it);
                    }
                    if(!FIRST[B].count("$")) {
                        didAll = false;
                        break;
                    }
                }
                if(!didAll) continue;
                for(auto x : FOLLOW[A]) {
                    SELECT[Y].insert(x);
                }

            }
        }
    }

    cout << "SELECT集合：" << endl;
    for(auto it : SELECT) {
        cout << it.first.first << "->";
        for(auto sym : it.first.second) {
            cout << sym.value;
        }
        cout << ": ";
        for(auto x : it.second) {
            cout << x << ", ";
        }
        cout << endl;
    }
    cout << endl;
}
bool checkSELECT() {
    map<string, set<string> > has;
    for(auto it : SELECT) {
        string A = it.first.first;
        for(auto x : it.second) {
            if(has[A].count(x)) return false;
            has[A].insert(x);
        }
    }
    return true;
}

Express AnalyseTable[100][100];
string toX[100], toY[100];
map<string, int> IDX, IDY;
int n = 0, m = 0;

void makeAnalyseTable() {
    for(auto it : SELECT) {
        string X = it.first.first;
        if(!IDX[X]) {
            toX[n + 1] = X;
            IDX[X] = ++n;
        }
        int i = IDX[X];
        Express E = it.first.second;
        for(auto Y : it.second) {
            if(!IDY[Y]) {
                toY[m + 1] = Y;
                IDY[Y] = ++m;
            }
            int j = IDY[Y];
            AnalyseTable[i][j] = E;
        }
    }

    cout << "LL(1)文法预测分析表: " << endl;
    for(int i = 1; i <= m; i++) {
        printf("%8s", toY[i].c_str());
    }
    cout << endl;
    for(int i = 1; i <= n; i++) {
        printf("%-7s  ", toX[i].c_str());
        for(int j = 1; j <= m; j++) {
            string str;
            for(auto x : AnalyseTable[i][j]) {
                str += x.value ;
            }
            printf("%-8s  ", str.c_str());
        }
        cout << endl;
    }
}
void OutputAnalyseTableToFile(char *fileName) {
    ofstream out(fileName, ios::out);
    out << start << endl;
    for(int i = 1; i <= n; i++) {
        for(int j = 1; j <= m; j++) {
            if(AnalyseTable[i][j].size() == 0) continue;
            out << toX[i] << " " << toY[j] << " " ;
            for(auto it : AnalyseTable[i][j]) {
                if(!it.isTerminal) out << "<" << it.value << "> " ;
                else               out << it.value << " ";
            }
            out << endl;
        }
    }
}
void OutputFollowToFile(char *fileName) {
    ofstream out(fileName, ios::out);
    for(auto x : FOLLOW) {
        out << x.first << " ";
        for(auto y : x.second) {
            out << y << " ";
        }
        out << endl;
    }
}
int main() {
    freopen("out.txt", "w", stdout);
    InputGrammer("in.txt");
    OutputGrammer();

    getFIRST();
    getFOLLOW(start);
    getSELECT();

    if(!checkSELECT()) {
        cout << "该文法不是LL(1)的" << endl;
    } else {
        makeAnalyseTable();
    }
    OutputFollowToFile("FOLLOW.txt");
    OutputAnalyseTableToFile("table.txt");

    return 0;
}
