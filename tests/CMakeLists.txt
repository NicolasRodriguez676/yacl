cmake_minimum_required(VERSION 3.22)

option(TEST-BASIC   "enable basic tests"    ON)
option(TEST-TOKEN   "enable token test"     ON)
option(TEST-STRESS  "enable stress test"    ON)
option(TEST-OVERRUN "enable overrun tests"  ON)
option(TEST-PRINT   "enable print tests"    ON)

enable_testing()

if (${TEST-BASIC})
    add_subdirectory(basic)
endif()

if (${TEST-TOKEN})
    add_subdirectory(token)
endif()

if (${TEST-STRESS})
    add_subdirectory(stress)
endif()

if (${TEST-OVERRUN})
    add_subdirectory(overrun)
endif()

if (${TEST-PRINT})
    add_subdirectory(print)
endif()
