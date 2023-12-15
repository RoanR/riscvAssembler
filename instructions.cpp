#include <string>
#include <vector>
#include <iostream>

#include <string.h>
#include <stdio.h>

#include "instructions.hpp"

using namespace std; 

/* 
 * Will translate num into string containing its binary representation.
 * The start/end variables determine how many bits to return, 
 * 	will start at end (exclusive) then decrement to start (inclusive)
 * For example binaryString(0b000101001110, 4, 0) -> "1110" 
 */
static string binary_to_string(int num, int start, int end) 
{
	string ret = ""; 
	for (int i = start-1; i >= end; i--) { 
		int k = num >> i; 
		if (k & 1) {
			ret += "1"; 
		} else {
			ret += "0";
		}
	}
	return ret;
}

static string get_word(string line, int pos, char delimiter) 
{

	string temp = "";
	for (size_t i = 0; i < line.length(); i++) {
		if (line[i] == delimiter) {
			if (pos == 0) {
				return temp;
			} else {
				temp = "";
				--pos; 
			}
		} else {
			temp += line[i];
		}
	}

	return temp;
}

static string r_to_string(instruction ins) 
{
	string ret = "";
	ret += binary_to_string(ins.funct7, 7, 0);
	ret += binary_to_string(ins.rs2.adr, 5, 0);
	ret += binary_to_string(ins.rs1.adr, 5, 0);
	ret += binary_to_string(ins.funct3, 3, 0);
	ret += binary_to_string(ins.rd.adr, 5, 0);
	ret += binary_to_string(ins.opcode, 7, 0); 
	return ret;
}

static string i_to_string(instruction ins) 
{
	string ret = "";
	ret += binary_to_string(ins.imm, 12, 0);
	ret += binary_to_string(ins.rs1.adr, 5, 0);
	ret += binary_to_string(ins.funct3, 3, 0);
	ret += binary_to_string(ins.rd.adr, 5, 0);
	ret += binary_to_string(ins.opcode, 7, 0); 
	return ret;
}

static string s_to_string(instruction ins) 
{
	string ret = "";
	ret += binary_to_string(ins.imm, 12, 5);
	ret += binary_to_string(ins.rs2.adr, 5, 0);
	ret += binary_to_string(ins.rs1.adr, 5, 0);
	ret += binary_to_string(ins.funct3, 3, 0);
	ret += binary_to_string(ins.imm, 5, 0);
	ret += binary_to_string(ins.opcode, 7, 0); 
	return ret;
}

static string b_to_string(instruction ins) 
{
	string ret = "";
	ret += binary_to_string(ins.imm, 13, 12);
	ret += binary_to_string(ins.imm, 11, 5);
	ret += binary_to_string(ins.rs2.adr, 5, 0);
	ret += binary_to_string(ins.rs1.adr, 5, 0);
	ret += binary_to_string(ins.funct3, 3, 0);
	ret += binary_to_string(ins.imm, 5, 1);
	ret += binary_to_string(ins.imm, 12, 11);
	ret += binary_to_string(ins.opcode, 7, 0);
	return ret;
}

static string u_to_string(instruction ins) 
{
	string ret = "";
	ret += binary_to_string(ins.imm, 32, 12);
	ret += binary_to_string(ins.rd.adr, 5, 0);
	ret += binary_to_string(ins.opcode, 7, 0);
	return ret;
}

static string j_to_string(instruction ins) 
{
	string ret = "";
	ret += binary_to_string(ins.imm, 21, 20);
	ret += binary_to_string(ins.imm, 11, 1);
	ret += binary_to_string(ins.imm, 12, 11);
	ret += binary_to_string(ins.imm, 20, 12);
	ret += binary_to_string(ins.rd.adr, 5, 0);
	ret += binary_to_string(ins.opcode, 7, 0);
	return ret;
}

string instr_to_string(instruction ins) 
{
	switch (ins.type) {
		case R_TYPE: return r_to_string(ins);
		case I_TYPE: return i_to_string(ins);
		case S_TYPE: return s_to_string(ins);
		case B_TYPE: return b_to_string(ins);
		case U_TYPE: return u_to_string(ins);
		case J_TYPE: return j_to_string(ins);
		default: return "ERROR: UNRECOGNISED STRING";
	}
}

