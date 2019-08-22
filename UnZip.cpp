#include "UnZip.h"

bool UnZip::unZipFile(string strFilePath, string strTempPath) {
    int nReturnValue;

    //打开zip文件
    unzFile unzfile = unzOpen(strFilePath.data());
    if (unzfile == NULL) {
        MessageBox(NULL, L"打开数据文件失败", L"提示", MB_OK | MB_ICONWARNING);
        return false;
    }

    //获取zip文件的信息
    unz_global_info* pGlobalInfo = new unz_global_info;
    nReturnValue = unzGetGlobalInfo(unzfile, pGlobalInfo);
    if (nReturnValue != UNZ_OK) {
        MessageBox(NULL, L"获取数据文件信息失败", L"提示", MB_OK | MB_ICONWARNING);
        return false;
    }

    //解析zip文件
    unz_file_info* pFileInfo = new unz_file_info;
    char szZipFName[MAX_PATH];  //存放从zip中解析出来的内部文件名
    for (uLong i = 0; i < pGlobalInfo->number_entry; i++) {
        //解析得到zip中的文件信息
        nReturnValue = unzGetCurrentFileInfo(unzfile, pFileInfo, szZipFName, MAX_PATH, NULL, 0, NULL, 0);
        if (nReturnValue != UNZ_OK) {
            MessageBox(NULL, L"解析数据文件信息失败", L"提示", MB_OK | MB_ICONWARNING);
            return false;
        }

        //判断是文件夹还是文件
        if (pFileInfo->external_fa == FILE_ATTRIBUTE_DIRECTORY) {//是文件夹
            string strDiskPath = strTempPath + "/" + szZipFName;
            CreateDirectoryA(strDiskPath.data(), NULL);
        } else {//是文件
            //创建文件
            string strDiskFile = strTempPath + "/" + szZipFName;
            HANDLE hFile =
                CreateFileA(strDiskFile.data(), GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_FLAG_WRITE_THROUGH, NULL);
            if (hFile == INVALID_HANDLE_VALUE) {
                MessageBox(NULL, L"创建文件失败", L"提示", MB_OK | MB_ICONWARNING);
                return false;
            }

            //打开压缩包内文件
            nReturnValue = unzOpenCurrentFile(unzfile);
            if (nReturnValue != UNZ_OK) {
                MessageBox(NULL, L"打开文件失败", L"提示", MB_OK | MB_ICONWARNING);
                CloseHandle(hFile);
                return false;
            }

            //读取压缩包内文件并写入本地文件
            const int BUFFER_SIZE = 4096;
            char szReadBuffer[BUFFER_SIZE];
            while (TRUE) {
                memset(szReadBuffer, 0, BUFFER_SIZE);
                int nReadFileSize = unzReadCurrentFile(unzfile, szReadBuffer, BUFFER_SIZE);
                if (nReadFileSize < 0) {  //读取文件失败
                    MessageBox(NULL, L"读取文件失败", L"提示", MB_OK | MB_ICONWARNING);
                    unzCloseCurrentFile(unzfile);
                    CloseHandle(hFile);
                    return false;
                } else if (nReadFileSize == 0) {  //读取文件完毕
                    unzCloseCurrentFile(unzfile);
                    CloseHandle(hFile);
                    break;
                } else {  //写入读取的内容
                    DWORD dWrite = 0;
                    BOOL bWriteSuccessed = WriteFile(hFile, szReadBuffer, BUFFER_SIZE, &dWrite, NULL);
                    if (!bWriteSuccessed) {
                        MessageBox(NULL, L"读取文件失败", L"提示", MB_OK | MB_ICONWARNING);
                        unzCloseCurrentFile(unzfile);
                        CloseHandle(hFile);
                        return false;
                    }
                }
            }
        }
        unzGoToNextFile(unzfile);
    }

    //关闭
    if (unzfile) {
        unzClose(unzfile);
    }
    return true;
}
