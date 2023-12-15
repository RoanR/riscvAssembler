#include <string>
#include <iostream>

#include "instructions.hpp"

using namespace std;

int main (void) 
{
	string test1 = "  ADDI x2, x1,   2000 ";
	instruction ins = string_to_instr(test1);
	cout << "String to Ins Complete" << endl;
	display_instr(ins); 
	string ans = instr_to_string(ins);
	cout << ans; 
	return 0;
}