#include <fstream>
#include <stdexcept>
#include <iostream>
#include <algorithm>
#include <regex>
#include <unordered_map>
#include <iterator>

using namespace std;

/*
 * A Custom Exception Class to be thrown when a line isn't properly formatted
 * @param msg Constructor takes string which can be accessed through what()
 * @public what() returns msg inputted when constructor was called
 */
class bad_line: public exception 
{
	private:
	string msg;

	public:
	// Constructor 
	bad_line(string msg) : msg(msg) {};

	// To Print the error
	string what()
	{
		return msg;
	}
};

/* 
 * Checks if a key exists in an unordered_map<string, int> 
 * @param k the key to search for
 * @param m the map to search in
 * @return boolean, true if present, otherwise false
 */
static bool check_key(string k, unordered_map<string, int> m) 
{
	return (m.find("k") != m.end());
}

/*
 * Checks a string for .global key word and then tries to make global mapping if present.
 * @param line string to check for global
 * @param global_map the map to add the potential global to
 * @param token_map needed to check no duplicate macros are added
 * @return If no Global return line, If global consume the line, return "". 
 * @exception bad_line: if there is the global keyword but the declaration is malformed, 
 * throws an exception describing this.
 */
static string get_global(string line, int ln, unordered_map<string, int>& global_map, 
	unordered_map<string, int> token_map) 
{
	// Since leading whitespaced removed global must be first word
	string global;
	if (line.length() > 7) {
		global = line.substr(0, 7);
	} else {
		return line;
	} 

	// No global defines on line
	if (global != ".GLOBAL") {
		return line; 
	}

	// Assuming global in form ".GLOBAL K(*) N(*)"
	// Therefore minimum length of 9
	if (line.length() > 11) {
		line = line.substr(8); 
	} else {
		throw bad_line("Malformed Global");
		return "";
	}

	// Split into symbol/value pair
	string sym = "";
	string val = "";
	bool sym_val = false;
	for (int i = 0; i < line.length(); i++) {
		if (line[i] == ' ') {
			if (sym_val) {
				throw bad_line("Malformed Global\n");
				return "";
			}
			sym_val = true;
		} else if (!sym_val) {
			sym += line[i];
		} else {
			val += line[i];
		}
	}
	
	if (check_key(sym, token_map) || check_key(sym, global_map)) {
		throw bad_line("Redefined Macro\n");
		return "";
	}

	try {
		global_map[sym] = stoi(val);
	} catch (invalid_argument& e) {
		cerr << "Caught Exception: " << e.what() << endl;
		cerr << "First Pass Failed in " << __func__ << endl;
	}
	return "";
}

/* 
 * Checks a string for example: and then tries to add to map if present, 
 * 	these are used for things like loops and branches.
 * @param line string to check for global
 * @param ln line number to map the token to
 * @param token_map the map to add the potential token to
 * @param global_map needed to check no duplicate macros are added
 * @return If no token return line, If token consume the line, return "".
 * @exception bad_line: if there is the ":" symbol but the declaration is malformed, 
 * throws an exception describing this.
 */
static string get_token(string line, int ln, unordered_map<string, int>& token_map, 
	unordered_map<string, int> global_map) 
{
	int i;
	for (i = 0; i < line.length(); i++) {
		// If correct there should be no spaces in token
		if (line[i] == ' ') {
			return line;
		} else if (line[i] == ':') {
			break;
		}
	}

	// No occurance of ':'
	if (i == line.length()) {
		return line;
	}

	// // Check doesn't contain an int
	// if (find_if(line.begin(), line.end(), ::isdigit) != line.end()) {
	// 	cout << line <<endl;
	// 	throw bad_line("Token Contains Digits\n");
	// 	return ""; 
	// }

	line = line.substr(0, i);

	if (check_key(line, token_map) || check_key(line, global_map)) {
		throw bad_line("Redefined Macro\n");
		return "";
	}

	token_map[line] = ln;

	return "";

}

/*
 * Converts each char in string into uppercase equivalent
 * @param line the string to convert 
 * @return string the converted string 
 */
