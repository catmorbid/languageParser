// LanguageParser.cpp : Defines the entry point for the application.
//

#include "LanguageParser.h"

#include "Parser.h"
#include "Compiler.h"


using namespace std;

string ReadTextFile(string filename)
{
	std::ifstream source;
	source.open(filename);
	stringstream ss = stringstream();
	string line;
    
	while (std::getline(source, line))
	{
        ss << line << endl;
		cout << line;
	}
	return ss.str();
}

int main(int argc, char* argv[])
{	
    if (argc < 2)
        cout << "Filename argument not found" << endl;
        return 100;
    
    string input;
    
    input = ReadTextFile(argv[1]);
    
		
	Parser parser(input);
	cout << parser.getOutput();	

	/*Compiler compiler;
	bool compile = compiler.Compile(parser.getExpressions());

	if (compile)
	{
		cout << "\n\nCOMPILE SUPER!\n\n";
	}
	else {
		cout << "\n\nERROR NO COMPILE WIN!\n\n";
		compiler.printErrorStack();
	}*/

	return 0;
}
