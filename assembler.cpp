#include <string>
#include <iostream>
#include <fstream>

#include "instructions.hpp"

using namespace std;

int binary_to_file(string target, string dest) 
{
	ifstream read_file(target);
	ofstream write_file(dest);
	string curr_line;

	while (getline(read_file, curr_line)) {
		instruction ins = string_to_instr(curr_line);
		display_instr(ins);
		string bin_ins = instr_to_string(ins);
		write_file << bin_ins << "\n";
	}

	read_file.close();
	write_file.close();

}

int main (void) 
{
	binary_to_file("test.txt", "res.txt");
	return 0;
}