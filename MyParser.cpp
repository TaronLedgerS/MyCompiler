#include "MyParser.h"

MyParser::MyParser() {
	TPara = 0;
	StartPtr = NULL;
	EndPtr = NULL;
	StepPtr = NULL;
	XPtr = NULL;
	YPtr = NULL;
	indent = 0;
}

MyParser::~MyParser() {}
//ͨ��MyScanner��GetToken()��ȡһ���Ǻ�
void MyParser::FetchToken() {
	token = Scanner.GetToken();
	if (token.type == ERRTOKEN) SyntaxError(1);//�Ƿ��Ǻ�
}

// �Ǻ�ƥ�� ���� 1. ���ʽ���������ƥ�� 2.�����֣��ؼ��֣�ƥ�䲢����ʱ��ӡ��ǰƥ��Ǻ�
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

//�����﷨���ڵ� �������
TreeNode MyParser::MakeTreeNode(Token_Type opcode, ...) {
	//��ʼ���ڵ�
	TreeNode TP = NULL;
	TP = new TreeNodeStruct;	
	TP->OpCode = opcode;
	va_list arg_ptr;//����б�(ָ��)
	va_start(arg_ptr, opcode);

	switch (opcode) {	//����ڵ�
		case CONST_ID:	//����
			TP->content.CaseConst = (double)va_arg(arg_ptr, double);
			break;
		case T:		//����T
			TP->content.CasePara = &TPara;	//TPara������MyParser
			break;
		case FUNC:	//����
			TP->content.CaseFunc.mathfuncptr = (func_ptr)va_arg(arg_ptr, func_ptr);
			TP->content.CaseFunc.child = (TreeNode)va_arg(arg_ptr, TreeNode);
			break;
		default:	//��Ԫ����
			TP->content.CaseOp.left = (TreeNode)va_arg(arg_ptr, TreeNode);
			TP->content.CaseOp.right = (TreeNode)va_arg(arg_ptr, TreeNode);
			break;
	}

	va_end(arg_ptr);
	return TP;
}
//����EBNF���õĵݹ��ӳ���
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
//���ʽ���﷨���ĵݹ鹹��
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
	//Factor  �� ( PLUS | MINUS ) Factor | Component
	TreeNode left, right;
	if (token.type == PLUS)	//һԪ�ӣ������Ҳ�����
	{
		MatchToken(PLUS);
		right = Factor();
	}
	else
		if (token.type == MINUS)	//һԪ��-->��Ԫ0-factor
		{
			MatchToken(MINUS);
			right = Factor();
			left = new TreeNodeStruct;
			left->OpCode = CONST_ID;
			left->content.CaseConst = 0.0;
			right = MakeTreeNode(MINUS, left, right);
		}
		else
			right = Component();
	return right;//�ҽ��
}
TreeNode MyParser::Component() {
	//Component �� Atom [ POWER Component ] //[]�ɱ��ƹ���·��
	TreeNode left, right;
	left = Atom();
	if (token.type == POWER)//�ҽ��
	{
		MatchToken(POWER);
		right = Component();	//�ݹ������ʵ��Power���ҽ���ԣ���
		left = MakeTreeNode(POWER, left, right);
	}
	return left;
}
TreeNode MyParser::Atom() {
//	Atom �� CONST_ID
//      | T
//      | FUNC L_BRACKET Expression R_BRACKET
//      | L_BRACKET Expression R_BRACKET 

	Token t = token;
	TreeNode TP = NULL, tmp;
	switch (token.type) {
		case  CONST_ID:
			MatchToken(CONST_ID); 	TP = MakeTreeNode(CONST_ID, t.value);
			break;
		case T:
			MatchToken(T);			TP = MakeTreeNode(T);
			break;
		case FUNC:
			MatchToken(FUNC);
			MatchToken(L_BRACKET);
			tmp = Expression();		TP = MakeTreeNode(FUNC, t.func_ptr, tmp);
			MatchToken(R_BRACKET);
			break;
		case L_BRACKET:
			MatchToken(L_BRACKET);
			TP = Expression();
			MatchToken(R_BRACKET);
			break;
		default:
			SyntaxError(2);
			break;
	}
	return TP;
}


//�﷨����������
//���������ӡ���ʽ���﷨�� �������
void MyParser::PrintSyntaxTree(TreeNode root, int indent) {//�˴�indentΪ�������
	for (int i = 1; i <= this->indent + indent; i++) cout << ' ';//����
	switch (root->OpCode) {
		case PLUS:	cout << "+" << endl; break;
		case MINUS: cout << "-" << endl; break;
		case MUL:	cout << "*" << endl; break;
		case DIV:	cout << "/" << endl; break;
		case POWER:	cout << "**" << endl; break;
		case T:		cout << "T" << endl; break;
		case CONST_ID:	cout << root->content.CaseConst << endl; break;
		case FUNC:	cout << (void*)(root->content.CaseFunc.mathfuncptr) << endl; break;
		default:	cout << "�Ƿ������ڵ㣡" << endl; exit(0);//TODO ������
	}
	if (root->OpCode == CONST_ID || root->OpCode == T) return; //Ҷ�ӽڵ�
	if (root->OpCode == FUNC) //�����ݹ��ӡ�ӽڵ�
		PrintSyntaxTree(root->content.CaseFunc.child, indent + 2);
	else {
		PrintSyntaxTree(root->content.CaseOp.left, indent + 2);
		PrintSyntaxTree(root->content.CaseOp.right, indent + 2);
	}
}
void MyParser::Enter(char * x) {
	for (int i = 0; i < indent; i++)	cout << " "; 
	cout << "EnterIn: " << x << endl;
	indent += 2;
}
void MyParser::Back(char * x) {
	indent -= 2;
	for (int i = 0; i < indent; i++)	cout << " "; 
	cout << "BackFrom: " << x << endl;
}
void MyParser::CallMatch(char * x) {
	for (int i = 0; i < indent; i++)	cout << " "; 
	cout << "MatchToken:" << x << endl;
}
void MyParser::TreeTrace(TreeNode x) { //��ӡ���ʽ�﷨��
	PrintSyntaxTree(x, 0);
}
