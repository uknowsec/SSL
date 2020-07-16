#pragma once
#include <stdio.h>
#include <Windows.h>
#include <Winhttp.h>
#pragma comment(lib,"Winhttp.lib")
typedef void(*DownLoadCallback)(int ContentSize, int CUR_LEN);

void dcallback(int ContentSize, int file_size);

void download(const wchar_t* Url, const wchar_t* FileName, DownLoadCallback Func);