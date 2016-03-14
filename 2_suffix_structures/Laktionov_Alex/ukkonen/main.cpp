#include <iostream>
#include <cstring>
#include <vector>
#include <algorithm>
#include <array>

using namespace std;

const int N = 10000;
const int ALPHABET = 30;

struct Pos{
    int finish;
    int fromStart;
    Pos (int finish, int fromStart) : finish(finish), fromStart(fromStart) {};
    Pos() = default;
};

struct Node{
    int parent;
    int leftInString;
    int rightInString;
    int suffixLink = -1;
    array<int, ALPHABET> go;
    Node (int l, int r, int parent) : leftInString(l), rightInString(r), parent(parent) {};
    Node() = default;
};

class Ukkonen{
    
public:
    
    Ukkonen(int k) : k(k) {};
    
    void buildTree();
    
    long long solver();
    
    void setInp(char* inp);
    
private:
    
    array<Node, N> st;
    int stSize = 0;
    char* s;
    int k;
    
    inline int getLength(int v){
        return st[v].rightInString - st[v].leftInString;
    }
    
    Pos jumpOnce(Pos p, int i);
    
    Pos jump(int v, int l, int r);
    
    int addChild(int l, int r, int parent);
    
    int split(Pos pos);
    
    int getSuffixLink(int v);
    
    Pos addLetter(Pos pos, int i);
};

void Ukkonen::buildTree(){
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < ALPHABET; ++j)
            st[i].go[j] = -1;
    stSize = 1;
    st[0].suffixLink = 0;
    st[0].parent = -1;
    Pos pos(0, 0);
    for (int i = 0; i < k; ++i){
        pos = addLetter(pos, i);
    }
}

long long Ukkonen::solver(){
    long long ans = 0;
    for(int i = 0; i < stSize; i++){
        ans += (long long)min(k - st[i].leftInString, getLength(i));
        for (int j = 0; j < ALPHABET; ++j)
            st[i].go[j] = -1;
    }
    stSize = 0;
    
    return ans;
}

void Ukkonen::setInp(char* inp) {
    s = inp;
}

Pos Ukkonen::jumpOnce(Pos p, int i){
    char c = s[i];
    int v = p.finish, fromStart = p.fromStart, up = getLength(v) - fromStart;
    if (fromStart != 0 && up != 0){
        int index = st[v].leftInString + fromStart;
        if (s[index] == c){
            return Pos(v, fromStart + 1);
        }
        return Pos(-1, -1);
    }else{
        if (up == 0){
            if (st[v].go[s[i] - 'a'] != -1){
                return Pos(st[v].go[s[i] - 'a'], 1);
            }
            return Pos(-1, -1);
        }
        if (st[st[v].parent].go[c - 'a'] != -1){
            return Pos(st[st[v].parent].go[c - 'a'], 1);
        }
    }
    return Pos(-1, -1);
}

Pos Ukkonen::jump(int v, int l, int r){
    while(l < r){
        int to = st[v].go[s[l] - 'a'];
        if (l + getLength(to) > r){
            return Pos(to, r-l);
        }
        v = to;
        l += getLength(to);
    }
    return Pos(v, getLength(v));
}

int Ukkonen::addChild(int l, int r, int parent){
    st[stSize] = Node(l, r, parent);
    st[parent].go[s[l] - 'a'] = stSize;
    ++stSize;
    return (stSize - 1);
}

int Ukkonen::split(Pos pos){
    int v = pos.finish, fromStart = pos.fromStart, up = getLength(v) - fromStart;
    if (up == 0){
        return v;
    }
    if (fromStart == 0){
        return st[v].parent;
    }
    int mid = addChild(st[v].leftInString, st[v].leftInString + fromStart, st[v].parent);
    st[v].leftInString += fromStart;
    st[v].parent = mid;
    st[mid].go[s[st[v].leftInString] - 'a'] = v;
    return mid;
}

int Ukkonen::getSuffixLink(int v){
    if (st[v].parent == -1)
        return 0;
    if (st[v].suffixLink == -1){
        int to = getSuffixLink (st[v].parent);
        int parent = st[v].parent;
        int rootShift = 0;
        if (parent == 0)
            rootShift = 1;
        st[v].suffixLink = split(jump(to, st[v].leftInString + rootShift, st[v].rightInString));
    }
    return st[v].suffixLink;
}

Pos Ukkonen::addLetter(Pos pos, int i){
    while(true){
        Pos to = jumpOnce(pos, i);
        if (to.finish != -1){
            return to;
        }
        int mid = split (pos);
        addChild(i, k, mid);
        pos.finish = getSuffixLink (mid);
        pos.fromStart = getLength(pos.finish);
        if (mid == 0){
            return pos;
        }
        
    }
    return pos;
}


int main() {
    
    // запихиваем на тимус: одно и то же дерево
    // для каждого окна шириной в k,
    // на каждом новом окне используем тот же
    // массив из дерева, но говорим, что его
    // размер = 0, переходы из всех вершин
    // обнуляем (:= -1)
    
    // в дерево передаётся указатель на начало
    // подстроки, которая входит в текущее
    // окно (дерево знает k - ширину окна, то
    // есть знает строку без её копирования и
    // с удобной 0-индексацией без шифтов)
    
    // все комменты стали кракозябрами, когда
    // залил на тимус, потерял прогу и достал
    // её оттуда - поэтому напишу их позже :)
    // все ассерты тоже убраны для запихивания
    
    int k;
    cin >> k;
    
    char* input = (char*)malloc(N);
    scanf("%s", input);
    
    size_t length = strlen(input);
    char* tail = (char*)malloc(k + 1);
    strncpy(tail, input, k);
    strcat(input, tail);
    
    Ukkonen ut(k);
    
    for (size_t start = 0; start < length; ++start){
        ut.setInp(input + start);
        ut.buildTree();
        cout << ut.solver() << " ";
    }
    
    
    return 0;
}

//тимус зло ><