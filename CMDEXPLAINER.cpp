#include "pch.h"
#include "CMDEXPLAINER.h"
#include<direct.h>
#include <stdio.h>
#include<iostream>
#include<fstream>
#include <Winsock2.h>
#include<WS2tcpip.h>
#include<Windows.h>
#pragma comment(lib, "WS2_32")  // 链接到WS2_32.lib


std::string execute(string cmd)
{
    // 创建匿名管道
    SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
    HANDLE hRead, hWrite;
    if (!CreatePipe(&hRead, &hWrite, &sa, 0))
    {
        return "";
    }

    LPCSTR pszSrc = cmd.c_str();
    int nLen = MultiByteToWideChar(CP_ACP, 0, cmd.c_str(), -1, NULL, 0);
    if (nLen == 0)
        return ("");

    wchar_t* pwszDst = new wchar_t[nLen];
    if (!pwszDst)
        return ("");

    MultiByteToWideChar(CP_ACP, 0, pszSrc, -1, pwszDst, nLen);
    std::wstring pszCmd(pwszDst);
    delete[] pwszDst;
    pwszDst = NULL;


    // 设置命令行进程启动信息(以隐藏方式启动命令并定位其输出到hWrite
    STARTUPINFO si = { sizeof(STARTUPINFO) };
    GetStartupInfo(&si);
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    si.wShowWindow = SW_HIDE;
    si.hStdError = hWrite;
    si.hStdOutput = hWrite;
    LPCVOID buffer = " ";
    DWORD dwWrite;
    WriteFile(hWrite, buffer, strlen((const char*)buffer), &dwWrite, NULL);
    // 启动命令行
    PROCESS_INFORMATION pi;
    if (!CreateProcess(NULL, (LPWSTR)pszCmd.c_str(), NULL, NULL, TRUE, NULL, NULL, NULL, &si, &pi))
    {
        return ("Cannot create process");
    }

    // 立即关闭hWrite
    CloseHandle(hWrite);
    //wait for process to exit

    WaitForSingleObject(pi.hProcess, 5000);

    // 读取命令行返回值
    std::string strRetTmp;
    char buff[1024] = { 0 };
    DWORD dwRead = 0;
    //memset(buff, sizeof(buff), 0);
    strRetTmp = "";
    int count = 0;
    while (ReadFile(hRead, buff, 1023, &dwRead, NULL))
    {
        
        cout << "*" << buff << endl;
        strRetTmp.append(buff);

        if (strlen(buff) < 1000&&count==0)break;
        memset(buff, 0, sizeof(buff));
        count++;
    }
    
    //strRetTmp.append(buff);
    CloseHandle(hRead);
    cout << "result before trans to wide string*" << strRetTmp << "*\n";
    
    return strRetTmp.substr(1,strRetTmp.length()-1);
}

void process_cd(string cmd)
{
    //分割字符串
    vector<string> res = split(cmd);
    //判断是否是cd指令
    if (res.size() == 0) {
        return;
    }
    if (res[0]._Equal("cd") == 0)return;
    //获取路径参数
    string path = "";
    if (res.size() >= 2) {
        path = res[1];
    }
    //转换进程路径
    _chdir(path.c_str());
}

std::vector<string> split(string str)
{
    vector<string> res;
    //移除两边的空白
    size_t begin = str.find_first_not_of(' ', 0);
    size_t end = str.find_last_not_of(' ', str.length());
    cout << begin << " " << end << endl;
    if (begin == string::npos) {
        return res;
    }
    str = str.substr(begin, end - begin + 1);
    if ("" == str) return res;
    //先将要切割的字符串从string类型转换为char*类型  
    const char* cstr = str.c_str();
    cout << cstr << endl;
    int size = str.length();
    begin = 0;
    int _count = 0;
    for (int i = 0; i < size; i++) {
        if (cstr[i] != ' ') {
            _count++;
        }
        else {
            if (_count != 0)res.push_back(str.substr(begin, _count));
            begin = i + 1;
            _count = 0;
        }

    }
    if (_count != 0)res.push_back(str.substr(begin, _count));
    return res;
}

std::wstring str2wstr(string str)
{
    LPCSTR src = str.c_str();
    int nLen = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);
    if (nLen == 0)
        return L"";

    wchar_t* dst = new wchar_t[nLen];
    if (!dst)
        return L"";

    MultiByteToWideChar(CP_ACP, 0, src, -1, dst, nLen);
    std::wstring res(dst);
    delete[] dst;
    dst = NULL;
    return res;
}
