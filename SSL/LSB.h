#pragma once
#include <iostream>
#include <tchar.h>
#include <Windows.h>

#ifndef IMAGELSB
#define IMAGELSB

bool LSB_encode(std::string text, const WCHAR *inputImage, std::string outputImage);
std::string LSB_decode(const WCHAR *ImageFileName);

#endif // IMAGELSB

