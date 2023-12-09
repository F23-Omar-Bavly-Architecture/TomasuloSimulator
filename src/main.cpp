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
        queue<Instruction> instructionQueue;
        vector<Instruction> inflightInstructions;
        array<int, 65536> Memory;
        RegisterFile registerFile;
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
            Instruction instruction = instructionQueue.front();
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
                    currentStation = "Load" + to_string(reservationStation.currentLoad);
                    if(registerStatus.status[instruction.RS1] != ""){ // if not ready
                        reservationStation.station[currentStation].Qj = registerStatus.status[instruction.RS1];
                    }else{
                        reservationStation.station[currentStation].Vj = registerFile[stoi(instruction.RS1)];
                        reservationStation.station[currentStation].Qj = "";
                    }

                    reservationStation.station[currentStation].A = instruction.offset;
                    reservationStation.station[currentStation].Busy = true;
                    registerStatus.status[instruction.RD] = currentStation;

                    instructionQueue.pop();
                    inflightInstructions.push_back(instruction);
                    reservationStation.currentLoad++;
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
                        reservationStation.station[currentStation].Vj = registerFile[stoi(instruction.RS1)];
                        reservationStation.station[currentStation].Qj = "";
                    }

                    if(registerStatus.status[instruction.RS2] != ""){ // if not ready
                        reservationStation.station[currentStation].Qk = registerStatus.status[instruction.RS2];
                    }else{
                        reservationStation.station[currentStation].Vk = registerFile[stoi(instruction.RS2)];
                        reservationStation.station[currentStation].Qk = "";
                    }

                    reservationStation.station[currentStation].A = instruction.offset;
                    reservationStation.station[currentStation].Busy = true;

                    instructionQueue.pop();
                    inflightInstructions.push_back(instruction);
                    reservationStation.currentStore++;
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
                        reservationStation.station[currentStation].Vj = registerFile[stoi(instruction.RS1)];
                        reservationStation.station[currentStation].Qj = "";
                    }

                    if(registerStatus.status[instruction.RS2] != ""){ // if not ready
                        reservationStation.station[currentStation].Qk = registerStatus.status[instruction.RS2];
                    }else{
                        reservationStation.station[currentStation].Vk = registerFile[stoi(instruction.RS2)];
                        reservationStation.station[currentStation].Qk = "";
                    }

                    reservationStation.station[currentStation].A = instruction.offset;
                    reservationStation.station[currentStation].Busy = true;

                    instructionQueue.pop();
                    inflightInstructions.push_back(instruction);
                    reservationStation.currentBne++;
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

                    reservationStation.station[currentStation].A = instruction.label;
                    reservationStation.station[currentStation].Busy = true;

                    instructionQueue.pop();
                    inflightInstructions.push_back(instruction);
                    reservationStation.currentCallRet++;
                    CallRetInFlight = true;

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

                    instructionQueue.pop();
                    inflightInstructions.push_back(instruction);
                    reservationStation.currentCallRet++;
                    CallRetInFlight = true;

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
                        reservationStation.station[currentStation].Vj = registerFile[stoi(instruction.RS1)];
                        reservationStation.station[currentStation].Qj = "";
                    }

                    if(registerStatus.status[instruction.RS2] != ""){ // if not ready
                        reservationStation.station[currentStation].Qk = registerStatus.status[instruction.RS2];
                    }else{
                        reservationStation.station[currentStation].Vk = registerFile[stoi(instruction.RS2)];
                        reservationStation.station[currentStation].Qk = "";
                    }

                    reservationStation.station[currentStation].Busy = true;
                    registerStatus.status[instruction.RD] = currentStation;

                    instructionQueue.pop();
                    inflightInstructions.push_back(instruction);
                    reservationStation.currentAdd++;
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
                        reservationStation.station[currentStation].Vj = registerFile[stoi(instruction.RS1)];
                        reservationStation.station[currentStation].Qj = "";
                    }

                    reservationStation.station[currentStation].Vk = instruction.IMM;
                    reservationStation.station[currentStation].Qk = "";

                    reservationStation.station[currentStation].Busy = true;
                    registerStatus.status[instruction.RD] = currentStation;

                    instructionQueue.pop();
                    inflightInstructions.push_back(instruction);
                    reservationStation.currentAdd++;
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
                        reservationStation.station[currentStation].Vj = registerFile[stoi(instruction.RS1)];
                        reservationStation.station[currentStation].Qj = "";
                    }

                    if(registerStatus.status[instruction.RS2] != ""){ // if not ready
                        reservationStation.station[currentStation].Qk = registerStatus.status[instruction.RS2];
                    }else{
                        reservationStation.station[currentStation].Vk = registerFile[stoi(instruction.RS2)];
                        reservationStation.station[currentStation].Qk = "";
                    }

                    reservationStation.station[currentStation].Busy = true;
                    registerStatus.status[instruction.RD] = currentStation;

                    instructionQueue.pop();
                    inflightInstructions.push_back(instruction);
                    reservationStation.currentNand++;
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
                        reservationStation.station[currentStation].Vj = registerFile[stoi(instruction.RS1)];
                        reservationStation.station[currentStation].Qj = "";
                    }

                    if(registerStatus.status[instruction.RS2] != ""){ // if not ready
                        reservationStation.station[currentStation].Qk = registerStatus.status[instruction.RS2];
                    }else{
                        reservationStation.station[currentStation].Vk = registerFile[stoi(instruction.RS2)];
                        reservationStation.station[currentStation].Qk = "";
                    }

                    reservationStation.station[currentStation].Busy = true;
                    registerStatus.status[instruction.RD] = currentStation;

                    instructionQueue.pop();
                    inflightInstructions.push_back(instruction);
                    reservationStation.currentDiv++;
                }
            
            }else{
                cout << "Invalid instruction" << endl;
            }

        };
        void Execute();
        void WriteBack();
};

int main(){

}