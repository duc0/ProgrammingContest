//#define SUBMIT

#define C11

#ifdef SUBMIT
#define LOGLEVEL 0
#define NDEBUG
#else
#define LOGLEVEL 1
#endif

#include <cstdio>
#include <algorithm>
#include <cstring>
#include <cassert>
#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <cmath>
#include <cstdlib>
#include <queue>
#include <stack>
#include <functional>
#include <sstream>
#include <deque>
#include <climits>
#include <cfloat>
#include <bitset>

#ifdef C11
#include <array>
#include <type_traits>
#include <random>
#include <unordered_set>
#include <unordered_map>
#endif

using namespace std;

#define LOG(l, x) if (l <= LOGLEVEL) cout << x << endl

#define int64 long long
#define repeat(x) for (auto repeat_var = 0; repeat_var < x; ++repeat_var)

#define for_inc(i, x) for (auto i = 0; i < x; ++i)
#define for_dec(i, x) for (auto i = x - 1; i >= 0; --i)
#define for_inc_range(i, x, y) for (auto i = x; i <= y; ++i)
#define for_dec_range(i, x, y) for (auto i = x; i >= y; --i)

#define countBit __builtin_popcount
#define countBit64 __builtin_popcountl

#define fill0(x) memset(x, 0, sizeof(x))
#define INT_INF ((int)2E9L)
#define INT64_INF ((int64)1E18L)


template <int BASE, int MODULO> class BasePowerUtils {
  
public:
  static int getBasePower(int n) {
    static vector<int> power; // cache
    if (n > (int)power.size() - 1) {
      int cur = (int)power.size() - 1;
      power.resize(n + 1);
      for_inc_range(i, cur + 1, n) {
        if (i == 0) {
          power[i] = 1;
        } else {
          power[i] = ((int64)power[i - 1] * BASE) % MODULO;
        }
      }
    }
    return power[n];
  }
};


template <int BASE, int MODULO> class StringHash {
  int length;
  int hash;
public:
  int getLength() const {
    return length;
  }
  
  int getHash() const {
    return hash;
  }
  
  StringHash(int hash, int length): hash(hash), length(length) {}
  
  template <class Iterator> StringHash(Iterator begin, Iterator end) {
    hash = 0;
    for (auto it = begin; it != end; ++it) {
      hash = ((int64)hash * BASE + *it) % MODULO;
    }
    length = (int) (end - begin);
  }
  
  static StringHash<BASE, MODULO> fromSingleChar(int singleChar) {
    return StringHash<BASE, MODULO>(singleChar % MODULO, 1);
  }
  
  StringHash<BASE, MODULO> concat(const StringHash<BASE, MODULO> &sh) const {
    return StringHash<BASE, MODULO>(((int64) getHash() * BasePowerUtils<BASE, MODULO>::getBasePower(sh.getLength()) +
                                     sh.getHash()) % MODULO, getLength() + sh.getLength());
  }
};

template <int BASE, int MODULO> class StringPrefixHash {
  vector<int> hash;
  
public:
  StringPrefixHash() {}
  
  template <class Iterator> StringPrefixHash(Iterator begin, Iterator end) {
    int n = (int)(end - begin);
    hash.resize(n);
    int idx = 0;
    int last = 0;
    for (auto it = begin; it != end; ++it) {
      hash[idx] = ((int64)last * BASE + *it) % MODULO;
      last = hash[idx];
      idx++;
    }
  }
  
  size_t getSize() const { return hash.size(); }
  
  StringHash<BASE, MODULO> getPrefixHash(int i) const {
    assert(i < getSize());
    return StringHash<BASE, MODULO>(hash[i], i);
  }
  
  StringHash<BASE, MODULO> getStringHash() const {
    return getPrefixHash(hash.size() - 1);
  }
  
  StringHash<BASE, MODULO> getSubstringHash(int first, int len) const {
    if (len == 0) return StringHash<BASE, MODULO>(0, 0);
    assert(0 <= first && first < getSize());
    assert(len >= 1);
    assert(first + len - 1 < getSize());
    int last = first + len - 1;
    if (first == 0) {
      return StringHash<BASE, MODULO>(hash[last], len);
    }
    int ret = (hash[last] - (int64) hash[first - 1] * BasePowerUtils<BASE, MODULO>::getBasePower(len)) % MODULO;
    if (ret < 0)
      ret += MODULO;
    return StringHash<BASE, MODULO>(ret, len);
  }
  
  // Range is inclusive
  StringHash<BASE, MODULO> getSubstringHashByRange(int first, int last) const {
    return getSubstringHash(first, last - first + 1);
  }
};

