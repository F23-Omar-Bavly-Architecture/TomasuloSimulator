#include <bits/stdc++.h>
#include "RegisterStatus.h"
#include "ReservationStation.h"
#include "RegisterFile.h"
#include "Instruction.h"

using namespace std;

// RegisterSt : map<string, string> key is the name of the register 
// and value indicates whcih reservation station will provide the value for the register

// ReservationStation : <string, ReservationStationEntry> key is the name of the reservation station

// ReservationStationEntry : class with the following fields
// op : string
// Vj : string
// Vk : string
// Qj : string
// Qk : string
// A : string
// Busy : bool
// Result : 16 bit int

// Instruction : class with the following fields
// op : string
// Dest : string
// Src1 : string
// Src2 : string

// InstructionQueue : queue<Instruction> queue of instructions to be executed   

// Memory : array of ints indexed by a 16 bit address

// RegisterFile : struct with the following fields
// struct bit16
// bit16 arr[8] : array of 16 bit ints indexed by register number
// bit16& operator[](int i) : overload [] operator to access the array, if i is 0 return 0
// RegisterFile() : constructor to initialize the array to 0

// PC : 16 bit int

// ClockCycle : int

class Tomasulo {
    public:
        RegisterStatus registerStatus;
        ReservationStation reservationStation;
        vector<Instruction> instructionQueue;
        vector<Instruction> inflightInstructions;
        queue<Instruction> issuedQueue;
        queue<Instruction> executingQueue;
        queue<Instruction> writeBackQueue;
        array<int, 65536> Memory;
        RegisterFile registerFile;
        queue<int> LoadStoreQueue;
        map<string, bool> pleaseFree;
        queue<int> predicting;
        bool RetInFlight;

        map<int,vector<string>> instructionStatus;

        uint16_t PC;
        int ClockCycle;
        int startingAddress;

        bool isFinished(){
            // loop over register status and reservation station to check if all are free
            for(auto it = registerStatus.status.begin(); it != registerStatus.status.end(); it++)
            {
                if(it->second != "")
                    return false;
            }
            for(auto it = reservationStation.station.begin(); it !=  reservationStation.station.end(); it++)
            {
                if(it->second.Busy)
                    return false;
            }
            return PC == instructionQueue.size() ;
        }


