#pragma once
#include<vector>
using namespace std;

std::string execute(string cmd);
void process_cd(string cmd);
std::vector<string> split(string src);
std::wstring str2wstr(string str);