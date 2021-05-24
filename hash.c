#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]){
    unsigned long hash = 5381;  
    int c;
    while (c = *argv[1]++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    printf("%lu",hash);

    return 0;
}