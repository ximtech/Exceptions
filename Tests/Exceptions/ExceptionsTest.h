#pragma once

#include "BaseTestTemplate.h"
#include "Exceptions.h"

DECLARE_EXCEPTION(CustomException);
DEFINE_EXCEPTION(CustomException, "msg", CustomException);

int errorCounter = 0;

void Error_Handler() {
    errorCounter++;
}

static MunitResult testExceptions(const MunitParameter params[], void *testString) {
    int counter = 0;

    try {
                throw(RuntimeException, "Test");
            } catch(RuntimeException) {
                Exception e = getException();
                assert_string_equal("Test", e.message);
            } finally {
                counter++;
            }

    assert_int(1, ==, counter);


    return MUNIT_OK;
}

static MunitResult testNestedExceptions(const MunitParameter params[], void *testString) {
    int counter = 0;

    try {
                throw (RuntimeException, "Test");
            } catch (RuntimeException) {
                counter++;
                Exception e = getException();
                assert_string_equal("Test", e.message);
                try {
                            throw (CustomException, "Nested error");
                        } catch (CustomException) {
                            counter++;
                            Exception ex = getException();
                            assert_string_equal("Nested error", ex.message);
                        } finally {
                            counter++;
                        }

            } finally {
                counter++;
            }

    assert_int(4, ==, counter);

    return MUNIT_OK;
}

static MunitResult testAssertion(const MunitParameter params[], void *testString) {
    ASSERT(false, "msg");
    assert_int(1, ==, errorCounter);
    return MUNIT_OK;
}


static MunitTest exceptionsTests[] = {
        {.name =  "Test exceptions - should correctly handle errors", .test = testExceptions},
        {.name =  "Test exceptions - should correctly handle nested errors", .test = testNestedExceptions},
        {.name =  "Test exceptions - should correctly handle assertion", .test = testAssertion},
        END_OF_TESTS
};

static const MunitSuite exceptionsTestSuite = {
        .prefix = "Exceptions: ",
        .tests = exceptionsTests,
        .suites = NULL,
        .iterations = 1,
        .options = MUNIT_SUITE_OPTION_NONE
};