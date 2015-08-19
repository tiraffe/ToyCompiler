#include <bits/stdc++.h>
using namespace std;

int Prior[128];    // ���ȼ�����

// ��Ԫʽ
struct Quadruple {
    char op, x, y, res;    // �������� ������x��������y�� ���res
    // ���캯��
    Quadruple(char O, char A, char B, char R) {
        op = O, x = A, y = B, res = R;
    }
    // ���
    void output() {
        printf("(%c, %c, %c, %c)\n", op, x, y, res);
    }
};

vector <Quadruple> QuadList; // ��Ԫʽ����

void initPrior() {          // ��ʼ�����ȼ�
    Prior['('] = 1, Prior[')'] = 10;
    Prior['+'] = Prior['-'] = 2;
    Prior['*'] = Prior['/'] = 3;
}
void getQuadList(vector<char> &RPN) { // �沨��ʽ������Ԫʽ����
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

    cout << "��Ԫʽ��" << endl;
    for(Quadruple x : QuadList) x.output();
}
void Analysis(string& str) {    // ��׺ת�沨��ʽ
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

    cout << "�沨��ʽ��" << endl;
    for(char c : NumStack) cout << c;
    cout << endl;

    getQuadList(NumStack);
}

int main() {
    freopen("in.txt", "r", stdin);
    string str;     // ����ı��ʽ��
    initPrior();    // �������ȼ�
    while(cin >> str) {
        cout << "��׺���ʽ��\n" << str << endl;
        Analysis(str);
        cout << endl;
    }
    return 0;
}
