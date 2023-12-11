#include <bits/stdc++.h>
using namespace std;

//LOAD RD, offset(RS1)
//STORE RS2, offset(RS2)
//BNE RS1, RS2, offset
//CALL label
//RET

//ADD RD, Rs1, Rs2
//ADDI RD, Rs1, imm
//NAND RD, Rs1, Rs2
//DIV RD, Rs1, Rs2
vector<string> splitstring(string s, string dem)
{
	vector<string> vec;
	size_t pos = 0;
	string token;
	while ((pos = s.find(dem)) != string::npos) {
		token = s.substr(0, pos);
		vec.push_back(token);
		s.erase(0, pos + dem.length());
	}
	vec.push_back(s);

	return vec;
}

class Instruction {
public:
        string Inst;
        string op;
        int IMM;
        int label;
        int offset;
        string RD;
        string RS1;
        string RS2;
        Instruction(string Inst) {
                this->Inst = Inst;
                parse();
        }
        Instruction() {
                Inst = "";
                op = "";
                IMM = 0;
                label = 0;
                offset = 0;
                RD = "";
                RS1 = "";
                RS2 = "";
        }
        void parse()
        {
                Inst.erase(remove(Inst.begin(), Inst.end(), ','), Inst.end());
                vector<string> vec = splitstring(Inst, " ");
                op = vec[0];
                if (op == "ADD" || op == "NAND" || op == "DIV") { //ADD RD, Rs1, Rs2 //NAND RD, Rs1, Rs2 //DIV RD, Rs1, Rs2
                        RD = vec[1];
                        RS1 = vec[2];
                        RS2 = vec[3];
                }
                else if (op == "ADDI") { //ADDI RD, Rs1, imm
                        RD = vec[1];
                        RS1 = vec[2];
                        IMM = stoi(vec[3]);
                }
                else if (op == "LOAD") {//LOAD RD, offset(RS1)
                        vector<string> tempvec = splitstring(vec[2], "(");
                        RD = vec[1];
                        offset = stoi(tempvec[0]);
                        RS1 = tempvec[1].substr(0, tempvec[1].length() - 1);
                }
                else if (op == "STORE") { //STORE RS2, offset(RS2)
                        vector<string> tempvec = splitstring(vec[2], "(");
                        RS2 = vec[1];
                        offset = stoi(tempvec[0]);
                        RS1 = tempvec[1].substr(0, tempvec[1].length() - 1);
                }
                else if (op == "BNE") {//BNE RS1, RS2, offset
                        RS1 = vec[1];
                        RS2 = vec[2];
                        label = stoi(vec[3]);
                }
                else if (op == "CALL") {//CALL label
                        label = stoi(vec[1]);
                }
                else if (op == "RET") {//RET
                        return;
                }
        }
};