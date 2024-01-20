#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core/engine.h"

int main(int argc, char** argv) {    
    
    InitProgram(argc? atoi(argv[0]): 0);

    return 0;
}