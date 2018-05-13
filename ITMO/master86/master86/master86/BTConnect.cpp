#pragma once
#include "BTConnect.h"
#include <atlbase.h>


btSender::btSender(string name) {
	comPortName = name;
	hCom = getBluetoothHandle(name);
	valid = (hCom != INVALID_HANDLE_VALUE);
}

btSender::~btSender()
{
	Disconnect();
}

void btSender::Disconnect() {
	if (valid && hCom != INVALID_HANDLE_VALUE) {
		CloseHandle(hCom);
		hCom = INVALID_HANDLE_VALUE;
	}
}

bool btSender::IsItConnected() {
	return valid;
}

std::string  btSender::GetComPortName () {
	return comPortName;
}

std::string btSender::GetErrorMessage() {
	return GetErrorMessage(errID);
}

//Returns the last Win32 error, in string format. Returns an empty string if there is no error.
std::string btSender::GetErrorMessage(DWORD errorMessageID) {
	//Get the error message, if any.

	if(errorMessageID == 0)
		return std::string(); //No error message has been recorded

	LPSTR messageBuffer = nullptr;
	size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, 
		errorMessageID, 
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
		(LPSTR)&messageBuffer, 
		0, 
		NULL);

	std::string message(messageBuffer, size);

	//Free the buffer.
	LocalFree(messageBuffer);

	return message;
}

DWORD btSender::GetErrorID() {
	return errID;
}

HANDLE btSender::getBluetoothHandle(string comPort) {
	HANDLE hCom;
	OVERLAPPED o;
	BOOL fSuccess;
	comPort = "\\\\.\\" + comPort;
	CA2W comport_temp(comPort.c_str());
	hCom = CreateFile(comport_temp,
		GENERIC_READ | GENERIC_WRITE,
		0,    // comm devices must be opened w/exclusive-access
		NULL, // no security attributes
		OPEN_EXISTING, // comm devices must use OPEN_EXISTING
		0,    // overlapped I/O
		NULL  // hTemplate must be NULL for comm devices
		);

	if (hCom == INVALID_HANDLE_VALUE) {
		errID = GetLastError();
		return hCom;
	}
	SetCommMask(hCom, EV_CTS | EV_DSR);

	o.hEvent = CreateEvent(
		NULL,   // default security attributes
		TRUE,   // manual-reset event
		FALSE,  // not signaled
		NULL    // no name
		);

	o.Internal = 0;
	o.InternalHigh = 0;
	o.Offset = 0;
	o.OffsetHigh = 0;

	//assert(o.hEvent);

	COMMTIMEOUTS CommTimeOuts;
	CommTimeOuts.ReadIntervalTimeout = 3;
	CommTimeOuts.ReadTotalTimeoutMultiplier = 1;
	CommTimeOuts.ReadTotalTimeoutConstant = 2;
	CommTimeOuts.WriteTotalTimeoutMultiplier = 0;
	CommTimeOuts.WriteTotalTimeoutConstant = 0;

	SetCommTimeouts(hCom, &CommTimeOuts);

	return hCom;
}

bool btSender::Send(short* bytesToSend, int size) {
	BOOL bErrorFlag = FALSE;
	byte b[100] = { 0x00, 0x00, 0x00, 0x09, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00 };

	b[0] = size * sizeof(short) + 4;

	DWORD bc = 0;

	for (int i = 0; i < size; i++)
	{
		for (int k = 0; k < 2; k++)
		{
			b[6 + k + i * sizeof(short)] = (byte) bytesToSend[i];
			bytesToSend[i]>>=8;
		}
	}
	if (hCom == INVALID_HANDLE_VALUE) return false;
	while (bc == 0)
	{
		bErrorFlag = WriteFile(hCom, b, b[0] + 2 , &bc, NULL);
		if (bErrorFlag == FALSE) {
			errID = GetLastError();
			return false;
		}
		Sleep(1);
	}
	return true;
}