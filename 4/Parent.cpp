#include <Windows.h>
#include <iostream>

#define BUFSIZE 4096

int main(int argc, char** argv) {
	HANDLE hChildWr = NULL;
	HANDLE hChildRd = NULL;

	SECURITY_ATTRIBUTES sa;
	
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = NULL;

	if (!CreatePipe(&hChildRd, &hChildWr, &sa, NULL))
		std::cout << "Failed creating pipe" << std::endl;

	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	ZeroMemory(&pi, sizeof(pi));

	si.cb = sizeof(STARTUPINFO);
	si.hStdInput = hChildRd;
	si.dwFlags |= STARTF_USESTDHANDLES;

	std::wstring CommandLine(L"C:\\Users\\qvarkk\\Desktop\\laba4_os_final\\Child\\x64\\Debug\\Child.exe");
	//std::wstring CommandLine(L"notepad.exe");
	LPWSTR lpwCmdlLine = &CommandLine[0];

	if (!CreateProcess(NULL, lpwCmdlLine, NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi))
		std::cout << "Failed creating process" << std::endl;

	DWORD dwWritten;
	CHAR wrBuf[BUFSIZE] = "this string has 30 characters.";

	if (!WriteFile(hChildWr, wrBuf, (DWORD)sizeof(wrBuf), &dwWritten, NULL))
		std::cout << "Failed writing to pipe" << std::endl;

	if (!CloseHandle(hChildWr))
		std::cout << "Failed closing stdin wr handle" << std::endl;

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	return 0;
}