template <int BASE1, int MODULO1, int BASE2, int MODULO2> class StringDoubleHash {
  int length;
  int hash1;
  int hash2;
public:
  int getLength() const {
    return length;
  }
  
  int64 getHashValue() const {
    return (int64) hash1 * MODULO2 + hash2;
  }
  
  StringHash<BASE1, MODULO1> getHash1() const {
    return StringHash<BASE1, MODULO1>(hash1, length);
  }
  
  StringHash<BASE2, MODULO2> getHash2() const {
    return StringHash<BASE2, MODULO2>(hash2, length);
  }
  
  StringDoubleHash(int hash1, int hash2, int length): hash1(hash1), hash2(hash2), length(length) {}
  
  StringDoubleHash(const StringHash<BASE1, MODULO1> &h1, const StringHash<BASE2, MODULO2> &h2): hash1(h1.getHash()), hash2(h2.getHash()), length(h1.getLength()) {
    assert(h1.getLength() == h2.getLength());
  }
  
  template <class Iterator> StringDoubleHash(Iterator begin, Iterator end) {
    hash1 = StringHash<BASE1, MODULO1>(begin, end).getHash();
    hash2 = StringHash<BASE2, MODULO2>(begin, end).getHash();
    length = (int) (end - begin);
  }
  
  static StringDoubleHash<BASE1, MODULO1, BASE2, MODULO2> fromSingleChar(int singleChar) {
    return StringDoubleHash<BASE1, MODULO1, BASE2, MODULO2>(singleChar % MODULO1, singleChar % MODULO2, 1);
  }
  
  StringDoubleHash<BASE1, MODULO1, BASE2, MODULO2> concat(const StringDoubleHash<BASE1, MODULO1, BASE2, MODULO2> &sh) const {
    return StringDoubleHash<BASE1, MODULO1, BASE2, MODULO2>(
                                                            getHash1().concat(sh.getHash1()),
                                                            getHash2().concat(sh.getHash2())
                                                            );
  }
};


template <int BASE1, int MODULO1, int BASE2, int MODULO2> class StringPrefixDoubleHash {
  StringPrefixHash<BASE1, MODULO1> hash1;
  StringPrefixHash<BASE2, MODULO2> hash2;
  
public:
  StringPrefixDoubleHash() {}
  
  template <class Iterator> StringPrefixDoubleHash(Iterator begin, Iterator end): hash1(StringPrefixHash<BASE1, MODULO1>(begin, end)), hash2(StringPrefixHash<BASE2, MODULO2>(begin, end)) {}
  
  size_t getSize() const { return hash1.getSize(); }
  
  StringDoubleHash<BASE1, MODULO1, BASE2, MODULO2> getPrefixHash(int i) const {
    return StringDoubleHash<BASE1, MODULO1, BASE2, MODULO2>(hash1.getPrefixHash(i), hash2.getPrefixHash(i));
  }
  
  StringDoubleHash<BASE1, MODULO1, BASE2, MODULO2> getStringHash() const {
    return getPrefixHash((int)getSize() - 1);
  }
  
  StringDoubleHash<BASE1, MODULO1, BASE2, MODULO2> getSubstringHash(int first, int len) const {
    return StringDoubleHash<BASE1, MODULO1, BASE2, MODULO2>(hash1.getSubstringHash(first, len), hash2.getSubstringHash(first, len));
  }
  
  // Range is inclusive
  StringDoubleHash<BASE1, MODULO1, BASE2, MODULO2> getSubstringHashByRange(int first, int last) const {
    return getSubstringHash(first, last - first + 1);
  }
};

vector<int> lowercaseStringToVector(const string &s) {
  vector<int> a;
  for (char c : s) {
    a.push_back(c - 'a');
  }
  return a;
}

void testGen() {
  freopen("biginput1.txt", "w", stdout);
  int n=  100000;
  cout << n << endl;
  for_inc(i, n) {
    cout << (char)(rand()%26 + 'a');
  }
  cout << endl;
  for_inc(i, n) {
    cout << (char)(rand()%26 + 'a');
  }
  cout << endl;
  
  fclose(stdout);
}

// VKCup 2015 - R2, E

#define HMOD1 1000001927
#define HMOD2 1000001963
#define BASE1 3
#define BASE2 5
#define SPH StringPrefixDoubleHash<BASE1, HMOD1, BASE2, HMOD2>
#define SH StringHash<BASE1, HMOD1, BASE2, HMOD2>

int lenS, lenT;
string s, t;
SPH hashS[26], hashT[26];
vector<int64> hT(26), htSorted(26);

bool check(int i) {
  vector<int64> hS(26);
  for_inc(c, 26) {
    hS[c] = hashS[c].getSubstringHash(i, lenT).getHashValue();
  }
  vector<int64> hsSorted = hS;
  sort(hsSorted.begin(), hsSorted.end());
  for_inc(c, 26) {
    if (hsSorted[c] != htSorted[c]) {
      return false;
    }
  }
  
  for_inc(c1, 26) {
    if (hS[c1] == 0) continue;
    for_inc(c2, 26) {
      if (hS[c1] == hT[c2] && hS[c2] != hT[c1]) {
        return false;
      }
    }
  }

  return true;
}

int main() {
  ios::sync_with_stdio(false);
#ifndef SUBMIT
  //testGen();
  freopen("input2.txt", "r", stdin);
#endif
  
  cin >> lenS >> lenT;
  cin >> s >> t;
  
  for_inc(c, 26) {
    vector<int> vs, vt;
    for_inc(i, lenS) {
      vs.push_back((s[i] - 'a') == c);
    }
    for_inc(i, lenT) {
      vt.push_back((t[i] - 'a') == c);
    }
    
    hashS[c] = SPH(vs.begin(), vs.end());
    hashT[c] = SPH(vt.begin(), vt.end());
  }
  for_inc(c, 26) {
    hT[c] = hashT[c].getStringHash().getHashValue();
  }
  htSorted = hT;
  sort(htSorted.begin(), htSorted.end());
  
  vector<int> ans;
  for_inc(i, lenS - lenT + 1) {
    if (check(i)) {
      ans.push_back(i);
    }
  }

  cout << ans.size() << endl;
  for (int i : ans) cout << (i + 1) << " ";
  
  return 0;
}
