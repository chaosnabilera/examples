#include "winwrap_compression.h"
#include "winwrap_wincrypt.h"

#include <vector>
#include <string>

static const size_t DEFAULT_RAW_SIZE = 128 * 1024 * 1024;

bool WWTestCompression();
bool WWTestCompressionAlgorithm(DWORD alg, const std::string& alg_name);
bool WWCreateSemiRandomBuffer(std::shared_ptr<unsigned char>* buf, size_t buf_size);

bool WWTestCompression() {
    std::vector<std::pair<DWORD, std::string>> test_list;
    test_list.push_back(std::make_pair(COMPRESS_ALGORITHM_XPRESS, "XPRESS"));
    test_list.push_back(std::make_pair(COMPRESS_ALGORITHM_XPRESS_HUFF, "XPRESS_HUFF"));
    test_list.push_back(std::make_pair(COMPRESS_ALGORITHM_MSZIP, "MSZIP"));
    test_list.push_back(std::make_pair(COMPRESS_ALGORITHM_LZMS, "LZMS"));

    for (auto& p : test_list) {
        if (!WWTestCompressionAlgorithm(p.first, p.second))
            return false;
    }
    return true;
}

bool WWTestCompressionAlgorithm(DWORD alg, const std::string& alg_name) {
    std::shared_ptr<unsigned char> raw;
    size_t raw_size = DEFAULT_RAW_SIZE;
    std::shared_ptr<unsigned char> compressed;
    size_t compressed_size = 0;
    std::shared_ptr<unsigned char> decompressed;
    size_t decompressed_size = 0;

    double raw_size_total = 0;
    double compressed_size_total = 0;

    raw = std::shared_ptr<unsigned char>(new unsigned char[raw_size], std::default_delete<unsigned char[]>());
    
    for (int i = 0; i < 10; ++i) {
        printf("*");
        
        if (!WWCreateSemiRandomBuffer(&raw, raw_size)) {
            printf("[WWTestCompressionAlgorithm] WWCreateSemiRandomBuffer failed\n");
            return false;
        }

        /*
        if (!WinCryptGenRandom(raw.get(), raw_size)) {
            printf("[WWTestCompressionAlgorithm] WinCryptGenRandom failed\n");
            return false;
        }
        */

        if (!WinCompressBuffered::compress(alg, raw.get(), raw_size, &compressed, &compressed_size)) {
            printf("[WWTestCompressionAlgorithm] WinCompressBuffered::compress %s failed\n", alg_name.c_str());
            return false;
        }

        if (!WinCompressBuffered::decompress(alg, compressed.get(), compressed_size, &decompressed, &decompressed_size)) {
            printf("[WWTestCompressionAlgorithm] WinCompressBuffered::decompress %s failed\n", alg_name.c_str());
            return false;
        }

        if (decompressed_size != raw_size || memcmp(raw.get(), decompressed.get(), raw_size) != 0) {
            printf("[WWTestCompressionAlgorithm] verification failed\n");
            return false;
        }

        raw_size_total += raw_size;
        compressed_size_total += compressed_size;
    }

    printf("\n[WWTestCompressionAlgorithm] %12s: ratio = %.4lf (%10lf/%10lf)\n", alg_name.c_str(), compressed_size_total / raw_size_total, compressed_size_total, raw_size_total);

    return true;
}

bool WWCreateSemiRandomBuffer(std::shared_ptr<unsigned char>* buf, size_t buf_size) {
    BYTE rand_buf[3] = { 0 };
    char rand_char;
    unsigned short run_length;
    size_t filled = 0;

    while (filled < buf_size) {
        if (!WinCryptGenRandom(rand_buf, sizeof(rand_buf))) {
            printf("[WWCreateSemiRandomBuffer] Failed to generate rand_buf\n");
            return false;
        }
        rand_char = rand_buf[0];
        run_length = *((unsigned short*)&rand_buf[1]);
        
        if (run_length > buf_size - filled) {
            run_length = (unsigned short)(buf_size - filled);
        }

        memset(buf->get() + filled, rand_char, run_length);
        
        filled += run_length;
    }
    
    return true;
}