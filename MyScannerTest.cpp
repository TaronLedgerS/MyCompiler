/*#include "MyScanner.h"
#include <iomanip>
#include <windows.h>

	//	���Դʷ������� DONE 2016-12-09 10:48:33


int main(int argc, char *argv[]) {
	MyScanner Scanner;
	Token token;
	if (argc < 2) {
		cout << "������Դ�ļ���\n";
		return 1;
	}
	if (!Scanner.InitScanner(argv[1])) {
		cout << "��Դ�ļ�ʧ��" << endl;
		return 2;
	}
	cout << "�Ǻ����     ԭʼ����     ֵ           ������ַ" << endl;
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