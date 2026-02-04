cmake_minimum_required(VERSION 3.21)
project(Semy LANGUAGES C VERSION 0.9.0)

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

option(SEMY_BUILD_TESTS "Build tests" OFF)
option(SEMY_CODE_COVERAGE "Toggle code coverage" OFF)
option(SEMY_UNDEFINED_BEHAVIOR_SANITIZER "Toggle undefined behavior sanitizer" OFF)
option(SEMY_ADDRESS_SANITIZER "Toggle address sanitizer" OFF)
option(SEMY_MEMORY_SANITIZER "Toggle address sanitizer" OFF)

# Generate version information.
write_basic_package_version_file(${CMAKE_BINARY_DIR}/SemyConfigVersion.cmake COMPATIBILITY SameMajorVersion)

# Generate configuration for CMake's find package tools.
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/SemyConfig.cmake.in" "${CMAKE_CURRENT_BINARY_DIR}/SemyConfig.cmake" @ONLY)

# Generate the pkg-config file.
configure_file("${CMAKE_SOURCE_DIR}/semy.pc.in" "${CMAKE_BINARY_DIR}/semy.pc" @ONLY)

# Enable development flags.
if (SEMY_CODE_COVERAGE)
    if (NOT CMAKE_C_COMPILER_ID STREQUAL "GNU")
        message(FATAL_ERROR "Code coverage can only be generated with GCC.")
    endif ()
    add_compile_options(-g -fprofile-arcs -ftest-coverage -O0)
    add_link_options(-fprofile-arcs -ftest-coverage -fbranch-probabilities -O0)
elseif (SEMY_UNDEFINED_BEHAVIOR_SANITIZER)
    add_compile_options(-g -fsanitize=undefined)
    add_link_options(-fsanitize=undefined)
elseif (SEMY_ADDRESS_SANITIZER)
    add_compile_options(-g -fsanitize=address)
    add_link_options(-fsanitize=address)
elseif (SEMY_MEMORY_SANITIZER)
    add_compile_options(-O0 -g -fsanitize=memory -fsanitize-memory-track-origins)
    add_link_options(-fsanitize=memory -fsanitize-memory-track-origins)
endif ()

# Register the library.
add_library(semy semy.c semy.h)
set_target_properties(semy PROPERTIES PUBLIC_HEADER semy.h)

# Register the static library.
add_library(semy_static STATIC semy.c semy.h)
set_target_properties(semy_static PROPERTIES PUBLIC_HEADER semy.h)

# Register the command-line interface.
add_executable(semy_cli semy-cli.c)
target_link_libraries(semy_cli semy)
set_target_properties(semy_cli PROPERTIES OUTPUT_NAME "semy")

# Enable compiler flags to detect common issues.
if (CMAKE_C_COMPILER_ID MATCHES "Clang" OR CMAKE_C_COMPILER_ID MATCHES "GNU")
    target_compile_options(semy PRIVATE -pedantic)
    if (CMAKE_C_COMPILER_ID MATCHES "Clang")
        target_compile_options(semy PRIVATE
            -Wall -Wextra -Wconversion -Wno-missing-field-initializers
            -Wdouble-promotion -Wno-unused-parameter -Wno-unused-function -Wno-sign-conversion)
    endif ()
endif ()

# Set installation files.
install(TARGETS semy_cli RUNTIME DESTINATION bin)
install(TARGETS semy LIBRARY DESTINATION lib PUBLIC_HEADER DESTINATION include)
install(TARGETS semy_static ARCHIVE DESTINATION lib PUBLIC_HEADER DESTINATION include)
install(FILES ${PROJECT_BINARY_DIR}/SemyConfig.cmake DESTINATION cmake)
install(FILES ${PROJECT_BINARY_DIR}/SemyConfigVersion.cmake DESTINATION cmake)
install(FILES semy.1 DESTINATION "${CMAKE_INSTALL_MANDIR}/man1" COMPONENT doc)
install(FILES semy.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)

# Register the tests if building from repo checkout.
# The release acrhive omits the unit test.
if (SEMY_BUILD_TESTS)
    include(CTest)
    add_subdirectory(tests)
endif()