static reg adr_to_reg(int adr) 
{
	reg ret = reg{};
	if (adr > 31 || adr < 0) {
		return ret;
	} else {
		ret.adr = adr;
		return ret; 
	}
}

static int string_to_adr(string s) 
{
	// Assuming in the Form "x1"
	// Stripping "x" predicate off
	s = s.substr(1, s.length()-1);
	return stoi(s, nullptr); 
}

static instruction string_to_r(instruction ins, string line)
{
	reg rd  = adr_to_reg(string_to_adr(get_word(line, 1, ' ')));
	reg rs1 = adr_to_reg(string_to_adr(get_word(line, 2, ' ')));
	reg rs2 = adr_to_reg(string_to_adr(get_word(line, 3, ' '))); 

	ins.rd = rd;
	ins.rs1 = rs1;
	ins.rs2 = rs2; 

	return ins;
}

static instruction string_to_i(instruction ins, string line) 
{
	reg rd  = adr_to_reg(string_to_adr(get_word(line, 1, ' ')));
	reg rs1 = adr_to_reg(string_to_adr(get_word(line, 2, ' ')));
	int imm = stoi(get_word(line, 3, ' ')); 

	ins.rd = rd;
	ins.rs1 = rs1;
	ins.imm = imm;

	return ins; 
}

static instruction string_to_sb(instruction ins, string line) 
{
	reg rs1 = adr_to_reg(string_to_adr(get_word(line, 1, ' ')));
	reg rs2 = adr_to_reg(string_to_adr(get_word(line, 2, ' ')));
	int imm = stoi(get_word(line, 3, ' '));

	ins.rs1 = rs1;
	ins.rs2 = rs2;
	ins.imm = imm;

	return ins; 
}

static instruction string_to_uj(instruction ins, string line) 
{
	reg rd = adr_to_reg(string_to_adr(get_word(line, 1, ' ')));
	int imm = stoi(get_word(line, 2, ' ')); 

	ins.rd = rd; 
	ins.imm = imm;

	return ins;
}

static vector<instruction> get_instruction_prototypes (void) 
{
	vector<instruction> all = {
		instruction{"LUI",   U_TYPE, 0b0110111},
		instruction{"AUIPC", U_TYPE, 0b0010111},
		instruction{"JAL",   J_TYPE, 0b1101111},
		instruction{"JALR",  I_TYPE, 0b1100111},
		// Branch Instructions
		instruction{"BEQ",   B_TYPE, 0b1100011, 0b000},
		instruction{"BNE",   B_TYPE, 0b1100011, 0b001},
		instruction{"BLT",   B_TYPE, 0b1100011, 0b100},
		instruction{"BGE",   B_TYPE, 0b1100011, 0b101},
		instruction{"BLTU",  B_TYPE, 0b1100011, 0b110},
		instruction{"BGEU",  B_TYPE, 0b1100011, 0b111},
		// Load Instructions
		instruction("LB",    I_TYPE, 0b0000011, 0b000), 
		instruction("LH",    I_TYPE, 0b0000011, 0b001),
		instruction("LW",    I_TYPE, 0b0000011, 0b010),  
		instruction("LBU",   I_TYPE, 0b0000011, 0b100), 
		instruction("LHU",   I_TYPE, 0b0000011, 0b101), 
		// Store Instructions
		instruction("SB",    S_TYPE, 0b0100011, 0b000),
		instruction("SH",    S_TYPE, 0b0100011, 0b001),
		instruction("SW",    S_TYPE, 0b0100011, 0b010),
		// Add Imm Instructions
		instruction("ADDI",  I_TYPE, 0b0010011, 0b000),
		instruction("SLTI",  I_TYPE, 0b0010011, 0b010),
		instruction("SLTIU", I_TYPE, 0b0010011, 0b011),
		instruction("XORI",  I_TYPE, 0b0010011, 0b100),
		instruction("ORI",   I_TYPE, 0b0010011, 0b110),
		instruction("ANDI",  I_TYPE, 0b0010011, 0b111),
		// Shifts with immediate
		instruction("SLLI",  I_TYPE, 0b0010011, 0b001),
		instruction("SRLI",  I_TYPE, 0b0010011, 0b101),
		instruction("SRAI",  I_TYPE, 0b0010011, 0b101),
		// ALU with regs 
		instruction("ADD",   R_TYPE, 0b0110011, 0b000, 0b0000000), 
		instruction("SUB",   R_TYPE, 0b0110011, 0b000, 0b0100000), 
		instruction("SLL",   R_TYPE, 0b0110011, 0b001, 0b0000000),
		instruction("SLT",   R_TYPE, 0b0110011, 0b010, 0b0000000),
		instruction("SLTU",  R_TYPE, 0b0110011, 0b011, 0b0000000),
		instruction("XOR",   R_TYPE, 0b0110011, 0b100, 0b0000000),
		instruction("SRL",   R_TYPE, 0b0110011, 0b101, 0b0000000),
		instruction("SRA",   R_TYPE, 0b0110011, 0b101, 0b0100000),
		instruction("OR",    R_TYPE, 0b0110011, 0b110, 0b0000000),
		instruction("AND",   R_TYPE, 0b0110011, 0b111, 0b0000000),		
	};

	return all;
}

