#include <iostream>
#include <windows.h>

using namespace std;

long negLengthTab[] = { -1, -2, -1, -3, -1, -2, -1, -4,-1, -2, -1, -3,-1, -2, -1, -5 };

long shiftTab[] = {
	32 - 7 * 1, 32 - 7 * 2, 32 - 7 * 1, 32 - 7 * 3,
	32 - 7 * 1, 32 - 7 * 2, 32 - 7 * 1, 32 - 7 * 4,
	32 - 7 * 1, 32 - 7 * 2, 32 - 7 * 1, 32 - 7 * 3,
	32 - 7 * 1, 32 - 7 * 2, 32 - 7 * 1, 0
};

DWORD DecodeCxxValue(BYTE* values, DWORD* size) {
	long lengthBits = *values & 0xf;
	long negLength = negLengthTab[lengthBits];
	long shift = shiftTab[lengthBits];
	long readPos = (-negLength) - 4;

	*size += ((~(negLength)) + 1);

	return *(DWORD*)(values + readPos) >> shift;
}

void EncodeCxxValue(BYTE* output, DWORD* size, DWORD data) {
	if (data < 128) {
		*output = data * 2;
		*size += 1;
	} else if (data < 16384) {
		BYTE modulo = data % 4;
		BYTE lownib = 1 + modulo * 4;
		DWORD highdata = data - modulo;
		*(WORD*)output = highdata * 4 | lownib;
		*size += 2;
	} else if (data < 2097152) {
		BYTE modulo = data % 2;
		BYTE lownib = 3 + modulo * 8;
		DWORD highdata = data - modulo;
		DWORD encoded = highdata * 8 | lownib;
		memcpy(output, &encoded, 3);
		*size += 3;
	} else if (data < 268435456) {
		BYTE lownib = 7;
		DWORD encoded = data * 16 | lownib;
		*(DWORD*)output = encoded;
		*size += 4;
	} else { //Up to DWORD max
		BYTE lownib = 15;
		ULONG64 encoded = data * 256ULL | lownib;
		memcpy(output, &encoded, 5);
		*size += 5;
	}
}

int main()
{
	DWORD size = 0; 
	ULONG64 d = 69;
	ULONG64 o = 0;
	EncodeCxxValue((BYTE*)&o, &size, d);
	cout << DecodeCxxValue((BYTE*)&o, &size) << endl;

	//for (ULONG64 d = 0; d < 0xffffffffff; d += 16) {
	//	cout << DecodeCxxValue((BYTE*)&d, &size) << endl;
	//}
}