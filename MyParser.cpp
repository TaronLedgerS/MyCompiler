#include "MyParser.h"

MyParser::MyParser() {
}

MyParser::~MyParser() {
}
//通过MyScanner的GetToken()获取一个记号
void MyParser::FetchToken() {
	token = Scanner.GetToken();
	if (token.type == ERRTOKEN) SyntaxError(1);//非法记号
}

// 记号匹配 重载 1. 表达式（运算符）匹配 2.保留字（关键字）匹配并测试时打印匹配结果
void MyParser::MatchToken(Token_Type ttoken) {
	if (token.type != ttoken) SyntaxError(2);//当前记号不是预期记号
	FetchToken();	//匹配成功，获取下一个记号
}
void MyParser::MatchToken(Token_Type ttoken, char * text) {
	if (token.type != ttoken) SyntaxError(2);//当前记号不是预期记号
	if (text != NULL)CallMatch(text); //测试 匹配成功
	FetchToken();	//匹配成功，获取下一个记号
}

//语法错误处理2类TODO 调用ErrorMsg进行报错
void MyParser::SyntaxError(int case_of) {
	switch (case_of) {
		case 1:
			ErrorMsg(Scanner.GetLineNo(),token.lexeme, "为非法记号");
			break;
		case 2:
			ErrorMsg(Scanner.GetLineNo(), token.lexeme, "不是预期记号");
			break;
	}
}
void MyParser::ErrorMsg(int line, char * sourcetext, char * descrip) {
	cout << "Line No." << line << ": " << sourcetext << descrip << endl;
	Scanner.CloseScanner();
	exit(1);//TODO 出错后不退出界面
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
	//OriginStatment → ORIGIN IS 
	//			L_BRACKET Expression COMMA Expression R_BRACKET
	Enter("OriginStatement");
	
	MatchToken(ORIGIN,"ORIGIN");
	MatchToken(IS,"IS");
	MatchToken(L_BRACKET, "(");
	XPtr = Expression();	TreeTrace(XPtr);	//打印语法分析树；
	MatchToken(COMMA, ",");
	YPtr = Expression();	TreeTrace(YPtr);	
	MatchToken(R_BRACKET, ")");
		
	Back("OriginStatement");
}
void MyParser::ScaleStatement() {
	//ScaleStatment  → SCALE IS 
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
void MyParser::RotStatement() {//RotStatment → ROT IS Expression
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
	//Expression → Term  { ( PLUS | MINUS) Term } 
	Enter("Expression");
	
	TreeNode left, right;
	Token_Type token_temp;	//当前记号类型

	left = Term();	//左操作数的语法树
	while (token.type == PLUS || token.type == MINUS) //左结合
	{
		token_temp = token.type;
		MatchToken(token_temp);	//第一类匹配
		right = Term();	//右操作数的语法树
		left = MakeTreeNode(token_temp, left, right);
				// 构造运算的语法树，结果为左子树，左结合？？
	}
	
	Back("Expression");
	return left;
}
TreeNode MyParser::Term() {
	//Term → Factor { ( MUL | DIV ) Factor }
	TreeNode left, right;
	Token_Type token_tmp;
	
	left = Factor();
	while (token.type == MUL || token.type == DIV)//左结合
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

//语法分析器入口
void MyParser::InitParser(char * FileName) {
	Enter("MyParser");	//	调试语法入口标志
	if (!Scanner.InitScanner(FileName)) {
		cout << "打开文件失败" << endl;
		return;
	}
	FetchToken();	//获取第一个记号
	Program();		//递归分析开始
	Scanner.CloseScanner();
	Back("MyParser");	//调试
	return;
}

//语法分析器测试跟踪 
//进入子层
void MyParser::Enter(char * x) {
	for (int i = 0; i < indent; i++)	cout << " "; 
	cout << "EnterIn: " << x << endl;
	indent += 2;
}
//退出子层	
void MyParser::Back(char * x) {
	indent -= 2;
	for (int i = 0; i < indent; i++)	cout << " "; 
	cout << "BackFrom: " << x << endl;
}
//匹配一记号成功
void MyParser::CallMatch(char * x) {
	for (int i = 0; i < indent; i++)	cout << " "; 
	cout << "MatchToken:" << x << endl;
}

void MyParser::TreeTrace(TreeNode x) {
}
