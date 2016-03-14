#include <string>
#include <vector>
#include <iostream>
#include <assert.h>
#include <algorithm>

using namespace std;

struct LMS_recursive{
    // структура, обеспечивающая передачу
    // параметров при возвращении из рекурсии
    vector<int> stringLMS; // строка с LMS, в которой они переименованы
    vector<int> positionsLMS; // положения начала всех LMS
    int alphabetSizeLMS;
};

class SA_IS{
public:
    
    SA_IS(vector<int> &data, int alphabetSize = 128){
        answer = runSA_IS(data);
    }
    
    SA_IS() = default;
    
    vector<int> getAnswer(){
        return answer;
    }
    
    vector<int> countLCP(string s, vector<int> &SA);
    
private:
    // символ имеет тип S, если
    // суффикс, начинающийся с
    // него, меньше (Smaller),
    // чем со следующего символа
    // L = Larger
    const bool S_TYPE = 0;
    const bool L_TYPE = 1;
    vector<int> answer;
    
    //SOLVER
    vector<int> runSA_IS(vector<int> &data, int alphabetSize = 128);
    
    template<typename T>
    void printv(vector<T> v);
    
    vector<int> mapStringToIntVector(string s);

    vector<bool> mapTypes(vector<int> &data);

    bool isLMS_char(int index, vector<bool> &types);
    
    bool compareLMS(vector<int> &data,
                    vector<bool> &types,
                    // сдвиги в начало LMS'ок
                    int posX,
                    int posY);

    vector<int> createBuckets(vector<int> &data, int alphabetSize = 128);
    
    vector<int> bucketsHeads(vector<int> &buckets, int alphabetSize = 128);
    
    vector<int> bucketsEnds(vector<int> &buckets, int alphabetSize = 128);
    
    vector<int> preSortLMS(vector<int> &data,
                           vector<int> &buckets,
                           vector<bool> &types);

    void sortL_TYPE(vector<int> &data,
               vector<int> &LMS_SA, // исправляется
               vector<int> &buckets,
                vector<bool> &types);

    void sortS_TYPE(vector<int> &data,
               vector<int> &LMS_SA, // исправляется
               vector<int> &buckets,
                vector<bool> &types);

    LMS_recursive reconstructSA(vector<int> &data,
                                vector<int> &LMS_SA,
                                vector<bool> &types);
    
    vector<int> countSA_LMS(vector<int> &stringLMS, int alphabetSizeLMS);
    
    vector<int> finalSortLMS(vector<int> &data,
                            vector<int> &buckets,
                            vector<bool> &types,
                            vector<int> &summarySuffixArray,
                            vector<int> &positionsLMS);

};

vector<int> SA_IS::runSA_IS(vector<int> &data, int alphabetSize){
    auto types = mapTypes(data);
    auto buckets = createBuckets(data, alphabetSize);
    auto LMS_SA = preSortLMS(data, buckets, types);
    sortL_TYPE(data, LMS_SA, buckets, types);
    sortS_TYPE(data, LMS_SA, buckets, types);
    LMS_recursive lms = reconstructSA(data, LMS_SA, types);
    auto stringLMS = lms.stringLMS;
    int alphabetSizeLMS = lms.alphabetSizeLMS;
    auto positionsLMS = lms.positionsLMS;
    auto summarySuffixArray = countSA_LMS(stringLMS, alphabetSizeLMS);
    auto result = finalSortLMS(data, buckets, types, summarySuffixArray, positionsLMS);
    sortL_TYPE(data, result, buckets, types);
    sortS_TYPE(data, result, buckets, types);
    return result;
}

template<typename T>
void SA_IS::printv(vector<T> v){
    for (size_t i = 0; i < v.size(); ++i){
        cout << v[i] << "  ";
    }
    cout << "\n";
}

vector<int> SA_IS::mapStringToIntVector(string s){
    // работаем с числами, а не со строками
    // из-за того, что дальше алфавит может
    // стать очень большим
    vector<int> result;
    for (int i = 0; i < s.length(); ++i){
        result.push_back((int)s[i]);
    }
    return result;
}

vector<bool> SA_IS::mapTypes(vector<int> &data){
    // определим тип каждого символа
    // S_TYPE или L_TYPE
    vector<bool> types(data.size() + 1);
    types[data.size()] = S_TYPE; // пустой суффикс
    if (data.size() == 0)
        return types;
    types[data.size() - 1] = L_TYPE; // по определению
    for (int i = (int)data.size() - 2; i >= 0; --i){
        if (data[i] == data[i + 1]){
            // этот суффикс совпадает по началу со
            // следующим, поэтому чтобы определить
            // тип, надо смотреть на (i+2)'й символ
            types[i] = types[i+1];
        }else if (data[i] < data[i + 1]){
            types[i] = S_TYPE;
        }else{
            types[i] = L_TYPE;
        }
    }
    return types;
}

