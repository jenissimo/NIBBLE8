#include "error_handling.h"
#include <stdio.h>
#include <string.h>
#include "debug/debug.h"

const ErrorInfo ErrorTable[ERROR_COUNT] = {
    {ERROR_CART_NOT_FOUND, "cart %s not found"},
    {ERROR_FILE_NOT_FOUND, "file not found: %s"},
    {ERROR_INVALID_FORMAT, "invalid format in file: %s"},
    {ERROR_MEMORY_ALLOCATION_FAILED, "memory allocation failed: %s"},
};

const char *get_error_text(ErrorCode code, const char *context)
{
    static char error_buffer[256]; // Buffer to store the formatted error message

    if (code >= ERROR_COUNT)
    {
        DEBUG_LOG("Unknown error code: %d", code);
        snprintf(error_buffer, sizeof(error_buffer), "unknown error code: %d", code);
    }
    else
    {
        const ErrorInfo *error_info = &ErrorTable[code];
        DEBUG_LOG("Error code: %d, message: %s", code, error_info->message);
        snprintf(error_buffer, sizeof(error_buffer), error_info->message, context);
    }

    return error_buffer;
}