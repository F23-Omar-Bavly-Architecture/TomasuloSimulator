# Project details

## Implementation language: C++

## Instruction set architecture (ISA):
The simulator assumes a simplified RISC ISA inspired by the ISA of the Ridiculously Simple Computer (RiSC-16) proposed by Bruce Jacob. 
As implied by its name, the word size of this computer is 16-bit. The processor has 8 general-purpose registers R0 to R7 (16-bit each). 
The register R0 always contains the value 0 and cannot be changed. Memory is word addressable and uses a 16-bit address (as such the memory capacity is 128 KB). 
The instruction format itself is not very important to the simulation and therefore is not described here. However, the simulator should support the following set of instructions (16-bit each):

### Load/Store:
- Load: Loads a word from memory into rA. Memory address is formed by adding offset to contents of rB, where offset is a 6-bit signed constant (ranging from -32 to 31) e.g. LOAD rA, offset(rB)
- Store: Stores value from rA into memory. Memory address is computed as in the case of the load word instruction e.g. STORE rA, offset(rB)
### Conditional branch: 
- Branch if not equal: branches to the address PC+1+offset if rA!=rB. Note that the PC is incremented by one when the branch is not taken. e.g. BNE rA, rB, offset

### Call/Return
- Call: Stores the value of PC+1 in R1 and branches (unconditionally) to the address specified by the label. The label is also encoded as a 6-bit signed constant. e.g. CALL label
- Return: branches (unconditionally) to the address stored in R1 e.g. RET

### Arithmetic and Logic
- Add: Adds the value of rB and rC storing the result in rA e.g. ADD rA, rB, rC
- Add immediate: Adds the value of rB to imm storing the result in rA where imm is a 6-bit signed constant. e.g. ADDI rA, rB, imm
- Nand: Performs a bitwise Nand operation between the values of rB and rC storing the result in regA e.g. NAND rA, rB, rC
- Divide: Divides the value of rB by rC storing the result in rA. e.g. DIV rA, rB, rC

## Simulator inputs:
- The assembly program to be simulated. The user should also specify its starting address. Instructions can be entered one by one or even selected from a list.
- Data required by the program to be initially loaded in the memory. For each data item both its value and its memory address should be specified. All values are assumed to be 16-bit values.

## Simulation:
The simulator should simulate the program’s execution on a single-issue processor (multiple-issue simulation is a bonus feature). 
It should follow the non-speculative version of Tomasulo’s algorithm. 
The simulator should only take the 3 backend stages into account: issue (1 cycle), execute (N cycles depending on the functional unit involved as detailed in the table below), and write (1 cycle).

You can assume that all instructions have already been fetched and decoded and are waiting to be issued. The following is also assumed about the functional units available:

|  | Number of reservation stations available | Number of cycles needed |
| --- | --- | --- |
| LOAD unit | 2 | 1 (compute address) + 2 (read from memory) |
| STORE unit | 2 | 1 (compute address) + 2 (writing to memory) |
| BNE unit | 1 | 1 (compute target and compare operands) |
| CALL/RET unit | 1 | 1 (compute target and return address) |
| ADD/ADDI unit | 3 | 2 |
| NAND unit | 1 | 1 |
| DIV unit | 1 | 10 |

For conditional branches, the processor uses an always not taken predictor. Recall that in the non-speculative version of Tomasulo, instructions can only be issued (but not executed) based on a prediction.

While simulating the execution, the program should record the number of instructions completed, the number of branches encountered, the number of cycles spanned, and the number of branch mispredictions.

## Simulator output: 
- A table listing the clock cycle time at which each instruction was: issued, started execution, finished
execution, and was written
- The total execution time expressed as the number of cycles spanned
- The IPC
- The branch misprediction percentage

## Simplifying assumptions:
- Fetching and decoding take 0 cycles and the instruction queue is already filled with all the instructions to be simulated.
- No floating-point instructions, registers, or functional units
- No input/output instructions are supported
- No interrupts or exceptions are to be handled
- For each program being executed, assume that the program and its data are fully loaded in the main memory
- There is a one-to-one mapping between reservation stations and functional units. i.e., Each reservation station has a functional unit dedicated to it
- No cache or virtual memory

## Chosen Bonus feature
Support a variable hardware organization. This bonus too will be counted as two features as far as grading is concerned. 
If implemented the user should specify the number of reservation stations for each class of instructions. 
Additionally, the user will specify the number of cycles needed by each functional unit type.
