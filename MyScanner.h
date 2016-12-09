#ifndef	 MYSCANNER_H__
#define MYSCANNER_H__
/*
	scanner.h
	2016-12-08 16:08:10
	TODO 变量的识别
*/
#include <iostream>
using namespace std;

const int Token_Maxlen = 100; //记号最大长度
const char Str_End = '\0'; //字符串结束标志符

enum Token_Type {	//记号种类
	ORIGIN,	SCALE,	ROT,	IS,	TO,	STEP,	DRAW,	FOR,	FROM,	
	T,	//参数
	SEMICO,L_BRACKET, R_BRACKET, COMMA,		// 分隔符号
	PLUS,	MINUS,	MUL,	DIV,	POWER,
	FUNC,	//函数
	CONST_ID,	//常数
	NONTOKEN,	//空
	ERRTOKEN,	//出错
};

struct Token	//记号的数据结构
{
	Token_Type type;	//类别
	char *lexeme;	//记号原始串
	double value;	//若是常熟,则常熟的值
	double (*func_ptr)(double);	//若是函数，则函数指针 --- NULL		func_ptr是一个指针,指向double f(double b){}这种函数 
};

class MyScanner {

private:
	FILE *InFile;	//输入文件流
	unsigned int LineNo;	//记号所在行号
	char TokenStr[Token_Maxlen];	//记号缓冲

public:
	MyScanner() { LineNo = 1; };
	~MyScanner() {};
	
	int	InitScanner(const char *FileName);	//初始化
	void CloseScanner();		//关闭

	unsigned int GetLineNo();	//获取记号所在行号
	Token GetToken();	//获取记号
	
	char GetChar();	//从输入读入一个字符
	void BackChar(char NextChar);	//把字符回退到输入中
	void AddToTokenStr(char NextChar);	//识别到的记号加入到记号缓冲中
	Token InTokenTable(const char *c_str);	//判断是否在符号表中
	void ClearTokenStr();	//清空记号缓冲
};

#endif MYSCANNER_H__
