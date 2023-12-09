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
        Tomasulo() {
            PC = 0;
            ClockCycle = 0;
        }
        void RunClockCycle();
        void Issue(){
            // Read instruction from instruction queue
            // Check type of instruction
            // Check for unused reservation station of that type
            Instruction instruction = instructionQueue.front();
            if (instruction.op == "LOAD"){
                // check for available load reservation station
                if(reservationStation.currentLoad < reservationStation.numLoad){
                    // check for available register
                    string currentStation = "Load" + to_string(reservationStation.currentLoad);
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
                    string currentStation = "Store" + to_string(reservationStation.currentStore);
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
                }
            }else if (instruction.op == "BNE"){
                if(reservationStation.currentBne < reservationStation.numBne){
                    
                }
            }else if (instruction.op == "CALL"){

            }else if (instruction.op == "RET"){
            
            }else if (instruction.op == "ADD"){
            
            }else if (instruction.op == "ADDI"){
            
            }else if (instruction.op == "NAND"){

            }else if (instruction.op == "DIV"){
            
            }else{
                cout << "Invalid instruction" << endl;
            }

        };
        void Execute();
        void WriteBack();
};

int main(){

}