#include <bits/stdc++.h>
using namespace std;

// �������
struct Symbol {
    string value;
    bool isTerminal;        // �Ƿ�Ϊ�ս��

    Symbol(string v, bool T): value(v), isTerminal(T) {}
    bool operator < (const Symbol& B) const {
        return value < B.value;
    }
    bool operator == (const Symbol& B) const {
        return value == B.value;
    }
};
// �������ʽ�Ҳ�
typedef vector<Symbol> Express;
ostream& operator << (ostream& out, Express& E) {
    for(Symbol s : E) {
        if(s.isTerminal) out << s.value << " " ;
        else out << "<" << s.value << "> ";
    }
    return out;
}
// ������Ŀ
struct Item {
    int dot = 0;            //��Ŀ���λ��
    string left;            //��Ŀ��
    Express express;        //��Ŀ�Ҳ�
    Item() {}
    Item(int d, string l, Express e): dot(d), left(l), express(e) {}
    bool isEnd() {          //��Ŀ�����ڽ�β
        return express.size() == dot || express[0].value == "$";
    }
    void debug() {
        cout << left << " --> ";
        for(int i = 0; i < express.size(); i++) {
            if(i == dot && !isEnd()) cout << ". " ;
            cout << express[i].value << " ";
        }
        if(isEnd()) cout << ". " ;
        cout << endl;
    }
    bool operator < (const Item& B) const {
        return express < B.express || (express == B.express && dot < B.dot);
    }
    bool operator == (const Item& B) const {
        return left == B.left && express == B.express && dot == B.dot;
    }
    bool operator != (const Item& B) const {
        return !(*this == B);
    }
};

//������Ŀ����
typedef set<Item> ItemSet;
bool operator == (ItemSet& A, ItemSet& B) {
    for(auto a = A.begin(), b = B.begin(); a != A.end() && b != B.end(); a++, b++) {
        if(*a != *b) return false;
    }
    return true;
}

//������ת�ƶ����Ķ���
enum ActionType {
    Reduction,  //��Լ
    Move,       //�ƽ�
    Accept      //����
};
struct Action {
    ActionType Type;
    int value;
    Action() {}
    Action(ActionType T, int v): Type(T), value(v) {}
};

map <string, set<Item> > Grammar;               //�ķ�
vector<ItemSet> Index_ItemSet;                  //��Ŀ���������������ϵ���Ŀ��
vector<ItemSet> Index_Closure;                  //��Ŀ���հ�����������������Ŀ��
vector<Item> Index_Item;                        //��Ŀ��Լ����
map<int, map<string, Action> > AnalysisTable;   //������

string start;                                   //��ʼ��
Item  BeginItem, EndItem;                       //��ʼ��Ŀ������ֹ��Ŀ��
map <string, set<string> > FIRST, FOLLOW;

