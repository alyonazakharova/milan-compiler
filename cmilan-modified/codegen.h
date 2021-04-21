#ifndef CMILAN_CODEGEN_H
#define CMILAN_CODEGEN_H

#include <vector>
#include <iostream>

using namespace std;

// ���������� ����������� ������ ������ 

enum Instruction
{
	NOP,		// ���������� ��������
	STOP,		// ��������� ������, ���������� ������ ���������
	LOAD,		// LOAD addr - �������� ����� ������ � ���� �� ������ �� ������ addr
	STORE,		// STORE addr - ������ ����� ������ � ������� ����� � ������ �� ������ addr
	BLOAD,		// BLOAD addr - �������� ����� ������ � ���� �� ������ �� ������ addr + �������� �� ������� �����
	BSTORE,		// BSTORE addr - ������ ����� ������ �� ������ addr + �������� �� ������� �����
	PUSH,		// PUSH n - �������� � ���� ��������� n
	POP,		// �������� ����� � ������� �����
	DUP,		// ����������� ����� �� ������� �����
	ADD,		// �������� ���� ���� �� ������� ����� � ������ ���������� ������ ���
	SUB,		// ��������� ���� ���� �� ������� ����� � ������ ���������� ������ ���
	MULT,		// ��������� ���� ���� �� ������� ����� � ������ ���������� ������ ���
	DIV,		// ������� ���� ���� �� ������� ����� � ������ ���������� ������ ���
	INVERT,		// ��������� ����� ����� �� ������� �����
	COMPARE,	// COMPARE cmp - ��������� ���� ���� �� ������� ����� � ������� �������� ��������� � ����� cmp
	JUMP,		// JUMP addr - ����������� ������� �� ������ addr
	JUMP_YES,	// JUMP_YES addr - ������� �� ������ addr, ���� �� ������� ����� �������� 1
	JUMP_NO,	// JUMP_NO addr - ������� �� ������ addr, ���� �� ������� ����� �������� 0
	INPUT,		// ������ ������ ����� �� ������������ ����� � �������� ��� � ����
	PRINT		// ������ �� ����������� ����� ����� � ������� �����
};

// ����� Command ������������ �������� ����������. 

class Command
{
public:
	// ����������� ��� ���������� ��� ����������
	Command(Instruction instruction)
		: instruction_(instruction), arg_(0)
	{}

	// ����������� ��� ���������� � ����� ����������
	Command(Instruction instruction, int arg)
		: instruction_(instruction), arg_(arg)
	{}

	// ������ ����������
	//     int address - ����� ����������
	//     ostream& os - ����� ������, ���� ����� ���������� ����������
	void print(int address, ostream& os);

private:
	Instruction instruction_; // ��� ����������
	int arg_;				  // �������� ����������
};

// �������������.
// ���������� ��������������:
// - ����������� ��������� ��� ����������� ������ ������
// - ����������� ����� ��������� ����������
// - ������������ ��������� � �������� �� � ��������� ����� ������

class CodeGen
{
public:
	explicit CodeGen(ostream& output)
		: output_(output)
	{
	}

	// ���������� ���������� ��� ���������� � ����� ���������
	void emit(Instruction instruction);

	// ���������� ���������� � ����� ���������� � ����� ���������
	void emit(Instruction instruction, int arg);

	// ������ ���������� ��� ���������� �� ���������� ������
	void emitAt(int address, Instruction instruction);

	// ������ ���������� � ����� ���������� �� ���������� ������
	void emitAt(int address, Instruction instruction, int arg);

	// ��������� ������, ��������������� ���������� �� ��������� ����������� � ���������
	int getCurrentAddress();

	// ������������ "������" ���������� (NOP) � ������� �� ������
	int reserve();

	// ������ ������������������ ���������� � �������� �����
	void flush();

private:
	ostream& output_;               // �������� �����
	vector<Command> commandBuffer_;	// ����� ����������
};

#endif
