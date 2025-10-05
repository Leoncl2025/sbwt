cmake_minimum_required(VERSION 3.24)
project(sbwt)

# Always include CTest early so the BUILD_TESTING option is created (default ON)
include(CTest)

# Check Platform
if(CYGWIN)
    add_definitions(
        -DS_IREAD=0000400
        -DS_IWRITE=0000200
    )
endif(CYGWIN)

include_directories(${CMAKE_SOURCE_DIR}/SeqAn-1.1 ${CMAKE_SOURCE_DIR}/src ) # -I flags for compiler include file
#set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w -std=c++11 -Wunused-local-typedefs -fpermissive -O3") # complier flag (Optimal)
#set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w -std=c++11 -Wunused-local-typedefs -fpermissive -Og -g3 -ggdb") # complier flag (Not optimal)
#set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w -std=c++11 -Wunused-local-typedefs -fpermissive -O0 -g") # complier flag (Not optimal at all)

# Set "out-of-source" flags
set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -w -std=c++11 -Wunused-local-typedefs -fpermissive -O3") # complier flag (Optimal)
set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -w -std=c++11 -Wunused-local-typedefs -fpermissive -O0 -g") # complier flag (Not optimal at all)

# Define macro variables
if (CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-DDEBUG_SECONDINDEX=1)
endif()

#####################################
#                                   #
# Include all xx.h and xx.cpp files #
#                                   #
#####################################

file ( GLOB varCC ${CMAKE_SOURCE_DIR}/src/*.cc)
file ( GLOB varC ${CMAKE_SOURCE_DIR}/src/*.c)
file ( GLOB varCPP ${CMAKE_SOURCE_DIR}/src/*.cpp)
# Also include sources in utils subdirectory (e.g., utils/sort.cc)
file ( GLOB varUTILS_CC ${CMAKE_SOURCE_DIR}/src/utils/*.cc)
file ( GLOB varUTILS_CPP ${CMAKE_SOURCE_DIR}/src/utils/*.cpp)

# exclude test_io.cpp to prevent targeting multi-main cpp file
list(REMOVE_ITEM varCC "${CMAKE_SOURCE_DIR}/src/build_index.cc")
list(REMOVE_ITEM varCC "${CMAKE_SOURCE_DIR}/src/count_occ.cc")
list(REMOVE_ITEM varCPP "${CMAKE_SOURCE_DIR}/src/sbwt_aligner.cpp")
list(REMOVE_ITEM varCPP "${CMAKE_SOURCE_DIR}/src/sbwt_test.cpp")

set(SRC_CC ${varCC} ${varUTILS_CC})
set(SRC_C ${varC})
set(SRC_CPP ${varCPP} ${varUTILS_CPP})

add_definitions(
        -DSBWT_VERBOSE
        #-DSBWT_DEBUG
        -DBUILTIN_POPCOUNT
)

add_executable(build_index ${CMAKE_SOURCE_DIR}/src/build_index.cc ${SRC_C} ${SRC_CC} ${SRC_CPP})

add_executable(count_occ ${CMAKE_SOURCE_DIR}/src/count_occ.cc ${SRC_C} ${SRC_CC} ${SRC_CPP})

add_executable(sbwt ${CMAKE_SOURCE_DIR}/src/sbwt_aligner.cpp ${SRC_C} ${SRC_CC} ${SRC_CPP})

add_executable(sbwt_test ${CMAKE_SOURCE_DIR}/src/sbwt_test.cpp ${SRC_C} ${SRC_CC} ${SRC_CPP})

# Unit test for utils/sort
if (BUILD_TESTING)
    add_executable(sort_unit ${CMAKE_SOURCE_DIR}/tests/test_sort.cpp ${SRC_C} ${SRC_CC} ${SRC_CPP})
endif()

# Register tests if requested
if(BUILD_TESTING)
    enable_testing() # Ensure testing targets are generated
    find_package(Python3 COMPONENTS Interpreter REQUIRED)
    add_test(
        NAME e2e_build_index
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_build_index_e2e.py $<TARGET_FILE:build_index>
    )
    add_test(
        NAME e2e_sbwt
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_sbwt_e2e.py $<TARGET_FILE:build_index> $<TARGET_FILE:sbwt>
    )
    set_tests_properties(e2e_build_index PROPERTIES
        PASS_REGULAR_EXPRESSION "All e2e checks passed"
    )
    set_tests_properties(e2e_sbwt PROPERTIES
        PASS_REGULAR_EXPRESSION "All e2e checks passed"
    )

    add_test(
        NAME unit_sort
        COMMAND $<TARGET_FILE:sort_unit>
    )
    set_tests_properties(unit_sort PROPERTIES
        PASS_REGULAR_EXPRESSION "All sort unit tests passed"
    )
endif()
