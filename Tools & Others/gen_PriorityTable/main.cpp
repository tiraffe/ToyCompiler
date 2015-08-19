#include <bits/stdc++.h>
using namespace std;

stack <pair<char, char> > S;
bool F[128][128], vis[128];
bool isOP[128];
vector<string> express[128], Re_express[128];
vector<char> FIRSTVT[128], LASTVT[128];
char PriorTable[128][128];

/***************** 生成FIRSTVT, LASTVT集合 ************************/
void Insert(char A, char a) {
    if(!F[A][a]) {
        F[A][a] = true;
        S.push(make_pair(A, a));
    }
}
void solve(vector<string> express[], vector<char> ans[]) {
    memset(F, 0, sizeof(F));
    while(!S.empty()) S.pop();

    for(int i = 0; i < 128; i++) {
        for(auto it : express[i]) {
            if(!isupper(it[0]))     Insert(i, it[0]);
            else if(it[1] != '\0')  Insert(i, it[1]);
        }
    }
    while(!S.empty()) {
        pair<char, char> P = S.top();
        S.pop();
        char B = P.first, a = P.second;
        for(int i = 0; i < 128; i++) {
            for(auto it : express[i]) {
                if(it[0] == B)  Insert(i, a);
            }
        }
    }
    for(int i = 0; i < 128; i++) {
        if(vis[i]) {
            cout << (char)i << ": ";
            for(int j = 0; j < 128; j++) {
                if(F[i][j]) {
                    ans[i].push_back(j);
                    cout << char(j) << ", ";
                }
            }
            cout << endl;
        }
    }
    cout << endl;
}

/***************** 生成算符优先级表格 ************************/
bool checkOPG(char a, char b, char op) {
    char& A = PriorTable[a][b];
    if(A != 0 && A != op) {
        return false;
    } else {
        A = op;
        return true;
    }
}
bool creatPriorTable() {
    for(int k = 0; k < 128; k++) {
        for(auto str : express[k]) {
            int n = str.size();
            if(!isupper(str[0])) isOP[str[0]] = true;
            for(int i = 0; i < n - 1; i++) {
                if(!isupper(str[i + 1])) isOP[str[i + 1]] = true;

                if(i < n - 1 && !isupper(str[i]) && !isupper(str[i + 1])) {
                    if(!checkOPG(str[i], str[i + 1], '=')) return false;
                }
                if(i < n - 2 && !isupper(str[i]) && !isupper(str[i + 2]) && isupper(str[i + 1])) {
                    if(!checkOPG(str[i], str[i + 2], '=')) return false;
                }
                if(!isupper(str[i]) && isupper(str[i + 1]) ) {
                    for(auto b : FIRSTVT[str[i + 1]]) {
                        if(!checkOPG(str[i], b, '<')) return false;
                    }
                }
                if(isupper(str[i]) && !isupper(str[i + 1])) {
                    for(auto a : LASTVT[str[i]]) {
                        if(!checkOPG(a, str[i + 1], '>')) return false;
                    }
                }
            }
        }
    }
    return true;
}
void outputPriorTable() {
    printf("算符优先关系矩阵:\n");
    for(int i = 0; i < 128; i++) {
        if(isOP[i]) printf("%8c", (char)i);
    }
    printf("\n");
    for(int i = 0; i < 128; i++) {
        if(!isOP[i]) continue;
        printf("%-7c", (char)i);
        for(int j = 0; j < 128; j++) {
            if(!isOP[j]) continue;
            printf("%-8c", PriorTable[i][j] == 0 ? ' ' : PriorTable[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}
/***************** 生成算符优先函数 ************************/
int fx[128], gx[128];

void dfs(int u, bool G[256][256], int &cnt, bool vis[]) {
    for(int v = 0; v < 256; v++) {
        if(!vis[v] && G[u][v]) {
            vis[v] = 1;
            dfs(v, G, ++cnt, vis);
        }
    }
}
bool checkPriorFunction() {
    for(int i = 0; i < 128; i++) {
        for(int j = 0; j < 128; j++) {
            if(PriorTable[i][j] == '=' && fx[i] != gx[j]) return false;
            if(PriorTable[i][j] == '>' && fx[i] <= gx[j]) return false;
            if(PriorTable[i][j] == '<' && fx[i] >= gx[j]) return false;
        }
    }
    return true;
}
bool getPriorFunction() {
    bool G[256][256] = {0};
    for(int i = 0; i < 128; i++) {
        for(int j = 0; j < 128; j++) {
            if(PriorTable[i][j] == '>' || PriorTable[i][j] == '=') {
                G[i][j + 128] = 1;
            }
            if(PriorTable[i][j] == '<' || PriorTable[i][j] == '=') {
                G[j + 128][i] = 1;
            }
        }
    }
    int cnt;
    bool vis[256];
    for(int i = 0; i < 128; i++) {
        if(!isOP[i]) continue;
        memset(vis, 0 , sizeof(vis));
        cnt = 1;
        vis[i] = 1;
        dfs(i, G, cnt, vis);
        fx[i] = cnt;
    }
    for(int i = 128; i < 256; i++) {
        if(!isOP[i % 128]) continue;
        memset(vis, 0 , sizeof(vis));
        cnt = 1;
        vis[i] = 1;
        dfs(i, G, cnt, vis);
        gx[i % 128] = cnt;
    }
    return checkPriorFunction();
}
void outputPriorFunction() {
    printf("算符优先函数表:\n");
    for(int i = 0; i < 128; i++) {
        if(isOP[i]) printf("%8c", (char)i);
    }
    printf("\nf[] :  ");
    for(int i = 0; i < 128; i++) {
        if(isOP[i]) {
            printf("%-8d", fx[i]);
        }
    }
    printf("\ng[] :  ");
    for(int i = 0; i < 128; i++) {
        if(isOP[i]) {
            printf("%-8d", gx[i]);
        }
    }
    printf("\n");
}
/***************** 主程序 ************************/
bool checkOG(string& A) {
    for(int i = 1; i < A.size(); i++) {
        if(isupper(A[i]) && isupper(A[i - 1])) return false;
    }
    return true;
}
int main() {
#ifdef TYH
    freopen("in.txt", "r", stdin);
    freopen("out.txt", "w", stdout);
#endif // TYH
    memset(F, 0, sizeof(F));
    char origin;
    string str;
    bool isOG = true;
    while(cin >> origin) {
        cin >> str;
        isOG &= checkOG(str);
        express[origin].push_back(str);
        reverse(str.begin(), str.end());
        Re_express[origin].push_back(str);
        vis[origin] = 1;
    }
    if(!isOG) {
        puts("该文法不是算符文法。");
        return 0;
    }

    cout << "FIRSTVT:" << endl;
    solve(express, FIRSTVT);
    cout << "LASTVT:" << endl;
    solve(Re_express, LASTVT);

    if(creatPriorTable()) {
        outputPriorTable();
    } else {
        cout << "该文法不是算符优先文法。" << endl;
        return 0;
    }

    if(getPriorFunction()) {
        outputPriorFunction();
    } else {
        cout << "该文法不存在优先函数。" << endl;
    }
    return 0;
}
