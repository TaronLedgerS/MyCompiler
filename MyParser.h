#ifndef MYPARSER_H__
#define MYPARSER_H__
/*
	语法分析器 2016-12-09 10:54:05
*/

#include "MyScanner.h"
#include <stdarg.h>

typedef double (*func_ptr)(double);

typedef struct TreeNodeStruct {  //表达式的语法树节点类型
	enum Token_Type OpCode;	//记号种类
	union  {
		struct { TreeNodeStruct *left, *right; } CaseOp; //二元运算
		struct { TreeNodeStruct *child;	func_ptr mathfuncptr; } CaseFunc; //函数
		double CaseConst;	//常数
		double *CasePara;	//参数T
	} content;
} * TreeNode;
class MyParser {
private:
	double TPara;	//参数T的值
	Token token;	//记号
	TreeNode 
		StartPtr, 	EndPtr,
		StepPtr,	//步长
		XPtr,	YPtr,
		AnglePtr;	//角度
	MyScanner Scanner;	
	int indent;// 测试用于缩进
public:
	MyParser();
	~MyParser();

	void FetchToken(); //获取记号
	void MatchToken(enum Token_Type ttoken);	//匹配记号
	void MatchToken(enum Token_Type ttoken, char * text);//?
	void SyntaxError(int case_of);	//语法错误处理，调用ErrorMsg
	virtual void ErrorMsg(int line, char * sourcetext,char* descrip);	//TODO MySemantics
	
	TreeNode MakeTreeNode(enum  Token_Type opcode, ...);	//构造语法树
	void PrintSyntaxTree(TreeNode root, int indent);	//打印语法树

	void InitParser(char *FileName);//语法分析器接口
	//根据BNCF构造的非终结符递归子程序 
	void Program();
	void Statement();
	//MySemantics中实现语法指导翻译
	virtual void ForStatement();
	virtual void OriginStatement();
	virtual void RotStatement();
	virtual void ScaleStatement();
	TreeNode Expression();
	TreeNode Term();
	TreeNode Factor();
	TreeNode Component();
	TreeNode Atom();


	//语法分析器测试用的成员函数，在MySemantics中重置为不起作用
	virtual void Enter(char* x);
	virtual void Back(char*x);
	virtual void CallMatch(char*x);
	virtual void TreeTrace(TreeNode x);
};
#endif MYPARSER_H__
