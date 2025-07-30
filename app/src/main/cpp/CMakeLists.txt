cmake_minimum_required(VERSION 3.18.1)

project(ufs-tools VERSION 1.0.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# pre-processing def required for building
add_definitions(-D_GNU_SOURCE)
add_definitions(-DANDROID_BUILD)

# optimization flags for increased performance
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2")

include_directories(.)

# finding android specific libs
find_library(log-lib log)
find_library(android-lib android)

# shared source files between two build targets
set(COMMON_SOURCES
    uic.c
    query.c
    ufs_bsg.c
    common.c
)

# lsufs shared library sources
set(LSUFS_LIB_SOURCES
    ${COMMON_SOURCES}
    shell.c
    lsufs.c
    jni_wrapper_lsufs.c
)

# ufseom shared library sources
set(UFSEOM_LIB_SOURCES
    ${COMMON_SOURCES}
    ufs_eom.c
    json.c
    jni_wrapper_ufseom.c
)

# create .so files
add_library(lsufs_lib SHARED ${LSUFS_LIB_SOURCES})
add_library(ufseom_lib SHARED ${UFSEOM_LIB_SOURCES})

set_target_properties(lsufs_lib PROPERTIES OUTPUT_NAME "lsufs")
set_target_properties(ufseom_lib PROPERTIES OUTPUT_NAME "ufseom")

# necessary linking for Android builds
target_link_libraries(lsufs_lib ${log-lib})
target_link_libraries(ufseom_lib ${log-lib})

if(android-lib)
    target_link_libraries(lsufs_lib ${android-lib})
    target_link_libraries(ufseom_lib ${android-lib})
endif()

# options for building shared libraries and removing unused code
target_compile_options(lsufs_lib PRIVATE
        -fPIC
        -ffunction-sections
        -fdata-sections
        -Wno-unused-parameter
)

target_compile_options(ufseom_lib PRIVATE
        -fPIC
        -ffunction-sections
        -fdata-sections
        -Wno-unused-parameter
)

# removes unused code and data sections to reduce binary size
target_link_options(lsufs_lib PRIVATE
        -Wl,--gc-sections
        -Wl,--strip-all
)

target_link_options(ufseom_lib PRIVATE
        -Wl,--gc-sections
        -Wl,--strip-all
)
