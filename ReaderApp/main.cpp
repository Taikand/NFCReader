// ConsoleApplication28.cpp : Defines the entry point for the console application.
//

#include "pch.h"

#include <iostream>
#include <winscard.h>

#pragma comment(lib,"WINSCard.lib")

using namespace std;



int main(void)
{
	SCARDCONTEXT context;
	LONG ret;
	ret=SCardEstablishContext(SCARD_SCOPE_USER, NULL, NULL, &context);
	if (ret != SCARD_S_SUCCESS)
	{
		cout << "Error establishing context.";
		return -1;
	}

	LPWSTR readerList;
	DWORD bufferSize = SCARD_AUTOALLOCATE;
	ret = SCardListReaders(context, NULL, (LPWSTR)&readerList, &bufferSize);

	if (ret != SCARD_S_SUCCESS)
	{
		cout << "Error getting readers.";
		return -1;
	}
	return 0;
}
