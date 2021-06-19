#include "c8.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

c8_word c8_util_file_to_byte_array(const char* filePath, c8_byte** buffer)
{
    FILE* file = NULL;
    c8_word fileSize = 0;

    file = fopen(filePath, "rb");
    if (file == NULL)
    {
        *buffer = NULL;
        return 0;
    }

    fseek(file, 0, SEEK_END);
    fileSize = (c8_word)ftell(file);
    rewind(file);

    *buffer = (c8_byte*)malloc(fileSize * sizeof(c8_byte));

    if (*buffer == NULL)
    {
        fclose(file);
        return 0;
    }

    fread(*buffer, fileSize * sizeof(c8_byte), 1, file);
    fclose(file);

    return fileSize;
}