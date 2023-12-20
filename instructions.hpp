#include <string>

#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

/* Instruction Opcode Defintions */
#define BRANCH		0b1100011 //Branch operation
#define LOAD		0b0000011 //Load operation
#define STORE		0b0100011 //Store operation
#define IMM_ALU		0b0010011 //Immediate ALU operation
#define REG_ALU		0b0110011 //Register ALU operation


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
	J_TYPE, 
	// Error type
	ERROR,
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
	// PC address of this instruction in I_MEM
	int pc;
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

	// Initialise with Error values so obvious if not overwritten
	instruction() : name("ERROR"), type(ERROR), pc(-1), opcode(-1), rs1(reg{}), 
		rs2(reg{}), rd(reg{}), imm(-1), funct3(-1), funct7(-1) {};
	// Constructor for generic template
	instruction(std::string n, instr_t t, int o) : name(n), type(t), pc(0), 
		opcode(o), rs1(reg{}), rs2(reg{}), rd(reg{}), imm(0), funct3(0),
		funct7(0) {};
	// Constructor for generic template with funct3
	instruction(std::string n, instr_t t, int o, int f3) : name(n), type(t), 
		pc(0), opcode(o), rs1(reg{}), rs2(reg{}), rd(reg{}), imm(0), funct3(f3),
		funct7(0) {};
	// Constructor for generic template with funct3
	instruction(std::string n, instr_t t, int o, int f3, int f7) : name(n), 
		type(t), pc(0), opcode(o), rs1(reg{}), rs2(reg{}), rd(reg{}), imm(0), 
		funct3(f3), funct7(f7) {};
	/*
	// Constructor for R_TYPE instructions
	instruction(std::string n, int f7, reg x1, reg x2, reg rd, int f3, int op) 
		: name(n), type(R_TYPE), opcode(op), rs1(x1), rs2(x2), rd(rd),
		imm(0), funct3(f3), funct7(f7) {};
	// Constructor for I_TYPE instructions
	instruction(std::string n, int i, reg x1, reg rd, int f3, int op) 
		: name(n), type(I_TYPE), opcode(op), rs1(x1), rd(rd), imm(i), 
		funct3(f3) {};
	// Constructor for S_TYPE/B_TYPE instructions
	instruction(std::string n, instr_t t, int i, reg x1, reg x2, int f3, int op) 
		: name(n), type(t), opcode(op), rs1(x1), rs2(x2), imm(i), 
		funct3(f3) {};
	// Constructor for U_TYPE/J_TYPE instructions
	instruction(std::string n, instr_t t, reg rd, int i, int op) 
		: name(n), type(t), opcode(op), imm(i) {};	
	*/
};

std::string instr_to_string(instruction ins); 

instruction string_to_instr(std::string line);

void display_instr(instruction ins); 

#endif