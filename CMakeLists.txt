cmake_minimum_required(VERSION 3.9.0)
project(kruskal VERSION 0.1.0 LANGUAGES C)

link_libraries("-static")
if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-ffunction-sections -fdata-sections)
endif()

add_subdirectory(extern/union_find)
add_subdirectory(extern/raylib)

set(BUILD_WITH_DOCS OFF)
set(BUILD_EXAMPLES OFF)

add_executable(kruskal src/main.c)

include(CheckIPOSupported)

if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
    target_link_options(kruskal PRIVATE -Wl,--gc-sections)
endif()

check_ipo_supported(RESULT result OUTPUT output LANGUAGES C)
if(result)
message(STATUS "IPO is supported")
set_property(TARGET kruskal PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
else()
message(WARNING "IPO is not supported: ${output}")
endif()

target_link_libraries(kruskal union_find_static raylib_static)

install(TARGETS kruskal CONFIGURATIONS Release RUNTIME DESTINATION .)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
set (CPACK_NSIS_MENU_LINKS "https://github.com/NilEis/kruskal" "GitHub Repository" "kruskal.exe" "Kruskal's algorithm for MSTs" "Uninstall.exe" "Uninstall")
set (CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
include(CPack)
