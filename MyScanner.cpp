#include "MyScanner.h"
#include <cmath>
/*  2016-12-08 20:49:06   */
static Token TokeTable[] =		//���ű�
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

//��ȡ�ļ���������ʼ��InFile
int MyScanner::InitScanner(const char * FileName) { 
	InFile = fopen(FileName, "r");
	if (InFile) return 1;
	else	return 0;
}

//�ر��ļ�
void MyScanner::CloseScanner() {
	if (InFile) fclose(InFile);
}


unsigned int MyScanner::GetLineNo() {
	return LineNo;
}


//�������ж���һ���ַ��������ִ�Сд��һ�ɴ�д����
char MyScanner::GetChar() {
	char NextChar = getc(InFile);
	return toupper(NextChar);
}

//��Ԥ���ַ��˻�������
void MyScanner::BackChar(char NextChar) {
	if (NextChar != EOF) ungetc(NextChar, InFile);
}

//�����ַ�������ʶ��ļǺŴ���
void MyScanner::AddToTokenStr(char NextChar) {
	int TokenLen;
	TokenLen = strlen(TokenStr);
	if (TokenLen + 1 >= sizeof(TokenStr)) return;	//����ж�
	TokenStr[TokenLen] = NextChar;	
	TokenStr[TokenLen+1] = Str_End;
}

//��ռǺŴ�
void MyScanner::ClearTokenStr() {
	memset(TokenStr, 0, Token_Maxlen);	//ȫ����0���ǣ������ַ�����������
}



Token MyScanner::InTokenTable(const char * c_str) {
	Token token;
//	cout << c_str << "----"<<endl;
	for (int i = 0;i<sizeof(TokeTable)/sizeof(TokeTable[0]);i++)
		if (strcmp(TokeTable[i].lexeme, c_str) == 0)
			return TokeTable[i];
	//������ڷ��ű�
	memset(&token, 0, sizeof(Token));
	token.func_ptr = NULL;
	token.value = 0.0;
	token.type = ERRTOKEN;
	return token;
}



//ʶ��һ���Ǻ�
Token MyScanner::GetToken() {
	Token token;	//��¼�Ǻ�
	char NextChar;
	//��ʼ��
	memset(&token, 0, sizeof(Token));
	token.func_ptr = NULL;
	token.value = 0.0;
	ClearTokenStr();
	token.lexeme = TokenStr;
	//���˿հ��ַ�
	while (1) {
		NextChar = GetChar();
		if (NextChar == EOF) {
			token.type = NONTOKEN;
			return token;
		}
		if (NextChar == '\n') LineNo++;
		if (!isspace(NextChar))break;
	}
	AddToTokenStr(NextChar);	//Ԥ���ַ�����ǺŴ���
	
	//��ʼʶ��Ǻ� DFA
	//1.��ĸ---�����֡�������
	if (isalpha(NextChar))	
	{
		while (true) {
			NextChar = GetChar();
			if (isalnum(NextChar)) {	//��cΪ����0 - 9����ĸa - z��A - Zʱ�����ط���ֵ�����򷵻��㡣
				AddToTokenStr(NextChar);
			}
			else  break;
		}
		BackChar(NextChar);
		//TODO ʶ�����id������ĸ��ͷ���ַ����ִ������� 
		token = InTokenTable(TokenStr);	//���ڱ��У���ֱ�ӻ�üǺ�
		token.lexeme = TokenStr;
		return token; 
	}
	else//2.����--���� 
		if (isdigit(NextChar)) {
			while (true) {
				NextChar = GetChar();
				if (isdigit(NextChar)) AddToTokenStr(NextChar);
				else break;
			}
			if (NextChar == '.')	//��С����Ļ�
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
		//3.�����
		{
			switch (NextChar) {
				case ';': token.type = SEMICO; break;
				case '(':token.type = L_BRACKET; break;
				case ')':token.type = R_BRACKET; break;
				case ',':token.type = COMMA; break;
				case '+':token.type = PLUS; break;
				case '-'://���Ż���ע��
					NextChar = GetChar();
					if (NextChar == '-') {
						while (NextChar != '\n'&&NextChar != EOF)
							NextChar = GetChar();//����ע�Ͳ���
						BackChar(NextChar);//����'\n'��EOF
						return GetToken();//�ݹ�ķ�����֤���ؼǺ�ֵ 
					}
					else{
						BackChar(NextChar);
						token.type = MINUS;
						break;
					}
				case '/'://���Ż���ע��
					NextChar = GetChar();
					if (NextChar == '/') {
						while (NextChar != '\n'&&NextChar != EOF)
							NextChar = GetChar();//����ע�Ͳ���
						BackChar(NextChar);//����'\n'��EOF
						return GetToken();//�ݹ�ķ�����֤���ؼǺ�ֵ 
					}
					else{
						BackChar(NextChar);
						token.type = DIV;
						break;
					}
				case '*'://�˺Ż���ָ��
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
				default://3�඼ʶ�𲻳���----����
					token.type = ERRTOKEN;
					break;
			}
		}

	return token;
}
