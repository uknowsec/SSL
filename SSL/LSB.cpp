#include "LSB.h"
#include <Windows.h>
#include <gdiplus.h>
#include <stdio.h>
#include <iostream>

using namespace Gdiplus;
#pragma comment (lib,"gdiplus.lib")

enum State
{
	Hiding,
	Filling_With_Zeros
};

Bitmap* embedText(std::string text, Bitmap* bmp)
{
    State state = Hiding;
    // holds the index of the character that is being hidden
    int charIndex = 0;

    // holds the value of the character converted to integer
    int charValue = 0;

    // holds the index of the color element (R or G or B) that is currently being processed
    long pixelElementIndex = 0;

    // holds the number of trailing zeros that have been added when finishing the process
    int zeros = 0;

    // hold pixel elements
    int R = 0, G = 0, B = 0;

    // pass through the rows
    for (int i = 0; i < bmp->GetHeight(); i++)
    {
        // pass through each row
        for (int j = 0; j < bmp->GetWidth(); j++)
        {
            // holds the pixel that is currently being processed
            Color pixel;
            bmp->GetPixel(j, i, &pixel);

            // now, clear the least significant bit (LSB) from each pixel element
            R = pixel.GetR() - pixel.GetR() % 2;
            G = pixel.GetG() - pixel.GetG() % 2;
            B = pixel.GetB() - pixel.GetB() % 2;

            // for each pixel, pass through its elements (RGB)
            for (int n = 0; n < 3; n++)
            {
                // check if new 8 bits has been processed
                if (pixelElementIndex % 8 == 0)
                {
                    // check if the whole process has finished
                    // we can say that it's finished when 8 zeros are added
                    if (state == Filling_With_Zeros && zeros == 8)
                    {
                        // apply the last pixel on the image
                        // even if only a part of its elements have been affected
                        if ((pixelElementIndex - 1) % 3 < 2)
                        {
                            bmp->SetPixel(j, i, Color(R, G, B));
                        }

                        // return the bitmap with the text hidden in
                        return bmp;
                    }

                    // check if all characters has been hidden
                    if (charIndex >= text.length())
                    {
                        // start adding zeros to mark the end of the text
                        state = Filling_With_Zeros;
                    }
                    else
                    {
                        // move to the next character and process again
                        charValue = text[charIndex++];
                    }
                }

                // check which pixel element has the turn to hide a bit in its LSB
                switch (pixelElementIndex % 3)
                {
                case 0:
                {
                    if (state == Hiding)
                    {
                        // the rightmost bit in the character will be (charValue % 2)
                        // to put this value instead of the LSB of the pixel element
                        // just add it to it
                        // recall that the LSB of the pixel element had been cleared
                        // before this operation
                        R += charValue % 2;

                        // removes the added rightmost bit of the character
                        // such that next time we can reach the next one
                        charValue /= 2;
                    }
                } break;
                case 1:
                {
                    if (state == Hiding)
                    {
                        G += charValue % 2;

                        charValue /= 2;
                    }
                } break;
                case 2:
                {
                    if (state == Hiding)
                    {
                        B += charValue % 2;

                        charValue /= 2;
                    }

                    bmp->SetPixel(j, i, Color(R, G, B));
                } break;
                }

                pixelElementIndex++;

                if (state == Filling_With_Zeros)
                {
                    // increment the value of zeros until it is 8
                    zeros++;
                }
            }
        }
    }

    return bmp;
}

void SaveToFile(std::string file, Bitmap* bmp) //Save bitmap to file
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
            clsidEncoder = imagecodecinfo[i].Clsid;//get jpeg codec id

    }

    free(imagecodecinfo);
    std::wstring ws;
    ws.assign(file.begin(), file.end());//sring to wstring
    bmp->Save(ws.c_str(), &clsidEncoder); //save in jpeg format
}

bool LSB_encode(std::string text, const WCHAR* inputImage, std::string outputImage)
{
    if (wcslen(inputImage) == 0 || text.length() == 0 || outputImage.length() == 0)
    {
        return false;
    }
	ULONG_PTR gdiplustoken;
	GdiplusStartupInput gdistartupinput;
	GdiplusStartupOutput gdistartupoutput;

	gdistartupinput.SuppressBackgroundThread = true;
	GdiplusStartup(&gdiplustoken, &gdistartupinput, &gdistartupoutput); //start GDI+

	Bitmap* bmp = Bitmap::FromFile(inputImage, true);
    //if (*bmp->GetWidth == 0 || *bmp->GetHeight == 0) return false;
	//std::string text = "BackDoor";


	Bitmap* bmp2 = embedText(text, bmp); //save encoded bitmap to new bitmap for decoding test
	//std::cout << extractText(bmp2);
	SaveToFile(outputImage, bmp); //SaveToFile(std::string PathOfOutputFile, Bitmap* bitmap)

	GdiplusShutdown(gdiplustoken);
    return true;
}

int reverseBits(int n)
{
    int result = 0;

    for (int i = 0; i < 8; i++)
    {
        result = result * 2 + n % 2;

        n /= 2;
    }

    return result;
}

std::string extractText(Bitmap* bmp)
{
    int colorUnitIndex = 0;
    int charValue = 0;

    // holds the text that will be extracted from the image
    std::string extractedText = "";

    // pass through the rows
    for (int i = 0; i < bmp->GetHeight(); i++)
    {
        // pass through each row
        for (int j = 0; j < bmp->GetWidth(); j++)
        {
            Color pixel;
            bmp->GetPixel(j, i, &pixel);

            // for each pixel, pass through its elements (RGB)
            for (int n = 0; n < 3; n++)
            {
                switch (colorUnitIndex % 3)
                {
                case 0:
                {
                    // get the LSB from the pixel element (will be pixel.R % 2)
                    // then add one bit to the right of the current character
                    // this can be done by (charValue = charValue * 2)
                    // replace the added bit (which value is by default 0) with
                    // the LSB of the pixel element, simply by addition
                    charValue = charValue * 2 + pixel.GetR() % 2;
                } break;
                case 1:
                {
                    charValue = charValue * 2 + pixel.GetG() % 2;
                } break;
                case 2:
                {
                    charValue = charValue * 2 + pixel.GetB() % 2;
                } break;
                }

                colorUnitIndex++;

                // if 8 bits has been added, then add the current character to the result text
                if (colorUnitIndex % 8 == 0)
                {
                    // reverse? of course, since each time the process happens on the right (for simplicity)
                    charValue = reverseBits(charValue);

                    // can only be 0 if it is the stop character (the 8 zeros)
                    if (charValue == 0)
                    {
                        return extractedText;
                    }

                    // convert the character value from int to char
                    char c = (char)charValue;

                    // add the current character to the result text
                    extractedText += c;
                }
            }
        }
    }

    return extractedText;
}

std::string LSB_decode(const WCHAR* ImageFileName)
{
    if (wcslen(ImageFileName) == 0)
    {
        return "";
    }

    std::string plainText = "";
    ULONG_PTR gdiplustoken;
    GdiplusStartupInput gdistartupinput;
    GdiplusStartupOutput gdistartupoutput;

    gdistartupinput.SuppressBackgroundThread = true;
    GdiplusStartup(&gdiplustoken, &gdistartupinput, &gdistartupoutput); //start GDI+

    Bitmap* bmp = Bitmap::FromFile(ImageFileName, true);
  

    plainText = extractText(bmp);

    GdiplusShutdown(gdiplustoken);

    return plainText;
}