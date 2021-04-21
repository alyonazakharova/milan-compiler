#include "parser.h"
#include <sstream>

// ��������� �������������� ������ ����� program.
// ���� �� ����� ������� �� ������������ ������� ������, �� ������� ������������������ ������ ����-������.
void Parser::parse()
{
	program();
	if (!error_) {
		codegen_->flush();
	}
}

void Parser::program()
{
	mustBe(T_BEGIN);
	statementList();
	mustBe(T_END);
	codegen_->emit(STOP);
}

void Parser::statementList()
{
	// ���� ������ ���������� ����, ��������� �������� ����� ���� �� ��������� "����������� ������": END, OD, ELSE, FI.
	// � ���� ������ ����������� ������� ����� ������ ���� (��� ������ ���������� ����� null).
	// ���� ��������� ������� �� ������ � ���� ������, �� �� �� ������� ������� ��������� � �������� ����� statement. 
	// ��������� ���������� ��������� �������� ���������� ����� ��������� ����� � �������.
	if (see(T_END) || see(T_OD) || see(T_ELSE) || see(T_FI)) {
		return;
	}
	else {
		bool more = true;
		while (more) {
			statement();
			more = match(T_SEMICOLON);
		}
	}
}

void Parser::statement()
{
	// ���� ��������� ����������, �� ���������� �� ����� ��� ��������� ����� ���� �� ���������. 
	// ��������� �������� ������ ���� ������������. ����� ���� ���� expression, ������� ���������� �������� �� ������� �����.
	// ���������� ��� �������� �� ������ ����� ����������.
	if (see(T_IDENTIFIER)) {
		int varAddress = findOrAddVariable(scanner_->getStringValue());
		next();
		mustBe(T_ASSIGN);
		expression();
		codegen_->emit(STORE, varAddress);
	}
	// ���� ��������� IF, �� ����� ������ ��������� �������. �� ������� ����� ����� 1 ��� 0 � ����������� �� ���������� �������.
	// ����� ������������� ����� ��� ��������� �������� JUMP_NO � ����� ELSE (������� � ������ ������� �������).
	// ����� �������� ������ ��������� ������ ����� ����, ��� ����� ������������ ��� ��� ����� THEN.
	else if (match(T_IF)) {
		relation();

		int jumpNoAddress = codegen_->reserve();

		mustBe(T_THEN);
		statementList();
		if (match(T_ELSE)) {
			// ���� ���� ���� ELSE, �� ����� �� ��������� ��� � ������ ���������� THEN, 
			// ������������� ����� ��� ������� JUMP � ����� ����� �����
			int jumpAddress = codegen_->reserve();
			// �������� ����������������� ����� ����� �������� ������� ����������� �������� � ������ ����� ELSE.
			codegen_->emitAt(jumpNoAddress, JUMP_NO, codegen_->getCurrentAddress());
			statementList();
			// �������� ������ ����� ����������� �������� � ����� ��������� ����� ELSE.
			codegen_->emitAt(jumpAddress, JUMP, codegen_->getCurrentAddress());
		}
		else {
			// ���� ���� ELSE �����������, �� � ����������������� ����� ����� �������� ������� ����� ��������
			// ���������� ��������� �������� � ����� ��������� IF...THEN
			codegen_->emitAt(jumpNoAddress, JUMP_NO, codegen_->getCurrentAddress());
		}

		mustBe(T_FI);
	}
	else if (match(T_WHILE)) {
		//���������� ����� ������ �������� �������
		int conditionAddress = codegen_->getCurrentAddress();
		relation();
		//����������� ����� ��� ���������� ��������� �������� ��� ������ �� �����
		int jumpNoAddress = codegen_->reserve();
		mustBe(T_DO);
		statementList();
		mustBe(T_OD);
		//��������� �� ������ �������� �������
		codegen_->emit(JUMP, conditionAddress);
		//��������� ����������������� ����� ����������� ��������� �������� �� ��������� �� ������ ��������.
		codegen_->emitAt(jumpNoAddress, JUMP_NO, codegen_->getCurrentAddress());
	}
	else if (match(T_REPEAT)) {
		//���������� ����� ������ ������ ����������
		int cycleStartAddress = codegen_->getCurrentAddress();
		statementList();
		mustBe(T_UNTIL);
		relation();
		//����������� ����� ��� ���������� ��������� �������� ��� ������ �� �����
		int jumpYesAddress = codegen_->reserve();
		//��������� ����������������� ����� ����������� ��������� �������� �� ��������� �� ������ ��������
		codegen_->emitAt(jumpYesAddress, JUMP_YES, codegen_->getCurrentAddress() + 1);
		//��������� �� ������ ������ ������ ����������	
		codegen_->emit(JUMP, cycleStartAddress);
	}
	else if (match(T_WRITE)) {
		mustBe(T_LPAREN);
		expression();
		mustBe(T_RPAREN);
		codegen_->emit(PRINT);
	}
	else {
		reportError("statement expected.");
	}
}

