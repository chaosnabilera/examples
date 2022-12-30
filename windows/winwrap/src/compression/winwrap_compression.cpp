#include <Windows.h>
#include <compressapi.h>

#include "winwrap_compression.h"

#include "dprintf.hpp"

bool WinCompressBuffered::compressXPRESS(const unsigned char* raw, SIZE_T raw_size, std::shared_ptr<unsigned char>* out_compressed, PSIZE_T out_compressed_size) {
    return compress(COMPRESS_ALGORITHM_XPRESS, raw, raw_size, out_compressed, out_compressed_size);
}

bool WinCompressBuffered::compressXPRESS_HUF(const unsigned char* raw, SIZE_T raw_size, std::shared_ptr<unsigned char>* out_compressed, PSIZE_T out_compressed_size) {
    return compress(COMPRESS_ALGORITHM_XPRESS_HUFF, raw, raw_size, out_compressed, out_compressed_size);
}

bool WinCompressBuffered::compressMSZIP(const unsigned char* raw, SIZE_T raw_size, std::shared_ptr<unsigned char>* out_compressed, PSIZE_T out_compressed_size) {
    return compress(COMPRESS_ALGORITHM_MSZIP, raw, raw_size, out_compressed, out_compressed_size);
}

bool WinCompressBuffered::compressLZMS(const unsigned char* raw, SIZE_T raw_size, std::shared_ptr<unsigned char>* out_compressed, PSIZE_T out_compressed_size) {
    return compress(COMPRESS_ALGORITHM_LZMS, raw, raw_size, out_compressed, out_compressed_size);
}

bool WinCompressBuffered::compress(DWORD alg, const unsigned char* raw, SIZE_T raw_size, std::shared_ptr<unsigned char>* out_compressed, PSIZE_T out_compressed_size) {
    bool result = false;
    COMPRESSOR_HANDLE compressor = NULL;
    DWORD getlasterror = 0;
    std::shared_ptr<unsigned char> compressed_buf;
    SIZE_T compressed_buf_len = 0;
    SIZE_T compress_result_len = 0;

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

bool WinCompressBuffered::decompressXPRESS(const unsigned char* compressed, SIZE_T compressed_size, std::shared_ptr<unsigned char>* out_raw, PSIZE_T out_raw_size) {
    return decompress(COMPRESS_ALGORITHM_XPRESS, compressed, compressed_size, out_raw, out_raw_size);
}

bool WinCompressBuffered::decompressXPRESS_HUF(const unsigned char* compressed, SIZE_T compressed_size, std::shared_ptr<unsigned char>* out_raw, PSIZE_T out_raw_size) {
    return decompress(COMPRESS_ALGORITHM_XPRESS_HUFF, compressed, compressed_size, out_raw, out_raw_size);
}

bool WinCompressBuffered::decompressMSZIP(const unsigned char* compressed, SIZE_T compressed_size, std::shared_ptr<unsigned char>* out_raw, PSIZE_T out_raw_size) {
    return decompress(COMPRESS_ALGORITHM_MSZIP, compressed, compressed_size, out_raw, out_raw_size);
}

bool WinCompressBuffered::decompressLZMS(const unsigned char* compressed, SIZE_T compressed_size, std::shared_ptr<unsigned char>* out_raw, PSIZE_T out_raw_size) {
    return decompress(COMPRESS_ALGORITHM_LZMS, compressed, compressed_size, out_raw, out_raw_size);
}

bool WinCompressBuffered::decompress(DWORD alg, const unsigned char* compressed, SIZE_T compressed_size, std::shared_ptr<unsigned char>* out_raw, PSIZE_T out_raw_size) {
    bool result = false;
    DECOMPRESSOR_HANDLE decompressor = NULL;
    DWORD getlasterror = 0;
    std::shared_ptr<unsigned char> decompressed_buf;
    SIZE_T decompressed_buf_len = 0;
    SIZE_T decompress_result_len = 0;

    do {
        if (!CreateDecompressor(alg, nullptr, &decompressor)) {
            dprintf("[WinCompressBuffered::decompress] CreateDecompressor failed");
            break;
        }
        if (Decompress(decompressor, compressed, compressed_size, nullptr, 0, &decompressed_buf_len)) {
            dprintf("[WinCompressBuffered::decompress] First Decompress supposed to fail but didn't");
            break;
        }
        if ((getlasterror = GetLastError()) != ERROR_INSUFFICIENT_BUFFER) {
            dprintf("[WinCompressBuffered::decompress] First GetLastError supposed to return ERROR_INSUFFICIENT_BUFFER but didn't");
            break;
        }

        decompressed_buf = std::shared_ptr<unsigned char>(new unsigned char[decompressed_buf_len], std::default_delete<unsigned char[]>());

        if (!Decompress(decompressor, compressed, compressed_size, decompressed_buf.get(), decompressed_buf_len, &decompress_result_len)) {
            dprintf("[WinCompressBuffered::decompress] Decompress failed");
            break;
        }

        *out_raw = decompressed_buf;
        *out_raw_size = decompress_result_len;
        result = true;
    } while (0);

    if (decompressor) {
        CloseDecompressor(decompressor);
    }

    return result;
}