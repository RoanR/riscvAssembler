#include <string>
#include <vector>
#include <iostream>
#include <exception>

#include <string.h>
#include <stdio.h>

#include "assembler.hpp"
#include "instructions.hpp"

using namespace std; 

static reg extract_adr(string word, string prev) 
{
	string reg_s;
	for (auto s : {word, prev}) {
		reg_s = "";
		for (size_t i = 1; i < word.length(); i++) {
			if (isdigit(word[i])) {
				if (word[i-1] == 'X') {
					reg_s += word[i];
				} else if ((i > 1) && word[i-2] == 'X') {
					reg_s += word[i];
				}
			}
		}
		if (reg_s != ""){
			break;
		}
	}
	
	// Check if register detected 
	int reg_a;
	if (reg_s == "") {
		throw bad_line("No Register Address\n");
	} else {
		reg_a = stoi(reg_s);
	}


	reg ret = reg{};
	if (reg_a > 31 || reg < 0) {
		return ret;
	} else {
		ret.adr = adr;
		return ret; 
	}
}

static int extract_imm(string word) 
{
	// Extracting the Imm
	string imm_s = "";
	for (size_t i = 0; i < word.length(); i++) {
		// Is it a Digit ?
		if (isdigit(word[i])) {

			// Check its not in X1...X9
			if ((i > 0) && word[i-1] == 'X') {
				continue;
			}

			// Check its not in X10 ... X32
			if ((i > 1) && word[i-2] == 'X') {
				continue;
			}

			imm_s += word[i];
		}
	}

	bool hex = false;

	// Checking if its a hexadecimal Value
	for (size_t i = 1; i < word.length(); i++) {
		if (word[i] == 'x' && word[i-1] == '0') {
			hex = true;
		}
	}
	if (imm_s == "") {
		return -1;
	} else if (hex) {
		return stoi(imm_s, nullptr, 16); 
	} else {
		return stoi(imm_s);
	}
}

/* 
 * Translates int into string containing its binary representation.
 * @param start/end determine section to translate, 
 *  from end (exclusive) then decrement to start (inclusive).
 * @param num the int to translate from
 * @return string containing binary represenation of num within specified bounds
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

/* 
 * Gets a word from the string
 * @param line the string to extract the word from
 * @param pos which word to get from string, zero indexed
 * @param delimiter the char used to seperate words, ie. ' '
 * @return string containing the word at index specified, ie get_word("hello me",1,' ')
 *  would return "me".  
 */
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
	int imm = extract_imm(get_word(line, 3, ' '));

	ins.rd = rd;
	ins.rs1 = rs1;
	ins.imm = imm;

	return ins; 
}

static instruction string_to_sb(instruction ins, string line) 
{
	string word, prev;
	prev = "";

	word = get_word(line, 1, ' ');
	if (word == "") {
		ins.rs1 = adr_to_reg(extract_adr(prev));
	} else {
		ins.rs1 = adr_to_reg(extract_adr(word));
	}

	if

	word = get_word(line, 2, ' ');
	if (word == "") {
		throw bad_line("Not Found Rs2\n");
	} else {
		ins.rs1 = adr_to_reg(extract_adr(word));
		prev = word;
	}

	word = get_word(line, 3, ' ');
	if (word == "") {
		ins.imm = extract_imm(prev);
	} else {
		ins.imm = extract_imm(word);
	}

	if (ins.imm == -1) {
		throw bad_line("Not Found Imm\n");
	}

	return ins; 
}

static instruction string_to_uj(instruction ins, string line) 
{
	cout << "Started Reg";
	reg rd = adr_to_reg(string_to_adr(get_word(line, 1, ' ')));
	cout << "... Ended Reg" << endl;
	int imm = extract_imm(get_word(line, 2, ' '));
	
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

instruction string_to_instr(string line) 
{
	instruction ret; 

	vector<instruction> protos = get_instruction_prototypes();

	string cmd = get_word(line, 0, ' ');

	for (size_t i = 0; i < protos.size(); i++) {
		if (cmd == protos[i].name) {
			ret = protos[i];
			break; 
		}
	}
	try {
		switch (ret.type) {
			case R_TYPE: return (string_to_r(ret, line));  
			case I_TYPE: return (string_to_i(ret, line));
			case S_TYPE:
			case B_TYPE: return (string_to_sb(ret, line));
			case U_TYPE:
			case J_TYPE: return (string_to_uj(ret, line));
			default:     return ret;
		}
	} catch (invalid_argument& e) {
		cerr << "Caught invalid_argument: " << e.what() << endl;
		cerr << "String to instruction Failed " << endl;
		cerr << "Line: " << line << endl;
		exit(EXIT_FAILURE);
	} catch (bad_line& e) {
		cerr << "Caught bad_line: " << e.what();
		cerr << "String to instruction Failed " << endl;
		cerr << "Line: " << line << endl;
		exit(EXIT_FAILURE);
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