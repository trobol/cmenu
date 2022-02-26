#include <stdio.h>

int read_ttf(const char* path); 

int main(int argc, char *argv[]) {
    if (argc < 2) { puts("missing path"); return 1; }

    read_ttf(argv[1]);
    

    return 0;

    

}