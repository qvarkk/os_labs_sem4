#include <Windows.h>
#include <iostream>
#include <string>

int DoCreateProcess();

int main(int argc, char** argv) {
	if (!DoCreateProcess()) {
		std::cout << "Was unable to create process" << std::endl;
		return 1;
	}

	return 0;
}

int DoCreateProcess() {
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	ZeroMemory(&pi, sizeof(pi));

	std::wstring CommandLine(L"notepad.exe");
	LPWSTR lpwCmdlLine = &CommandLine[0];

	BOOL b = NULL;
	b = CreateProcessW(NULL, lpwCmdlLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);

	if (b) {
		std::cout << "Process created" << std::endl;
		std::cout << "Process Id: " << GetProcessId(pi.hProcess) << std::endl;

		HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pi.dwProcessId);

		std::cout << "Process handle opened" << std::endl;

		FILETIME creationTime, exitTime, kernelTime, userTime;
		SYSTEMTIME sCreationTime, sExitTime, sKernelTime, sUserTime;

		GetProcessTimes(processHandle, &creationTime, &exitTime, &kernelTime, &userTime);

		std::cout << "Got process times (1/4)" << std::endl;

		FileTimeToSystemTime(&creationTime, &sCreationTime);

		std::cout << "Creation time: " << sCreationTime.wHour << ":" << sCreationTime.wMinute << ":" << sCreationTime.wSecond << std::endl;
		std::cout << "Close notepad when ready" << std::endl;

		WaitForSingleObject(processHandle, INFINITE);

		GetProcessTimes(processHandle, &creationTime, &exitTime, &kernelTime, &userTime);

		std::cout << "Got process times (4/4)" << std::endl;

		FileTimeToSystemTime(&exitTime, &sExitTime);

		std::cout << "Exit time: " << sExitTime.wHour << ":" << sExitTime.wMinute << ":" << sExitTime.wSecond << std::endl;
		std::cout << "Kernel time: " << kernelTime.dwLowDateTime / 100000.0 << " crocodiles" << std::endl;
		std::cout << "User time: " << userTime.dwLowDateTime / 100000.0 << " crocodiles" << std::endl;

		std::cout << "Press any key to terminate process and continue..." << std::endl;

		system("pause > nul");

		TerminateProcess(processHandle, 1);
		CloseHandle(processHandle);
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	} else {
		return 0;
	}

	return 1;
}