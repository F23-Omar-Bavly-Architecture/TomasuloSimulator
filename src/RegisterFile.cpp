#include <bits/stdc++.h>
using namespace std;


struct RegisterFile {
        uint16_t val[8];
        uint16_t& operator[](size_t index) {
                val[0] = 0;
                return val[index];
        }
};