#include <cstdlib>
#include <iostream>
#include <fstream>

#include "BT_Connect.h"

using namespace std;

int main()
{
	int bytesToSend_1[100];
	int bytesToSend_2[100];

	cout << "Input COM for Robot 1: ";
	string comport1;
	cin >> comport1;
	bt_out robot1(comport1);

	cout << "Input COM for Robot 2: ";
	string comport2;
	cin >> comport2;
	bt_out robot2(comport2);

	bytesToSend_1[0] = 5; //cond
	bytesToSend_1[1] = 0; //finish

	bytesToSend_2[0] = 5; //cond
	bytesToSend_2[1] = 0; //finish

	cout << "READY" << endl;

	while (!GetAsyncKeyState(VK_SPACE)) {};
	bytesToSend_1[1] = 1;
	bytesToSend_2[1] = 1;


	while (!GetAsyncKeyState(VK_ESCAPE))
	{
		if (GetAsyncKeyState('Q'))
		{
			cout << "+ robot 1" << endl;
			bytesToSend_1[0] += 1;
			while (GetAsyncKeyState('Q')) {};
		}
		if (GetAsyncKeyState('S'))
		{
			cout << "- robot 1" << endl;
			bytesToSend_1[0] -= 1;
			while (GetAsyncKeyState('S')) {};
		}
		if (GetAsyncKeyState('E'))
		{
			bytesToSend_1[1] = 2;
			while (GetAsyncKeyState('E')) {};
		}

		if (GetAsyncKeyState('I'))
		{
			cout << "+ robot 2" << endl;
			bytesToSend_2[0] += 1;
			while (GetAsyncKeyState('I')) {};
		}
		if (GetAsyncKeyState('L'))
		{
			cout << "- robot 2" << endl;
			bytesToSend_2[0] -= 1;
			while (GetAsyncKeyState('L')) {};
		}
		if (GetAsyncKeyState('P'))
		{
			bytesToSend_2[1] = 2;
			while (GetAsyncKeyState('P')) {};
		}

		robot1.bt_send(bytesToSend_1, 2);
		robot2.bt_send(bytesToSend_2, 2);
	}

	return 0;
}