#ifndef CMILAN_SCANNER_H
#define CMILAN_SCANNER_H

#include <fstream>
#include <string>
#include <map>

using namespace std;

enum Token {
	T_EOF,			// ����� ���������� ������
	T_ILLEGAL,		// ������� ������������� �������
	T_IDENTIFIER,		// �������������
	T_NUMBER,		// ������������� �������
	T_BEGIN,		// �������� ����� "begin"
	T_END,			// �������� ����� "end"
	T_IF,			// �������� ����� "if"
	T_THEN,			// �������� ����� "then"
	T_ELSE,			// �������� ����� "else"
	T_FI,			// �������� ����� "fi"
	T_WHILE,		// �������� ����� "while"
	T_DO,			// �������� ����� "do"
	T_OD,			// �������� ����� "od"
	T_WRITE,		// �������� ����� "write"
	T_READ,			// �������� ����� "read"
	T_ASSIGN,		// �������� ":="
	T_ADDOP,		// ������� ������� ��� "+" � "-" (�������� ���� ��������)
	T_MULOP,		// ������� ������� ��� "*" � "/" (�������� ���� ���������)
	T_CMP,			// ������� ������� ��� ���������� ���������
	T_LPAREN,		// ����������� ������
	T_RPAREN,		// ����������� ������
	T_SEMICOLON,	// ";"
	T_REPEAT,		// �������� ����� "repeat"
	T_UNTIL			// �������� ����� "until"
};

// ������� tokenToString ���������� �������� �������.
// ������������ ��� ������ ��������� �� ������.
const char* tokenToString(Token t);

// ���� �������� ���������
enum Cmp {
	C_EQ,   // �������� ��������� "="
	C_NE,	// �������� ��������� "!="
	C_LT,	// �������� ��������� "<"
	C_LE,	// �������� ��������� "<="
	C_GT,	// �������� ��������� ">"
	C_GE	// �������� ��������� ">="
};

// ���� �������������� ��������
enum Arithmetic {
	A_PLUS,		//�������� "+"
	A_MINUS,	//�������� "-"
	A_MULTIPLY,	//�������� "*"
	A_DIVIDE	//�������� "/"
};

// ����������� ����������

class Scanner
{
public:
	// �����������. � �������� ��������� ��������� ��� ����� � �����,
	// �� �������� ����� �������� ������� ������������� ���������.

	explicit Scanner(const string& fileName, istream& input)
		: fileName_(fileName), lineNumber_(1), input_(input)
	{
		keywords_["begin"] = T_BEGIN;
		keywords_["end"] = T_END;
		keywords_["if"] = T_IF;
		keywords_["then"] = T_THEN;
		keywords_["else"] = T_ELSE;
		keywords_["fi"] = T_FI;
		keywords_["while"] = T_WHILE;
		keywords_["do"] = T_DO;
		keywords_["od"] = T_OD;
		keywords_["write"] = T_WRITE;
		keywords_["read"] = T_READ;
		keywords_["repeat"] = T_REPEAT;
		keywords_["until"] = T_UNTIL;

		nextChar();
	}

	// ����������
	virtual ~Scanner()
	{}

	//getters ���� private ����������
	const string& getFileName() const //�� ������������
	{
		return fileName_;
	}

	int getLineNumber() const
	{
		return lineNumber_;
	}

	Token token() const
	{
		return token_;
	}

	int getIntValue() const
	{
		return intValue_;
	}

	string getStringValue() const
	{
		return stringValue_;
	}

	Cmp getCmpValue() const
	{
		return cmpValue_;
	}

	Arithmetic getArithmeticValue() const
	{
		return arithmeticValue_;
	}

	// ������� � ��������� �������.
	// ������� ������� ������������ � token_ � ��������� �� ������.
	void nextToken();
private:

	// ������� ���� ���������� �������. 
	// ���� ����������� ������ �������� ������, ����� ������� ������
	// (lineNumber) ������������� �� �������.
	void skipSpace();


	void nextChar(); //��������� � ���������� �������
	//�������� ���������� �� ������ ������ (������ ���� ������ ���������� ��������)
	bool isIdentifierStart(char c)
	{
		return ((c >= 'a' && c <= 'z') ||
			(c >= 'A' && c <= 'Z'));
	}
	//�������� �� ��������� ������� ���������� (����� ��� �����)
	bool isIdentifierBody(char c)
	{
		return isIdentifierStart(c) || isdigit(c);
	}


	const string fileName_; //������� ����
	int lineNumber_; //����� ������� ������ ����

	Token token_; //������� �������
	int intValue_; //�������� �������� ������
	string stringValue_; //��� ����������
	Cmp cmpValue_; //�������� ��������� ��������� (>, <, =, !=, >=, <=)
	Arithmetic arithmeticValue_; //�������� ����� (+,-,*,/)

	map<string, Token> keywords_; //������������� ������ � ��������� � 
	//���������������� �� ������������������ ������� � �������� ��������

	istream& input_; //������� ����� ��� ������ �� �����.
	char ch_; //������� ������
};

#endif