bool SA_IS::isLMS_char(int index, vector<bool> &types){
    assert(index >= 0);
    //символ - LMS, если он имеет тип S,
    //тот, кто перед ним - L
    if (index == 0)
        //первый никогда не LMS
        return false;
    if (types[index - 1] == L_TYPE && types[index] == S_TYPE){
        return true;
    }else{
        return false;
    }
}

bool SA_IS::compareLMS(vector<int> &data,
                vector<bool> &types,
                // сдвиги в начало LMS'ок
                int posX,
                int posY)
{
    // строка называется LMS, если она начинается
    // и кончается символами-LMS, в внутри других LMS нет
    // т.е. как бы почти отсортированная, выглядит как
    // SSSSS...SSSSLLLL....LLLLLS
    assert(posX >= 0);
    assert(posY >= 0);
    // LMS равны, если у них равные
    // длины и одинаковые символы
    if (posX == data.size() || posY == data.size())
        return false;
    int i = 0;
    while (true){
        bool isLMS_charX = isLMS_char(posX + i, types);
        bool isLMS_charY = isLMS_char(posY + i, types);
        if (i > 0 && isLMS_charX && isLMS_charY){
            // дошли до конца
            return true;
        }
        if (isLMS_charX != isLMS_charY || data[posX + i] != data[posY + i])
            return false;
        i++;
    }
}

vector<int> SA_IS::createBuckets(vector<int> &data, int alphabetSize){
    //создает и считает размеры групп в bucketSort
    vector<int> buckets(alphabetSize, 0);
    for (size_t i = 0; i < data.size(); ++i){
        buckets[data[i]]++;
    }
    return buckets;
}

vector<int> SA_IS::bucketsHeads(vector<int> &buckets, int alphabetSize){
    //возвращает номера начал корзин
    int index = 1;
    vector<int> heads;
    for (size_t i = 0; i < buckets.size(); ++i){
        heads.push_back(index);
        index += buckets[i];
    }
    return heads;
}

vector<int> SA_IS::bucketsEnds(vector<int> &buckets, int alphabetSize){
    //возвращает номера концов корзин
    int index = 1;
    vector<int> ends;
    for (size_t i = 0; i < buckets.size(); ++i){
        index += buckets[i];
        ends.push_back(index - 1);
    }
    return ends;
}

vector<int> SA_IS::preSortLMS(vector<int> &data,
                       vector<int> &buckets,
                       vector<bool> &types)
{
    //попытаемся сделать суффмас для LMS
    vector<int> LMS_SA(data.size() + 1, -1);
    auto ends = bucketsEnds(buckets);
    for (int i = 0; i < data.size(); ++i){
        if (!isLMS_char(i, types))
            continue;
        int bucketPos = data[i];
        assert(bucketPos >= 0);
        LMS_SA[ends[bucketPos]] = i;
        ends[bucketPos]--;
    }
    LMS_SA[0] = (int)data.size();
    return LMS_SA;
}

void SA_IS::sortL_TYPE(vector<int> &data,
                vector<int> &LMS_SA, // исправляется
                vector<int> &buckets,
                vector<bool> &types)
{
    auto heads = bucketsHeads(buckets);
    for (int i = 0; i < LMS_SA.size(); ++i){
        if (LMS_SA[i] == -1 || LMS_SA[i] == 0)
            continue;
        int prev = LMS_SA[i] - 1;
        if (prev < 0 || types[prev] != L_TYPE)
            continue;
        int bucketPos = data[prev];
        LMS_SA[heads[bucketPos]] = prev;
        heads[bucketPos]++;
    }
}

void SA_IS::sortS_TYPE(vector<int> &data,
                vector<int> &LMS_SA, // исправляется
                vector<int> &buckets,
                vector<bool> &types)
{
    auto ends = bucketsEnds(buckets);
    for (int i = (int)LMS_SA.size() - 1; i >= 0; --i){
        int prev = LMS_SA[i] - 1;
        if (prev < 0 || types[prev] != S_TYPE)
            continue;
        int bucketPos = data[prev];
        LMS_SA[ends[bucketPos]] = prev;
        ends[bucketPos]--;
    }
}

