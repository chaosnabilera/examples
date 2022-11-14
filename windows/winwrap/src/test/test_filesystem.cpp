#include <iostream>
#include <string>

#include <locale.h>

#include "winwrap_encoding.h"
#include "winwrap_filesystem.h"

static bool WWPrintCurrentWorkingDirectory();
static bool WWGetNextInputWord(std::wstring& out_word);

bool WWGetNextInputWord(std::wstring& out_word) {
    char mbline[0x1000];
    int assign_cnt = 0;
    std::shared_ptr<WCHAR> wcline(nullptr);
    
    assign_cnt = scanf("%s", mbline);
    if (!WinEncoding::convertMultiByteToWCHAR(mbline, &wcline)) {
        printf("Unable to convert input to WCHAR\n");
        return false;
    }
    out_word = wcline.get();
    return true;
}

void WWNavigateFileSystem(bool use_ifileoperation) {
    std::wstring icmd, iarg1, iarg2;
    std::shared_ptr<WinIFileOperation> ifo(nullptr);
    bool use_recycle_bin = false;
    bool overwrite = true;

    if (use_ifileoperation) {
        if (!WinIFileOperation::createWinIFileOperation(&ifo)) {
            printf("[WWNavigateFileSystem] Unable to create WinIFileOperation\n");
            return;
        }
        printf("[WWNavigateFileSystem] Using WinIFileOperation\n");
    }
    else {
        printf("[WWNavigateFileSystem] Using SHFileOperation\n");
    }
    
    while (true) {
        printf("%S>", WinPath::getCWDW().c_str());

        if (!WWGetNextInputWord(icmd))
            continue;
        
        if (icmd == L"exit") {
            break;
        }
        else if (icmd == L"cd") {
            if (!WWGetNextInputWord(iarg1))
                continue;
            if (!WinPath::setCWDW(iarg1)) {
                printf("WinPath::setCWDW(%S) failed\n", iarg1.c_str());
                continue;
            }
        }
        else if (icmd == L"cwd") {
            printf("%S\n", WinPath::getCWDW().c_str());
        }
        else if (icmd == L"ls") {
            if (!WWPrintCurrentWorkingDirectory()) {
                printf("[WWNavigateFileSystem] WWPrintCurrentWorkingDirectory failed!\n");
            }
        }
        else if (icmd == L"mkdir") {
            if (!WWGetNextInputWord(iarg1)) {
                continue;
            }
            if (WinPath::createDirW(iarg1)) {
                printf("[WWNavigateFileSystem] mkdir %S success\n", iarg1.c_str());
            }
            else {
                printf("[WWNavigateFileSystem] mkdir %S failed\n", iarg1.c_str());
            }
        }
        else if (icmd == L"del" || icmd == L"del_recycle") {
            if (!WWGetNextInputWord(iarg1)) {
                continue;
            }
            use_recycle_bin = (icmd == L"del_recycle");
            
            if (use_ifileoperation) {
                if (ifo->deletePath(iarg1, use_recycle_bin)) {
                    printf("[WWNavigateFileSystem] ifo->deletePath %S success\n", iarg1.c_str());
                }
                else {
                    printf("[WWNavigateFileSystem] ifo->deletePath %S failed\n", iarg1.c_str());
                }
            }
            else {
                if (WinPath::shellDeletePathW(iarg1, use_recycle_bin)) {
                    printf("[WWNavigateFileSystem] WinPath::shellDeletePathW %S success\n", iarg1.c_str());
                }
                else {
                    printf("[WWNavigateFileSystem] WinPath::shellDeletePathW %S failed\n", iarg1.c_str());
                }
            }
        }
        else if (icmd == L"cp") {
            if (!WWGetNextInputWord(iarg1) || !WWGetNextInputWord(iarg2)) {
                continue;
            }

            if (use_ifileoperation) {
                if (ifo->copyPath(iarg1, iarg2, overwrite)) {
                    printf("[WWNavigateFileSystem] ifo->copyPath(%S,%S) success\n", iarg1.c_str(), iarg2.c_str());
                }
                else {
                    printf("[WWNavigateFileSystem] ifo->copyPath(%S,%S) failed\n", iarg1.c_str(), iarg2.c_str());
                }
            }
            else {
                if (WinPath::shellCopyPathW(iarg1, iarg2, overwrite)) {
                    printf("[WWNavigateFileSystem] WinPath::shellCopyPathW(%S,%S) success\n", iarg1.c_str(), iarg2.c_str());
                }
                else {
                    printf("[WWNavigateFileSystem] WinPath::shellCopyPathW(%S,%S) failed\n", iarg1.c_str(), iarg2.c_str());
                }
            }
        }
        else if (icmd == L"mv") {
            if (!WWGetNextInputWord(iarg1) || !WWGetNextInputWord(iarg2)) {
                continue;
            }

            if (use_ifileoperation) {
                if (ifo->movePath(iarg1, iarg2, overwrite)) {
                    printf("[WWNavigateFileSystem] ifo->movePath(%S, %S) success\n", iarg1.c_str(), iarg2.c_str());
                }
                else {
                    printf("[WWNavigateFileSystem] ifo->movePath(%S, %S) failed\n", iarg1.c_str(), iarg2.c_str());
                }
            }
            else {
                if (WinPath::shellMovePathW(iarg1, iarg2, overwrite)) {
                    printf("[WWNavigateFileSystem] WinPath::shellMovePathW(%S,%S) success\n", iarg1.c_str(), iarg2.c_str());
                }
                else {
                    printf("[WWNavigateFileSystem] WinPath::shellMovePathW(%S,%S) failed\n", iarg1.c_str(), iarg2.c_str());
                }
            }
            /*
            if (WinPath::moveFileW(iarg1, iarg2, true)) {
                printf("[WWNavigateFileSystem] %S -> %S success\n", iarg1.c_str(), iarg2.c_str());
            }
            else {
                printf("[[WWNavigateFileSystem] %S -> %S failed\n", iarg1.c_str(), iarg2.c_str());
            }
            */
        }
        else if (icmd == L"ren") {
            if (!WWGetNextInputWord(iarg1) || !WWGetNextInputWord(iarg2)) {
                continue;
            }
            
            if (use_ifileoperation) {
                if (ifo->renamePath(iarg1, iarg2, overwrite)) {
                    printf("[WWNavigateFileSystem] ifo->renamePath(%S,%S) success\n", iarg1.c_str(), iarg2.c_str());
                }
                else {
                    printf("[WWNavigateFileSystem] ifo->renamePath(%S,%S) failed\n", iarg1.c_str(), iarg2.c_str());
                }
            }
            else {
                if (WinPath::shellRenamePathW(iarg1, iarg2, overwrite)) {
                    printf("[WWNavigateFileSystem] WinPath::shellRenamePathW(%S,%S) success\n", iarg1.c_str(), iarg2.c_str());
                }
                else {
                    printf("[WWNavigateFileSystem] WinPath::shellRenamePathW(%S,%S) failed\n", iarg1.c_str(), iarg2.c_str());
                }
            }
        }
        else if (icmd == L"walk") {
            std::shared_ptr<WinFileSystemWalk> walk;
            std::shared_ptr<WinFileSystemWalkState> state;
            if (WinFileSystemWalk::create(L".", &walk)) {
                do {
                    if (!walk->get(&state)) {
                        printf("[WWNavigateFileSystem] walk->get failed");
                        break;
                    }
                    for (auto& dirname : *(state->pDirList)) {
                        printf("%S\\%S\\\n", state->pRoot->c_str(), dirname.c_str());
                    }
                    for (auto& filename : *(state->pFileList)) {
                        printf("%S\\%S\n",  state->pRoot->c_str(), filename.c_str());
                    }
                } while (walk->advance());
            }
        }
    }
}

