#include "BasicHandling.h"
#include <Windows.h>
#include <gdiplus.h>
#include <stdio.h>
#include <iostream>

using namespace Gdiplus;
#pragma comment (lib,"gdiplus.lib")



void SaveToFileX(std::string file, Bitmap* bmp) //Save bitmap to file
{
    UINT num;
    UINT size;

    ImageCodecInfo* imagecodecinfo;
    GetImageEncodersSize(&num, &size); //get count of codec

    imagecodecinfo = (ImageCodecInfo*)(malloc(size));
    GetImageEncoders(num, size, imagecodecinfo);//get codec

    CLSID clsidEncoder;

    for (int i = 0; i < num; i++)
    {
        if (wcscmp(imagecodecinfo[i].MimeType, L"image/png") == 0)
            clsidEncoder = imagecodecinfo[i].Clsid;//get png codec id

    }

    free(imagecodecinfo);
    std::wstring ws;
    ws.assign(file.begin(), file.end());//sring to wstring
    bmp->Save(ws.c_str(), &clsidEncoder); //save in png format
}

void CreateMaliciousImage(const WCHAR* inputImage, std::string outputImage)
{
    ULONG_PTR gdiplustoken;
    GdiplusStartupInput gdistartupinput;
    GdiplusStartupOutput gdistartupoutput;

    gdistartupinput.SuppressBackgroundThread = true;
    GdiplusStartup(&gdiplustoken, &gdistartupinput, &gdistartupoutput); //start GDI+

    Bitmap* bmp = Bitmap::FromFile(inputImage, true);

    SaveToFileX(outputImage, bmp); //SaveToFile(std::string PathOfOutputFile, Bitmap* bitmap)

    GdiplusShutdown(gdiplustoken);

}