static string into_caps(string line) 
{
	for (size_t i = 0; i < line.length(); i++) {
		line[i] = toupper(line[i]);
	}
	return line;
}

/*
 * Applies various regex functions to clean initial string.
 * @details Will remove comments(#), leading and trailing whitespace. Will add
 * space around '[' and ']', turn all spaces into single spaces. Will also replace
 * commas with single spaces. 
 * @param line the string to be cleaned
 * @returns string the cleaned string
 */
static string clean_line(string line) 
{
	// Remove Comments, add Spaces around '[' and ']', 
	// Remove leading and trailing whitespace and single space everything
	// @TODO Replace regex implementation as has slow run/compile time 
	try {
		// Remove Comments
		regex comments("#.*");
		line = regex_replace(line, comments, "");
		// Replace Commas with single spaces
		regex commas(",");
		line = regex_replace(line, commas, " "); 
		// Add spaces around '['
		regex open_bracket("\\[");
		line = regex_replace(line, open_bracket, " [ ");
		// Add spaces around ']'
		regex close_bracket("\\]");
		line = regex_replace(line, close_bracket, " ] ");
		// Remove leading and trailing whitespace 
		regex trail("(^( |\t)*)|(( |\t)*$)");
		line = regex_replace(line, trail, "");
		// Single space 
		regex spaces("([  ]+|[\t]+)+");
		line = regex_replace(line, spaces, " "); 
	} catch (const regex_error& e) {
		cerr << "Caught Regex Exception: " << e.what();
		cerr << "First Pass Failed in " << __func__ << endl;
		exit(EXIT_FAILURE);
	}

	line = into_caps(line);
	return line;
}

/* 
 * Applies the first pass of preprocessing stage
 * @details Containing cleaning of each line, removing empty lines, 
 * 	detecting keywords and globals
 * @param src_name the name of the source file (must exist)
 * @param dest_name the name of the destination file (will be overwritten/created)
 * @param globals map from globals by name to thier defined value
 * @param tokens map from other tokens such as branch targets, from name to value
 * @return zero on success, otherwise return non-zero. 
 */
static int first_pass(string src_name, string dest_name, 
	unordered_map<string, int> globals, unordered_map<string, int> tokens) 
{
	ifstream src(src_name);
	if (!src.good()) {
		cerr << "Could not find " << src_name << "... exiting" << endl;
		return 1;
	}

	ofstream dest(dest_name);
	string curr_line;
	int ln = 0; 

	// Check for macros (tokens | globals)
	while (getline(src, curr_line)) {
		try {
			curr_line = clean_line(curr_line);
			curr_line = get_global(curr_line, ln, globals, tokens);
			curr_line = get_token(curr_line, ln, tokens, globals);
		} catch (bad_line& e) {
			cerr << "Caught Exception: " << e.what();
			cerr << "First Pass Failed, line: " << ln <<  endl;
			return 1; 
		} catch (...) {
			cerr << "Caught Unknown Exception Exception" << endl;
			cerr << "First Pass Failed, line: " << ln << endl;
			return 2;
		}
		if (curr_line != "") {
			dest << curr_line << "\n";
			ln++;
		}
	}

	src.close();
	dest.close();
	return 0;
}

/*
 * Calls all the preprocessing functions, cleaning input and doing macro expansion
 * @param src the name of the source file, this should exist
 * @param dest the name of the destination file, this should be different from source
 * and may be created/overwritten.
 * @returns int zero on success otherwise non-zero.  
 */
int preprocessing(string src, string dest) 
{
	if (src == dest) {
		cerr << src << " is the name of both source and destination files\n";
		return 1;
	}

	unordered_map<string, int> globals;
	unordered_map<string, int> tokens;

	// First pass is detecting all the macro definitions 
	// 	and building a map of them
	if (!first_pass(src, dest, globals, tokens)) {
		return 1;
	}
	
	return 0;
}

int main(void) 
{
	preprocessing("test.S", "res.txt"); 
}