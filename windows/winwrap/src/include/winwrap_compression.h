#pragma once
#ifndef __WINWRAP_COMPRESSION_H__
#define __WINWRAP_COMPRESSION_H__

// Note : This requires Windows 8 or newer
#include <Windows.h>
#include <compressapi.h>

#include <memory>
#include <string>

#include "winwrap_filesystem.h"

/*
* There are 'buffered' mode and 'blocked' mode for Compression API
* 
* 'buffered' mode creates compressed/decompressed buffer at once
* 'blocked' mode creates compressed/decompressed buffer block-by-block
* 
* It's kind of pointless to implement 'blocked' mode for
* XPRESS, XPRESS_HUFF, MSZIP because we can't really configure
* block size for those alogrithms (XPRESS*:1GB, MSZIP: 32KB)
* 
* If we really need to 'block' compress, we can just use chop data up
* ourselves and save uncompressed_size + compressed_size + actual data
* in a row
* 
* In case LZMS we can set block size up to 64 GB (at least 1MB recommended)
* but LZMS block mode requires developer to save uncompresed_size & compressed_size
* for decompression, which is kind of same as above
* of course, LZMS block mode would be more efficient as it will be maintaining
* compression context for whole data though...
* 
* Anyways, just implemented buffered mode for now, as it will be enough for
* most of the cases and it would be better using 'real' compression algorithms
* for large scale compression/decompression...
*/

class WinCompressBuffered {
public:
    // convenience names
    static bool compressXPRESS(const unsigned char* raw, SIZE_T raw_size, std::shared_ptr<unsigned char>* out_compressed, PSIZE_T out_compressed_size);
    static bool compressXPRESS_HUF(const unsigned char* raw, SIZE_T raw_size, std::shared_ptr<unsigned char>* out_compressed, PSIZE_T out_compressed_size);
    static bool compressMSZIP(const unsigned char* raw, SIZE_T raw_size, std::shared_ptr<unsigned char>* out_compressed, PSIZE_T out_compressed_size);
    static bool compressLZMS(const unsigned char* raw, SIZE_T raw_size, std::shared_ptr<unsigned char>* out_compressed, PSIZE_T out_compressed_size);
    static bool decompressXPRESS(const unsigned char* compressed, SIZE_T compressed_size, std::shared_ptr<unsigned char>* out_raw, PSIZE_T out_raw_size);
    static bool decompressXPRESS_HUF(const unsigned char* compressed, SIZE_T compressed_size, std::shared_ptr<unsigned char>* out_raw, PSIZE_T out_raw_size);
    static bool decompressMSZIP(const unsigned char* compressed, SIZE_T compressed_size, std::shared_ptr<unsigned char>* out_raw, PSIZE_T out_raw_size);
    static bool decompressLZMS(const unsigned char* compressed, SIZE_T compressed_size, std::shared_ptr<unsigned char>* out_raw, PSIZE_T out_raw_size);

    /* actual implementation 
    * alg : 
    *   - COMPRESS_ALGORITHM_XPRESS
    *   - COMPRESS_ALGORITHM_XPRESS_HUFF
    *   - COMPRESS_ALGORITHM_MSZIP
    *   - COMPRESS_ALGORITHM_LZMS
    */
    static bool compress(DWORD alg, const unsigned char* raw, SIZE_T raw_size, std::shared_ptr<unsigned char>* out_compressed, PSIZE_T out_compressed_size);
    static bool decompress(DWORD alg, const unsigned char* compressed, SIZE_T compressed_size, std::shared_ptr<unsigned char>* out_raw, PSIZE_T out_raw_size);
private:
};

#endif