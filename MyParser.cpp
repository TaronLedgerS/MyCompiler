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
//通过MyScanner的GetToken()获取一个记号
void MyParser::FetchToken() {
	token = Scanner.GetToken();
	if (token.type == ERRTOKEN) SyntaxError(1);//非法记号
}

// 记号匹配 重载 1. 表达式（运算符）匹配 2.保留字（关键字）匹配并测试时打印当前匹配记号
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

//生成语法树节点 四类情况
TreeNode MyParser::MakeTreeNode(Token_Type opcode, ...) {
	//初始化节点
	TreeNode TP = NULL;
	TP = new TreeNodeStruct;	
	TP->OpCode = opcode;
	va_list arg_ptr;//变参列表(指针)
	va_start(arg_ptr, opcode);

	switch (opcode) {	//四类节点
		case CONST_ID:	//常量
			TP->content.CaseConst = (double)va_arg(arg_ptr, double);
			break;
		case T:		//参数T
			TP->content.CasePara = &TPara;	//TPara作用域MyParser
			break;
		case FUNC:	//函数
			TP->content.CaseFunc.mathfuncptr = (func_ptr)va_arg(arg_ptr, func_ptr);
			TP->content.CaseFunc.child = (TreeNode)va_arg(arg_ptr, TreeNode);
			break;
		default:	//二元运算
			TP->content.CaseOp.left = (TreeNode)va_arg(arg_ptr, TreeNode);
			TP->content.CaseOp.right = (TreeNode)va_arg(arg_ptr, TreeNode);
			break;
	}

	va_end(arg_ptr);
	return TP;
}
//根据EBNF所得的递归子程序
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
//表达式的语法树的递归构造
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
	//Factor  → ( PLUS | MINUS ) Factor | Component
	TreeNode left, right;
	if (token.type == PLUS)	//一元加，仅有右操作数
	{
		MatchToken(PLUS);
		right = Factor();
	}
	else
		if (token.type == MINUS)	//一元减-->二元0-factor
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
	return right;//右结合
}
TreeNode MyParser::Component() {
	//Component → Atom [ POWER Component ] //[]可被绕过的路径
	TreeNode left, right;
	left = Atom();
	if (token.type == POWER)//右结合
	{
		MatchToken(POWER);
		right = Component();	//递归调用以实现Power的右结合性！！
		left = MakeTreeNode(POWER, left, right);
	}
	return left;
}
TreeNode MyParser::Atom() {
//	Atom → CONST_ID
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


//语法分析器测试
//先序遍历打印表达式的语法树 四类情况
void MyParser::PrintSyntaxTree(TreeNode root, int indent) {//此处indent为相对缩进
	for (int i = 1; i <= this->indent + indent; i++) cout << ' ';//缩进
	switch (root->OpCode) {
		case PLUS:	cout << "+" << endl; break;
		case MINUS: cout << "-" << endl; break;
		case MUL:	cout << "*" << endl; break;
		case DIV:	cout << "/" << endl; break;
		case POWER:	cout << "**" << endl; break;
		case T:		cout << "T" << endl; break;
		case CONST_ID:	cout << root->content.CaseConst << endl; break;
		case FUNC:	cout << (void*)(root->content.CaseFunc.mathfuncptr) << endl; break;
		default:	cout << "非法的树节点！" << endl; exit(0);//TODO 报错窗口
	}
	if (root->OpCode == CONST_ID || root->OpCode == T) return; //叶子节点
	if (root->OpCode == FUNC) //继续递归打印子节点
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
void MyParser::TreeTrace(TreeNode x) { //打印表达式语法树
	PrintSyntaxTree(x, 0);
}
