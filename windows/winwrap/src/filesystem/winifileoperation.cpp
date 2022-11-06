#include "winwrap_filesystem.h"

#include "dprintf.hpp"

bool WinIFileOperation::createWinIFileOperation(std::shared_ptr<WinIFileOperation>* out_pinstance, bool execute_remote) {
    bool result = false;
    IFileOperation* fileOperation = nullptr;
    HRESULT hresult = E_UNEXPECTED;
    IShellItem* shell_item = nullptr;
    DWORD clsctx = CLSCTX_ALL;

    do {
        hresult = CoInitializeEx(
            NULL,    //  first parameter is reserved and must be NULL
            
            COINIT_MULTITHREADED |   // Use COINIT_APARTMENTTHREADED if we access COM from a single thread + 
                                     // thread has message loop
                                     // Note that some COM require a particular threading model
            
            COINIT_DISABLE_OLE1DDE      // Setting this flag avoids some overhead associated with 
                                        // Object Linking and Embedding (OLE) 1.0, an obsolete technology
        );

        if (FAILED(hresult)) {
            dprintf("[WinIFileOperation::createWinIFileOperation] CoInitializeEx failed: 0x%08x", hresult);
            break;
        }
        
        if (execute_remote) {
            clsctx = CLSCTX_LOCAL_SERVER;
        }
        
        hresult = CoCreateInstance(
            CLSID_FileOperation,  // [in] rclsid : CLSID identifies objects. In this case FileOperation

            NULL,                 // [in] pUnkOuter : If you are creating an aggregate object, 
                                  // the pUnkOuter parameter is a pointer to the controlling 
                                  // IUnknown interface of the aggregate object. 
                                  // Otherwise, this parameter must be NULL.

            clsctx,               // [in] dwClsContext : Specifies the context in which the code that 
                                  // manages the newly created object will run. 
                                  // CLSCTX_ALL: Use the most efficient option that the object supports 
                                  // (in-process > out-of-process > cross-computer)

            IID_PPV_ARGS(&fileOperation) // IID_PPV_ARGS is a macro. 
                                         // gets the UUID of the interface (4th arg) + 
                                         // passes the pointer
        );
        if (FAILED(hresult)) {
            dprintf("[WinIFileOperation::createWinIFileOperation] CoCreateInstance CLSID_FileOperation failed : 0x%08x", hresult);
            break;
        }

        hresult = fileOperation->SetOperationFlags(FOF_NO_UI);
        if (FAILED(hresult)) {
            dprintf("[WinIFileOperation::createWinIFileOperation] fileOperation->SetOperationFlags(FOF_NO_UI) failed : 0x%08x", hresult);
            break;
        }
        
        result = true;
    } while (0);

    if (result) {
        *out_pinstance = std::shared_ptr<WinIFileOperation>(new WinIFileOperation(fileOperation));
    }
    else {
        if (fileOperation) { // if SetOperationFlags failed
            hresult = fileOperation->Release();
            if (FAILED(hresult)) {
                dprintf("[WinIFileOperation::createWinIFileOperation] fileOperation->Release failed : 0x%08x", hresult);
            }
        }
    }

    return result;
}

WinIFileOperation::WinIFileOperation(IFileOperation* in_fileOperation) : 
    fileOperation(in_fileOperation) {
}

WinIFileOperation::~WinIFileOperation() {
    HRESULT hresult = E_UNEXPECTED;
    hresult = fileOperation->Release();
    if (FAILED(hresult)) {
        dprintf("[WinIFileOperation::~WinIFileOperation] fileOperation->Release failed : 0x%08x", hresult);
    }
}

bool WinIFileOperation::deletePath(std::wstring& path, bool use_recycle_bin) {
    bool result = false;
    HRESULT hresult = E_UNEXPECTED;
    IShellItem* shellitem = nullptr;
    std::wstring abspath;
    DWORD operation_flag = FOF_NO_UI;

    do {
        if (!WinPath::getAbsPathW(path, &abspath)) {
            dprintf("[WinIFileOperation::deletePath] WinPath::getAbsPathW path: %S failed", path.c_str());
            break;
        }

        hresult = SHCreateItemFromParsingName(
            abspath.c_str(), // [in] pszPath
            NULL,            // [in] pbc : Optional. A pointer to a bind context used to pass 
            // parameters as inputs and outputs to the parsing function
            IID_PPV_ARGS(&shellitem) // [in] riid
        );
        if (FAILED(hresult)) {
            dprintf("[WinIFileOperation::deletePath] SHCreateItemFromParsingName abspath: %S failed : 0x%08x", abspath.c_str(), hresult);
            break;
        }

        operation_flag = (use_recycle_bin) ? (FOF_NO_UI | FOF_ALLOWUNDO) : FOF_NO_UI;

        hresult = fileOperation->SetOperationFlags(operation_flag);
        if (FAILED(hresult)) {
            dprintf("[WinIFileOperation::deletePath] SetOperationFlags failed : 0x%08x", hresult);
            break;
        }

        hresult = fileOperation->DeleteItem(shellitem, NULL);
        if (FAILED(hresult)) {
            dprintf("[WinIFileOperation::deletePath] DeleteItem failed : 0x%08x", hresult);
            break;
        }

        // Actual operation happens here
        hresult = fileOperation->PerformOperations();
        if (FAILED(hresult)) {
            dprintf("[WinIFileOperation::deletePath] PerformOperations failed : 0x%08x", hresult);
            break;
        }

        result = true;
    } while (0);

    if (shellitem) {
        hresult = shellitem->Release();
        if (FAILED(hresult)) {
            dprintf("[WinIFileOperation::deletePath] shellitem->Release failed : 0x%08x", hresult);
        }
    }

    return result;
}

