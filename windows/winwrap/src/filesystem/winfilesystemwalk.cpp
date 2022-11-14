#include "winwrap_filesystem.h"

#include "dprintf.hpp"

bool WinFileSystemWalk::create(std::wstring path, std::shared_ptr<WinFileSystemWalk>* out_walk) {
    bool result = false;

    std::shared_ptr<std::wstring> root_path;
    std::shared_ptr<std::vector<std::wstring>> file_list;
    std::shared_ptr<std::vector<std::wstring>> dir_list;
    std::shared_ptr<WinFileSystemWalkState> init_state;

    do {
        if (out_walk == nullptr) {
            dprintf("[[WinFileSystemWalk::create] output pointer is null");
            break;
        }
        while (path.length() > 0 && (path.back() == L'\\' || path.back() == L'/')) {
            path.pop_back();
        }

        if (!listDir(path, &file_list, &dir_list)) {
            dprintf("[WinFileSystemWalk::create] listDir failed!");
            break;
        }
        root_path = std::shared_ptr<std::wstring>(new std::wstring(path));
        
        init_state = std::shared_ptr<WinFileSystemWalkState>(new WinFileSystemWalkState(root_path, file_list, dir_list));
        *out_walk = std::shared_ptr<WinFileSystemWalk>(new WinFileSystemWalk(init_state));
        
        result = true;
    } while (false);

    return result;
}

bool WinFileSystemWalk::listDir(std::wstring path, std::shared_ptr<std::vector<std::wstring>>* out_filelist, std::shared_ptr<std::vector<std::wstring>>* out_dirlist) {
    bool result = false;
    std::wstring search_path;
    std::wstring dot = L".";
    std::wstring dotdot = L"..";

    HANDLE hfind = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATAW ffd = { 0 };

    do {
        if (!out_filelist || !out_dirlist) {
            dprintf("[WinFileSystemWalk::listDir] out_filelist or out_dirlist is nullptr");
            break;
        }
        
        while (path.length() > 0 && (path.back() == L'\\' || path.back() == L'/')) {
            path.pop_back();
        }
        if (!WinPath::isDirW(path)) {
            dprintf("[WinFileSystemWalk::listDir] %S is not a directory", path.c_str());
            break;
        }
        search_path = path + L"\\*";

        if ((hfind = FindFirstFileW(search_path.c_str(), &ffd)) == INVALID_HANDLE_VALUE) {
            dprintf("[WinFileSystemWalk::listDir] FindFirstFileW error : 0x%08x", GetLastError());
            break;
        }

        *out_filelist = std::shared_ptr<std::vector<std::wstring>>(new std::vector<std::wstring>());
        *out_dirlist = std::shared_ptr<std::vector<std::wstring>>(new std::vector<std::wstring>());

        do {
            if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                if (dot == ffd.cFileName || dotdot == ffd.cFileName) {
                    continue;
                }
                (*out_dirlist)->push_back(ffd.cFileName);
            }
            else {
                (*out_filelist)->push_back(ffd.cFileName);
            }
        } while (FindNextFileW(hfind, &ffd) != 0);

        result = true;
    } while (0);

    if (hfind != INVALID_HANDLE_VALUE) {
        FindClose(hfind);
    }

    return result;
}

WinFileSystemWalk::WinFileSystemWalk(std::shared_ptr<WinFileSystemWalkState> init_state) {
    stack.push_back(std::make_pair(init_state, 0));
}

bool WinFileSystemWalk::get(std::shared_ptr<WinFileSystemWalkState>* out_state) {
    if (stack.empty())
        return false;
    
    *out_state = stack.back().first;
    return true;
}

bool WinFileSystemWalk::advance() {
    bool result = false;
    std::shared_ptr<std::wstring> next_root;
    std::shared_ptr<std::vector<std::wstring>> file_list;
    std::shared_ptr<std::vector<std::wstring>> dir_list;
    std::shared_ptr<WinFileSystemWalkState> next_state;
    
    while (!stack.empty() && !result) {
        WinFileSystemWalkState& cur = *(stack.back().first);
        size_t& cur_idx = stack.back().second; // note that this is reference
        
        if (cur_idx >= cur.pDirList->size()) {
            stack.pop_back();
            continue;
        }
        
        next_root = std::shared_ptr<std::wstring>(new std::wstring((*cur.pRoot) + L"\\" + (*cur.pDirList)[cur_idx]));
        if (listDir(*next_root, &file_list, &dir_list)) {
            next_state = std::shared_ptr<WinFileSystemWalkState>(new WinFileSystemWalkState(next_root, file_list, dir_list));
            stack.push_back(std::make_pair(next_state, 0));
            result = true;
        }
        else {
            dprintf("[WinFileSystemWalk::advance] listDir %S failed. Moving to next", (*next_root).c_str());
        }
        
        ++cur_idx;
    }
    return result;
}