LMS_recursive SA_IS::reconstructSA(vector<int> &data,
                            vector<int> &LMS_SA,
                            vector<bool> &types)
{
    //восстанавливаем суффмас из суффмаса для LMS
    vector<int> labelsLMS(data.size() + 1, -1);
    int cur = 0;
    int LMS_position;
    labelsLMS[LMS_SA[0]] = cur;
    LMS_position = LMS_SA[0];
    assert(LMS_position >= 0);
    // попытаемся переименовать LMS
    // чтобы создать для них суффмассив
    for (int i = 1; i < LMS_SA.size(); ++i){
        int suffPos = LMS_SA[i];
        assert(suffPos >= 0);
        if (!isLMS_char(suffPos, types))
            continue;
        if (!compareLMS(data, types, suffPos, LMS_position))
            cur++;
        LMS_position = suffPos;
        labelsLMS[suffPos] = cur;
    }
    
    vector<int> positionsLMS;
    vector<int> stringLMS;
    for (int i = 0; i < labelsLMS.size(); ++i){
        if (labelsLMS[i] == -1)
            continue;
        positionsLMS.push_back(i);
        stringLMS.push_back(labelsLMS[i]);
    }
    cur++;
    int alphabetSizeLMS = cur;
    return LMS_recursive{stringLMS, positionsLMS, alphabetSizeLMS};
}

vector<int> SA_IS::countSA_LMS(vector<int> &stringLMS, int alphabetSizeLMS){
    vector<int> summarySuffixArray;
    if (alphabetSizeLMS == stringLMS.size()){
        summarySuffixArray.assign(stringLMS.size() + 1, -1);
        summarySuffixArray[0] = (int)stringLMS.size();
        for (int i = 0; i < stringLMS.size(); ++i){
            summarySuffixArray[stringLMS[i] + 1] = i;
        }
    }else{
        summarySuffixArray = runSA_IS(stringLMS, alphabetSizeLMS);
    }
    return summarySuffixArray;
}

vector<int> SA_IS::finalSortLMS(vector<int> &data,
                         vector<int> &buckets,
                         vector<bool> &types,
                         vector<int> &summarySuffixArray,
                         vector<int> &positionsLMS)
{
    // добиваем отсортированных LMS
    // (могут сместиться)
    vector<int> suffixOffsets(data.size() + 1, -1);
    auto ends = bucketsEnds(buckets);
    for (int i = (int)summarySuffixArray.size() - 1; i > 1; --i){
        int stringIndex = positionsLMS[summarySuffixArray[i]];
        int bucketIndex = data[stringIndex];
        suffixOffsets[ends[bucketIndex]] = stringIndex;
        ends[bucketIndex]--;
    }
    suffixOffsets[0] = (int)data.size();
    return suffixOffsets;
}

long long differentSubstrings(string s){
    vector<int> s_v;
    for (int i = 0; i < s.length(); ++i)
        s_v.push_back((int)s[i]);
    SA_IS issa(s_v);
    auto dollarSA = issa.getAnswer();
    s += '$';
    auto LCP = issa.countLCP(s, dollarSA);
    
    unsigned long long sum = 0;
    for (int i = 0; i < s.length(); ++i){
        sum += (unsigned long long)s.length() - 1 - (unsigned long long)dollarSA[i];
    }
    for (int i = 0; i < s.length() - 1; ++i){
        sum -= (unsigned long long)LCP[i];
    }
    if (sum == 0 && s.length() != 0)
        ++sum;
    return sum;
}

vector<int> SA_IS::countLCP(string s, vector<int> &SA){
    vector<int> LCP(s.length()), conversedSA(s.length());
    for (int i = 0; i < s.length() - 1; ++i){
        conversedSA[SA[i]] = i;
    }
    int k = 0;
    for (int i = 0; i < s.length(); ++i){
        if (k > 0)
            --k;
        if (conversedSA[i] == s.length() - 1){
            LCP[s.length() - 1] = -1;
            k = 0;
        }else{
            int j = SA[conversedSA[i] + 1];
            while (max(i+k, j+k) < s.length() && s[i+k] == s[j+k])
                ++k;
            LCP[conversedSA[i]] = k;
        }
    }
    return LCP;
}

void runTimusContest(){
    // OK, ID посылки: 6576919
    int k;
    cin >> k;
    string s;
    cin >> s;
    size_t len = s.length();
    s += string(s.begin(), s.begin() + k);
    for (size_t i = 0; i < len; ++i){
        string subS(s.begin() + i, s.begin() + i + k);
        cout << differentSubstrings(subS) << " ";
    }
}

int main() {
    runTimusContest();
    
    return 0;
}