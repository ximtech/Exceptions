#pragma once

#include <stddef.h>
#include <setjmp.h>
#include <stdbool.h>
#include <stdint.h>

/* Maximum number of nested `try` blocks */
#ifndef EXCEPTION_MAX_FRAMES
# define EXCEPTION_MAX_FRAMES 16
#endif

/* Maximum length (in bytes) of an exception message */
#ifndef EXCEPTION_MESSAGE_SIZE
# define EXCEPTION_MESSAGE_SIZE 128
#endif

/* Controls whether file/line info is attached to exceptions */
#ifndef EXCEPTION_DEBUG_ENABLED
# define EXCEPTION_DEBUG_INFO __FILE__, __LINE__
#else
# define EXCEPTION_DEBUG_INFO NULL, 0
#endif

/* Represents an exception type */
typedef struct ExceptionType {
    const char *name;
    const char *defaultMessage;
    const struct ExceptionType *supertype;
} ExceptionType;

/* Represents an instance of an exception type */
typedef struct Exception {
    char message[EXCEPTION_MESSAGE_SIZE];
    const char *file;
    uint32_t line;
    const ExceptionType *type;
} Exception;


/* Declarations and definitions of exception types */
#define DECLARE_EXCEPTION(name) extern const struct ExceptionType name
#define DEFINE_EXCEPTION(name, defaultMessage, supertype) const struct ExceptionType name = { #name, defaultMessage, &(supertype) }

/* Predefined exception types */
DECLARE_EXCEPTION(RuntimeException);
DECLARE_EXCEPTION(NullPointerException);


#define try if(tryBlock(EXCEPTION_DEBUG_INFO)) while(exceptionHook(false)) if(isInTryBlock())
#define catch(type) else if(isInCatchBlock() && isExceptionInstanceOf(&(type)) && exceptionHook(true))
#define finally else if(isInFinallyBlock() == true)
#define throw(type, message) throwException(&(type), EXCEPTION_DEBUG_INFO, message)


#define ASSERT(expr, msg)  ASSERT_THROW(expr, msg, RuntimeException)
#define ASSERT_THROW(expr, msg, errorType)  \
  do {                                      \
      try {                                 \
         if (!(expr)) {                     \
            throw(errorType, (msg));        \
         }                                  \
      } catch(errorType) {                  \
            Error_Handler();                \
      }                                     \
  } while (0);                              \


/* Retrieve current thrown exception */
Exception getException();
/* Returns whether current exception is of a given type */
bool isExceptionInstanceOf(const ExceptionType *type);

/* This functions must be called only via try, catch, finally and throw */
bool tryBlock(const char *file, uint32_t line);
bool isInTryBlock();
bool isInCatchBlock();
bool isInFinallyBlock();
bool exceptionHook(bool isInCatch);
bool exceptionExtends(const ExceptionType *child, const ExceptionType *parent);
void throwException(const ExceptionType *exceptionType, const char *file, uint32_t line, const char *message);