static string clean_line(string line) 
{
	// Remove leading and trailing whitespace
	auto str_begin = line.find_first_not_of(" \t");
	if (str_begin == string::npos)
		return "";
	auto str_end = line.find_last_not_of(" \t");  
	line = line.substr(str_begin, str_end-str_begin+1);

	// Remove any tabs and replace with single spaces
	size_t start_pos = 0;
	while((start_pos = line.find('\t', start_pos)) != string::npos) {
		line.replace(start_pos, 1, " ");
    	}

	// Replace any ',' with single spaces
	while((start_pos = line.find(',', start_pos)) != string::npos) {
		line.replace(start_pos, 1, " "); 
	}


	// Remove any "  " and replace with single spaces
	start_pos = 0;
	while((start_pos = line.find("  ", start_pos)) != string::npos) {
		line.replace(start_pos, 2, " ");
	}

	return line;
}

static string remove_comments(string line) 
{
	auto comment_begin = line.find_first_of('#');
	if (comment_begin == string::npos)
		return line;
	line = line.substr(0, comment_begin);
	return line;
	
}

static string case_correct(string word)
{
	for (size_t i = 0; i < word.length(); ++i) {
		word[i] = toupper(word[i]); 
	}
	return word; 
}

instruction string_to_instr(string line) 
{
	instruction ret; 

	vector<instruction> protos = get_instruction_prototypes();

	line = clean_line(line);
	line = remove_comments(line);
	string cmd = get_word(line, 0, ' ');
	cmd = case_correct(cmd); 

	for (size_t i = 0; i < protos.size(); i++) {
		if (cmd == protos[i].name) {
			ret = protos[i];
			break; 
		}
	}

	switch (ret.type) {
		case R_TYPE: return (string_to_r(ret, line));  
		case I_TYPE: return (string_to_i(ret, line));
		case S_TYPE | B_TYPE: return (string_to_sb(ret, line));
		case U_TYPE | J_TYPE: return (string_to_uj(ret, line));
		default: return ret;
	}

}

void display_instr(instruction ins) 
{
	cout<< ins.name << "(";
	switch (ins.type) {
		case R_TYPE: cout << "R type) ";
			break;
		case I_TYPE: cout << "I type) "; 
			break;
		case S_TYPE: cout << "S type) "; 
			break;
		case B_TYPE: cout << "B type) ";
			break; 
		case U_TYPE: cout << "U type) ";
			break; 
		case J_TYPE: cout << "J type) ";
			break; 
		default: cout << "ERROR) ";
	}
	cout <<"rd("     << ins.rd.adr  << "), ";
	cout <<"rs1("    << ins.rs1.adr << "), ";
	cout <<"rs2("    << ins.rs2.adr << "), ";
	cout <<"funct3(" << ins.funct3  << "), ";
	cout <<"funct7(" << ins.funct7  << "), ";
	cout <<"imm("    << ins.imm     << "); " << endl;
}