bool WinIFileOperation::movePath(std::wstring& src, std::wstring& dst, bool overwrite) {
    bool result = false;
    HRESULT hresult = E_UNEXPECTED;
    IShellItem* shellitem_src = nullptr;
    IShellItem* shellitem_dstdir = nullptr;
    std::wstring src_abspath, dst_abspath;
    std::wstring dst_absdir, dst_name;
    size_t idx;
    bool created_dstdir = false;

    do {
        if (!WinPath::getAbsPathW(src, &src_abspath)) {
            dprintf("[WinIFileOperation::movePath] WinPath::getAbsPathW src: %S failed", src.c_str());
            break;
        }

        if (!WinPath::getAbsPathW(dst, &dst_abspath)) {
            dprintf("[WinIFileOperation::movePath] WinPath::getAbsPathW dst: %S failed", dst.c_str());
            break;
        }

        if (!overwrite && WinPath::isPathW(dst_abspath)) {
            dprintf("[WinIFileOperation::movePath] dst_abspath: %S already exists", dst_abspath.c_str());
            break;
        }

        hresult = SHCreateItemFromParsingName(
            src_abspath.c_str(), // [in] pszPath
            NULL,                // [in] pbc : Optional. A pointer to a bind context used to pass 
                                 // parameters as inputs and outputs to the parsing function
            IID_PPV_ARGS(&shellitem_src) // [in] riid
        );
        if (FAILED(hresult)) {
            dprintf("[WinIFileOperation::movePath] SHCreateItemFromParsingName src_abspath: %S failed : 0x%08x", src_abspath.c_str(), hresult);
            break;
        }

        if ((idx = dst_abspath.rfind(L'\\')) == std::wstring::npos) {
            dprintf("[WinIFileOperation::movePath] dst_abspath: %S does not contain a backslash", dst_abspath.c_str());
            break;
        }
        dst_absdir = dst_abspath.substr(0, idx);
        dst_name = dst_abspath.substr(idx + 1); // either file or folder

        if (WinPath::isFileW(dst_absdir)) {
            dprintf("[WinIFileOperation::movePath] dst_absdir : %S is a file", dst_absdir.c_str());
            break;
        }
        
        // IFileOperation won't created directory for us
        // So we have to create it manually (SHFileOperation is better...)
        if (!WinPath::isDirW(dst_absdir)) {
            if (WinPath::createDirW(dst_absdir)) {
                created_dstdir = true;
                dprintf("[WinIFileOperation::movePath] created dst_absdir %S because it didn't exist", dst_absdir.c_str());
            }
            else {
                dprintf("[WinIFileOperation::movePath] creating dst_absdir %S failed");
                break;
            }
        }

        hresult = SHCreateItemFromParsingName(dst_absdir.c_str(), NULL, IID_PPV_ARGS(&shellitem_dstdir));
        if (FAILED(hresult)) {
            dprintf("[WinIFileOperation::movePath] SHCreateItemFromParsingName dst_absdir: %S failed : 0x%08x", dst_absdir.c_str(), hresult);
            break;
        }
        
        hresult = fileOperation->SetOperationFlags(FOF_NO_UI);
        if (FAILED(hresult)) {
            dprintf("[WinIFileOperation::movePath] SetOperationFlags failed : 0x%08x", hresult);
            break;
        }

        hresult = fileOperation->MoveItem(shellitem_src, shellitem_dstdir, dst_name.c_str(), NULL);
        if (FAILED(hresult)) {
            dprintf("[WinIFileOperation::movePath] MoveItem failed : 0x%08x", hresult);
            break;
        }

        // Actual operation happens here
        hresult = fileOperation->PerformOperations();
        if (FAILED(hresult)) {
            dprintf("[WinIFileOperation::movePath] PerformOperations failed : 0x%08x", hresult);
            break;
        }

        result = true;
    } while (0);

    if (!result && created_dstdir) {
        if (deletePath(dst_absdir, false)) {
            dprintf("[WinIFileOperation::movePath] rollback creation of dst_absdir success");
        }
        else {
            dprintf("[WinIFileOperation::movePath] rollback creation of dst_absdir failed");
        }
    }
    
    if (shellitem_src) {
        hresult = shellitem_src->Release();
        if (FAILED(hresult)) {
            dprintf("[WinIFileOperation::movePath] shellitem_src->Release failed : 0x%08x", hresult);
        }
    }
    if (shellitem_dstdir){
        hresult = shellitem_dstdir->Release();
        if (FAILED(hresult)) {
            dprintf("[WinIFileOperation::movePath] shellitem_dstdir->Release failed : 0x%08x", hresult);
        }
    }
    
    return result;
}