void readGrammer(char *fileName) {
    ifstream in(fileName, ios::in);
    string str;
    while(getline(in, str)) {
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
                if(sym == "$") sym = "$";
                isTerminal = true;
            }
            if(sym == "|") {
                Item it(0, origin, Ex);
                Grammar[origin].insert(it);
                Index_Item.push_back(it);
                Ex.clear();
            } else {
                Ex.push_back({sym, isTerminal});
            }
        }
    }
    Express B;
    B.push_back({start, 0});
    BeginItem = {0, "Start", B};
    EndItem = {1, "Start", B};
}
void getFIRST() {
    bool isUpdated;
    do {
        isUpdated = false;
        for(auto grm : Grammar) {
            string X = grm.first;
            int sz = FIRST[X].size();
            for(auto I : grm.second) {
                Express ex = I.express;
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
}
void getFOLLOW(string start) {

    getFIRST();
    FOLLOW[start].insert("#");
    bool isUpdated;
    do {
        isUpdated = false;
        for(auto grm : Grammar) {
            string X = grm.first;
            for(auto I : grm.second) {
                Express ex = I.express;
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
}
/********************************************** Ԥ��������� *****************************************************************/

int getPosition(Item x) { //�õ���Ŀx����Ŀ�����еı��
    int n = Index_Item.size();
    x.dot = 0;
    for(int i = 0 ; i < n; i++) {
        if(x == Index_Item[i]) return i;
    }
    exit(-1);
}
int getPosition(ItemSet x) { //�õ���Ŀ����x������Ŀ���������еı��
    int n = Index_ItemSet.size();
    for(int i = 0 ; i < n; i++) {
        if(x == Index_ItemSet[i]) return i;
    }
    return -1;
}
void checkConflict(ItemSet& S) {
    set <string> visited;
    // �ս���������ɵļ���
    for(Item it : S) {
        if(it.isEnd()) continue;
        string sym = it.express[it.dot].value;
        visited.insert(sym);
    }
    // �ж� �ս����FOLLOW[] �޽���
    for(Item it : S) {
        if(!it.isEnd()) continue;
        for(string sym : FOLLOW[it.left]) {
            if(!visited.count(sym)) {
                visited.insert(sym);
            } else {
                printf("�ƽ����Լ�����ĳ�ͻ�޷����⣬��˸��ķ�����SLR(1)\n");
                exit(0);
            }
        }
    }
}

void getClosure(int pos) {
    cout << "\n------getClosure(" << pos << ")  -->  begin------ \n" << endl;

    ItemSet& X = Index_ItemSet[pos];
    ItemSet Closure;
    Closure.clear();

    queue <string> UpdateQue;
    set <string> visited;

    // ����ǰ�Ŀ��ƽ��ķ��ս��������¶���
    cout << "����ǰ:" << endl;
    for(Item it : X) {
        Closure.insert(it);
        it.debug();
        if(it.isEnd() || it.express[it.dot].isTerminal == true) continue;
        string afterDot =  it.express[it.dot].value;
        if(!visited.count(afterDot)) {
            visited.insert(afterDot);
            UpdateQue.push(afterDot);
        }
    }

    // ����Ŀ���հ�
    while(!UpdateQue.empty()) {
        string u = UpdateQue.front();
        UpdateQue.pop();
        for(Item it : Grammar[u]) {
            Closure.insert(it);
            if(it.express[it.dot].isTerminal == true) continue;
            string afterDot =  it.express[it.dot].value;
            if(!visited.count(afterDot)) {
                visited.insert(afterDot);
                UpdateQue.push(afterDot);
            }
        }
    }
    cout << "�����" << endl ;
    for(auto x : Closure) {
        x.debug();
    }
    Index_Closure.push_back(Closure);
    checkConflict(Closure);

    // DFS��չ�µ���Ŀ��
    map <Symbol, set<Item> > to; //��Ҫ��չ����Ŀ��
    for(Item it : Closure) {
        if(!it.isEnd()) {
            Symbol afterdot = it.express[it.dot];
            to[afterdot].insert(it);
        } else {
            string L = it.left;
            if(it == EndItem) {
                AnalysisTable[pos]["#"] = (Action) {
                    Accept, -1
                };
                continue;
            }
            for(string s : FOLLOW[L]) {
                AnalysisTable[pos][s] = (Action) {
                    Reduction, getPosition(it)
                };
            }
        }
    }
    for(auto P : to) {
        Symbol afterdot = P.first;
        ItemSet temp;
        for(Item x : P.second) {
            temp.insert((Item) {
                x.dot + 1, x.left, x.express,
            });
        }
        int id = getPosition(temp);
        if(id == -1) { //����Ŀ����δ��չ��
            id = Index_ItemSet.size();
            Index_ItemSet.push_back(temp);
            getClosure(id);
        }
        AnalysisTable[pos][afterdot.value] = (Action) {
            Move, id
        };
    }
}
void OutputAnalysisTable(char *fileName) {
    ofstream out(fileName, ios::out);
    for(auto P : AnalysisTable) {
        for(auto PP : P.second) {
            out << P.first << " " ;
            out << PP.first << " ";
            Action x = PP.second;
            out << "\t" << x.Type << " " << x.value << endl;
        }
    }
}
void OutputItem(char *fileName) {
    ofstream out(fileName, ios::out);
    for(Item it : Index_Item) {
        out << "<" << it.left << "> -> " << it.express << endl;
    }
}
void genAnalysisTable() {
    ItemSet BeginSet;
    BeginSet.insert(BeginItem);
    BeginItem.debug();
    Index_ItemSet.push_back(BeginSet);
    getClosure(0);
    cout << endl;
    cout << "genAnalysisTable()  ->  " << "done" << endl;
}

int main() {
    freopen("log.txt", "w", stdout);

    readGrammer("in.txt");
    getFOLLOW(start);
    genAnalysisTable();
    OutputAnalysisTable("table.txt");
    OutputItem("index.txt");
    return 0;
}
