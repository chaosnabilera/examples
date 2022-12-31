#include <Windows.h>
#include <iostream>

static void sPrintUsage(char* argv_0){
    printf("Usage : %s <path to dll>\n", argv_0);
}

int main(int argc, char** argv){
    HMODULE hlib = NULL;

    if(argc == 2){
        hlib = LoadLibraryA(argv[1]);
        if(hlib == NULL){
            printf("Failed loading %s\n", argv[1]);
            sPrintUsage(argv[0]);
        }
        printf("press enter to exit\n");
        getchar();
    }
    else{
        sPrintUsage(argv[0]);
    }
    return 0;
}