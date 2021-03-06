#ifndef CMILAN_PARSER_H
#define CMILAN_PARSER_H

#include "scanner.h"
#include "codegen.h"
#include <iostream>
#include <sstream>
#include <string>
#include <map>

using namespace std;

/* ?????????????? ??????????.
 *
 * ??????:
 * - ???????? ???????????? ?????????,
 * - ????????? ???? ??? ??????????? ?????? ? ???????? ???????,
 * - ?????????? ?????????????? ????? ??????.
 *
 * ?????????????? ?????????? ????? ?????.
 *
 * ?????? ? ??????? ??????????? ??? ??? ????????????? ???????????? ???????????
 * ?????? ?? ????? ??????? ? ?? ?????? ?????????? ?????? ?????????? ??? ???
 * ???????? ??????????? ??????. ?????????????? ?????? ??????????? ???????
 * ???????????? ??????.
 *
 * ??? ??????????? ?????? ?????? ???????? ????????? ? ?????????? ?????? ??
 * ?????????? ?????????, ????? ? ???????? ??????? ????? ??? ????? ?????? ??????.
 * ????????? ????????? ?????????????? ????? ?????? ????? ??????, ???????? ??????
 * ????????? ? ?????????????? ("??????????") ??????? ??? ??????? ?????????
 * ?????? ??? ?????? ?????????. ???? ? ???????? ??????? ???? ??????? ???? ??
 * ???? ??????, ??? ??? ??????????? ?????? ?? ??????????.*/

class Parser
{
public:
	// ???????????
	//    const string& fileName - ??? ????? ? ?????????? ??? ???????
	//
	// ??????????? ??????? ?????????? ???????????? ??????????? ? ??????????.

	Parser(const string& fileName, istream& input)
		: output_(cout), error_(false), recovered_(true), lastVar_(0)
	{
		scanner_ = new Scanner(fileName, input);
		codegen_ = new CodeGen(output_);
		next();
	}

	~Parser()
	{
		delete codegen_;
		delete scanner_;
	}

	void parse();	//???????? ?????????????? ?????? 

private:
	typedef map<string, int> VarTable;
	//???????? ??????.
	void program(); //?????? ?????????. BEGIN statementList END
	void statementList(); // ?????? ?????? ??????????.
	void statement(); //?????? ?????????.
	void expression(); //?????? ??????????????? ?????????.
	void term(); //?????? ??????????.
	void factor(); //?????? ?????????.
	void relation(); //?????? ???????.

	// ????????? ??????? ??????? ? ????????. ??????? ??????? ? ?????? ?????? ?? ??????????.
	bool see(Token t)
	{
		return scanner_->token() == t;
	}

	// ???????? ?????????? ??????? ??????? ? ????????. ???? ??????? ? ??????? ?????????,
	// ??????? ????????? ?? ??????.

	bool match(Token t)
	{
		if (scanner_->token() == t) {
			scanner_->nextToken();
			return true;
		}
		else {
			return false;
		}
	}

	// ??????? ? ????????? ???????.

	void next()
	{
		scanner_->nextToken();
	}

	// ?????????? ??????.
	void reportError(const string& message)
	{
		cerr << "Line " << scanner_->getLineNumber() << ": " << message << endl;
		error_ = true;
	}

	void mustBe(Token t); //?????????, ????????? ?? ?????? ??????? ? ????????. ???? ??, ?? ??????? ????????? ?? ??????.
	//????? ??????? ????????? ?? ?????? ? ??????? ??????????????
	void recover(Token t); //?????????????? ????? ??????: ???? ?? ???? ?? ??? ???, 
	//???? ?? ???????? ??? ??????? ??? ??????? ????? ?????.
	int findOrAddVariable(const string&); //??????? ????????? ?? variables_. 
	//???? ??????? ?????? ?????????? - ?????????? ?? ?????, ????? ????????? ?? ? ??????, ??????????? lastVar ? ?????????? ???.

	Scanner* scanner_; //??????????? ?????????? ??? ????????????
	CodeGen* codegen_; //????????? ?? ??????????? ??????
	ostream& output_; //???????? ????? (? ?????? ?????? ?????????? cout)
	bool error_; //???? ??????. ???????????? ????? ??????????, ??????? ?? ?????? ?????? ????? ??????? ??? ???
	bool recovered_; //?? ????????????
	VarTable variables_; //?????? ??????????, ????????? ? ?????????
	int lastVar_; //????? ????????? ?????????? ??????????
};

#endif
