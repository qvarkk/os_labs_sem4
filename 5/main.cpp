/*------------------------------------------------------

������ ������ ������������� "�������������-�����������".
�������� ������.

--------------------------------------------------------*/

#include <windows.h>
#include <iostream>	 	 
#include "buffer.h"	 

#define cProducers 3	/*���������� ��������������*/	
#define cConsumers 5	/*���������� ������������*/	 
#define BufferSize 5	/*������ ������*/	 

int cOperations = 30; /*���������� �������� ��� �������*/

HANDLE hMutex; /* Global Mutex */
HANDLE hSemaphoreWr, hSemaphoreRd;

DWORD __stdcall getkey(void* b) {
	cin.get();

	/* ����������� ���������� ������� �������� �������� ����� �������������� ����������� */

	cout << cOperations << endl;
	return cOperations = 0;
}

/* �����-������������� */ 
DWORD __stdcall producer(void* b) {
	DWORD dwWaitResult;

	while (cOperations > 0) {

		dwWaitResult = WaitForSingleObject(hSemaphoreWr, 0);

		switch (dwWaitResult) {
		case WAIT_OBJECT_0:
			dwWaitResult = WaitForSingleObject(hMutex, INFINITE);

			int item;

			switch (dwWaitResult) {
			case WAIT_OBJECT_0:
				item = rand();
				((Buffer*)b)->PutItem(item);
				cout << "Put item: " << item << endl;
				cOperations--;
				ReleaseSemaphore(hSemaphoreRd, 1, NULL);
				Sleep(500 + rand() % 100);
				ReleaseMutex(hMutex);
				break;
			case WAIT_ABANDONED:
				return 0;
			}
			break;
		case WAIT_ABANDONED:
			return 0;
		}
	}

	return 0;
}

/* �����-����������� */ 
DWORD __stdcall consumer(void* b) {
	DWORD dwWaitResult;

	while (cOperations > 0) {
		
		dwWaitResult = WaitForSingleObject(hSemaphoreRd, 0);

		switch (dwWaitResult) {
		case WAIT_OBJECT_0:
			dwWaitResult = WaitForSingleObject(hMutex, INFINITE);

			switch (dwWaitResult) {
			case WAIT_OBJECT_0:
				cout << "Get item: " << ((Buffer*)b)->GetItem() << endl;
				// ((Buffer*)b)->GetItem();
				cOperations--;
				ReleaseSemaphore(hSemaphoreWr, 1, NULL);
				Sleep(500 + rand() % 100);
				ReleaseMutex(hMutex);
				break;
			case WAIT_ABANDONED:
				return 0;
			}
			break;
		case WAIT_ABANDONED:
			return 9;
		}
	}

	return 0;
}

int main() {
	srand((unsigned)time(NULL));

	Buffer* Buf = Buffer::CreateBuffer(BufferSize); /*�������� ������*/

	HANDLE hThreads[cProducers + cConsumers];

	/* ��������������� �����, ��������� ������� ������� */
	CreateThread(0, 0, getkey, 0, 0, 0);

	/* �������� �������� */
	hMutex = CreateMutexW(NULL, FALSE, NULL);
	hSemaphoreRd = CreateSemaphoreW(NULL, 0, BufferSize, NULL);
	hSemaphoreWr = CreateSemaphoreW(NULL, BufferSize, BufferSize, NULL);

	if (hMutex == NULL || hSemaphoreRd == NULL || hSemaphoreWr == NULL)
		return 1;

	/* �������� �������-�������������� */
	for (int i = 0; i < cProducers; i++) {
		hThreads[i] = CreateThread(0, 0, producer, Buf, 0, 0);

		if (hThreads[i] == NULL)
			return 1;
	}

	/* �������� �������-������������ */
	for (int i = cProducers; i < cProducers + cConsumers; i++) {
		hThreads[i] = CreateThread(0, 0, consumer, Buf, 0, 0);

		if (hThreads[i] == NULL)
			return 1;
	}

	WaitForMultipleObjects(cProducers + cConsumers, hThreads, true, INFINITE);

	/* �������� ���� �������� ������������ */
	for (int i = 0; i < cProducers + cConsumers; i++)
		CloseHandle(hThreads[i]);
	CloseHandle(hMutex);

	cout << "Threads finished" << endl;
	cin.get();

	return 0;
}

