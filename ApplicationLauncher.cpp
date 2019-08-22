#include "UnZip.h"
#include <Windows.h>
#include <string>
using std::string;

#define APPLICATION_ZIP_NAME "SerialTool(NoSearch).comp"
#define APPLICATION_DIR "SerialToolNoSearch"
#define APPLICATION_NAME "SerialToolNoSearch.exe"

string getTempDir() {
    char lpTempPathBuffer[MAX_PATH];
    if (GetTempPathA(MAX_PATH, lpTempPathBuffer) < 1) {
        MessageBox(NULL, L"临时目录获取失败", L"错误", MB_OK | MB_ICONERROR);
        return "";
    }
    return lpTempPathBuffer;
}

bool dirExist(string path) {
    if (GetFileAttributesA(path.data()) != INVALID_FILE_ATTRIBUTES) {
        return true;
    }
    return false;
}

void runApplication(string path) {
    SHELLEXECUTEINFOA sei;
    ZeroMemory(&sei, sizeof(SHELLEXECUTEINFOA));
    sei.cbSize = sizeof(SHELLEXECUTEINFOA);
    sei.lpVerb = "open";
    sei.lpFile = path.data();
    sei.nShow = SW_SHOW;
    if (!ShellExecuteExA(&sei)) {
        MessageBox(NULL, L"程序打开失败", L"错误", MB_OK | MB_ICONERROR);
    }
}

int main(int argc, char** argv) {
    string tempDir = getTempDir();
    if (tempDir.length() != 0) {
        string targetDir = tempDir + APPLICATION_DIR;
        if (dirExist(targetDir)) {
            string rmCmd = "rmdir /s /q " + targetDir;
            if (system(rmCmd.data())) {
                MessageBox(NULL, L"临时目录被占用", L"错误", MB_OK | MB_ICONERROR);
                return 1;
            }
        }
        CreateDirectoryA(targetDir.data(), NULL);
        if (UnZip::unZipFile(APPLICATION_ZIP_NAME, targetDir)) {
            runApplication(targetDir + "\\" + APPLICATION_NAME);
        }
    }
    return 0;
}
