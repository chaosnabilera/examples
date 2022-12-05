#include <Windows.h>

#include <memory>

#include "zlib.h"
#include "winwrap_wincrypt.h"

const DWORD TEST_BUF_SIZE = 128 * 1024 * 1024;

bool WWCreateSemiRandomBuffer(std::shared_ptr<unsigned char>* buf, size_t buf_size);

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

int main(int argc, char** argv) {
    std::shared_ptr<BYTE> raw_buffer = std::shared_ptr<BYTE>(new BYTE[TEST_BUF_SIZE], std::default_delete<BYTE[]>());
    // zlib requires compressed_buffer size to be at least 0.1% larger than sourceLen plus 12 bytes
    std::shared_ptr<BYTE> compressed_buffer = std::shared_ptr<BYTE>(new BYTE[TEST_BUF_SIZE*2], std::default_delete<BYTE[]>());
    std::shared_ptr<BYTE> decompressed_buffer = std::shared_ptr<BYTE>(new BYTE[TEST_BUF_SIZE], std::default_delete<BYTE[]>());

    const DWORD compressed_buffer_size = TEST_BUF_SIZE * 2;
    DWORD compressed_size = 0;
    DWORD decompressed_size = 0;

    printf("\n");
    printf("[main] Test begin\n");
    
    for (int i = 0; i < 100; ++i) {
        do {
            if (!WWCreateSemiRandomBuffer(&raw_buffer, TEST_BUF_SIZE)) {
                printf("[main] Failed to create raw_buffer\n");
                return 0;
            }
            compressed_size = compressed_buffer_size; // input : buffer size. output: actual compressed size
            if (compress(compressed_buffer.get(), &compressed_size, raw_buffer.get(), TEST_BUF_SIZE) != Z_OK) {
                printf("[main] Failed to compress\n");
                return 0;
            }
            decompressed_size = TEST_BUF_SIZE; // input : buffer size. output: actual compressed size
            if (uncompress(decompressed_buffer.get(), &decompressed_size, compressed_buffer.get(), compressed_size) != Z_OK) {
                printf("[main] Failed to decompress\n");
                return 0;
            }
            if (memcmp(raw_buffer.get(), decompressed_buffer.get(), TEST_BUF_SIZE)) {
                printf("[main] Decompressed buffer is different from original\n");
                return 0;
            }
            printf("%3d : compressed : %u    decompressed : %u    (%lf)\n", i, compressed_size, decompressed_size, ((double)compressed_size)/((double)decompressed_size));
            
        } while (0);
    }
    printf("\n");
    printf("[main] Test end\n");
}