        Tomasulo() {
            PC = 0;
            ClockCycle = 0;
            RetInFlight = false;
            // populate instruction queue either with use input or from file
            string input;
            cout << "Would you like to enter instructions manually (choosing no will require you provide a file)? (y/n): ";
            cin >> input;
            while(input != "y" && input != "n"){
                cout << "Invalid input" << endl;
                cout << "Would you like to enter instructions manually (choosing no will require you provide a file)? (y/n): ";
                cin >> input;
            }
            if(input == "y"){
                ReadInstructionsFromUser();
            }else{
                string filename;
                cout << "Please enter file path relative to this file: ";
                cin >> filename;
                ReadInstructionsFromFile(filename);
            }
            // prompt for starting address if file is used
            cout << "\n\n----------------------------------------------------------------------------------------\n\n";
            cout << "Please enter starting address(will be used during CALL): ";
            cin >> startingAddress;
            // prompt for data to initialize memory
            initializeMem();
            for(auto it = reservationStation.station.begin(); it !=  reservationStation.station.end(); it++)
            {
                pleaseFree[it->first] = false;
            }
        }
        void RunClockCycle(){
            WriteBack();
            Execute();
            Issue();
            ClockCycle++;
        };
        void Issue(){
            // Read instruction from instruction queue
            // Check type of instruction
            // Check for unused reservation station of that type

            if(RetInFlight){
                for(auto it = pleaseFree.begin(); it != pleaseFree.end(); it++)
                {
                if(it->second)
                {
                    reservationStation.station[it->first].Op = "";
                    reservationStation.station[it->first].Vj = 0;
                    reservationStation.station[it->first].Vk = 0;
                    reservationStation.station[it->first].Qj = "";
                    reservationStation.station[it->first].Qk = "";
                    reservationStation.station[it->first].A = 0;
                    reservationStation.station[it->first].Busy = false;
                    reservationStation.station[it->first].clockCycle = 0;
                    reservationStation.station[it->first].finishesExecutionInCycle = 0;
                    reservationStation.station[it->first].Result = 0;
                    reservationStation.station[it->first].executed = false;
                    it->second = false;
                    if(it->first[0] == 'C' || it->first[0] == 'R') reservationStation.currentCallRet--;
                    else if(it->first[0] == 'A') reservationStation.currentAdd--;
                    else if(it->first[0] == 'L') reservationStation.currentLoad--;
                    else if(it->first[0] == 'D') reservationStation.currentDiv--;
                    else if(it->first[0] == 'N') reservationStation.currentNand--;
                    else if(it->first[0] == 'S') reservationStation.currentStore--;
                    else if(it->first[0] == 'B') reservationStation.currentBne--;
                }
                }
                return;
            }
            if(PC >= instructionQueue.size()){
                for(auto it = pleaseFree.begin(); it != pleaseFree.end(); it++)
                {
                if(it->second)
                {
                    reservationStation.station[it->first].Op = "";
                    reservationStation.station[it->first].Vj = 0;
                    reservationStation.station[it->first].Vk = 0;
                    reservationStation.station[it->first].Qj = "";
                    reservationStation.station[it->first].Qk = "";
                    reservationStation.station[it->first].A = 0;
                    reservationStation.station[it->first].Busy = false;
                    reservationStation.station[it->first].clockCycle = 0;
                    reservationStation.station[it->first].finishesExecutionInCycle = 0;
                    reservationStation.station[it->first].Result = 0;
                    reservationStation.station[it->first].executed = false;
                    it->second = false;
                    if(it->first[0] == 'C' || it->first[0] == 'R') reservationStation.currentCallRet--;
                    else if(it->first[0] == 'A') reservationStation.currentAdd--;
                    else if(it->first[0] == 'L') reservationStation.currentLoad--;
                    else if(it->first[0] == 'D') reservationStation.currentDiv--;
                    else if(it->first[0] == 'N') reservationStation.currentNand--;
                    else if(it->first[0] == 'S') reservationStation.currentStore--;
                    else if(it->first[0] == 'B') reservationStation.currentBne--;
                }
                }
                return;
            }
            Instruction instruction = instructionQueue[PC];
            string currentStation;
            if (instruction.op == "LOAD"){
                // check for available load reservation station
                if(reservationStation.currentLoad < reservationStation.numLoad){
                    // check for available register
                    //loop through Load reservation stations to find available one
                    for(int i = 0; i < reservationStation.numLoad; i++){
                        currentStation = "Load" + to_string(i);
                        if(!reservationStation.station[currentStation].Busy){
                            break;
                        }
                    }
                    instructionStatus[ClockCycle].push_back(instruction.Inst);
                    instructionStatus[ClockCycle].push_back(to_string(ClockCycle));
                    if(registerStatus.status[instruction.RS1] != ""){ // if not ready
                        reservationStation.station[currentStation].Qj = registerStatus.status[instruction.RS1];
                    }else{
                        reservationStation.station[currentStation].Vj = registerFile[(instruction.RS1[1]-'0')];
                        reservationStation.station[currentStation].Qj = "";
                    }

                    reservationStation.station[currentStation].A = (instruction.offset);
                    reservationStation.station[currentStation].Busy = true;

                    reservationStation.station[currentStation].PCStart = PC;
                    reservationStation.station[currentStation].clockCycle = ClockCycle;
                    reservationStation.station[currentStation].Op = instruction.op;

                    
                    //reservationStation.station[currentStation].instruction = instruction;

                    registerStatus.status[instruction.RD] = currentStation;

                    inflightInstructions.push_back(instruction);
                    LoadStoreQueue.push(ClockCycle);
                    reservationStation.currentLoad++;
                    issuedQueue.push(instruction);

                    //if(predicting.front()) reservationStation.station[currentStation].isPredicted = true;
                    PC++;
                }
            }else if (instruction.op == "STORE"){
                if(reservationStation.currentStore < reservationStation.numStore){
                    // check for available register
                    //loop through Load reservation stations to find available one
                    for(int i = 0; i < reservationStation.numStore; i++){
                        currentStation = "Store" + to_string(i);
                        if(!reservationStation.station[currentStation].Busy){
                            break;
                        }
                    }
                    instructionStatus[ClockCycle].push_back(instruction.Inst);
                    instructionStatus[ClockCycle].push_back(to_string(ClockCycle));
                    if(registerStatus.status[instruction.RS1] != ""){ // if not ready
                        reservationStation.station[currentStation].Qj = registerStatus.status[instruction.RS1];
                    }else{
                        reservationStation.station[currentStation].Vj = registerFile[(instruction.RS1[1]-'0')];
                        reservationStation.station[currentStation].Qj = "";
                    }

                    if(registerStatus.status[instruction.RS2] != ""){ // if not ready
                        reservationStation.station[currentStation].Qk = registerStatus.status[instruction.RS2];
                    }else{
                        reservationStation.station[currentStation].Vk = registerFile[(instruction.RS2[1]-'0')];
                        reservationStation.station[currentStation].Qk = "";
                    }

                    reservationStation.station[currentStation].A = (instruction.offset);
                    reservationStation.station[currentStation].Busy = true;
                    //reservationStation.station[currentStation].instruction = instruction;

                    reservationStation.station[currentStation].PCStart = PC;
                    reservationStation.station[currentStation].clockCycle = ClockCycle;
                    reservationStation.station[currentStation].Op = instruction.op;

                    inflightInstructions.push_back(instruction);
                    LoadStoreQueue.push(ClockCycle);
                    reservationStation.currentStore++;
                    issuedQueue.push(instruction);

                    //if(predicting) reservationStation.station[currentStation].isPredicted = true;
                    PC++;
                }
            }else if (instruction.op == "BNE"){
                if(reservationStation.currentBne < reservationStation.numBne){
                    // check for available register
                    //loop through Load reservation stations to find available one
                    for(int i = 0; i < reservationStation.numBne; i++){
                        currentStation = "Bne" + to_string(i);
                        if(!reservationStation.station[currentStation].Busy){
                            break;
                        }
                    }
                    instructionStatus[ClockCycle].push_back(instruction.Inst);
                    instructionStatus[ClockCycle].push_back(to_string(ClockCycle));

                    if(registerStatus.status[instruction.RS1] != ""){ // if not ready
                        reservationStation.station[currentStation].Qj = registerStatus.status[instruction.RS1];
                    }else{
                        reservationStation.station[currentStation].Vj = registerFile[(instruction.RS1[1]-'0')];
                        reservationStation.station[currentStation].Qj = "";
                    }

                    if(registerStatus.status[instruction.RS2] != ""){ // if not ready
                        reservationStation.station[currentStation].Qk = registerStatus.status[instruction.RS2];
                    }else{
                        reservationStation.station[currentStation].Vk = registerFile[(instruction.RS2[1]-'0')];
                        reservationStation.station[currentStation].Qk = "";
                    }

                    reservationStation.station[currentStation].A = (instruction.offset);
                    reservationStation.station[currentStation].Busy = true;
                    //reservationStation.station[currentStation].instruction = instruction;

                    reservationStation.station[currentStation].PCStart = PC;
                    reservationStation.station[currentStation].clockCycle = ClockCycle;
                    reservationStation.station[currentStation].Op = instruction.op;

                    inflightInstructions.push_back(instruction);
                    reservationStation.currentBne++;
                    issuedQueue.push(instruction);

                    //if(predicting) reservationStation.station[currentStation].isPredicted = true;

                    //predicting = true;
                    predicting.push(ClockCycle);
                    PC++;
                }
            }else if (instruction.op == "CALL"){
                if(reservationStation.currentCallRet < reservationStation.numCallRet){
                    // check for available register
                    //loop through Load reservation stations to find available one
                    if(predicting.empty() || predicting.front() > ClockCycle){
                        
                        for(int i = 0; i < reservationStation.numCallRet; i++){
                            currentStation = "CallRet" + to_string(i);
                            if(!reservationStation.station[currentStation].Busy){
                                break;
                            }
                        }
                   
                        instructionStatus[ClockCycle].push_back(instruction.Inst);
                        instructionStatus[ClockCycle].push_back(to_string(ClockCycle));

                        reservationStation.station[currentStation].A = (instruction.label);
                        registerStatus.status["R1"] = currentStation;
                        reservationStation.station[currentStation].Busy = true;
                        //reservationStation.station[currentStation].instruction = instruction;

                        reservationStation.station[currentStation].PCStart = PC;
                        reservationStation.station[currentStation].clockCycle = ClockCycle;
                        reservationStation.station[currentStation].Op = instruction.op;

                        //if(predicting) reservationStation.station[currentStation].isPredicted = true;
                        PC = instruction.label - startingAddress;

                        inflightInstructions.push_back(instruction);
                        reservationStation.currentCallRet++;
                        issuedQueue.push(instruction);

                    //PC++;
                    }

                }
            }else if (instruction.op == "RET"){
                if(reservationStation.currentCallRet < reservationStation.numCallRet){
                    // check for available register
                    //loop through Load reservation stations to find available one
                    for(int i = 0; i < reservationStation.numCallRet; i++){
                        currentStation = "CallRet" + to_string(i);
                        if(!reservationStation.station[currentStation].Busy){
                            break;
                        }
                    }
                    instructionStatus[ClockCycle].push_back(instruction.Inst);
                    instructionStatus[ClockCycle].push_back(to_string(ClockCycle));

                    reservationStation.station[currentStation].Busy = true;
                    //reservationStation.station[currentStation].instruction = instruction;

                    reservationStation.station[currentStation].PCStart = PC;
                    reservationStation.station[currentStation].clockCycle = ClockCycle;
                    reservationStation.station[currentStation].Op = instruction.op;

                    inflightInstructions.push_back(instruction);
                    reservationStation.currentCallRet++;
                    RetInFlight = true;
                    issuedQueue.push(instruction);

                    //if(predicting) reservationStation.station[currentStation].isPredicted = true;
                    //PC++;
                }
            }else if (instruction.op == "ADD"){
                if(reservationStation.currentAdd < reservationStation.numAdd){
                    // check for available register
                    //loop through Load reservation stations to find available one
                    for(int i = 0; i < reservationStation.numAdd; i++){
                        currentStation = "Add" + to_string(i);
                        if(!reservationStation.station[currentStation].Busy){
                            break;
                        }
                    }
                    instructionStatus[ClockCycle].push_back(instruction.Inst);
                    instructionStatus[ClockCycle].push_back(to_string(ClockCycle));

                    if(registerStatus.status[instruction.RS1] != ""){ // if not ready
                        reservationStation.station[currentStation].Qj = registerStatus.status[instruction.RS1];
                    }else{
                        reservationStation.station[currentStation].Vj = registerFile[(instruction.RS1[1]-'0')];
                        reservationStation.station[currentStation].Qj = "";
                    }

                    if(registerStatus.status[instruction.RS2] != ""){ // if not ready
                        reservationStation.station[currentStation].Qk = registerStatus.status[instruction.RS2];
                    }else{
                        reservationStation.station[currentStation].Vk = registerFile[(instruction.RS2[1]-'0')];
                        reservationStation.station[currentStation].Qk = "";
                    }

                    reservationStation.station[currentStation].Busy = true;
                    //reservationStation.station[currentStation].instruction = instruction;

                    reservationStation.station[currentStation].PCStart = PC;
                    reservationStation.station[currentStation].clockCycle = ClockCycle;
                    reservationStation.station[currentStation].Op = instruction.op;

                    registerStatus.status[instruction.RD] = currentStation;

                    inflightInstructions.push_back(instruction);
                    reservationStation.currentAdd++;
                    issuedQueue.push(instruction);

                    //if(predicting) reservationStation.station[currentStation].isPredicted = true;
                    PC++;
                }
            }else if (instruction.op == "ADDI"){
                if(reservationStation.currentAdd < reservationStation.numAdd){
                    // check for available register
                    //loop through Load reservation stations to find available one
                    for(int i = 0; i < reservationStation.numAdd; i++){
                        currentStation = "Add" + to_string(i);
                        if(!reservationStation.station[currentStation].Busy){
                            break;
                        }
                    }
                    instructionStatus[ClockCycle].push_back(instruction.Inst);
                    instructionStatus[ClockCycle].push_back(to_string(ClockCycle));

                    if(registerStatus.status[instruction.RS1] != ""){ // if not ready
                        reservationStation.station[currentStation].Qj = registerStatus.status[instruction.RS1];
                    }else{
                        reservationStation.station[currentStation].Vj = registerFile[(instruction.RS1[1]-'0')];
                        reservationStation.station[currentStation].Qj = "";
                    }

                    reservationStation.station[currentStation].Vk = instruction.IMM;
                    reservationStation.station[currentStation].Qk = "";
                    //reservationStation.station[currentStation].instruction = instruction;

                    reservationStation.station[currentStation].PCStart = PC;
                    reservationStation.station[currentStation].clockCycle = ClockCycle;
                    reservationStation.station[currentStation].Op = instruction.op;

                    reservationStation.station[currentStation].Busy = true;
                    registerStatus.status[instruction.RD] = currentStation;

                    inflightInstructions.push_back(instruction);
                    reservationStation.currentAdd++;
                    issuedQueue.push(instruction);

                    //if(predicting) reservationStation.station[currentStation].isPredicted = true;
                    PC++;
                }
            
            }else if (instruction.op == "NAND"){
                if(reservationStation.currentNand < reservationStation.numNand){
                    // check for available register
                    //loop through Load reservation stations to find available one
                    for(int i = 0; i < reservationStation.numNand; i++){
                        currentStation = "Nand" + to_string(i);
                        if(!reservationStation.station[currentStation].Busy){
                            break;
                        }
                    }

                    instructionStatus[ClockCycle].push_back(instruction.Inst);
                    instructionStatus[ClockCycle].push_back(to_string(ClockCycle));

                    if(registerStatus.status[instruction.RS1] != ""){ // if not ready
                        reservationStation.station[currentStation].Qj = registerStatus.status[instruction.RS1];
                    }else{
                        reservationStation.station[currentStation].Vj = registerFile[(instruction.RS1[1]-'0')];
                        reservationStation.station[currentStation].Qj = "";
                    }

                    if(registerStatus.status[instruction.RS2] != ""){ // if not ready
                        reservationStation.station[currentStation].Qk = registerStatus.status[instruction.RS2];
                    }else{
                        reservationStation.station[currentStation].Vk = registerFile[(instruction.RS2[1]-'0')];
                        reservationStation.station[currentStation].Qk = "";
                    }

                    reservationStation.station[currentStation].Busy = true;
                    registerStatus.status[instruction.RD] = currentStation;
                    //reservationStation.station[currentStation].instruction = instruction;

                    reservationStation.station[currentStation].PCStart = PC;
                    reservationStation.station[currentStation].clockCycle = ClockCycle;
                    reservationStation.station[currentStation].Op = instruction.op;

                    inflightInstructions.push_back(instruction);
                    reservationStation.currentNand++;
                    issuedQueue.push(instruction);

                    //if(predicting) reservationStation.station[currentStation].isPredicted = true;
                    PC++;
                }
            }else if (instruction.op == "DIV"){
                if(reservationStation.currentDiv < reservationStation.numDiv){
                    // check for available register
                    //loop through Load reservation stations to find available one
                    for(int i = 0; i < reservationStation.numDiv; i++){
                        currentStation = "Div" + to_string(i);
                        if(!reservationStation.station[currentStation].Busy){
                            break;
                        }
                    }

                    instructionStatus[ClockCycle].push_back(instruction.Inst);
                    instructionStatus[ClockCycle].push_back(to_string(ClockCycle));

                    if(registerStatus.status[instruction.RS1] != ""){ // if not ready
                        reservationStation.station[currentStation].Qj = registerStatus.status[instruction.RS1];
                    }else{
                        reservationStation.station[currentStation].Vj = registerFile[(instruction.RS1[1]-'0')];
                        reservationStation.station[currentStation].Qj = "";
                    }

                    if(registerStatus.status[instruction.RS2] != ""){ // if not ready
                        reservationStation.station[currentStation].Qk = registerStatus.status[instruction.RS2];
                    }else{
                        reservationStation.station[currentStation].Vk = registerFile[(instruction.RS2[1]-'0')];
                        reservationStation.station[currentStation].Qk = "";
                    }

                    reservationStation.station[currentStation].Busy = true;
                    //reservationStation.station[currentStation].instruction = instruction;

                    reservationStation.station[currentStation].PCStart = PC;
                    reservationStation.station[currentStation].clockCycle = ClockCycle;
                    reservationStation.station[currentStation].Op = instruction.op;

                    registerStatus.status[instruction.RD] = currentStation;

                    inflightInstructions.push_back(instruction);
                    reservationStation.currentDiv++;
                    issuedQueue.push(instruction);

                    //if(predicting) reservationStation.station[currentStation].isPredicted = true;
                    PC++;
                }
            
            }else{
                cout << "Invalid instruction" << endl;
            }

            for(auto it = pleaseFree.begin(); it != pleaseFree.end(); it++)
            {
                if(it->second)
                {
                    reservationStation.station[it->first].Op = "";
                    reservationStation.station[it->first].Vj = 0;
                    reservationStation.station[it->first].Vk = 0;
                    reservationStation.station[it->first].Qj = "";
                    reservationStation.station[it->first].Qk = "";
                    reservationStation.station[it->first].A = 0;
                    reservationStation.station[it->first].Busy = false;
                    reservationStation.station[it->first].clockCycle = 0;
                    reservationStation.station[it->first].finishesExecutionInCycle = 0;
                    reservationStation.station[it->first].Result = 0;
                    reservationStation.station[it->first].executed = false;
                    it->second = false;
                    if(it->first[0] == 'C' || it->first[0] == 'R') reservationStation.currentCallRet--;
                    else if(it->first[0] == 'A') reservationStation.currentAdd--;
                    else if(it->first[0] == 'L') reservationStation.currentLoad--;
                    else if(it->first[0] == 'D') reservationStation.currentDiv--;
                    else if(it->first[0] == 'N') reservationStation.currentNand--;
                    else if(it->first[0] == 'S') reservationStation.currentStore--;
                    else if(it->first[0] == 'B') reservationStation.currentBne--;
                }
            }

        };
        void Execute(){
            // iterate over all busy reservation stations
            if(RetInFlight){
                auto it = reservationStation.station.begin();
                while(it != reservationStation.station.end()){
                    if(it->second.Busy && !it->second.executed && ((predicting.empty()) || !(predicting.front() < it->second.clockCycle)) && it->second.Op == "RET"){
                        it->second.finishesExecutionInCycle = ClockCycle + reservationStation.cyclesCallRet;
                        //it->second.Result = registerFile[1];
                        instructionStatus[it->second.clockCycle].push_back(to_string(ClockCycle));
                        instructionStatus[it->second.clockCycle].push_back(to_string(it->second.finishesExecutionInCycle));
                        it->second.executed = true;
                    }
                    it++;
                }
                
                
                return;
            }
            auto it = reservationStation.station.begin();
            bool popLoadStore = false;
            while(it != reservationStation.station.end()){
                if(it->second.Busy && !it->second.executed && ((predicting.empty()) || !(predicting.front() < it->second.clockCycle))){
                    if(it->second.Op == "LOAD"){
                        if(it->second.Qj == "" && it->second.clockCycle == LoadStoreQueue.front()){
                            it->second.Result = Memory[it->second.Vj + (it->second.A)];
                            popLoadStore = true;
                            it->second.finishesExecutionInCycle = ClockCycle + reservationStation.cyclesLoad;
                            instructionStatus[it->second.clockCycle].push_back(to_string(ClockCycle));
                            instructionStatus[it->second.clockCycle].push_back(to_string(it->second.finishesExecutionInCycle));
                            it->second.executed = true;
                        }
                    }else if(it->second.Op == "STORE"){
                        if(it->second.Qj == "" && it->second.clockCycle == LoadStoreQueue.front()){
                            it->second.A = it->second.Vj + (it->second.A);
                            popLoadStore = true;
                            it->second.finishesExecutionInCycle = ClockCycle + reservationStation.cyclesStore;
                            instructionStatus[it->second.clockCycle].push_back(to_string(ClockCycle));
                            instructionStatus[it->second.clockCycle].push_back(to_string(it->second.finishesExecutionInCycle));
                            it->second.executed = true;
                        }

                    }else if(it->second.Op == "BNE"){
                        if(it->second.Qj == "" && it->second.Qk == ""){
                            it->second.Result = it->second.Vj != it->second.Vk;
                            it->second.finishesExecutionInCycle = ClockCycle + reservationStation.cyclesBne;
                            instructionStatus[it->second.clockCycle].push_back(to_string(ClockCycle));
                            instructionStatus[it->second.clockCycle].push_back(to_string(it->second.finishesExecutionInCycle));
                            it->second.executed = true;
                        }
                        
                    }else if(it->second.Op == "CALL"){
                        if(it->second.Qj == "" && it->second.Qk == ""){
                            it->second.Result = it->second.PCStart + 1;
                            it->second.finishesExecutionInCycle = ClockCycle + reservationStation.cyclesCallRet - 1;
                            instructionStatus[it->second.clockCycle].push_back(to_string(ClockCycle));
                            instructionStatus[it->second.clockCycle].push_back(to_string(it->second.finishesExecutionInCycle));
                            it->second.executed = true;
                            //PC = it->second.A - startingAddress;
                        }
                    }else if(it->second.Op == "RET"){
                        it->second.finishesExecutionInCycle = ClockCycle + reservationStation.cyclesCallRet;
                        instructionStatus[it->second.clockCycle].push_back(to_string(ClockCycle));
                        instructionStatus[it->second.clockCycle].push_back(to_string(it->second.finishesExecutionInCycle));
                        it->second.executed = true;
                    
                    }else if(it->second.Op == "ADD"){
                        if(it->second.Qj == "" && it->second.Qk == ""){
                            it->second.Result = it->second.Vj + it->second.Vk;
                            it->second.finishesExecutionInCycle = ClockCycle + reservationStation.cyclesAdd - 1;
                            instructionStatus[it->second.clockCycle].push_back(to_string(ClockCycle));
                            instructionStatus[it->second.clockCycle].push_back(to_string(it->second.finishesExecutionInCycle));
                            it->second.executed = true;
                        }
                    }else if(it->second.Op == "ADDI"){
                        if(it->second.Qj == ""){
                            it->second.Result = it->second.Vj + it->second.Vk;
                            it->second.finishesExecutionInCycle = ClockCycle + reservationStation.cyclesAdd - 1;
                            instructionStatus[it->second.clockCycle].push_back(to_string(ClockCycle));
                            instructionStatus[it->second.clockCycle].push_back(to_string(it->second.finishesExecutionInCycle));
                            it->second.executed = true;
                        }

                    }else if(it->second.Op == "NAND"){
                        if(it->second.Qj == "" && it->second.Qk == ""){
                            it->second.Result = ~(it->second.Vj & it->second.Vk);
                            it->second.finishesExecutionInCycle = ClockCycle + reservationStation.cyclesNand - 1;
                            instructionStatus[it->second.clockCycle].push_back(to_string(ClockCycle));
                            instructionStatus[it->second.clockCycle].push_back(to_string(it->second.finishesExecutionInCycle));
                            it->second.executed = true;
                        }

                    }else if(it->second.Op == "DIV"){
                        if(it->second.Qj == "" && it->second.Qk == ""){
                            it->second.Result = it->second.Vj / it->second.Vk;
                            it->second.finishesExecutionInCycle = ClockCycle + reservationStation.cyclesDiv - 1;
                            instructionStatus[it->second.clockCycle].push_back(to_string(ClockCycle));
                            instructionStatus[it->second.clockCycle].push_back(to_string(it->second.finishesExecutionInCycle));
                            it->second.executed = true;
                        }

                    }else{
                        cout << "Invalid instruction" << endl;
                    }
                    
                }
                it++;
            }
            
            if(popLoadStore){
                LoadStoreQueue.pop();
                popLoadStore = false;
            }
        }
        class myCompare {
        public:
        int operator()(const ReservationStationEntry& lhs, const ReservationStationEntry& rhs) const {
            return lhs.clockCycle > rhs.clockCycle; 
        }
        };
        void WriteBack() {

            // create a min heap of reservation stations based on finishesExecutionInCycle
            // populate heap
        //create a priority queue of reservation stations based on finishesExecutionInCycle
        priority_queue<ReservationStationEntry, vector<ReservationStationEntry>, myCompare> pq;

        for(auto it = reservationStation.station.begin(); it !=  reservationStation.station.end(); it++)
        {
            if(it->second.Busy && it->second.executed && it->second.finishesExecutionInCycle < ClockCycle)
                pq.push(it->second);
        }
        // while(!pq.empty() && !pq.top().Busy)
        // {
        //     pq.pop();
        // }
        if(pq.empty())
        {
            return;
        }
        // pop the top of the heap and check if it is ready to write back
        if(pq.top().stationName[0] == 'A'||pq.top().stationName[0] == 'L'||pq.top().stationName[0] == 'D'||pq.top().stationName[0] == 'N')
        {
            if(pq.top().finishesExecutionInCycle < ClockCycle)
            {
                instructionStatus[pq.top().clockCycle].push_back(to_string(ClockCycle));
                for(int i = 0; i < 8; i++)
                {
                    if(registerStatus.status["R" + to_string(i)] == pq.top().stationName)
                    {
                        registerFile[i] = pq.top().Result;
                        registerStatus.status["R" + to_string(i)] = "";
                    }
                }
                for(auto it = reservationStation.station.begin(); it !=  reservationStation.station.end(); it++)
                {
                    if(it->second.Qj == pq.top().stationName)
                    {
                        it->second.Vj = pq.top().Result;
                        it->second.Qj = "";
                    }
                    if(it->second.Qk == pq.top().stationName)
                    {
                        it->second.Vk = pq.top().Result;
                        it->second.Qk = "";
                    }
                }
                if(pq.top().stationName[0] == 'A')
                {
                    //reservationStation.currentAdd--;
                    pleaseFree[pq.top().stationName] = true;
                }
                else if(pq.top().stationName[0] == 'L')
                {
                    //reservationStation.currentLoad--;
                    pleaseFree[pq.top().stationName] = true;
                }
                else if(pq.top().stationName[0] == 'D')
                {
                    //reservationStation.currentDiv--;
                    pleaseFree[pq.top().stationName] = true;
                }
                else if(pq.top().stationName[0] == 'N')
                {
                    //reservationStation.currentNand--;
                    pleaseFree[pq.top().stationName] = true;
                }
            }
        }
        else if(pq.top().stationName[0] == 'S' && pq.top().finishesExecutionInCycle < ClockCycle)
        {
            if(pq.top().Qk == "")
            { 
            Memory[pq.top().A] = pq.top().Vk;
            //reservationStation.currentStore--;
            pleaseFree[pq.top().stationName] = true;
            instructionStatus[pq.top().clockCycle].push_back(to_string(ClockCycle));
            }
        }
        else if(pq.top().stationName[0] == 'B' && pq.top().finishesExecutionInCycle < ClockCycle) 
        {
            //predicting = false;
            // remove this inst from the prediction queue
            predicting.pop();
            if(pq.top().Result) // taken logic
            {
                // flush all instructions after the branch
                PC = pq.top().A + pq.top().PCStart;
                //reservationStation.currentBne--;
                instructionStatus[pq.top().clockCycle].push_back(to_string(ClockCycle));
                pleaseFree[pq.top().stationName] = true;

                for(auto it = reservationStation.station.begin(); it !=  reservationStation.station.end(); it++)
                {
                    if(it->second.clockCycle > pq.top().clockCycle)
                    {
                        // if the instruction is a branch remove it from the branch queue
                        if(it->second.stationName[0] == 'B')
                        {
                            // remove this inst from the prediction queue
                            predicting.pop();
                        }
                        it->second.Op = "";
                        it->second.Vj = 0;
                        it->second.Vk = 0;
                        it->second.Qj = "";
                        it->second.Qk = "";
                        it->second.A = 0;
                        it->second.Busy = false;
                        it->second.clockCycle = 0;
                        it->second.finishesExecutionInCycle = 0;
                        it->second.Result = 0;
                        if(it->first[0] == 'C' || it->first[0] == 'R') reservationStation.currentCallRet--;
                        else if(it->first[0] == 'A') reservationStation.currentAdd--;
                        else if(it->first[0] == 'L') reservationStation.currentLoad--;
                        else if(it->first[0] == 'D') reservationStation.currentDiv--;
                        else if(it->first[0] == 'N') reservationStation.currentNand--;
                        else if(it->first[0] == 'S') reservationStation.currentStore--;
                        else if(it->first[0] == 'B') reservationStation.currentBne--;
                    }
                }
            }
        }
        else if(pq.top().stationName[0] == 'C' && pq.top().finishesExecutionInCycle < ClockCycle)
        {
            if(pq.top().Op == "CALL")
            {
            if(registerStatus.status["R1"] == pq.top().stationName)
                    {
                        registerFile[1] = pq.top().Result;
                        registerStatus.status["R1"] = "";
                    }
            //reservationStation.currentCallRet--;
            pleaseFree[pq.top().stationName] = true;
            instructionStatus[pq.top().clockCycle].push_back(to_string(ClockCycle));
            }else{
                // RET
                if(registerStatus.status["R1"] == "")
                {
                    RetInFlight = false;
                    PC = registerFile[1];
                    //reservationStation.currentCallRet--;
                    pleaseFree[pq.top().stationName] = true;
                    instructionStatus[pq.top().clockCycle].push_back(to_string(ClockCycle));
                }
            
            }
        }
        else if(pq.top().stationName[0] == 'R' && pq.top().finishesExecutionInCycle < ClockCycle)
        {
            if(registerStatus.status["R1"] == "")
            {
            RetInFlight = false;
            PC = registerFile[1];
            //reservationStation.currentCallRet--;
            pleaseFree[pq.top().stationName] = true;
            instructionStatus[pq.top().clockCycle].push_back(to_string(ClockCycle));
            }
            
        }
        else
        {
            cout << "Invalid instruction " << pq.top().stationName << " " << pq.top().Op << endl;
            pq.pop();
        }
        //pleaseFree[pq.top().stationName] = true;

    }
        void initializeMem()
        {
            cout << "\n\n----------------------------------------------------------------------------------------\n\n";
            cout << "Enter values for memory" << endl;
            cout << "Enter -1 to stop entering values" << endl << endl;
            int address = 0;
            while(address != -1)
            {
                cout << "Enter address: ";
                cin >> address;
                while(address < -1 || address > 65535)
                {
                    cout << "Address must be between 0 and 65535" << endl;
                    cout << "Enter address: ";
                    cin >> address;
                }
                if(address != -1)
                {
                    cout << "Enter value: ";
                    cin >> Memory[address];
                }
                cout << endl;
            }
            cout << "\n\n----------------------------------------------------------------------------------------\n\n";
        }
    

