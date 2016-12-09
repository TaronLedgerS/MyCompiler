#include "MyScanner.h"
#include <cmath>
/*  2016-12-08 20:49:06   */
static Token TokeTable[] =		//符号表
{
	{CONST_ID,	"PI",	3.145926,	NULL},
	{CONST_ID,	"E",	2.71828,	NULL},
	{T,	"T",	0.0,	NULL},
	{FUNC,	"SIN",	0.0,	sin},
	{FUNC,	"COS",	0.0,	cos},
	{FUNC,	"TAN",	0.0,	tan},
	{FUNC,	"LN",	0.0,	log},
	{FUNC,	"EXP",	0.0,	exp},
	{FUNC,	"SQRT",	0.0,	sqrt},
	{ORIGIN,	"ORIGIN",	0.0,	NULL},
	{SCALE,		"SCALE",	0.0,	NULL},
	{ROT,	"ROT",	0.0,	NULL},
	{IS,	"IS",	0.0,	NULL},
	{FOR,	"FOR",	0.0,	NULL},
	{FROM,	"FROM",	0.0,	NULL},
	{TO,	"TO",	0.0,	NULL},
	{STEP,	"STEP",	0.0,	NULL},
	{DRAW,	"DRAW",	0.0,	NULL},
};

//获取文件输入留初始化InFile
int MyScanner::InitScanner(const char * FileName) { 
	InFile = fopen(FileName, "r");
	if (InFile) return 1;
	else	return 0;
}

//关闭文件
void MyScanner::CloseScanner() {
	if (InFile) fclose(InFile);
}


unsigned int MyScanner::GetLineNo() {
	return LineNo;
}


//从输入中读入一个字符（不区分大小写，一律大写处理）
char MyScanner::GetChar() {
	char NextChar = getc(InFile);
	return toupper(NextChar);
}

//把预读字符退回输入流
void MyScanner::BackChar(char NextChar) {
	if (NextChar != EOF) ungetc(NextChar, InFile);
}

//加入字符到正在识别的记号串中
void MyScanner::AddToTokenStr(char NextChar) {
	int TokenLen;
	TokenLen = strlen(TokenStr);
	if (TokenLen + 1 >= sizeof(TokenStr)) return;	//溢出判断
	TokenStr[TokenLen] = NextChar;	
	TokenStr[TokenLen+1] = Str_End;
}

//清空记号串
void MyScanner::ClearTokenStr() {
	memset(TokenStr, 0, Token_Maxlen);	//全部用0覆盖，包括字符串结束符号
}



Token MyScanner::InTokenTable(const char * c_str) {
	Token token;
//	cout << c_str << "----"<<endl;
	for (int i = 0;i<sizeof(TokeTable)/sizeof(TokeTable[0]);i++)
		if (strcmp(TokeTable[i].lexeme, c_str) == 0)
			return TokeTable[i];
	//如果不在符号表
	memset(&token, 0, sizeof(Token));
	token.func_ptr = NULL;
	token.value = 0.0;
	token.type = ERRTOKEN;
	return token;
}



//识别一个记号
Token MyScanner::GetToken() {
	Token token;	//记录记号
	char NextChar;
	//初始化
	memset(&token, 0, sizeof(Token));
	token.func_ptr = NULL;
	token.value = 0.0;
	ClearTokenStr();
	token.lexeme = TokenStr;
	//过滤空白字符
	while (1) {
		NextChar = GetChar();
		if (NextChar == EOF) {
			token.type = NONTOKEN;
			return token;
		}
		if (NextChar == '\n') LineNo++;
		if (!isspace(NextChar))break;
	}
	AddToTokenStr(NextChar);	//预读字符存入记号串中
	
	//开始识别记号 DFA
	//1.字母---保留字、参数等
	if (isalpha(NextChar))	
	{
		while (true) {
			NextChar = GetChar();
			if (isalnum(NextChar)) {	//当c为数字0 - 9或字母a - z及A - Z时，返回非零值，否则返回零。
				AddToTokenStr(NextChar);
			}
			else  break;
		}
		BackChar(NextChar);
		//TODO 识别变量id（以字母开头的字符数字串）？？ 
		token = InTokenTable(TokenStr);	//若在表中，则直接获得记号
		token.lexeme = TokenStr;
		return token; 
	}
	else//2.数字--常量 
		if (isdigit(NextChar)) {
			while (true) {
				NextChar = GetChar();
				if (isdigit(NextChar)) AddToTokenStr(NextChar);
				else break;
			}
			if (NextChar == '.')	//有小数点的话
			{
				AddToTokenStr(NextChar);
				while (true) {
					NextChar = GetChar();
					if (isdigit(NextChar)) AddToTokenStr(NextChar);
					else break;
				}
			}
			BackChar(NextChar);
			token.type = CONST_ID;
			token.value = atof(TokenStr);
			return token;
		}else
		//3.运算符
		{
			switch (NextChar) {
				case ';': token.type = SEMICO; break;
				case '(':token.type = L_BRACKET; break;
				case ')':token.type = R_BRACKET; break;
				case ',':token.type = COMMA; break;
				case '+':token.type = PLUS; break;
				case '-'://减号或者注释
					NextChar = GetChar();
					if (NextChar == '-') {
						while (NextChar != '\n'&&NextChar != EOF)
							NextChar = GetChar();//过滤注释部分
						BackChar(NextChar);//回退'\n'与EOF
						return GetToken();//递归的方法保证返回记号值 
					}
					else{
						BackChar(NextChar);
						token.type = MINUS;
						break;
					}
				case '/'://除号或者注释
					NextChar = GetChar();
					if (NextChar == '/') {
						while (NextChar != '\n'&&NextChar != EOF)
							NextChar = GetChar();//过滤注释部分
						BackChar(NextChar);//回退'\n'与EOF
						return GetToken();//递归的方法保证返回记号值 
					}
					else{
						BackChar(NextChar);
						token.type = DIV;
						break;
					}
				case '*'://乘号或者指数
					NextChar = GetChar();
					if (NextChar='*')
					{
						token.type = POWER;
						AddToTokenStr(NextChar);
					}
					else{
						BackChar(NextChar);
						token.type = MUL;
					}
					break;
				default://3类都识别不出来----出错
					token.type = ERRTOKEN;
					break;
			}
		}

	return token;
}
