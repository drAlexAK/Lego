#pragma once
#include "BT_Connect.h"
#include <atlbase.h>

bt_out::bt_out()
{
	valid = false;
}

bt_out::bt_out(string a) {
	valid = false;
	h_com = get_bluetooth_handle(a);
	if (h_com != INVALID_HANDLE_VALUE)
		valid = true;
	else
		printf("serial handle invalid\n");
}

bt_out::~bt_out()
{
	if (valid && h_com != INVALID_HANDLE_VALUE)
		CloseHandle(h_com);
}

HANDLE bt_out::get_bluetooth_handle(string comport) {
	HANDLE hCom;
	OVERLAPPED o;
	BOOL fSuccess;
	DWORD dwEvtMask;
	comport = "\\\\.\\" + comport;
	CA2W comport_temp(comport.c_str());
	hCom = CreateFile(comport_temp,
		GENERIC_READ | GENERIC_WRITE,
		0,    // comm devices must be opened w/exclusive-access
		NULL, // no security attributes
		OPEN_EXISTING, // comm devices must use OPEN_EXISTING
		0,    // overlapped I/O
		NULL  // hTemplate must be NULL for comm devices
	);

	fSuccess = SetCommMask(hCom, EV_CTS | EV_DSR);

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

void bt_out::bt_send(int* bytesToSend, int size) {
	byte bluetooth_buffer_[100];

	bluetooth_buffer_[0] = (0x07); bluetooth_buffer_[1] = (0x00); bluetooth_buffer_[2] = (0x80);	bluetooth_buffer_[3] = (0x09);
	bluetooth_buffer_[4] = (0x00); bluetooth_buffer_[5] = (0x03); bluetooth_buffer_[6] = 0; bluetooth_buffer_[7] = 0; bluetooth_buffer_[8] = 0;

	DWORD bc = 0;
	bluetooth_buffer_[0] = size + 4;
	bluetooth_buffer_[1] = 0;
	bluetooth_buffer_[5] = size;
	for (int i = 0; i < size; i++)
	{
		bluetooth_buffer_[i + 6] = bytesToSend[i];
	}
	if (h_com == INVALID_HANDLE_VALUE) return;
	while (bc == 0)
	{
		WriteFile(h_com, bluetooth_buffer_, size + 6, &bc, NULL);
		Sleep(1);
	}
}
