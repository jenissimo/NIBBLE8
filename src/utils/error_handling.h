#ifndef ERROR_HANDLING_H
#define ERROR_HANDLING_H

typedef enum {
    ERROR_SUCCESS = 0,
    ERROR_CART_NOT_FOUND,
    ERROR_FILE_NOT_FOUND,
    ERROR_INVALID_FORMAT,
    ERROR_COUNT
} ErrorCode;

typedef struct {
    ErrorCode code;
    const char *message;
} ErrorInfo;

extern const ErrorInfo ErrorTable[ERROR_COUNT];

const char *get_error_text(ErrorCode code, const char *context);

#endif // ERROR_HANDLING_H
