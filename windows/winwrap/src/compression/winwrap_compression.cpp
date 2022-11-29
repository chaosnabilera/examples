#include <Windows.h>
#include <compressapi.h>

#include "winwrap_compression.h"

#include "dprintf.hpp"

bool WinCompressBuffered::compressXPRESS(const unsigned char* raw, size_t raw_size, std::shared_ptr<unsigned char>* out_compressed, size_t* out_compressed_size) {
    return compress(COMPRESS_ALGORITHM_XPRESS, raw, raw_size, out_compressed, out_compressed_size);
}

bool WinCompressBuffered::compressXPRESS_HUF(const unsigned char* raw, size_t raw_size, std::shared_ptr<unsigned char>* out_compressed, size_t* out_compressed_size) {
    return compress(COMPRESS_ALGORITHM_XPRESS_HUFF, raw, raw_size, out_compressed, out_compressed_size);
}

bool WinCompressBuffered::compressMSZIP(const unsigned char* raw, size_t raw_size, std::shared_ptr<unsigned char>* out_compressed, size_t* out_compressed_size) {
    return compress(COMPRESS_ALGORITHM_MSZIP, raw, raw_size, out_compressed, out_compressed_size);
}

bool WinCompressBuffered::compressLZMS(const unsigned char* raw, size_t raw_size, std::shared_ptr<unsigned char>* out_compressed, size_t* out_compressed_size) {
    return compress(COMPRESS_ALGORITHM_LZMS, raw, raw_size, out_compressed, out_compressed_size);
}

bool WinCompressBuffered::compress(DWORD alg, const unsigned char* raw, size_t raw_size, std::shared_ptr<unsigned char>* out_compressed, size_t* out_compressed_size) {
    bool result = false;
    COMPRESSOR_HANDLE compressor = NULL;
    DWORD getlasterror = 0;
    std::shared_ptr<unsigned char> compressed_buf;
    size_t compressed_buf_len = 0;
    size_t compress_result_len = 0;

    do {
        if (!CreateCompressor(alg, nullptr, &compressor)) {
            dprintf("[WinCompressBuffered::compress] CreateCompressor failed");
            break;
        }
        if (Compress(compressor, raw, raw_size, nullptr, 0, &compressed_buf_len)) {
            dprintf("[WinCompressBuffered::compress] First Compress supposed to fail but didn't");
            break;
        }
        if ((getlasterror = GetLastError()) != ERROR_INSUFFICIENT_BUFFER) {
            dprintf("[WinCompressBuffered::compress] First GetLastError supposed to return ERROR_INSUFFICIENT_BUFFER but didn't");
            break;
        }

        compressed_buf = std::shared_ptr<unsigned char>(new unsigned char[compressed_buf_len], std::default_delete<unsigned char[]>());
        
        if (!Compress(compressor, raw, raw_size, compressed_buf.get(), compressed_buf_len, &compress_result_len)) {
            dprintf("[WinCompressBuffered::compress] Compress failed");
            break;
        }

        *out_compressed = compressed_buf;
        *out_compressed_size = compress_result_len;
        result = true;
    } while (0);

    if (compressor) {
        CloseCompressor(compressor);
    }

    return result;
}