/***************************************************************************************************
 *
 *
 *
 *
 *
 ***************************************************************************************************
 *
 * Created By: Jared Fowler
 * Date: July 23, 2019
 *
 **************************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// -------------------------------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int error = 0;
    size_t fileSize = 0;
    size_t startOffset = 0;
    size_t endOffset = 0;
    size_t outBufferOffset = 0;
    bool notFirstWord = false;
    char outBuffer[128];
    FILE* inputFile = NULL;
    FILE* outputFile = NULL;
    char* contents = NULL;

    if (3 != argc)
    {
        printf("\r\n");
        printf("Usage: DictionaryToCharArray <inputFile> <outputFile>\r\n\n");
        error = -1;
        goto DictionaryToCharArray_END;
    }

    // Open Files

    if (NULL == (inputFile = fopen(argv[1], "r")))
    {
        error = -2;
        goto DictionaryToCharArray_CLEANUP;
    }

    if (NULL == (outputFile = fopen(argv[2], "w")))
    {
        error = -3;
        goto DictionaryToCharArray_CLEANUP;
    }

    // Get input file size

    fseek(inputFile, 0, SEEK_END);
    fileSize = ftell(inputFile);
    fseek(inputFile, 0, SEEK_SET);

    // Create dynamic memory to hold the contents of input file

    contents = new char[fileSize];
    memset(contents, 0, sizeof(char) * fileSize);

    // Read the input file into the content array

    if (1 != fread((void*)contents, sizeof(char) * fileSize, 1, inputFile))
    {
        error = -4;
        goto DictionaryToCharArray_CLEANUP;
    }

    // Traverse through the content. Each word should be on its own line.

    while (fileSize > startOffset)
    {
        // Find the next end line

        while ('\n' != contents[endOffset])
        {
            ++endOffset;
        }

        // Prepare output buffer

        outBufferOffset = 0;

        if (true == notFirstWord)
        {
            outBuffer[outBufferOffset++] = ',';
            outBuffer[outBufferOffset++] = ' ';
        }
        else
        {
            notFirstWord = true;
        }

        outBuffer[outBufferOffset++] = '\"';

        memcpy((outBuffer + outBufferOffset), (contents + startOffset), (endOffset - startOffset));
        for (size_t i = 0; i < (endOffset - startOffset); i++)
        {
            outBuffer[outBufferOffset + i] = toupper(outBuffer[outBufferOffset + i]);
        }

        outBufferOffset += (endOffset - startOffset);
        outBuffer[outBufferOffset++] = '\"';

        if (1 != fwrite((void*)outBuffer, sizeof(char) * outBufferOffset, 1, outputFile))
        {
            error = -5;
            goto DictionaryToCharArray_CLEANUP;
        }

        // Update offsets

        startOffset = (endOffset + 1);
        endOffset = startOffset;
    }

    DictionaryToCharArray_CLEANUP:

    if (NULL != contents)
    {
        delete [] contents;
        contents = NULL;
    }

    if (NULL != inputFile)
    {
        fclose(inputFile);
        inputFile = NULL;
    }

    if (NULL != outputFile)
    {
        fclose(outputFile);
        outputFile = NULL;
    }

    DictionaryToCharArray_END:

    if (0 != error)
    {
        printf("Error Code: %d\r\n", error);
    }

    return error;
}
