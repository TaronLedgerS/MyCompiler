#ifndef	 MYSCANNER_H__
#define MYSCANNER_H__
/*
	scanner.h
	2016-12-08 16:08:10
	TODO ������ʶ��
*/
#include <iostream>
using namespace std;

const int Token_Maxlen = 100; //�Ǻ���󳤶�
const char Str_End = '\0'; //�ַ���������־��

enum Token_Type {	//�Ǻ�����
	ORIGIN,	SCALE,	ROT,	IS,	TO,	STEP,	DRAW,	FOR,	FROM,	
	T,	//����
	SEMICO,L_BRACKET, R_BRACKET, COMMA,		// �ָ�����
	PLUS,	MINUS,	MUL,	DIV,	POWER,
	FUNC,	//����
	CONST_ID,	//����
	NONTOKEN,	//��
	ERRTOKEN,	//����
};

struct Token	//�Ǻŵ����ݽṹ
{
	Token_Type type;	//���
	char *lexeme;	//�Ǻ�ԭʼ��
	double value;	//���ǳ���,�����ֵ
	double (*func_ptr)(double);	//���Ǻ���������ָ�� --- NULL		func_ptr��һ��ָ��,ָ��double f(double b){}���ֺ��� 
};

class MyScanner {

private:
	FILE *InFile;	//�����ļ���
	unsigned int LineNo;	//�Ǻ������к�
	char TokenStr[Token_Maxlen];	//�ǺŻ���

public:
	MyScanner() { LineNo = 1; };
	~MyScanner() {};
	
	int	InitScanner(const char *FileName);	//��ʼ��
	void CloseScanner();		//�ر�

	unsigned int GetLineNo();	//��ȡ�Ǻ������к�
	Token GetToken();	//��ȡ�Ǻ�
	
	char GetChar();	//���������һ���ַ�
	void BackChar(char NextChar);	//���ַ����˵�������
	void AddToTokenStr(char NextChar);	//ʶ�𵽵ļǺż��뵽�ǺŻ�����
	Token InTokenTable(const char *c_str);	//�ж��Ƿ��ڷ��ű���
	void ClearTokenStr();	//��ռǺŻ���
};

#endif MYSCANNER_H__
