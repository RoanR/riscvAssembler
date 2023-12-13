#include <string>
#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

/* Instruction Opcode Defintions */
#define BRANCH		0b1100011 //Branch operation
#define LOAD		0b0000011 //Load operation
#define STORE		0b0100011 //Store operation
#define IMM_ALU		0b0010011 //Immediate ALU operation
#define REG_ALU		0b0110011 //Register ALU operation

/* Blank definitions */
#define B_5_0		0b00000 // 5bit binary set to 0  
#define B_3_0		0b000	// 3bit binary set to 0
#define B_7_0		0b0000000	// 7bit binary set to 0
#define B_7_01		0b0100000	// 7bit binary set 2nd upper bit
#define B_12_0		0b000000000000	// 12bit binary set to 0
#define B_20_0		0b00000000000000000000 	// 20bit binary set to zero

enum instr_t 
{
	// | funct7 | rs2 | rs1 | funct3 | rd | opcode |
	R_TYPE,
	// | imm[11:0] | rs1 | funct3 | rd | opcode |
	I_TYPE,
	// | imm[11:5] | rs2 | rs1 | funct3 | imm[4:0] | opcode |
	S_TYPE,
	// | imm[12|10:5] | rs2 | rs1 | funct3 | imm[4:1|11] | opcode |
	B_TYPE,
	// | imm[31:12] | rd | opcode |
	U_TYPE,
	// | imm[20|10:1|11|19:12] | rd | opcode |
	J_TYPE
};

struct reg  
{
	// Address of register in register bank (max: 31)
	int adr;
	// Value of register at time of reading (fetch)
	int val;
};

struct instruction
{
	// Name of Instruction in riscv asm
	std::string name;
	// The Instruction type, R, I, S, B, U, J
	instr_t type;
	// 7-bit Instruction Opcode
	int opcode;
	// First source register containing address and value
	reg rs1;
	// Second source register containing address and value
	reg rs2;
	// Destination register containing address and value to write
	reg rd;
	// Int containing the Immediate value
	int imm;
	// Funct 3, usually contains the ALU functionality
	int funct3;
	// Funct 7, used as an extra method to differentiate between instructions
	int funct7;
};

#endif