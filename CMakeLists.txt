

########################################################
# Set up
########################################################
cmake_minimum_required(VERSION 3.14)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS -std=c++17)

PROJECT(
        TinyKV
        VERSION 2023.05
        LANGUAGES C CXX
)

MESSAGE("The environment system : ${CMAKE_SYSTEM_NAME}")

SET(THREADS_PREFER_PTHREAD_FLAG ON)
FIND_PACKAGE(Threads REQUIRED)

set(TINYKV_BUILD_SUPPORT_DIR  "${CMAKE_SOURCE_DIR}/build_support")

#############################################
## Include
#############################################
SET(TINYKV_SRC_DIR ${PROJECT_SOURCE_DIR}/src)
SET(TINYKV_SRC_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/src/include)
SET(TINYKV_SRC_TEST_DIR ${PROJECT_SOURCE_DIR}/test)
SET(TINYKV_SRC_UTIL_DIR ${PROJECT_SOURCE_DIR}/src/util)
#SET(TINYKV_SRC_DEMO_DIR ${PROJECT_SOURCE_DIR}/src)


include_directories(${TINYKV_SRC_INCLUDE_DIR})
message(${TINYKV_SRC_INCLUDE_DIR})

#############################################
## Build
#############################################
set(CMAKE_COMPILER_FLAG -O3 -Wall -g -Werror)

# Build the util_library
FILE(GLOB TINYKV_UTIL_SOURCE RELATIVE ${CMAKE_SOURCE_DIR} "src/util/*.cc")
ADD_LIBRARY(tinykv_util ${TINYKV_UTIL_SOURCE})
TARGET_LINK_LIBRARIES(tinykv_util Threads::Threads)
TARGET_COMPILE_OPTIONS(tinykv_util PRIVATE ${CMAKE_COMPILE_FLAG})
target_include_directories(
        tinykv_util
        PUBLIC ${TINYKV_SRC_INCLUDE_DIR}        #将include目录暴露出来
)

# Build the memory_pool_library
FILE(GLOB TINYKV_MEMORY_SOURCE RELATIVE ${CMAKE_SOURCE_DIR} "src/memory/*.cc")
ADD_LIBRARY(tinykv_memory ${TINYKV_MEMORY_SOURCE})
TARGET_LINK_LIBRARIES(tinykv_memory Threads::Threads)
TARGET_COMPILE_OPTIONS(tinykv_memory PRIVATE ${CMAKE_COMPILE_FLAG})
target_include_directories(
        tinykv_memory
        PUBLIC ${TINYKV_SRC_INCLUDE_DIR}
)

# Build the bloomfilter_library
FILE(GLOB TINYKV_FILTER_SOURCE RELATIVE ${CMAKE_SOURCE_DIR} "src/filter/*.cc")
ADD_LIBRARY(tinykv_filter ${TINYKV_FILTER_SOURCE})
TARGET_LINK_LIBRARIES(tinykv_filter Threads::Threads tinykv_util)
TARGET_COMPILE_OPTIONS(tinykv_filter PRIVATE ${CMAKE_COMPILE_FLAG})
target_include_directories(
        tinykv_filter
        PUBLIC ${TINYKV_SRC_INCLUDE_DIR}
)

# Build the wal_library
FILE(GLOB TINYKV_WAL_SOURCE RELATIVE ${CMAKE_SOURCE_DIR} "src/wal/*.cc")
ADD_LIBRARY(tinykv_wal ${TINYKV_FILTER_SOURCE})
TARGET_LINK_LIBRARIES(tinykv_wal Threads::Threads tinykv_util)
TARGET_COMPILE_OPTIONS(tinykv_wal PRIVATE ${CMAKE_COMPILE_FLAG})
target_include_directories(
        tinykv_wal
        PUBLIC ${TINYKV_SRC_INCLUDE_DIR}
)


#############################################
## Test (Doctest)
#############################################

# test frame
include(FetchContent)
FetchContent_Declare(
        doctest
        GIT_REPOSITORY https://gitee.com/acking-you/doctest.git
        GIT_TAG v2.4.9
        GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(doctest)

ADD_EXECUTABLE(memory_pool_test ${TINYKV_SRC_TEST_DIR}/memory/memory_pool_test.cc)
target_link_libraries(memory_pool_test doctest_with_main tinykv_memory tinykv_util)

ADD_EXECUTABLE(bloomfilter_test ${TINYKV_SRC_TEST_DIR}/bloomfilter/bloomfilter_test.cc)
target_link_libraries(bloomfilter_test doctest_with_main tinykv_filter)













