cmake_minimum_required(VERSION 3.10)
project(lmdb_write_test C)
set(TARGET ${PROJECT_NAME})

set(CMAKE_C_STANDARD 99)

set(SOURCES main.c)


find_package(PkgConfig REQUIRED)
pkg_check_modules(lmdb REQUIRED lmdb)



link_directories(
        ${lmdb_LIBRARY_DIRS}
)

include_directories(
        ${lmdb_INCLUDE_DIRS}
)

add_compile_options(
        ${lmdb_CFLAGS}
)

add_executable(${TARGET} ${SOURCES})

target_link_libraries(${TARGET}
    ${lmdb_LIBRARIES}
)
