// Standard
#include <errno.h>
#include <stdio.h>

// Project
#include "log.h"


int cmd_cat(int argc, char **argv) {
    if (argc != 2) {
        LOG_WARNING("Unexpected number of arguments: %d\n", argc);
        return -1;
    }

    char *name = argv[1];
    FILE *fp = fopen(name, "r");
    if (fp == NULL) {
        LOG_ERROR("Failed to open %s errno=%d\n", name, errno);
        return -1;
    }

    int c = fgetc(fp);
    while (! feof(fp)) {
        printf("%c", c);
        c = fgetc(fp);
    }
    fflush(stdout);

/*
    size_t size = 255;
    char buffer[size];
    while (fgets(buffer, bufferLength, filePointer)) {
        print_str(buffer);
    }
*/

    fclose(fp);
    return 0;
}
