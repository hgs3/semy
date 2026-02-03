include(CheckCSourceCompiles)

find_package(Audition REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..) # For finding semvar.h

add_executable(test_parse test_parse.c ${CMAKE_CURRENT_SOURCE_DIR}/../semy.h)
target_include_directories(test_parse PRIVATE ${AUDITION_INCLUDE_DIR})
target_link_libraries(test_parse semy)
target_link_libraries(test_parse ${AUDITION_LIBRARY})
add_test(parse test_parse)

add_executable(test_compare test_compare.c ${CMAKE_CURRENT_SOURCE_DIR}/../semy.h)
target_include_directories(test_compare PRIVATE ${AUDITION_INCLUDE_DIR})
target_link_libraries(test_compare semy)
target_link_libraries(test_compare ${AUDITION_LIBRARY})
add_test(compare test_compare)

# Check if Clang is available as well as its fuzzer.
# Note that fuzzing is only compatible with address sanitizer.
if (NOT SEMY_UNDEFINED_BEHAVIOR_SANITIZER AND NOT SEMY_MEMORY_SANITIZER)
    if (CMAKE_C_COMPILER_ID STREQUAL "Clang")
        set(CMAKE_REQUIRED_FLAGS -fsanitize=address,fuzzer)
        set(CMAKE_REQUIRED_LINK_OPTIONS -fsanitize=address,fuzzer)
        check_c_source_compiles(
            "
    #include <stddef.h>
    #include <stdint.h>
    int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) { return 0; }
            "
            HAVE_FUZZER
        )
        if (HAVE_FUZZER)
            # These options are common to all fuzz tests.
            add_compile_options(-g -O0 -fsanitize=address,fuzzer)
            add_link_options(-fsanitize=address,fuzzer)
            add_link_options(-fprofile-instr-generate)
            add_executable(fuzz fuzz.c)
            target_link_libraries(fuzz semy)
            add_test(NAME fuzz
                    COMMAND fuzz "${CMAKE_CURRENT_SOURCE_DIR}/corpus" -runs=10000 -use_value_profile=1 -use_cmp=1 -dict=${CMAKE_CURRENT_SOURCE_DIR}/semver.dict)
        endif ()
    endif ()
endif ()