void Parser::expression()
{

	/*
		�������������� ��������� ����������� ���������� ���������: <expression> -> <term> | <term> + <term> | <term> - <term>
		��� ������� ������� ������� ������ ����, ����� ����������� ��������� ������. ���� ��� '+' ��� '-',
		������� ��� �� ������ � ��������� ��������� ��������� (����������). ��������� �������� � ������ ����������
		�����, ���� �� �������� �� ������ ������, �������� �� '+' � '-'
	*/

	term();
	while (see(T_ADDOP)) {
		Arithmetic op = scanner_->getArithmeticValue();
		next();
		term();

		if (op == A_PLUS) {
			codegen_->emit(ADD);
		}
		else {
			codegen_->emit(SUB);
		}
	}
}

void Parser::term()
{
	/*
		���� ����������� ���������� ���������: <expression> -> <factor> | <factor> + <factor> | <factor> - <factor>
		��� ������� ������� ������� ������ ���������, ����� ����������� ��������� ������. ���� ��� '*' ��� '/',
		������� ��� �� ������ � ��������� ��������� ��������� (����������). ��������� �������� � ������ ����������
		���������, ���� �� �������� �� ��� ������, �������� �� '*' � '/'
   */
	factor();
	while (see(T_MULOP)) {
		Arithmetic op = scanner_->getArithmeticValue();
		next();
		factor();

		if (op == A_MULTIPLY) {
			codegen_->emit(MULT);
		}
		else {
			codegen_->emit(DIV);
		}
	}
}

void Parser::factor()
{
	/*
		��������� ����������� ���������� ���������:
		<factor> -> number | identifier | -<factor> | (<expression>) | READ
	*/
	if (see(T_NUMBER)) {
		int value = scanner_->getIntValue();
		next();
		codegen_->emit(PUSH, value);
		//���� ��������� �����, �� ����������� ��� � ����� � ���������� �� ������� �����
	}
	else if (see(T_IDENTIFIER)) {
		int varAddress = findOrAddVariable(scanner_->getStringValue());
		next();
		codegen_->emit(LOAD, varAddress);
		//���� ��������� ����������, �� ��������� ��������, ������� �� �� ������, �� ������� ����� 
	}
	else if (see(T_ADDOP) && scanner_->getArithmeticValue() == A_MINUS) {
		next();
		factor();
		codegen_->emit(INVERT);
		//���� ��������� ���� "-", � �� ��� <factor> �� ����������� ��������, ������� �� ������� �����
	}
	else if (match(T_LPAREN)) {
		expression();
		mustBe(T_RPAREN);
		//���� ��������� ����������� ������, ����� ������ ����� ���� ����� �������������� ��������� � �����������
		//����������� ������.
	}
	else if (match(T_READ)) {
		codegen_->emit(INPUT);
		//���� ��������� ����������������� ����� READ, �� ���������� �� ������� ����� ���� ������ �� ������������ �����
	}
	else {
		reportError("expression expected.");
	}
}

void Parser::relation()
{
	//������� ���������� ��� ��������� �� ������-���� �� ������. ������ ���� ����� ���� �����. � ����������� �� 
	//���������� ��������� �� ������� ����� �������� 0 ��� 1.
	expression();
	if (see(T_CMP)) {
		Cmp cmp = scanner_->getCmpValue();
		next();
		expression();
		switch (cmp) {
			//��� ����� "=" - ����� 0
		case C_EQ:
			codegen_->emit(COMPARE, 0);
			break;
			//��� ����� "!=" - ����� 1
		case C_NE:
			codegen_->emit(COMPARE, 1);
			break;
			//��� ����� "<" - ����� 2
		case C_LT:
			codegen_->emit(COMPARE, 2);
			break;
			//��� ����� ">" - ����� 3
		case C_GT:
			codegen_->emit(COMPARE, 3);
			break;
			//��� ����� "<=" - ����� 4
		case C_LE:
			codegen_->emit(COMPARE, 4);
			break;
			//��� ����� ">=" - ����� 5
		case C_GE:
			codegen_->emit(COMPARE, 5);
			break;
		};
	}
	else {
		reportError("comparison operator expected.");
	}
}

int Parser::findOrAddVariable(const string& var)
{
	VarTable::iterator it = variables_.find(var);
	if (it == variables_.end()) {
		variables_[var] = lastVar_;
		return lastVar_++;
	}
	else {
		return it->second;
	}
}

void Parser::mustBe(Token t)
{
	if (!match(t)) {
		error_ = true;

		// ���������� ��������� �� ������
		std::ostringstream msg;
		msg << tokenToString(scanner_->token()) << " found while " << tokenToString(t) << " expected.";
		reportError(msg.str());

		// ������� �������������� ����� ������.
		recover(t);
	}
}

void Parser::recover(Token t)
{
	while (!see(t) && !see(T_EOF)) {
		next();
	}

	if (see(t)) {
		next();
	}
}