bool WWPrintCurrentWorkingDirectory() {
    bool result = false;
    bool pathlist_result = false;
    std::vector<std::wstring> pathlist;
    std::wstring cur = L".";
    std::wstring cur_abspath;
    std::wstring sub_abspath;
    std::wstring is_dir;
    std::wstring is_file;
    SYSTEMTIME creation_time = { 0 };
    SYSTEMTIME last_access_time = { 0 };
    SYSTEMTIME last_write_time = { 0 };
    
    do {
        if (!WinPath::getAbsPathW(cur, &cur_abspath)) {
            printf("[WWPrintCurrentWorkingDirectory] getAbsPathW current working directory failed\n");
            break;
        }
        if (!WinPath::listDirW(cur_abspath, &pathlist)) {
            printf("[WWPrintCurrentWorkingDirectory] WinPath::listDirW(%S) failed\n", cur_abspath.c_str());
            break;
        }
        
        pathlist_result = true;
        for (std::wstring& sub : pathlist) {
            if (!WinPath::getAbsPathW(sub, &sub_abspath)) {
                printf("[WWPrintCurrentWorkingDirectory] WinPath::getAbsPathW %S failed\n", sub.c_str());
                break;
            }
            
            is_dir = WinPath::isDirW(sub_abspath) ? L"dir" : L"";
            is_file = WinPath::isFileW(sub_abspath) ? L"file" : L"";

            if (WinPath::getFileTimeAsSystemTimeW(sub_abspath, &creation_time, &last_access_time, &last_write_time)) {
                /*
                printf("%S %S %S %S ct: %04d-%02d-%02d %02d:%02d:%02d at: %04d-%02d-%02d %02d:%02d:%02d wt : %04d-%02d-%02d %02d:%02d:%02d\n",
                    sub.c_str(), sub_abspath.c_str(), is_dir.c_str(), is_file.c_str(),
                    creation_time.wYear, creation_time.wMonth, creation_time.wDay, creation_time.wHour, creation_time.wMinute, creation_time.wSecond,
                    last_access_time.wYear, last_access_time.wMonth, last_access_time.wDay, last_access_time.wHour, last_access_time.wMinute, last_access_time.wSecond,
                    last_write_time.wYear, last_write_time.wMonth, last_write_time.wDay, last_write_time.wHour, last_write_time.wMinute, last_write_time.wSecond);
                */
                printf("%-50S %3S wt : %04d-%02d-%02d %02d:%02d:%02d\n", sub.c_str(), is_dir.c_str(), last_write_time.wYear, last_write_time.wMonth, last_write_time.wDay, last_write_time.wHour, last_write_time.wMinute, last_write_time.wSecond);
            }
            else {
                // printf("%S %S %S %S (file time access failed)\n", sub.c_str(), sub_abspath.c_str(), is_dir.c_str(), is_file.c_str());
                printf("%-50S %3S wt : access denied\n", is_dir.c_str(), sub.c_str());
            }            
        }
        
        if (!pathlist_result) {
            break;
        }
        
        result = true;
    } while (0);

    return result;
    
}

void WWCopyFileSimple(std::string src, std::string dst) {
    std::shared_ptr<WinFile> ifile(nullptr);
    std::shared_ptr<WinFile> ofile(nullptr);
    std::shared_ptr<BYTE> ifile_buf(nullptr);
    long long ifile_buflen = 0LL;

    do {
        if (!WinPath::isFileA(src)) {
            printf("%s is not a file!\n", src.c_str());
            break;
        }
        if (!WinPath::openFileA(src, "r", &ifile)) {
            printf("failed to open %s for read\n", src.c_str());
            break;
        }

        if (!ifile->readAll(&ifile_buf, &ifile_buflen)) {
            printf("failed to read_all %s\n", src.c_str());
            break;
        }

        printf("size of %s: %lld\n", src.c_str(), ifile_buflen);

        if (!WinPath::openFileA(dst, "w", &ofile)) {
            printf("failed to open %s for write\n", src.c_str());
            break;
        }

        if (!(ofile->writeBytes(ifile_buf.get(), ifile_buflen))) {
            printf("failed to write to %s\n", dst.c_str());
            break;
        }

        printf("copy %s -> %s success!\n", src.c_str(), dst.c_str());
    } while (0);
}