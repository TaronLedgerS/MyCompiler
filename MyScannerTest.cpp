/*#include "MyScanner.h"
#include <iomanip>
#include <windows.h>

	//	测试词法分析器 DONE 2016-12-09 10:48:33


int main(int argc, char *argv[]) {
	MyScanner Scanner;
	Token token;
	if (argc < 2) {
		cout << "请输入源文件！\n";
		return 1;
	}
	if (!Scanner.InitScanner(argv[1])) {
		cout << "打开源文件失败" << endl;
		return 2;
	}
	cout << "记号类别     原始输入     值           函数地址" << endl;
	cout << "-----------------------------------------------" << endl;
	while (1) {
		token = Scanner.GetToken();
		if (token.type != NONTOKEN) {
			cout << left << setw(13) << token.type;
			cout << left << setw(13) << token.lexeme;
			cout << setprecision(2) << left << setw(13) << token.value;
			cout << left << setw(13) << token.func_ptr << endl;
		}
		else break;
	}
	cout << "-----------------------------------------------" << endl;
	
	Scanner.CloseScanner();
}*/