#pragma once
#include <windows.h>
#include <string>
using namespace std;
class bt_out {
	HANDLE h_com;
	bool valid;
	HANDLE get_bluetooth_handle(string comport);
public:
	bt_out();
	bt_out(string a);
	void bt_send(int* bytesToSend, int size);
	~bt_out();
};