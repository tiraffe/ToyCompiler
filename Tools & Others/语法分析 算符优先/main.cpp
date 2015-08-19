#include <bits/stdc++.h>
using namespace std;

int f[128], g[128];
string R[3] = {"F+F", "i", "F*F"};
bool Reduction(int left, int right, char *S) {
    string str;
    for(int i = left; i <= right; i++) {
        str += isupper(S[i]) ? 'F' : S[i];
    }
    for(int i = 0; i < 3; i++) {
        if(R[i] == str) return true;
    }
    return false;
}
void initPrior() {
    f['i'] = 6; f['*'] = 6; f['+'] = 4; f['#'] = 2;
    g['i'] = 7; g['*'] = 5; g['+'] = 3; g['#'] = 2;
}
int cmpPrior(char a, char b) {
    return f[a] - g[b];
}
bool doAnalyse(string str) {
    char S[1005];
    int top = 1;
    S[top] = '#';
    for(int x = 0; x < str.size();) {
        char a = str[x];
        S[top + 1] = 0;
        printf("Õ»¶¥·ûºÅ£º%-10s µ±Ç°·ûºÅ£º%c \n", S + 1, a);
        int j = isupper(S[top]) ? top - 1 : top;
        if(cmpPrior(S[j], a) > 0) {
            char Q;
            do {
                Q = S[j];
                j = isupper(S[j - 1]) ? j - 2 : j - 1;
            } while(cmpPrior(S[j], Q) >= 0);
            if(Reduction(j + 1, top, S)) {
                top = j + 1;
                S[top] = 'F';
            } else return false;
        } else if(cmpPrior(S[j], a) < 0) {
            S[++top] = a;
            x++;
        } else if(cmpPrior(S[j], a) == 0) {
            if(S[j] == '#') {
                if(j == 1 && a == '#') {
                    return true;
                } else {
                    return false;
                }
            } else {
                S[++top] = a;
                x++;
            }
        } else {
            return false;
        }
    }
    return false;
}
int main() {
    string str;
    initPrior();
    while(cin >> str) {
        str += '#';
        if(doAnalyse(str)) {
            puts("Yes");
        } else {
            puts("No");
        }
    }
    return 0;
}
