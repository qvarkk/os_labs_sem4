#include <Windows.h>
#include <iostream>

#define BUFSIZE 4096

int main(int argc, char** argv) {
	HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);

	CHAR buffer[BUFSIZE];
	DWORD dwRead;
	BOOL bSuccess = FALSE;

	if (!ReadFile(hStdin, buffer, BUFSIZE, &dwRead, NULL))
		return -1;
	
	int i;
	for (i = 0; buffer[i] != '\0'; i++);
	std::cout << i << std::endl;

	system("pause");

	return 0;
}