bool WinIFileOperation::copyPath(std::wstring& src, std::wstring& dst, bool overwrite) {
    bool result = false;
    HRESULT hresult = E_UNEXPECTED;
    IShellItem* shellitem_src = nullptr;
    IShellItem* shellitem_dstdir = nullptr;
    std::wstring src_abspath, dst_abspath;
    std::wstring dst_absdir, dst_name;
    size_t idx = 0;
    bool created_dstdir = false;

    do {
        if (!WinPath::getAbsPathW(src, &src_abspath)) {
            dprintf("[WinIFileOperation::copyPath] WinPath::getAbsPathW src: %S failed", src.c_str());
            break;
        }

        if (!WinPath::getAbsPathW(dst, &dst_abspath)) {
            dprintf("[WinIFileOperation::copyPath] WinPath::getAbsPathW dst: %S failed", dst.c_str());
            break;
        }

        if (!overwrite && WinPath::isPathW(dst_abspath)) {
            dprintf("[WinIFileOperation::copyPath] dst_abspath: %S already exists", dst_abspath.c_str());
            break;
        }

        hresult = SHCreateItemFromParsingName(
            src_abspath.c_str(), // [in] pszPath
            NULL,                // [in] pbc : Optional. A pointer to a bind context used to pass 
                                 // parameters as inputs and outputs to the parsing function
            IID_PPV_ARGS(&shellitem_src) // [in] riid
        );
        if (FAILED(hresult)) {
            dprintf("[WinIFileOperation::copyPath] SHCreateItemFromParsingName src_abspath: %S failed : 0x%08x", src_abspath.c_str(), hresult);
            break;
        }

        if ((idx = dst_abspath.rfind(L'\\')) == std::wstring::npos) {
            dprintf("[WinIFileOperation::copyPath] dst_abspath: %S does not contain a backslash", dst_abspath.c_str());
            break;
        }
        dst_absdir = dst_abspath.substr(0, idx);
        dst_name = dst_abspath.substr(idx + 1); // either file or folder

        if (WinPath::isFileW(dst_absdir)) {
            dprintf("[WinIFileOperation::copyPath] dst_absdir : %S is a file", dst_absdir.c_str());
            break;
        }

        // IFileOperation won't created directory for us
        // So we have to create it manually (SHFileOperation is better...)
        if (!WinPath::isDirW(dst_absdir)) {
            if (WinPath::createDirW(dst_absdir)) {
                created_dstdir = true;
                dprintf("[WinIFileOperation::copyPath] created dst_absdir %S because it didn't exist", dst_absdir.c_str());
            }
            else {
                dprintf("[WinIFileOperation::copyPath] creating dst_absdir %S failed");
                break;
            }
        }

        hresult = SHCreateItemFromParsingName(dst_absdir.c_str(), NULL, IID_PPV_ARGS(&shellitem_dstdir));
        if (FAILED(hresult)) {
            dprintf("[WinIFileOperation::copyPath] SHCreateItemFromParsingName dst_absdir: %S failed : 0x%08x", dst_absdir.c_str(), hresult);
            break;
        }

        hresult = fileOperation->SetOperationFlags(FOF_NO_UI);
        if (FAILED(hresult)) {
            dprintf("[WinIFileOperation::copyPath] SetOperationFlags failed : 0x%08x", hresult);
            break;
        }

        hresult = fileOperation->CopyItem(shellitem_src, shellitem_dstdir, dst_name.c_str(), NULL);
        if (FAILED(hresult)) {
            dprintf("[WinIFileOperation::copyPath] CopyItem failed : 0x%08x", hresult);
            break;
        }

        // Actual operation happens here
        hresult = fileOperation->PerformOperations();
        if (FAILED(hresult)) {
            dprintf("[WinIFileOperation::copyPath] PerformOperations failed : 0x%08x", hresult);
            break;
        }

        result = true;
    } while (0);

    if (!result && created_dstdir) {
        if (deletePath(dst_absdir, false)) {
            dprintf("[WinIFileOperation::copyPath] rollback creation of dst_absdir success");
        }
        else {
            dprintf("[WinIFileOperation::copyPath] rollback creation of dst_absdir failed");
        }
    }

    if (shellitem_src) {
        hresult = shellitem_src->Release();
        if (FAILED(hresult)) {
            dprintf("[WinIFileOperation::copyPath] shellitem_src->Release failed : 0x%08x", hresult);
        }
    }
    if (shellitem_dstdir) {
        hresult = shellitem_dstdir->Release();
        if (FAILED(hresult)) {
            dprintf("[WinIFileOperation::copyPath] shellitem_dstdir->Release failed : 0x%08x", hresult);
        }
    }

    return result;
}

