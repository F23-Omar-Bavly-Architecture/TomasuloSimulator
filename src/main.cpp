#include <bits/stdc++.h>
#include "RegisterStatus.h"
#include "ReservationStation.h"
#include "RegisterFile.h"

using namespace std;

// RegisterSt : map<string, string> key is the name of the register 
// and value indicates whcih reservation station will provide the value for the register

// ReservationStation : <string, ReservationStationEntry> key is the name of the reservation station

// ReservationStationEntry : class with the following fields
// Op : string
// Vj : string
// Vk : string
// Qj : string
// Qk : string
// A : string
// Busy : bool
// Result : 16 bit int

// Instruction : class with the following fields
// Op : string
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
        //queue<Instruction> instructionQueue;
        //vector<Instruction> inflightInstructions;
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
            case(instruction.Op){
                "LOAD":

                    break;
                "STORE":
                    
                    break;
                "BNE":
                        
                    break;
                "CALL":
                        
                    break;
                "RET":
                            
                    break;
                "ADD":
                                    
                    break;
                "ADDI":
                                        
                    break;
                "NAND":
                                                
                    break;
                "DIV":
                                                            
                    break;
                default:
                    cout << "Invalid instruction" << endl;
                    break;
            }

        };
        void Execute();
        void WriteBack();
};

int main(){

}