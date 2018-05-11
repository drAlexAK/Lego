#pragma once
#include <windows.h>
#include <string>
using namespace std;
class bt_out {
	HANDLE h_com;
	bool valid;
	string comPortName;
	DWORD errID;
	HANDLE get_bluetooth_handle(string comport);
public:
	bt_out();
	bt_out(string name);
	bool Send(short* bytesToSend, int size);
	bool IsItConnected();
	string GetComPortName();
	DWORD GetErrorID();
	string GetErrorMessage(DWORD errorMessageID);
	string GetErrorMessage();
	~bt_out();
};