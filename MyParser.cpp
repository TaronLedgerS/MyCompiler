#include "MyParser.h"

MyParser::MyParser() {
}

MyParser::~MyParser() {
}
//ͨ��MyScanner��GetToken()��ȡһ���Ǻ�
void MyParser::FetchToken() {
	token = Scanner.GetToken();
	if (token.type == ERRTOKEN) SyntaxError(1);//�Ƿ��Ǻ�
}

// �Ǻ�ƥ�� ���� 1. ���ʽ���������ƥ�� 2.�����֣��ؼ��֣�ƥ�䲢����ʱ��ӡƥ����
void MyParser::MatchToken(Token_Type ttoken) {
	if (token.type != ttoken) SyntaxError(2);//��ǰ�ǺŲ���Ԥ�ڼǺ�
	FetchToken();	//ƥ��ɹ�����ȡ��һ���Ǻ�
}
void MyParser::MatchToken(Token_Type ttoken, char * text) {
	if (token.type != ttoken) SyntaxError(2);//��ǰ�ǺŲ���Ԥ�ڼǺ�
	if (text != NULL)CallMatch(text); //���� ƥ��ɹ�
	FetchToken();	//ƥ��ɹ�����ȡ��һ���Ǻ�
}

//�﷨������2��TODO ����ErrorMsg���б���
void MyParser::SyntaxError(int case_of) {
	switch (case_of) {
		case 1:
			ErrorMsg(Scanner.GetLineNo(),token.lexeme, "Ϊ�Ƿ��Ǻ�");
			break;
		case 2:
			ErrorMsg(Scanner.GetLineNo(), token.lexeme, "����Ԥ�ڼǺ�");
			break;
	}
}
void MyParser::ErrorMsg(int line, char * sourcetext, char * descrip) {
	cout << "Line No." << line << ": " << sourcetext << descrip << endl;
	Scanner.CloseScanner();
	exit(1);//TODO ������˳�����
}

TreeNode MyParser::MakeTreeNode(Token_Type opcode, ...) {
	return TreeNode();
}

void MyParser::PrintSyntaxTree(TreeNode root, int indent) {
}


TreeNode MyParser::MakeTreeNode(Token_Type opcode, ...) {
	return TreeNode();
}

void MyParser::PrintSyntaxTree(TreeNode root, int indent) {
}

void MyParser::Program() {//Program -> {Statement ;}
	Enter("Program");
	while (token.type!=NONTOKEN)
	{
		Statement();
		MatchToken(SEMICO, ";");
	}
	Back("Program");
}
void MyParser::Statement() { //Statement->OriginStatement|ScaleStatement|RotStatement|ForStatement
	Enter("Statement");
	switch (token.type) {
		case ORIGIN:OriginStatement(); break;
		case SCALE:ScaleStatement();break;
		case ROT:RotStatement();break;
		case FOR:ForStatement(); break;
		default: SyntaxError(2);	break;
	}
	Back("Statement");
}
void MyParser::OriginStatement() {
	//OriginStatment �� ORIGIN IS 
	//			L_BRACKET Expression COMMA Expression R_BRACKET
	Enter("OriginStatement");
	
	MatchToken(ORIGIN,"ORIGIN");
	MatchToken(IS,"IS");
	MatchToken(L_BRACKET, "(");
	XPtr = Expression();	TreeTrace(XPtr);	//��ӡ�﷨��������
	MatchToken(COMMA, ",");
	YPtr = Expression();	TreeTrace(YPtr);	
	MatchToken(R_BRACKET, ")");
		
	Back("OriginStatement");
}
void MyParser::ScaleStatement() {
	//ScaleStatment  �� SCALE IS 
    //       L_BRACKET Expression COMMA Expression R_BRACKET
	Enter("ScaleStatement");

	MatchToken(SCALE, "SCALE");
	MatchToken(IS, "IS");
	MatchToken(L_BRACKET, "(");
	XPtr = Expression();		TreeTrace(XPtr);
	MatchToken(COMMA, ",");
	YPtr = Expression();		TreeTrace(YPtr);
	MatchToken(R_BRACKET, ")");

	Back("ScaleStatement");
}
void MyParser::RotStatement() {//RotStatment �� ROT IS Expression
	Enter("RotStatement");
	
	MatchToken(ROT, "ROT");
	MatchToken(IS, "IS");
	AnglePtr = Expression();		TreeTrace(AnglePtr);
	
	Back("RotStatement");
}
void MyParser::ForStatement() {
	//ForStatement ->FOR T FROM Expression TO Expression STEP Expression
	//					DRAW (Expression,Expression)
	Enter("ForStatement");

	MatchToken(FOR, "FOR");
	MatchToken(T, "T");
	MatchToken(FROM, "FROM");
	StartPtr = Expression();		TreeTrace(StartPtr);
	MatchToken(TO, "TO");
	EndPtr = Expression();			TreeTrace(EndPtr);
	MatchToken(STEP, "STEP");
	StepPtr = Expression();			TreeTrace(StepPtr);
	MatchToken(DRAW, "DRAW");
	MatchToken(L_BRACKET, "(");
	XPtr = Expression();			TreeTrace(XPtr);
	MatchToken(COMMA, ",");			
	YPtr = Expression();			TreeTrace(YPtr);
	MatchToken(R_BRACKET, ")");

	Back("ForStatement");
}

