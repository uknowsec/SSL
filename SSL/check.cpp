#include <sysinfoapi.h>
#include <winioctl.h>
#include <ioapiset.h>
#include <sysinfoapi.h>
#include <winioctl.h>
#include <ioapiset.h>
#include <fileapi.h>
#include  <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <Winnls.h>
#include <iostream>
#include <windows.h>
#include <comdef.h>
#include <Wbemidl.h>
#include <ShlObj.h>
#include <LM.h>
#include <TlHelp32.h>
#include <string.h>
#include <string>
#include <atlbase.h>

#pragma comment(lib, "wbemuuid.lib")
#pragma comment(lib, "netapi32.lib")
#pragma comment(lib,"Kernel32.lib")



BOOL checkthreatbook() {
	struct stat info;
	const char* pathname = "C:\\Program Files (x86)\\Acrylic DNS Proxy";
	if (stat(pathname, &info) != 0)
		return false;
	else if (info.st_mode & S_IFDIR)  // S_ISDIR() doesn't exist on my windows 
		return true;
	else
		return false;
}

BOOL language()
{
	LANGID lid = GetSystemDefaultLangID();
	if (lid == 0x0409)
	{
		return true;
	}
	else {
		return false;
	}
}

// 检查CPU温度（需要管理员权限）
// Get-WMIObject MSAcpi_ThermalZoneTemperature -Namespace "root/wmi"
// VM中无返回结果
// https://docs.microsoft.com/en-us/windows/win32/wmisdk/example--getting-wmi-data-from-the-local-computer
BOOL checkCPUTemperature()
{
	HRESULT hres;
	BOOL res = -1;

	do
	{
		// Step 1: --------------------------------------------------
	// Initialize COM. ------------------------------------------

		hres = CoInitializeEx(0, COINIT_MULTITHREADED);
		if (FAILED(hres))
		{
			// cout << "Failed to initialize COM library. Error code = 0x" << hex << hres << endl;
			break;                  // Program has failed.
		}

		// Step 2: --------------------------------------------------
		// Set general COM security levels --------------------------

		hres = CoInitializeSecurity(
			NULL,
			-1,                          // COM authentication
			NULL,                        // Authentication services
			NULL,                        // Reserved
			RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication 
			RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation  
			NULL,                        // Authentication info
			EOAC_NONE,                   // Additional capabilities 
			NULL                         // Reserved
		);

		if (FAILED(hres))
		{
			// cout << "Failed to initialize security. Error code = 0x" << hex << hres << endl;
			CoUninitialize();
			break;                    // Program has failed.
		}

		// Step 3: ---------------------------------------------------
		// Obtain the initial locator to WMI -------------------------

		IWbemLocator* pLoc = NULL;

		hres = CoCreateInstance(
			CLSID_WbemLocator,
			0,
			CLSCTX_INPROC_SERVER,
			IID_IWbemLocator, (LPVOID*)&pLoc);

		if (FAILED(hres))
		{
			// cout << "Failed to create IWbemLocator object." << " Err code = 0x" << hex << hres << endl;
			CoUninitialize();
			break;                 // Program has failed.
		}

		// Step 4: -----------------------------------------------------
		// Connect to WMI through the IWbemLocator::ConnectServer method

		IWbemServices* pSvc = NULL;

		// Connect to the root\cimv2 namespace with
		// the current user and obtain pointer pSvc
		// to make IWbemServices calls.
		hres = pLoc->ConnectServer(
			// _bstr_t(L"ROOT\\CIMV2"), // Object path of WMI namespace
			_bstr_t(L"ROOT\\WMI"),
			NULL,                    // User name. NULL = current user
			NULL,                    // User password. NULL = current
			0,                       // Locale. NULL indicates current
			NULL,                    // Security flags.
			0,                       // Authority (for example, Kerberos)
			0,                       // Context object 
			&pSvc                    // pointer to IWbemServices proxy
		);

		if (FAILED(hres))
		{
			// cout << "Could not connect. Error code = 0x" << hex << hres << endl;
			pLoc->Release();
			CoUninitialize();
			break;                // Program has failed.
		}

		// cout << "Connected to ROOT\\WMI WMI namespace" << endl;

		// Step 5: --------------------------------------------------
		// Set security levels on the proxy -------------------------

		hres = CoSetProxyBlanket(
			pSvc,                        // Indicates the proxy to set
			RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
			RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
			NULL,                        // Server principal name 
			RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx 
			RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
			NULL,                        // client identity
			EOAC_NONE                    // proxy capabilities 
		);

		if (FAILED(hres))
		{
			// cout << "Could not set proxy blanket. Error code = 0x" << hex << hres << endl;
			pSvc->Release();
			pLoc->Release();
			CoUninitialize();
			break;               // Program has failed.
		}

		// Step 6: --------------------------------------------------
		// Use the IWbemServices pointer to make requests of WMI ----

		// For example, get the name of the operating system
		IEnumWbemClassObject* pEnumerator = NULL;
		hres = pSvc->ExecQuery(
			bstr_t("WQL"),
			bstr_t("SELECT * FROM MSAcpi_ThermalZoneTemperature"),
			WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
			NULL,
			&pEnumerator);

		if (FAILED(hres))
		{
			// cout << "Query for operating system name failed." << " Error code = 0x" << hex << hres << endl;
			pSvc->Release();
			pLoc->Release();
			CoUninitialize();
			break;               // Program has failed.
		}

		// Step 7: -------------------------------------------------
		// Get the data from the query in step 6 -------------------

		IWbemClassObject* pclsObj = NULL;
		ULONG uReturn = 0;

		while (pEnumerator)
		{
			HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);

			if (0 == uReturn) // VM中结果为空
			{
				if (-1 == res)
				{
					res = TRUE;
				}
				break;
			}

			VARIANT vtProp;

			// Get the value of the Name property
			hr = pclsObj->Get(L"CurrentTemperature", 0, &vtProp, 0, 0);
			// res = vtProp.ullVal / 10.0 - 273.15; // 开氏转摄氏
			res = FALSE;

			VariantClear(&vtProp);

			pclsObj->Release();
		}

		// Cleanup
		// ========

		pSvc->Release();
		pLoc->Release();
		pEnumerator->Release();
		CoUninitialize();

	} while (false);

	return res;
}

BOOL checkMemory(INT memory)
{
	_MEMORYSTATUSEX mst;
	mst.dwLength = sizeof(mst);
	GlobalMemoryStatusEx(&mst);
	if (mst.ullTotalPhys / (1024.0 * 1024 * 1024) < memory) // B
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

// 检测开机时间
BOOL checkUptime(DWORD msTime)
{
	DWORD UpTime = GetTickCount();
	if (UpTime < msTime)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

// 检测ffice临时目录下的文件数量
BOOL checkoffice(INT aNum)
{
	int file_count = 0;
	DWORD dwRet;
	LPSTR pszOldVal;
	pszOldVal = (LPSTR)malloc(MAX_PATH * sizeof(char));
	dwRet = GetEnvironmentVariableA("APPDATA", pszOldVal, MAX_PATH);

	std::string stdstr = pszOldVal;
	stdstr += "\\Microsoft\\office\\Recent\\*.doc.lnk";

	LPSTR s = const_cast<char*>(stdstr.c_str());

	WIN32_FIND_DATAA data;
	HANDLE hFind = FindFirstFileA(s, &data);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			file_count++;
		} while (FindNextFileA(hFind, &data));
		FindClose(hFind);
	}

	if (file_count < aNum)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}