#include <string>
#include <iostream>
#include <fstream>

#include "assembler.hpp"
#include "instructions.hpp"

using namespace std;

int binary_to_file(string target, string dest) 
{
	if (preprocessing(target, "processed.txt") != 0) {
		return 1;
	}

	ifstream read_file("processed.txt");
	ofstream write_file(dest);
	string curr_line;

	while (getline(read_file, curr_line)) {
		instruction ins = string_to_instr(curr_line);
		if (ins.type != ERROR) {
			display_instr(ins);
			string bin_ins = instr_to_string(ins);
			write_file << bin_ins << "\n";
		}
	}

	read_file.close();
	write_file.close();
	return 0; 

}

int main (void) 
{
	binary_to_file("test.S", "res.txt");
	return 0;
}