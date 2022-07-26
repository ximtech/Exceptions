# Exceptions

***Forked from*** [exceptions4c](https://github.com/guillermocalvo/exceptions4c)

Lightweight version of library for error handling in C. Suitable for embedded device exceptions handling like in Java

### Add as CPM project dependency

How to add CPM to the project, check the [link](https://github.com/cpm-cmake/CPM.cmake)

```cmake
CPMAddPackage(
        NAME Exceptions
        GITHUB_REPOSITORY ximtech/Exceptions
        GIT_TAG origin/main
)

target_link_libraries(${PROJECT_NAME} Exceptions)
```

### Usage

#### Base usage

```c
#include "Exceptions.h"

int main() {

    try {
        throw(RuntimeException, "Test");
    } catch(RuntimeException) {
        Exception e = getException();
        printf("%s\n", e.message);
    } finally {
        printf("Finally block\n");
    }
}

```

#### Assertions

```c
#include "Exceptions.h"

// default method name for exception handling
void Exception_Handler() {
    Exception e = getException();
    if (isExceptionInstanceOf(RuntimeException)) {
        printf("RuntimeException: %s\n", e.message);
    } else if (isExceptionInstanceOf(NullPointerException)) {
        printf("NullPointerException: %s\n", e.message);
    }
}

int main() {
    int a = 5;
    ASSERT(a < 5, "Wrong!");    // calls exception handler
    
    int *ptr = NULL;
    ASSERT_THROW(ptr != NULL, "NPE", NullPointerException);
}
```

#### Custom Exceptions
```c
#include "Exceptions.h"

DECLARE_EXCEPTION(CustomException);
DEFINE_EXCEPTION(CustomException, "msg", RuntimeException);

int main() {

    try {
        throw(CustomException, "My exception");
    } catch(CustomException) {
        Exception e = getException();
        printf("%s\n", e.message);
    }
}

```



