// LanguageParser.cpp : Defines the entry point for the application.
//

#include "LanguageParser.h"
#include <string>
#include <vector>
#include <sstream>
#include <regex>

using namespace std;

enum TokenType {
	ACCESS,
	DECLARE,
	SCOPE,
	STARTSCOPE,
	ENDSCOPE,
	UNKNOWN
};

const string TOKENS[] = { "ACCESS", "DECLARE", "SCOPE","{", "}", "UNKNOWN" };
const char CH_END_EXPRESSION = ';';
const string ROOT_SCOPE = "Root";

class Expression {
	TokenType m_command;
	string m_variable;	
	string m_scope;
	int m_line;
	///determines whether the expression requires to be fed more lines (has sub-expressions). When complete, none more is required.
	bool m_complete = false;
	vector<Expression*> m_childExpressions;

	string parseVariable(string var) {
		char last = var.at(var.length() - 1);
		if (last == CH_END_EXPRESSION) {			
			//valid variable name, remove the last char.
			var.pop_back();
		}

		//determines scope. Nested scopes are not allowed!
		stringstream ss = stringstream(var);
		vector<string> result;
		string s;

		vector<string> scope = FindScope(var);
		m_scope = ROOT_SCOPE;
		if (scope.size() > 1)
		{
			m_scope = scope.at(scope.size() - 2);
			var = scope.at(scope.size()-1);
		}		

		return var;
	}

	vector<string> FindScope(string var)
	{
		vector<string> result;
		string delim = "::";
		std::size_t current, previous = 0;
		current = var.find(delim);
		while (current != std::string::npos) {
			result.push_back(var.substr(previous, current - previous));
			previous = current + delim.size();
			current = var.find(delim, previous);
		}
		result.push_back(var.substr(previous, current - previous));

		return result;
	}

public:

	bool error = false;
	string errorMsg = "";	
	

	Expression(int nline) {		
		m_command = UNKNOWN;
		m_line = nline;
	}

	Expression(int nline,TokenType t) {
		m_command = t;
		m_line = nline;
	}

	Expression(int nline,TokenType t, string variable, bool complete = true)
	{
		m_command = t;
		m_variable = parseVariable(variable);
		m_complete = complete;
		m_line = nline;
	}

	int GetNumLine() { return m_line; }
	TokenType GetCommand() { return m_command; }

	string GetVariable() { return m_variable; }

	string GetScope() { return m_scope; }

	bool IsCommand(TokenType t) {
		return m_command == t;
	}

	bool IsComplete() {
		return m_complete;
	}

	void Feed(int ln, string line)
	{
		Expression *e = Parse(ln, line);		
		if (e->IsCommand(ENDSCOPE))
		{
			m_complete = true;
		}
		m_childExpressions.push_back(e);
	}

	string printError() {
		if (error)
			return " -> "+errorMsg;
		return " -> Syntax OK";
	}

	string PrintLine() {
		stringstream ss;
		ss << GetNumLine() << ": " << TOKENS[m_command] <<" "+ m_scope << " " << m_variable << printError() << endl;
		for (Expression* exp : m_childExpressions)
		{			
			ss << exp->PrintLine();
		}
		return ss.str();
	}

	vector<Expression*> Children() {
		return m_childExpressions;
	}


	static Expression* Parse(int ln, string line) {
		istringstream iss(line);
		vector<string> words(istream_iterator<string>{iss},
			istream_iterator<string>());

		//debug
		//cout << "DEBUG: words: ";
		//for (string word : words) {
		//   cout << word + ", ";
		//}
		//cout << "\n";

		// can only be endscope
		if (words.size() == 1) {
			if (words.at(0) == TOKENS[ENDSCOPE])
				return new Expression(ln,ENDSCOPE);
		}
		//check for ACCESS DECLARE
		else if (words.size() == 2) {
			if (words.at(0) == TOKENS[ACCESS])
				return new Expression(ln,ACCESS, words.at(1));
			else if (words.at(0) == TOKENS[DECLARE])
				return new Expression(ln,DECLARE, words.at(1));
		}
		else if (words.size() == 3)
		{
			if (words.at(0) == TOKENS[SCOPE] && words.at(2) == TOKENS[STARTSCOPE])
				return new Expression(ln,SCOPE, words.at(1), false);
			}
		//handle error
		return Expression::Error(ln,"Invalid word count: " + words.size());		
	}

	static Expression* Error(int ln, string msg)
	{
		Expression *e = new Expression(ln);
		e->error = true;
		e->errorMsg = msg;
		return e;
	}

	

};

class Parser {

public:
	Parser() {

	}

	Parser(string input) {
		m_input = input;
		Parse();
		//string matchNormal = ".*\\w*;";
		//string matchScope = ".*\\w*\{";		//regex is not working?!
		//m_parsedTokens = FindTokens(input,matchScope);

	}	

	string getOutput() {
		string output = "\nOutput:\n";

		int nline = 0;
		for (int i = 0; i < m_expressions.size(); i++)
		{
			output += m_expressions.at(i)->PrintLine();
		}
		output += "\n";
		return output;
	}

	vector<Expression*> getExpressions() {
		return m_expressions;
	}

private:

	string m_input;
	vector<Expression*> m_expressions;

