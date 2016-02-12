
#include <iostream>
#include <windows.h>

#include "Bee.h"

using namespace std;
using namespace Bee;

void Bee::osError()
{
	const int buffer_length = 256;
	char buffer[buffer_length];

	ulong code = GetLastError();
	getOsErrorMessage(code, buffer, buffer_length);
	cout << "Error " << code << " " << buffer << endl;
	ExitProcess(-1);
}


void Bee::getOsErrorMessage(ulong code, char buffer[], int buffer_length)
{
	int count = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, code, 0, buffer, buffer_length, 0);

}