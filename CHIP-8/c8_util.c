#include "c8.h"

#include <stdio.h>
#include <stdlib.h>

c8_word c8_util_file_to_byte_array(const char* filePath, c8_byte** buffer)
{
    FILE* file = NULL;
    c8_word fileSize = 0;

    if (fopen_s(&file, filePath, "rb") || file == NULL)
    {
        *buffer = NULL;
        return 0;
    }

    fseek(file, 0, SEEK_END);
    fileSize = (c8_word)ftell(file);
    rewind(file);

    *buffer = (uint8_t*)malloc(fileSize * sizeof(uint8_t));

    if (*buffer == NULL)
    {
        fclose(file);
        return 0;
    }

    fread_s(*buffer, fileSize * sizeof(uint8_t), fileSize, 1, file);
    fclose(file);

    return fileSize;
}