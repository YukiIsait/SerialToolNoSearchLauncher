#pragma once
#pragma comment(lib, "zlib/zlibstat.lib")

#include <Windows.h>
#include "zlib/unzip.h"
#include <string>
using std::string;

class UnZip {
public:
    static bool unZipFile(string strFilePath, string strTempPath);
};
