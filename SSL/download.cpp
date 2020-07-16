#include "download.h"


typedef struct _URL_INFO
{
	WCHAR szScheme[512];
	WCHAR szHostName[512];
	WCHAR szUserName[512];
	WCHAR szPassword[512];
	WCHAR szUrlPath[512];
	WCHAR szExtraInfo[512];
}URL_INFO, * PURL_INFO;


void dcallback(int ContentSize, int file_size)
{
	printf("count:%d,size:%d\n", ContentSize, file_size);
}

void download(const wchar_t* Url, const wchar_t* FileName, DownLoadCallback Func)
{
	URL_INFO url_info = { 0 };
	URL_COMPONENTSW lpUrlComponents = { 0 };
	lpUrlComponents.dwStructSize = sizeof(lpUrlComponents);
	lpUrlComponents.lpszExtraInfo = url_info.szExtraInfo;
	lpUrlComponents.lpszHostName = url_info.szHostName;
	lpUrlComponents.lpszPassword = url_info.szPassword;
	lpUrlComponents.lpszScheme = url_info.szScheme;
	lpUrlComponents.lpszUrlPath = url_info.szUrlPath;
	lpUrlComponents.lpszUserName = url_info.szUserName;

	lpUrlComponents.dwExtraInfoLength =
		lpUrlComponents.dwHostNameLength =
		lpUrlComponents.dwPasswordLength =
		lpUrlComponents.dwSchemeLength =
		lpUrlComponents.dwUrlPathLength =
		lpUrlComponents.dwUserNameLength = 512;

	WinHttpCrackUrl(Url, 0, ICU_ESCAPE, &lpUrlComponents);

	// 创建一个会话
	HINTERNET hSession = WinHttpOpen(NULL, WINHTTP_ACCESS_TYPE_NO_PROXY, NULL, NULL, 0);
	DWORD dwReadBytes, dwSizeDW = sizeof(dwSizeDW), dwContentSize, dwIndex = 0;
	// 创建一个连接
	HINTERNET hConnect = WinHttpConnect(hSession, lpUrlComponents.lpszHostName, lpUrlComponents.nPort, 0);
	// 创建一个请求，先查询内容的大小
	HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"HEAD", lpUrlComponents.lpszUrlPath, L"HTTP/1.1", WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_REFRESH);
	WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0);
	WinHttpReceiveResponse(hRequest, 0);
	WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_CONTENT_LENGTH | WINHTTP_QUERY_FLAG_NUMBER, NULL, &dwContentSize, &dwSizeDW, &dwIndex);
	WinHttpCloseHandle(hRequest);

	// 创建一个请求，获取数据
	hRequest = WinHttpOpenRequest(hConnect, L"GET", lpUrlComponents.lpszUrlPath, L"HTTP/1.1", WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_REFRESH);
	WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0);
	WinHttpReceiveResponse(hRequest, 0);

	// 分段回调显示进度
	DWORD BUF_LEN = 1024, ReadedLen = 0;
	BYTE* pBuffer = NULL;
	pBuffer = new BYTE[BUF_LEN];

	HANDLE hFile = CreateFileW(FileName, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	while (dwContentSize > ReadedLen)
	{
		ZeroMemory(pBuffer, BUF_LEN);
		WinHttpReadData(hRequest, pBuffer, BUF_LEN, &dwReadBytes);
		ReadedLen += dwReadBytes;

		// 写入文件
		WriteFile(hFile, pBuffer, dwReadBytes, &dwReadBytes, NULL);
		// 进度回调
	//	Func(dwContentSize, ReadedLen);

	}

	CloseHandle(hFile);
	delete pBuffer;


	/*
	// 一次性写入整个文件
	BYTE *pBuffer = NULL;

	pBuffer = new BYTE[dwContentSize];
	ZeroMemory(pBuffer, dwContentSize);
	//do{
		WinHttpReadData(hRequest, pBuffer, dwContentSize, &dwReadBytes);
		Func(dwContentSize, dwReadBytes);
	//} while (dwReadBytes == 0);


	HANDLE hFile = CreateFileW(FileName, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	WriteFile(hFile, pBuffer, dwContentSize, &dwReadBytes, NULL);
	CloseHandle(hFile);

	delete pBuffer;
	*/
	WinHttpCloseHandle(hRequest);
	WinHttpCloseHandle(hConnect);
	WinHttpCloseHandle(hSession);
}