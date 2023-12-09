#include <bits/stdc++.h>
using namespace std;

class RegisterStatus {
public:
        map<string, string> status;
        RegisterStatus() {
                status["R0"] = "";
                status["R1"] = "";
                status["R2"] = "";
                status["R3"] = "";
                status["R4"] = "";
                status["R5"] = "";
                status["R6"] = "";
                status["R7"] = "";
        }
};