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

        uint16_t PC;
        int ClockCycle;

        bool isFinished(){
            return instructionQueue.empty() && inflightInstructions.empty();
        }

        bool CallRetInFlight = false;

        Tomasulo() {
            PC = 0;
            ClockCycle = 0;
            // populate instruction queue either with use input or from file
            // prompt for starting address if file is used
            // prompt for data to initialize memory
        }
        void RunClockCycle();
        void Issue(){
            // Read instruction from instruction queue
            // Check type of instruction
            // Check for unused reservation station of that type
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
                    if(registerStatus.status[instruction.RS1] != ""){ // if not ready
                        reservationStation.station[currentStation].Qj = registerStatus.status[instruction.RS1];
                    }else{
                        reservationStation.station[currentStation].Vj = registerFile[(instruction.RS1[1]-'0')];
                        reservationStation.station[currentStation].Qj = "";
                    }

                    reservationStation.station[currentStation].A = (instruction.offset);
                    reservationStation.station[currentStation].Busy = true;
                    //reservationStation.station[currentStation].instruction = instruction;

                    registerStatus.status[instruction.RD] = currentStation;

                    inflightInstructions.push_back(instruction);
                    LoadStoreQueue.push(ClockCycle);
                    reservationStation.currentLoad++;
                    issuedQueue.push(instruction);
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

                    inflightInstructions.push_back(instruction);
                    LoadStoreQueue.push(ClockCycle);
                    reservationStation.currentStore++;
                    issuedQueue.push(instruction);
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

                    inflightInstructions.push_back(instruction);
                    reservationStation.currentBne++;
                    issuedQueue.push(instruction);
                }
            }else if (instruction.op == "CALL"){
                if(reservationStation.currentCallRet < reservationStation.numCallRet){
                    // check for available register
                    //loop through Load reservation stations to find available one
                    for(int i = 0; i < reservationStation.numCallRet; i++){
                        currentStation = "CallRet" + to_string(i);
                        if(!reservationStation.station[currentStation].Busy){
                            break;
                        }
                    }

                    reservationStation.station[currentStation].A = (instruction.label);

                    reservationStation.station[currentStation].Busy = true;
                    //reservationStation.station[currentStation].instruction = instruction;

                    inflightInstructions.push_back(instruction);
                    reservationStation.currentCallRet++;
                    CallRetInFlight = true;
                    issuedQueue.push(instruction);

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

                    reservationStation.station[currentStation].Busy = true;
                    //reservationStation.station[currentStation].instruction = instruction;

                    inflightInstructions.push_back(instruction);
                    reservationStation.currentCallRet++;
                    CallRetInFlight = true;
                    issuedQueue.push(instruction);

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

                    registerStatus.status[instruction.RD] = currentStation;

                    inflightInstructions.push_back(instruction);
                    reservationStation.currentAdd++;
                    issuedQueue.push(instruction);
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
                    if(registerStatus.status[instruction.RS1] != ""){ // if not ready
                        reservationStation.station[currentStation].Qj = registerStatus.status[instruction.RS1];
                    }else{
                        reservationStation.station[currentStation].Vj = registerFile[(instruction.RS1[1]-'0')];
                        reservationStation.station[currentStation].Qj = "";
                    }

                    reservationStation.station[currentStation].Vk = instruction.IMM;
                    reservationStation.station[currentStation].Qk = "";
                    //reservationStation.station[currentStation].instruction = instruction;

                    reservationStation.station[currentStation].Busy = true;
                    registerStatus.status[instruction.RD] = currentStation;

                    inflightInstructions.push_back(instruction);
                    reservationStation.currentAdd++;
                    issuedQueue.push(instruction);
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

                    inflightInstructions.push_back(instruction);
                    reservationStation.currentNand++;
                    issuedQueue.push(instruction);
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
                    registerStatus.status[instruction.RD] = currentStation;

                    inflightInstructions.push_back(instruction);
                    reservationStation.currentDiv++;
                    issuedQueue.push(instruction);
                }
            
            }else{
                cout << "Invalid instruction" << endl;
            }

        };
        void Execute(){
            // iterate over all busy reservation stations
            auto it = reservationStation.station.begin();
            while(it != reservationStation.station.end()){
                if(it->second.Busy){
                    if(it->second.Op == "LOAD"){
                        if(it->second.Qj == "" && it->second.clockCycle == LoadStoreQueue.front()){
                            it->second.Result = Memory[it->second.Vj + (it->second.A)];
                            executingQueue.push(inflightInstructions.front());
                            inflightInstructions.erase(inflightInstructions.begin());
                            it->second.Busy = false;
                            reservationStation.currentLoad--;
                            LoadStoreQueue.pop();
                            
                        }
                    }else if(it->second.Op == "STORE"){

                    }else if(it->second.Op == "BNE"){

                    }else if(it->second.Op == "CALL"){
                    
                    }else if(it->second.Op == "RET"){
                    
                    }else if(it->second.Op == "ADD"){

                    }else if(it->second.Op == "ADDI"){

                    }else if(it->second.Op == "NAND"){

                    }else if(it->second.Op == "DIV"){

                    }else{
                        cout << "Invalid instruction" << endl;
                    }

                }
            }
        }
        struct Compare {
        bool operator()(const ReservationStationEntry& lhs, const ReservationStationEntry& rhs) const {
            return lhs.finishesExecutionInCycle < rhs.finishesExecutionInCycle; 
        }
        };
        void WriteBack() {

            // create a min heap of reservation stations based on finishesExecutionInCycle
            // populate heap
        //create a priority queue of reservation stations based on finishesExecutionInCycle
        Compare compare;
        priority_queue<ReservationStationEntry, vector<ReservationStationEntry>, Compare> pq;

        for(auto it = reservationStation.station.begin(); it !=  reservationStation.station.end(); it++)
        {
            pq.push(it->second);
        }
        while(!pq.top().Busy)
        {
            pq.pop();
        }

        // pop the top of the heap and check if it is ready to write back
        if(pq.top().stationName[0] == 'A'||pq.top().stationName[0] == 'L'||pq.top().stationName[0] == 'D'||pq.top().stationName[0] == 'N')
        {
            if(pq.top().finishesExecutionInCycle > ClockCycle)
            {
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
                reservationStation.station[pq.top().stationName].Busy = false;
            }
        }
        else
        {
            if(pq.top().stationName[0] == 'S')
            {
                Memory[pq.top().A] = pq.top().Result;
                reservationStation.station[pq.top().stationName].Busy = false;
            }
            else if(pq.top().stationName[0] == 'B')
            {
                if(pq.top().finishesExecutionInCycle > ClockCycle)
                {
                    if(pq.top().Result)
                    {
                        PC = pq.top().A+pq.top().clockCycle;
                    }
                    reservationStation.station[pq.top().stationName].Busy = false;
                }
            }

        }




        }
};

int main(){

    Tomasulo tomasulo;
    vector<Instruction> instructionQueue;

    instructionQueue.push_back(Instruction("ADD, R1, R2, R3"));
    instructionQueue.push_back(Instruction("BNE RS1, RS2, offset"));
    instructionQueue.push_back(Instruction("NAND RD, Rs1, Rs2"));
    instructionQueue.push_back(Instruction("DIV RD, Rs1, Rs2"));

    tomasulo.instructionQueue = instructionQueue;
    tomasulo.Issue();
    tomasulo.PC++;
    tomasulo.ClockCycle++;
    tomasulo.Issue();
    tomasulo.Issue();
    tomasulo.Issue();


    return 0;
    

}