	void Parse() {

		vector<string> lines;
		stringstream ss = stringstream(m_input);
		string line;
		int i = 0;

		while (getline(ss, line)) {
			//cout << "DEBUG: parse line: "+to_string(++i)+": "+ line << endl;
			lines.push_back(line);
		}
		
		for (int i = 0; i < lines.size(); i++) {
			string line = lines.at(i);
			if (line.size() == 0)
				continue;
			Expression *expr = Expression::Parse(i+1,line);

			if (expr->error)
			{
				cout << "SYNTAX ERROR at line: " + to_string(i + 1);
				break;
			}

			while (!expr->IsComplete() && !expr->error)
			{
				int index = ++i;
				expr->Feed(index+1, lines.at(index));
			}
			m_expressions.push_back(expr);
		}

		

	}
	

	vector<string> FindTokens(const string& input, string expr) {

		vector<string> result;

		try {
			std::smatch m;
			std::regex r(expr);
			std::regex_search(input, m, r);


			/*for (auto v : m)
				result.push_back(v);*/

		}
		catch (std::regex_error e) {
			cout << e.what();
		}

		return result;
	}	

};


class Scope {
	vector<string> m_variables;
	string m_name;

	public:
		Scope(string name) {
			m_name = name;
		}
		void AddVariable(string var)
		{
			m_variables.push_back(var);
		}

		bool Is(string scope)
		{
			return scope == m_name;
		}

		bool IsDeclared(string variable)
		{
			for (string var : m_variables)
			{
				if (var == variable)
					return true;
			}
			return false;
		}
};

class Compiler {
		
	vector<Scope*> m_scopes;
	vector<string> m_errorStack;
	Scope m_rootScope = Scope(ROOT_SCOPE);
	Scope* m_currentScope;
	stringstream ss = stringstream();
	   
	bool Evaluate(Expression* exp)
	{		
		ss.clear();		
		// handle pre-compilation errors
		if (exp->error) {
			m_errorStack.push_back(exp->errorMsg);
			return false;
		}
		string scop;
		bool fail = false;
		//handle commands
		switch (exp->GetCommand())
		{
			case ACCESS:				
				return HandleAccessCommand(exp);
			case DECLARE:
				return HandleDeclareCommand(exp);
			case SCOPE:
				return HandleScopeCommand(exp);
			case ENDSCOPE:
				return HandleEndScopeCommand(exp);


		}
		ss << "Unhandled COMMAND: "<< exp->GetCommand() << " at line: " << exp->GetNumLine() << endl;
		m_errorStack.push_back(ss.str());
		return false;
	}

	bool HandleEndScopeCommand(Expression* exp)
	{
		m_currentScope = &m_rootScope;
		return true;
	}

	bool HandleAccessCommand(Expression* exp)
	{
		string scop = exp->GetScope();
		if (scop != ROOT_SCOPE && !m_currentScope->Is(scop))
		{
			for (Scope* s : m_scopes)
			{
				if (s->Is(scop) && s->IsDeclared(exp->GetVariable()))
					return true;
			}
			ss << "Illegal scope: " << scop << " at " << exp->GetNumLine() << endl;
			m_errorStack.push_back(ss.str());
			return false;
		}
		else if (m_currentScope->IsDeclared(exp->GetVariable()))
		{
			return true;
		}

		ss << "Variable " << exp->GetVariable() << " not declared at line: " << exp->GetNumLine() << endl;
		m_errorStack.push_back(ss.str());
		return false;
	}

	bool HandleDeclareCommand(Expression* exp) {
		m_currentScope->AddVariable(exp->GetVariable());
		return true;
	}

	bool HandleScopeCommand(Expression* exp)
	{
		Scope* newScope = new Scope(exp->GetVariable());
		m_currentScope = newScope;
		m_scopes.push_back(m_currentScope);
		for (Expression* e : exp->Children()) {
			if (!Evaluate(e))
				return false;
		}
		return true;
	}

	Scope* GetScope(Expression* exp)
	{
		return new Scope(exp->GetScope());
	}

public:

	Compiler() {
		m_currentScope = &m_rootScope;
	}

	bool Compile(vector<Expression*> expressions)
	{
		for (Expression* exp : expressions)
		{
			if (!Evaluate(exp)) {
				m_errorStack.push_back("Expression Failed: " + exp->PrintLine());
				return false;
			}
		}
		return true;
	}

	void printErrorStack() {
		for (string e : m_errorStack)
		{
			cout << e << endl;
		}
	}


};

int main()
{
	cout << "Hello Language Parser." << endl;	

	// simple use case, will succeed
	string input1 = "DECLARE hello;\nACCESS hello;";	
	//complex use case, will fail, because trying to access undeclared variable
	string input2 = "SCOPE moi {\n                 DECLARE yo;\nACCESS yo;\n}\n\nDECLARE hello;\nACCESS hello;\nACCESS moi::yo;\nACCESS none;";	
	//will fail because no variables have been declared
	string input3 = "ACCESS lol::peelo;\nACCESS ::lolo::peelo;";
	//complex, will succeed
	string input4 = "DECLARE foo;\n DECLARE bar;\nSCOPE peon {\n    DECLARE foobar;\nACCESS foobar;\n}\nACCESS foo;\nACCESS bar;";
		
	Parser parser(input4);
	cout << parser.getOutput();	

	Compiler compiler;
	bool compile = compiler.Compile(parser.getExpressions());

	if (compile)
	{
		cout << "\n\nCOMPILE SUPER!\n\n";
	}
	else {
		cout << "\n\nERROR NO COMPILE WIN!\n\n";
		compiler.printErrorStack();
	}

	return 0;
}
