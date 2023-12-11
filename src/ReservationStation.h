#include <bits/stdc++.h>
using namespace std;

class ReservationStationEntry {
public:
        string Op;
        int Vj;
        int Vk;
        string Qj;
        string Qk;
        int A;
        Instruction instruction;
        bool Busy;
        int clockCycle;
        int finishesExecutionInCycle;
        uint16_t Result;
        ReservationStationEntry() {
                Op = "";
                Vj = 0;
                Vk = 0;
                Qj = "";
                Qk = "";
                A = 0;
                Busy = false;
                clockCycle = 0;
                finishesExecutionInCycle = 0;
                Result = 0;
        }
};

class ReservationStation {
public:
        map<string, ReservationStationEntry> station;
        int cyclesLoad, cyclesStore, cyclesBne, cyclesCallRet, cyclesAdd, cyclesNand, cyclesDiv;
        int numLoad, numStore, numBne, numCallRet, numAdd, numNand, numDiv;
        int currentLoad = 0, currentStore = 0, currentBne =0, currentCallRet = 0, currentAdd = 0, currentNand = 0, currentDiv = 0;

        ReservationStation() {
                // Prompt for number of each type of reservation station: LOAD, STORE, BNE, CALL/RET, ADD/ADDI, NAND, DIV must be > 0
                cout << "Enter number of LOAD reservation stations: ";
                cin >> numLoad;
                while(numLoad <= 0) {
                        cout << "Number of LOAD reservation stations must be greater than 0" << endl;
                        cout << "Enter number of LOAD reservation stations: ";
                        cin >> numLoad;
                }
                cout << "Enter number of STORE reservation stations: ";
                cin >> numStore;
                while(numStore <= 0) {
                        cout << "Number of STORE reservation stations must be greater than 0" << endl;
                        cout << "Enter number of STORE reservation stations: ";
                        cin >> numStore;
                }
                cout << "Enter number of BNE reservation stations: ";
                cin >> numBne;
                while(numBne <= 0) {
                        cout << "Number of BNE reservation stations must be greater than 0" << endl;
                        cout << "Enter number of BNE reservation stations: ";
                        cin >> numBne;
                }
                cout << "Enter number of CALL/RET reservation stations: ";
                cin >> numCallRet;
                while(numCallRet <= 0) {
                        cout << "Number of CALL/RET reservation stations must be greater than 0" << endl;
                        cout << "Enter number of CALL/RET reservation stations: ";
                        cin >> numCallRet;
                }
                cout << "Enter number of ADD/ADDI reservation stations: ";
                cin >> numAdd;
                while(numAdd <= 0) {
                        cout << "Number of ADD/ADDI reservation stations must be greater than 0" << endl;
                        cout << "Enter number of ADD/ADDI reservation stations: ";
                        cin >> numAdd;
                }
                cout << "Enter number of NAND reservation stations: ";
                cin >> numNand;
                while(numNand <= 0) {
                        cout << "Number of NAND reservation stations must be greater than 0" << endl;
                        cout << "Enter number of NAND reservation stations: ";
                        cin >> numNand;
                }
                cout << "Enter number of DIV reservation stations: ";
                cin >> numDiv;
                while(numDiv <= 0) {
                        cout << "Number of DIV reservation stations must be greater than 0" << endl;
                        cout << "Enter number of DIV reservation stations: ";
                        cin >> numDiv;
                }
                for (int i = 0; i < numLoad; i++) {
                        station["Load" + to_string(i)] = ReservationStationEntry();
                }
                for (int i = 0; i < numStore; i++) {
                        station["Store" + to_string(i)] = ReservationStationEntry();
                }
                for (int i = 0; i < numBne; i++) {
                        station["Bne" + to_string(i)] = ReservationStationEntry();
                }
                for (int i = 0; i < numCallRet; i++) {
                        station["CallRet" + to_string(i)] = ReservationStationEntry();
                }
                for (int i = 0; i < numAdd; i++) {
                        station["Add" + to_string(i)] = ReservationStationEntry();
                }
                for (int i = 0; i < numNand; i++) {
                        station["Nand" + to_string(i)] = ReservationStationEntry();
                }
                for (int i = 0; i < numDiv; i++) {
                        station["Div" + to_string(i)] = ReservationStationEntry();
                }

                // prompt user for the number of cycles required by each FU, must be > 0
                cout << "Enter number of cycles required by LOAD FU: ";
                cin >> cyclesLoad;
                while(cyclesLoad <= 0) {
                        cout << "Number of cycles required by LOAD FU must be greater than 0" << endl;
                        cout << "Enter number of cycles required by LOAD FU: ";
                        cin >> cyclesLoad;
                }
                cout << "Enter number of cycles required by STORE FU: ";
                cin >> cyclesStore;
                while(cyclesStore <= 0) {
                        cout << "Number of cycles required by STORE FU must be greater than 0" << endl;
                        cout << "Enter number of cycles required by STORE FU: ";
                        cin >> cyclesStore;
                }
                cout << "Enter number of cycles required by BNE FU: ";
                cin >> cyclesBne;
                while(cyclesBne <= 0) {
                        cout << "Number of cycles required by BNE FU must be greater than 0" << endl;
                        cout << "Enter number of cycles required by BNE FU: ";
                        cin >> cyclesBne;
                }
                cout << "Enter number of cycles required by CALL/RET FU: ";
                cin >> cyclesCallRet;
                while(cyclesCallRet <= 0) {
                        cout << "Number of cycles required by CALL/RET FU must be greater than 0" << endl;
                        cout << "Enter number of cycles required by CALL/RET FU: ";
                        cin >> cyclesCallRet;
                }
                cout << "Enter number of cycles required by ADD/ADDI FU: ";
                cin >> cyclesAdd;
                while(cyclesAdd <= 0) {
                        cout << "Number of cycles required by ADD/ADDI FU must be greater than 0" << endl;
                        cout << "Enter number of cycles required by ADD/ADDI FU: ";
                        cin >> cyclesAdd;
                }
                cout << "Enter number of cycles required by NAND FU: ";
                cin >> cyclesNand;
                while(cyclesNand <= 0) {
                        cout << "Number of cycles required by NAND FU must be greater than 0" << endl;
                        cout << "Enter number of cycles required by NAND FU: ";
                        cin >> cyclesNand;
                }
                cout << "Enter number of cycles required by DIV FU: ";
                cin >> cyclesDiv;
                while(cyclesDiv <= 0) {
                        cout << "Number of cycles required by DIV FU must be greater than 0" << endl;
                        cout << "Enter number of cycles required by DIV FU: ";
                        cin >> cyclesDiv;
                }

        }
};