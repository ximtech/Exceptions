#include <stdlib.h>
#include <stdio.h>
#include "Exceptions.h"

DEFINE_EXCEPTION(RuntimeException, "Runtime exception.", RuntimeException);
DEFINE_EXCEPTION(NullPointerException, "Null pointer.", RuntimeException);

typedef enum ExceptionStage {
    EXCEPTION_BEGINNING,
    EXCEPTION_TRYING,
    EXCEPTION_CATCHING,
    EXCEPTION_FINALIZING,
    EXCEPTION_DONE
} ExceptionStage;

struct ExceptionContext {
    jmp_buf jump[EXCEPTION_MAX_FRAMES];
    struct Exception exception;
    struct {
        unsigned char stage;
        unsigned char uncaught;
    } frame[EXCEPTION_MAX_FRAMES + 1];
    int frames;
} context = {0};


static const char *UNCAUGHT_ERROR_MESSAGE[] = {"\n\nError: %s (%s)\n\n", "\n\nUncaught %s: %s\n\n    thrown at %s:%d\n\n"};

static void propagateException();


Exception getException() {
    return context.exception;
}

bool isExceptionInstanceOf(const ExceptionType *type) {
    Exception exception = context.exception;
    return exception.type == type || exceptionExtends(exception.type, type);
}

bool tryBlock(const char *file, uint32_t line) {
    if (context.frames >= EXCEPTION_MAX_FRAMES) {
        throwException(&RuntimeException, file, line, "Too many `try` blocks nested.");
    }

    context.frames++;
    context.frame[context.frames].stage = EXCEPTION_BEGINNING;
    context.frame[context.frames].uncaught = 0;
    return setjmp(context.jump[context.frames - 1]) >= 0;
}

bool isInTryBlock() {
    return context.frame[context.frames].stage == EXCEPTION_TRYING;
}

bool isInCatchBlock() {
    return context.frame[context.frames].stage == EXCEPTION_CATCHING;
}

bool isInFinallyBlock() {
    return context.frame[context.frames].stage == EXCEPTION_FINALIZING;
}

bool exceptionHook(bool isInCatch) {
    if (isInCatch) {
        context.frame[context.frames].uncaught = 0;
        return true;
    }

    bool uncaught = context.frame[context.frames].uncaught;

    context.frame[context.frames].stage++;
    if (context.frame[context.frames].stage == EXCEPTION_CATCHING && !uncaught) {
        context.frame[context.frames].stage++;
    }

    if (context.frame[context.frames].stage < EXCEPTION_DONE) {
        return true;
    }

    context.frames--;

    if (uncaught) {
        propagateException();
    }

    return false;
}

bool exceptionExtends(const ExceptionType *child, const ExceptionType *parent) {
    for (; child && child->supertype != child; child = child->supertype) {
        if (child->supertype == parent) {
            return true;
        }
    }
    return false;
}

void throwException(const ExceptionType *exceptionType, const char *file, uint32_t line, const char *message) {
    context.exception.type = (exceptionType ? exceptionType : &NullPointerException);
    context.exception.file = file;
    context.exception.line = line;

    sprintf(context.exception.message, "%.*s", EXCEPTION_MESSAGE_SIZE - 1, (message ? message : context.exception.type->defaultMessage));
    propagateException();
}

static void propagateException() {
    context.frame[context.frames].uncaught = 1;

    if (context.frames > 0) {
        longjmp(context.jump[context.frames - 1], 1);
    }

    if (fprintf(stderr, context.exception.file ? UNCAUGHT_ERROR_MESSAGE[1] : UNCAUGHT_ERROR_MESSAGE[0], context.exception.type->name, context.exception.message, context.exception.file, context.exception.line) > 0) {
        (void) fflush(stderr);
    }
    exit(EXIT_FAILURE);
}
