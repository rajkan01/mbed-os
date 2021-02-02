set(LIB_NAME MbedOS)

# Setup c++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MINGW)
  # enable PRIx formatting globally
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D__STDC_FORMAT_MACROS")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__STDC_FORMAT_MACROS")
endif (MINGW)

find_package(Threads)

####################
# GTEST
####################
include(FetchContent)
# Download and unpack googletest
if(NOT GTEST_FOUND)
    FetchContent_Declare(googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.10.0
    )
    FetchContent_MakeAvailable(googletest)
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

####################
# TESTING
####################

include(CTest)

set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES
  "${CMAKE_BINARY_DIR}/Testing"
)

####################
# CODE COVERAGE SETUP
####################

if (COVERAGE)

  if (NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(WARNING "Non-debug build may result misleading code coverage results.")
  endif()

  # Append coverage compiler flags
  set(COVERAGE_COMPILER_FLAGS "-g -O0 --coverage")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COVERAGE_COMPILER_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COVERAGE_COMPILER_FLAGS}")

endif(COVERAGE)

if (VALGRIND)
  find_program(MEMORYCHECK_COMMAND valgrind)
endif(VALGRIND)

####################
# UNIT TESTS
####################

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DUNITTEST")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUNITTEST")

# Set include dirs.
set(unittest-includes-base
  "./target_h"
  "../events/tests/UNITTESTS/target_h"
  "../events/tests/UNITTESTS/target_h/equeue"
  "./target_h/platform"
  "./target_h/platform/cxxsupport"
  "./target_h/drivers"
  "./target_h/rtos/include"
  "./stubs"
  "../"
  "../features"
  "../platform/include"
  "../platform/include/platform"
  "../platform/mbed-trace/include"
  "../storage/filesystem/littlefs/include"
  "../storage/filesystem/fat/include"
  "../storage/blockdevice/include"
  "../storage/filesystem/include"
  "../storage/kvstore/include"
  "../storage/kvstore/kv_config"
  "../storage/kvstore/kv_config/include"
  "../storage/kvstore/tdbstore/include"
  "../storage/kvstore/filesystemstore/include"
  "../storage/kvstore/kvstore_global_api/include"
  "../drivers"
  "../drivers/include"
  "../drivers/include/drivers"
  "../drivers/include/drivers/internal"
  "../hal"
  "../hal/include"
  "../events/include"
  "../events/include/events/internal"
  "../events/source"
  "../rtos/include"
  "../features/frameworks"
  "../connectivity/libraries/nanostack-libservice"
  "../connectivity/libraries/nanostack-libservice/mbed-client-libservice"
  "../connectivity/netsocket/include"
  "../features/filesystem/fat"
  "../features/filesystem/fat/ChaN"
  "../features/filesystem/bd"
  "../features/filesystem/"
  "../features/filesystem/littlefs"
  "../features/filesystem/littlefs/littlefs"
  "../connectivity/cellular/include/cellular/framework/API"
  "../connectivity/cellular/include/cellular/framework/AT"
  "../connectivity/cellular/include/cellular/framework/device"
  "../connectivity/cellular/include/cellular/framework"
  "../connectivity/cellular/include/cellular/framework/common"
  "../connectivity"
  "../connectivity/lorawan/include/lorawan"
  "../connectivity/lorawan/lorastack"
  "../connectivity/lorawan/lorastack/mac"
  "../connectivity/lorawan/lorastack/phy"
  "../connectivity/lorawan"
  "../connectivity/mbedtls"
  "../connectivity/mbedtls/include"
)

# Create a list for test suites.
set(TEST_SUITES)

# Get all matched tests.
file(GLOB_RECURSE unittest-file-list
  "../unittest.cmake" # matches any ../**/unittest.cmake
)

if ("${unittest-file-list}" STREQUAL "")
  message(FATAL_ERROR "No tests found. Exiting...")
endif()

# Create unit test targets
foreach(testfile ${unittest-file-list})
  ####################
  # DEFINE TARGETS
  ####################

  # Init file lists.
  set(unittest-includes ${unittest-includes-base})
  set(unittest-sources)
  set(unittest-test-sources)
  set(unittest-test-flags)

  # Get source files
  include("${testfile}")

  get_filename_component(TEST_SUITE_DIR ${testfile} DIRECTORY)

  file(RELATIVE_PATH
       TEST_SUITE_NAME # output
       "${PROJECT_SOURCE_DIR}" # root
       ${TEST_SUITE_DIR} #abs dirpath
  )

  string(REGEX REPLACE "/|\\\\" "-" TEST_SUITE_NAME ${TEST_SUITE_NAME})

  set(TEST_SUITES ${TEST_SUITES} ${TEST_SUITE_NAME})

  set(LIBS_TO_BE_LINKED gmock_main ${CMAKE_THREAD_LIBS_INIT})

  # Build directories list
  set(BUILD_DIRECTORIES)

  if (unittest-sources)
    # Create the testable static library.
    add_library("${TEST_SUITE_NAME}.${LIB_NAME}" STATIC ${unittest-sources})
    target_include_directories("${TEST_SUITE_NAME}.${LIB_NAME}" PRIVATE
      ${unittest-includes})
    target_compile_options("${TEST_SUITE_NAME}.${LIB_NAME}" PRIVATE
      ${unittest-test-flags})
    set(LIBS_TO_BE_LINKED ${LIBS_TO_BE_LINKED} "${TEST_SUITE_NAME}.${LIB_NAME}")

    # Append lib build directory to list
    list(APPEND BUILD_DIRECTORIES "./CMakeFiles/${TEST_SUITE_NAME}.${LIB_NAME}.dir")
  endif(unittest-sources)

  if (unittest-test-sources)
    # Create the executable.
    add_executable(${TEST_SUITE_NAME} ${unittest-test-sources})

    target_include_directories(${TEST_SUITE_NAME} PRIVATE
      ${unittest-includes})
    target_compile_options(${TEST_SUITE_NAME} PRIVATE
      ${unittest-test-flags})

    # Link the executable with the libraries.
    target_link_libraries(${TEST_SUITE_NAME} ${LIBS_TO_BE_LINKED})

    add_test(NAME "${TEST_SUITE_NAME}" COMMAND ${TEST_SUITE_NAME})

    # Append test build directory to list
    list(APPEND BUILD_DIRECTORIES "./CMakeFiles/${TEST_SUITE_NAME}.dir")
  else()
    message(WARNING "No test source files found for ${TEST_SUITE_NAME}.\n")
  endif(unittest-test-sources)
endforeach(testfile)

