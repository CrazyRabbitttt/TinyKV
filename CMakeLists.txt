

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

#############################################
## Include
#############################################
SET(TINYKV_SRC_DIR ${PROJECT_SOURCE_DIR}/src)
SET(TINYKV_SRC_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/src/include)

include_directories(${TINYKV_SRC_INCLUDE_DIR})
















