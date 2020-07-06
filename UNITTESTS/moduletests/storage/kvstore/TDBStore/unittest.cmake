
####################
# UNIT TESTS
####################

set(unittest-includes ${unittest-includes}
  .
  ..
  ../features/frameworks/mbed-trace/mbed-trace
)

set(unittest-sources
  ../storage/blockdevice/FlashSimBlockDevice.cpp
  ../storage/blockdevice/HeapBlockDevice.cpp
  ../storage/blockdevice/BufferedBlockDevice.cpp
  ../storage/kvstore/tdbstore/TDBStore.cpp
  ../features/frameworks/mbed-trace/source/mbed_trace.c
  stubs/mbed_atomic_stub.c
  stubs/mbed_assert_stub.cpp
  stubs/mbed_error.c
)

set(unittest-test-sources
  moduletests/storage/kvstore/TDBStore/moduletest.cpp
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
