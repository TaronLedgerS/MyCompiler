#ifndef MYPARSER_H__
#define MYPARSER_H__
/*
	�﷨������ 2016-12-09 10:54:05
*/

#include "MyScanner.h"
#include <stdarg.h>

typedef double (*func_ptr)(double);

typedef struct TreeNodeStruct {  //���ʽ���﷨���ڵ�����
	enum Token_Type OpCode;	//�Ǻ�����
	union  {
		struct { TreeNodeStruct *left, *right; } CaseOp; //��Ԫ����
		struct { TreeNodeStruct *child;	func_ptr mathfuncptr; } CaseFunc; //����
		double CaseConst;	//����
		double *CasePara;	//����T
	} content;
} * TreeNode;
class MyParser {
private:
	double TPara;	//����T��ֵ
	Token token;	//�Ǻ�
	TreeNode 
		StartPtr, 	EndPtr,
		StepPtr,	//����
		XPtr,	YPtr,
		AnglePtr;	//�Ƕ�
	MyScanner Scanner;	
	int indent;// ������������
public:
	MyParser();
	~MyParser();

	void FetchToken(); //��ȡ�Ǻ�
	void MatchToken(enum Token_Type ttoken);	//ƥ��Ǻ�
	void MatchToken(enum Token_Type ttoken, char * text);//?
	void SyntaxError(int case_of);	//�﷨����������ErrorMsg
	virtual void ErrorMsg(int line, char * sourcetext,char* descrip);	//TODO MySemantics
	
	TreeNode MakeTreeNode(enum  Token_Type opcode, ...);	//�����﷨��
	void PrintSyntaxTree(TreeNode root, int indent);	//��ӡ�﷨��

	void InitParser(char *FileName);//�﷨�������ӿ�
	//����BNCF����ķ��ս���ݹ��ӳ��� 
	void Program();
	void Statement();
	//MySemantics��ʵ���﷨ָ������
	virtual void ForStatement();
	virtual void OriginStatement();
	virtual void RotStatement();
	virtual void ScaleStatement();
	TreeNode Expression();
	TreeNode Term();
	TreeNode Factor();
	TreeNode Component();
	TreeNode Atom();


	//�﷨�����������õĳ�Ա��������MySemantics������Ϊ��������
	virtual void Enter(char* x);
	virtual void Back(char*x);
	virtual void CallMatch(char*x);
	virtual void TreeTrace(TreeNode x);
};
#endif MYPARSER_H__