TreeNode MyParser::Expression() {
	//Expression �� Term  { ( PLUS | MINUS) Term } 
	Enter("Expression");
	
	TreeNode left, right;
	Token_Type token_temp;	//��ǰ�Ǻ�����

	left = Term();	//����������﷨��
	while (token.type == PLUS || token.type == MINUS) //����
	{
		token_temp = token.type;
		MatchToken(token_temp);	//��һ��ƥ��
		right = Term();	//�Ҳ��������﷨��
		left = MakeTreeNode(token_temp, left, right);
				// ����������﷨�������Ϊ�����������ϣ���
	}
	
	Back("Expression");
	return left;
}
TreeNode MyParser::Term() {
	//Term �� Factor { ( MUL | DIV ) Factor }
	TreeNode left, right;
	Token_Type token_tmp;
	
	left = Factor();
	while (token.type == MUL || token.type == DIV)//����
	{
		token_tmp = token.type;
		MatchToken(token_tmp);
		right = Factor();
		left = MakeTreeNode(token_tmp, left, right);
	}
	return left;
}

TreeNode MyParser::Factor() {
	return TreeNode();
}

TreeNode MyParser::Component() {
	return TreeNode();
}

TreeNode MyParser::atom() {
	return TreeNode();
}

//�﷨���������
void MyParser::InitParser(char * FileName) {
	Enter("MyParser");	//	�����﷨��ڱ�־
	if (!Scanner.InitScanner(FileName)) {
		cout << "���ļ�ʧ��" << endl;
		return;
	}
	FetchToken();	//��ȡ��һ���Ǻ�
	Program();		//�ݹ������ʼ
	Scanner.CloseScanner();
	Back("MyParser");	//����
	return;
}

//�﷨���������Ը��� 
//�����Ӳ�
void MyParser::Enter(char * x) {
	for (int i = 0; i < indent; i++)	cout << " "; 
	cout << "EnterIn: " << x << endl;
	indent += 2;
}
//�˳��Ӳ�	
void MyParser::Back(char * x) {
	indent -= 2;
	for (int i = 0; i < indent; i++)	cout << " "; 
	cout << "BackFrom: " << x << endl;
}
//ƥ��һ�Ǻųɹ�
void MyParser::CallMatch(char * x) {
	for (int i = 0; i < indent; i++)	cout << " "; 
	cout << "MatchToken:" << x << endl;
}

void MyParser::TreeTrace(TreeNode x) {
}