        void ReadInstructionsFromFile(string filename){
            // output error and prompt user to enter filename again if file does not exist
            // read instructions from file and populate instruction queue
            ifstream file(filename);
            while(!file.is_open()){
                cout << "File does not exist" << endl;
                cout << "Please enter the file path relative to this file:: ";
                cin >> filename;
                file.open(filename);
            }
            string line;
            while(getline(file, line)){
                instructionQueue.push_back(Instruction(line));
            }
            file.close();
            
        }
        void ReadInstructionsFromUser(){
            // prompt user to enter instructions
            // populate instruction queue
            string input;
            cout << "Please enter instructions (enter done to stop): " << endl;
            while(input != "done"){
                getline(cin, input);
                if(input != "done"){
                    instructionQueue.push_back(Instruction(input));
                }
            }
        }
        void printTable()
        {
             cout << "Cycle \t" << "Instruction \t\t" <<"Issue \t" <<"Start EX \t"<<"End EX\t"<<"WB\t" <<endl;
            for(auto it = instructionStatus.begin(); it != instructionStatus.end(); it++)
            {
                // only print if not an empty entry
                if(it->second.size() == 5){
                    cout << it->first << "\t" << it->second[0] << "\t\t" << it->second[1] << "\t" << it->second[2] << "\t" << it->second[3] << "\t" << it->second[4] << endl;
                }else if(it->second.size() == 2){
                    cout << it->first << "\t" << it->second[0] << "\t\t" << it->second[1] << "\tFLUSHED" << endl;
                }
            }
        }
};

int main(){

    Tomasulo tomasulo;
    //vector<Instruction> instructionQueue;
    //tomasulo.initializeMem();

    // instructionQueue.push_back(Instruction("LOAD R1, 0(R0)"));
    // instructionQueue.push_back(Instruction("LOAD R2, 1(R0)"));
    // instructionQueue.push_back(Instruction("ADD R3, R1, R2"));
    // instructionQueue.push_back(Instruction("STORE R3, 2(R0)"));
    // tomasulo.instructionQueue = instructionQueue;

    while(!tomasulo.isFinished()){
        tomasulo.RunClockCycle();
    }
    tomasulo.printTable();
    cout << "Memory: " << tomasulo.Memory[2] << endl;
    cout << "R1: " << tomasulo.registerFile[1] << endl;
    cout << "R2: " << tomasulo.registerFile[2] << endl;
    cout << "R3: " << tomasulo.registerFile[3] << endl;
    return 0;
    

}