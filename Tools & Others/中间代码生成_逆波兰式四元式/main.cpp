#include <bits/stdc++.h>
using namespace std;

int Prior[128];    // 优先级数组

// 四元式
struct Quadruple {
    char op, x, y, res;    // 操作符， 操作数x，操作数y， 结果res
    // 构造函数
    Quadruple(char O, char A, char B, char R) {
        op = O, x = A, y = B, res = R;
    }
    // 输出
    void output() {
        printf("(%c, %c, %c, %c)\n", op, x, y, res);
    }
};

vector <Quadruple> QuadList; // 四元式序列

void initPrior() {          // 初始化优先级
    Prior['('] = 1, Prior[')'] = 10;
    Prior['+'] = Prior['-'] = 2;
    Prior['*'] = Prior['/'] = 3;
}
void getQuadList(vector<char> &RPN) { // 逆波兰式生成四元式序列
    QuadList.clear();
    stack <char> Num;
    char id = 'A';
    for(int i = 0; i < RPN.size(); i++) {
        char c = RPN[i];
        if(isalnum(c)) {
            Num.push(c);
        } else {
            char x = Num.top();
            Num.pop();
            char y = Num.top();
            Num.pop();
            QuadList.push_back((Quadruple) {
                c, y, x, id
            });
            Num.push(id);
            id++;
        }
    }

    cout << "四元式：" << endl;
    for(Quadruple x : QuadList) x.output();
}
void Analysis(string& str) {    // 中缀转逆波兰式
    vector <char> NumStack, OpStack;
    for(int i = 0; i < str.size(); i++) {
        char c = str[i];
        if(isalnum(c)) {
            NumStack.push_back(c);
        } else {
            if(c == '(') {
                OpStack.push_back(c);
            } else if(c == ')') {
                while(OpStack.back() != '(') {
                    NumStack.push_back(OpStack.back());
                    OpStack.pop_back();
                }
                OpStack.pop_back();
            } else {
                while(!OpStack.empty() && Prior[c] <= Prior[OpStack.back()]) {
                    NumStack.push_back(OpStack.back());
                    OpStack.pop_back();
                }
                OpStack.push_back(c);
            }
        }
    }
    while(!OpStack.empty()) {
        NumStack.push_back(OpStack.back());
        OpStack.pop_back();
    }

    cout << "逆波兰式：" << endl;
    for(char c : NumStack) cout << c;
    cout << endl;

    getQuadList(NumStack);
}

int main() {
    freopen("in.txt", "r", stdin);
    string str;     // 输入的表达式串
    initPrior();    // 设置优先级
    while(cin >> str) {
        cout << "中缀表达式：\n" << str << endl;
        Analysis(str);
        cout << endl;
    }
    return 0;
}
