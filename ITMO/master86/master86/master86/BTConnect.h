#pragma once
#include <windows.h>
#include <string>
using namespace std;
class btSender {
	HANDLE hCom;
	bool valid;
	string comPortName;
	DWORD errID;
	HANDLE getBluetoothHandle(string comport);
public:
	btSender(string name);
	void Disconnect();
	bool Send(short* bytesToSend, int size);
	bool IsItConnected();
	string GetComPortName();
	DWORD GetErrorID();
	string GetErrorMessage(DWORD errorMessageID);
	string GetErrorMessage();
	~btSender();
};