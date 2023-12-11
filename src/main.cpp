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


        uint16_t PC;
        int ClockCycle;
        int startingAddress;

        bool isFinished(){
            return instructionQueue.empty() && inflightInstructions.empty();
        }


        Tomasulo() {
            PC = 0;
            ClockCycle = 0;
            RetInFlight = false;
            // populate instruction queue either with use input or from file
            // prompt for starting address if file is used
            cout << "Please enter starting address: ";
            cin >> startingAddress;
            // prompt for data to initialize memory
            for(auto it = reservationStation.station.begin(); it !=  reservationStation.station.end(); it++)
            {
                pleaseFree[it->first] = false;
            }
        }
        void RunClockCycle();
        void Issue(){
            // Read instruction from instruction queue
            // Check type of instruction
            // Check for unused reservation station of that type

            if(RetInFlight){
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

                        reservationStation.station[currentStation].A = (instruction.label);
                        registerStatus.status["R1"] = currentStation;
                        reservationStation.station[currentStation].Busy = true;
                        //reservationStation.station[currentStation].instruction = instruction;

                        reservationStation.station[currentStation].PCStart = PC;
                        reservationStation.station[currentStation].clockCycle = ClockCycle;

                        //if(predicting) reservationStation.station[currentStation].isPredicted = true;
                        PC = instruction.label - startingAddress;

                        inflightInstructions.push_back(instruction);
                        reservationStation.currentCallRet++;
                        issuedQueue.push(instruction);

                    PC++;
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

                    reservationStation.station[currentStation].Busy = true;
                    //reservationStation.station[currentStation].instruction = instruction;

                    reservationStation.station[currentStation].PCStart = PC;
                    reservationStation.station[currentStation].clockCycle = ClockCycle;

                    inflightInstructions.push_back(instruction);
                    reservationStation.currentCallRet++;
                    RetInFlight = true;
                    issuedQueue.push(instruction);

                    //if(predicting) reservationStation.station[currentStation].isPredicted = true;
                    PC++;
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

                    reservationStation.station[currentStation].PCStart = PC;
                    reservationStation.station[currentStation].clockCycle = ClockCycle;

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

        };
        void Execute(){
            // iterate over all busy reservation stations
            if(RetInFlight){
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
                        }
                    }else if(it->second.Op == "STORE"){
                        if(it->second.Qj == "" && it->second.clockCycle == LoadStoreQueue.front()){
                            it->second.A = it->second.Vj + (it->second.A);
                            popLoadStore = true;
                            it->second.finishesExecutionInCycle = ClockCycle + reservationStation.cyclesStore;
                        }

                    }else if(it->second.Op == "BNE"){
                        if(it->second.Qj == "" && it->second.Qk == ""){
                            it->second.Result = it->second.Vj != it->second.Vk;
                            it->second.finishesExecutionInCycle = ClockCycle + reservationStation.cyclesBne;
                        }
                        
                    }else if(it->second.Op == "CALL"){
                        if(it->second.Qj == "" && it->second.Qk == ""){
                            it->second.Result = it->second.PCStart + 1;
                            it->second.finishesExecutionInCycle = ClockCycle + reservationStation.cyclesCallRet;
                        }
                    }else if(it->second.Op == "RET"){
                        it->second.finishesExecutionInCycle = ClockCycle + reservationStation.cyclesCallRet;
                    
                    }else if(it->second.Op == "ADD"){
                        if(it->second.Qj == "" && it->second.Qk == ""){
                            it->second.Result = it->second.Vj + it->second.Vk;
                            it->second.finishesExecutionInCycle = ClockCycle + reservationStation.cyclesAdd;
                        }
                    }else if(it->second.Op == "ADDI"){
                        if(it->second.Qj == ""){
                            it->second.Result = it->second.Vj + it->second.Vk;
                            it->second.finishesExecutionInCycle = ClockCycle + reservationStation.cyclesAdd - 1;
                        }

                    }else if(it->second.Op == "NAND"){
                        if(it->second.Qj == "" && it->second.Qk == ""){
                            it->second.Result = ~(it->second.Vj & it->second.Vk);
                            it->second.finishesExecutionInCycle = ClockCycle + reservationStation.cyclesNand - 1;
                        }

                    }else if(it->second.Op == "DIV"){
                        if(it->second.Qj == "" && it->second.Qk == ""){
                            it->second.Result = it->second.Vj / it->second.Vk;
                            it->second.finishesExecutionInCycle = ClockCycle + reservationStation.cyclesDiv - 1;
                        }

                    }else{
                        cout << "Invalid instruction" << endl;
                    }
                    it->second.executed = true;
                }
                it++;
            }
            
            if(popLoadStore){
                LoadStoreQueue.pop();
                popLoadStore = false;
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
            if(pq.top().finishesExecutionInCycle < ClockCycle)
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
                if(pq.top().stationName[0] == 'A')
                {
                    reservationStation.station[pq.top().stationName].Busy = false;
                    reservationStation.currentAdd--;
                    pleaseFree[pq.top().stationName] = true;
                }
                else if(pq.top().stationName[0] == 'L')
                {
                    reservationStation.station[pq.top().stationName].Busy = false;
                    reservationStation.currentLoad--;
                    pleaseFree[pq.top().stationName] = true;
                }
                else if(pq.top().stationName[0] == 'D')
                {
                    reservationStation.station[pq.top().stationName].Busy = false;
                    reservationStation.currentDiv--;
                    pleaseFree[pq.top().stationName] = true;
                }
                else if(pq.top().stationName[0] == 'N')
                {
                    reservationStation.station[pq.top().stationName].Busy = false;
                    reservationStation.currentNand--;
                    pleaseFree[pq.top().stationName] = true;
                }
            }
        }
        else if(pq.top().stationName[0] == 'S' && pq.top().finishesExecutionInCycle < ClockCycle)
        {
            if(pq.top().Qk == "")
            { 
            Memory[pq.top().A] = pq.top().Vk;
            reservationStation.station[pq.top().stationName].Busy = false;
            reservationStation.currentStore--;
            pleaseFree[pq.top().stationName] = true;
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
                reservationStation.station[pq.top().stationName].Busy = false;
                reservationStation.currentBne--;
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
                    }
                }
            }
        }
        else if(pq.top().stationName[0] == 'C' && pq.top().finishesExecutionInCycle < ClockCycle)
        {
            registerFile[1] = pq.top().Result;
            reservationStation.station[pq.top().stationName].Busy = false;
            reservationStation.currentCallRet--;
        }
        else if(pq.top().stationName[0] == 'R' && pq.top().finishesExecutionInCycle < ClockCycle)
        {
            if(registerStatus.status["R1"] == "")
            {
            RetInFlight = false;
            PC = registerFile[1];
            reservationStation.station[pq.top().stationName].Busy = false;
            reservationStation.currentCallRet--;
            }
            
        }
        else
        {
            pq.pop();
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