bool WinIFileOperation::renamePath(std::wstring& src, std::wstring& dst, bool overwrite) {
    bool result = false;
    HRESULT hresult = E_UNEXPECTED;
    IShellItem* shellitem_src = nullptr;
    std::wstring src_abspath, dst_abspath;
    std::wstring dst_absdir, dst_name;
    size_t idx = 0;
    bool created_dstdir = false;

    do {
        if (!WinPath::getAbsPathW(src, &src_abspath)) {
            dprintf("[WinIFileOperation::renamePath] WinPath::getAbsPathW src: %S failed", src.c_str());
            break;
        }

        if (!WinPath::getAbsPathW(dst, &dst_abspath)) {
            dprintf("[WinIFileOperation::renamePath] WinPath::getAbsPathW dst: %S failed", dst.c_str());
            break;
        }

        if (!overwrite && WinPath::isPathW(dst_abspath)) {
            dprintf("[WinIFileOperation::renamePath] dst_abspath: %S already exists", dst_abspath.c_str());
            break;
        }

        hresult = SHCreateItemFromParsingName(
            src_abspath.c_str(), // [in] pszPath
            NULL,                // [in] pbc : Optional. A pointer to a bind context used to pass 
            // parameters as inputs and outputs to the parsing function
            IID_PPV_ARGS(&shellitem_src) // [in] riid
        );
        if (FAILED(hresult)) {
            dprintf("[WinIFileOperation::renamePath] SHCreateItemFromParsingName src_abspath: %S failed : 0x%08x", src_abspath.c_str(), hresult);
            break;
        }
        
        if ((idx = dst_abspath.rfind(L'\\')) == std::wstring::npos) {
            dprintf("[WinIFileOperation::renamePath] dst_abspath: %S does not contain a backslash", dst_abspath.c_str());
            break;
        }
        dst_absdir = dst_abspath.substr(0, idx);
        dst_name = dst_abspath.substr(idx + 1); // either file or folder

        if (WinPath::isFileW(dst_absdir)) {
            dprintf("[WinIFileOperation::renamePath] dst_absdir : %S is a file", dst_absdir.c_str());
            break;
        }

        // IFileOperation would **HANG** at PerformOperations if dst_absdir doesn't exist
        // So we have to create it manually (SHFileOperation is better...)
        if (!WinPath::isDirW(dst_absdir)) {
            if (WinPath::createDirW(dst_absdir)) {
                created_dstdir = true;
                dprintf("[WinIFileOperation::renamePath] created dst_absdir %S because it didn't exist", dst_absdir.c_str());
            }
            else {
                dprintf("[WinIFileOperation::renamePath] creating dst_absdir %S failed");
                break;
            }
        }

        hresult = fileOperation->SetOperationFlags(FOF_NO_UI);
        if (FAILED(hresult)) {
            dprintf("[WinIFileOperation::renamePath] SetOperationFlags failed : 0x%08x", hresult);
            break;
        }

        hresult = fileOperation->RenameItem(shellitem_src, dst_abspath.c_str(), NULL);
        if (FAILED(hresult)) {
            dprintf("[WinIFileOperation::renamePath] RenameItem failed : 0x%08x", hresult);
            break;
        }

        // Would **HANG** if dst_absdir does not exist. this really sucks
        // Actual operation happens here
        hresult = fileOperation->PerformOperations();
        if (FAILED(hresult)) {
            dprintf("[WinIFileOperation::renamePath] PerformOperations failed : 0x%08x", hresult);
            break;
        }

        result = true;
    } while (0);

    if (!result && created_dstdir) {
        if (deletePath(dst_absdir, false)) {
            dprintf("[WinIFileOperation::copyPath] rollback creation of dst_absdir success");
        }
        else {
            dprintf("[WinIFileOperation::copyPath] rollback creation of dst_absdir failed");
        }
    }

    if (shellitem_src) {
        hresult = shellitem_src->Release();
        if (FAILED(hresult)) {
            dprintf("[WinIFileOperation::renamePath] shellitem_src->Release failed : 0x%08x", hresult);
        }
    }

    return result;
}