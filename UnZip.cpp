#include "UnZip.h"

bool UnZip::unZipFile(string strFilePath, string strTempPath) {
    int nReturnValue;

    //��zip�ļ�
    unzFile unzfile = unzOpen(strFilePath.data());
    if (unzfile == NULL) {
        MessageBox(NULL, L"�������ļ�ʧ��", L"��ʾ", MB_OK | MB_ICONWARNING);
        return false;
    }

    //��ȡzip�ļ�����Ϣ
    unz_global_info* pGlobalInfo = new unz_global_info;
    nReturnValue = unzGetGlobalInfo(unzfile, pGlobalInfo);
    if (nReturnValue != UNZ_OK) {
        MessageBox(NULL, L"��ȡ�����ļ���Ϣʧ��", L"��ʾ", MB_OK | MB_ICONWARNING);
        return false;
    }

    //����zip�ļ�
    unz_file_info* pFileInfo = new unz_file_info;
    char szZipFName[MAX_PATH];  //��Ŵ�zip�н����������ڲ��ļ���
    for (uLong i = 0; i < pGlobalInfo->number_entry; i++) {
        //�����õ�zip�е��ļ���Ϣ
        nReturnValue = unzGetCurrentFileInfo(unzfile, pFileInfo, szZipFName, MAX_PATH, NULL, 0, NULL, 0);
        if (nReturnValue != UNZ_OK) {
            MessageBox(NULL, L"���������ļ���Ϣʧ��", L"��ʾ", MB_OK | MB_ICONWARNING);
            return false;
        }

        //�ж����ļ��л����ļ�
        if (pFileInfo->external_fa == FILE_ATTRIBUTE_DIRECTORY) {//���ļ���
            string strDiskPath = strTempPath + "/" + szZipFName;
            CreateDirectoryA(strDiskPath.data(), NULL);
        } else {//���ļ�
            //�����ļ�
            string strDiskFile = strTempPath + "/" + szZipFName;
            HANDLE hFile =
                CreateFileA(strDiskFile.data(), GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_FLAG_WRITE_THROUGH, NULL);
            if (hFile == INVALID_HANDLE_VALUE) {
                MessageBox(NULL, L"�����ļ�ʧ��", L"��ʾ", MB_OK | MB_ICONWARNING);
                return false;
            }

            //��ѹ�������ļ�
            nReturnValue = unzOpenCurrentFile(unzfile);
            if (nReturnValue != UNZ_OK) {
                MessageBox(NULL, L"���ļ�ʧ��", L"��ʾ", MB_OK | MB_ICONWARNING);
                CloseHandle(hFile);
                return false;
            }

            //��ȡѹ�������ļ���д�뱾���ļ�
            const int BUFFER_SIZE = 4096;
            char szReadBuffer[BUFFER_SIZE];
            while (TRUE) {
                memset(szReadBuffer, 0, BUFFER_SIZE);
                int nReadFileSize = unzReadCurrentFile(unzfile, szReadBuffer, BUFFER_SIZE);
                if (nReadFileSize < 0) {  //��ȡ�ļ�ʧ��
                    MessageBox(NULL, L"��ȡ�ļ�ʧ��", L"��ʾ", MB_OK | MB_ICONWARNING);
                    unzCloseCurrentFile(unzfile);
                    CloseHandle(hFile);
                    return false;
                } else if (nReadFileSize == 0) {  //��ȡ�ļ����
                    unzCloseCurrentFile(unzfile);
                    CloseHandle(hFile);
                    break;
                } else {  //д���ȡ������
                    DWORD dWrite = 0;
                    BOOL bWriteSuccessed = WriteFile(hFile, szReadBuffer, BUFFER_SIZE, &dWrite, NULL);
                    if (!bWriteSuccessed) {
                        MessageBox(NULL, L"��ȡ�ļ�ʧ��", L"��ʾ", MB_OK | MB_ICONWARNING);
                        unzCloseCurrentFile(unzfile);
                        CloseHandle(hFile);
                        return false;
                    }
                }
            }
        }
        unzGoToNextFile(unzfile);
    }

    //�ر�
    if (unzfile) {
        unzClose(unzfile);
    }
    return true;
}
