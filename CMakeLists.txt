cmake_minimum_required(VERSION 3.9.0)
project(
  kruskal
  VERSION 0.1.0
  LANGUAGES C)

if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
  add_compile_options(-ffunction-sections -fdata-sections -Wall)
endif()

if (EMSCRIPTEN)
    # set(SUPPORT_CLANGD OFF CACHE BOOL "" FORCE)
    set(PLATFORM
            "Web"
            CACHE STRING "" FORCE)
    # remove --closure-args="--compilation_level SIMPLE" when
    # https://github.com/emscripten-core/emscripten/issues/16706 is fixed
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --no-heap-copy")
    set(CMAKE_EXECUTABLE_SUFFIX ".html")

    set(LINK_FLAGS CMAKE_EXE_LINKER_FLAGS)

    set(${LINK_FLAGS} "${${LINK_FLAGS}} -sASYNCIFY")
    set(${LINK_FLAGS} "${${LINK_FLAGS}} -sUSE_GLFW=3")
    set(${LINK_FLAGS} "${${LINK_FLAGS}} -sWASM=1")
    set(${LINK_FLAGS} "${${LINK_FLAGS}} -sMINIFY_HTML=1")
    set(${LINK_FLAGS} "${${LINK_FLAGS}} -sENVIRONMENT=web")
    set(${LINK_FLAGS} "${${LINK_FLAGS}} -sFILESYSTEM=1")
    set(${LINK_FLAGS} "${${LINK_FLAGS}} -sALLOW_MEMORY_GROWTH=1")
    set(${LINK_FLAGS} "${${LINK_FLAGS}} --closure 0")
    set(${LINK_FLAGS} "${${LINK_FLAGS}} -flto")
    # set(${LINK_FLAGS} "${${LINK_FLAGS}} -sWASM_ASYNC_COMPILATION")
    set(${LINK_FLAGS}
            "${${LINK_FLAGS}} --shell-file ${CMAKE_CURRENT_SOURCE_DIR}/src/shell.html"
    )

    # Remove the -rdynamic flag because otherwise emscripten does not generate
    # HTML+JS+WASM files, only a non-working and fat HTML
    string(REPLACE "-rdynamic" "" CMAKE_SHARED_LIBRARY_LINK_C_FLAGS
            "${CMAKE_SHARED_LIBRARY_LINK_C_FLAGS}")
endif ()

add_subdirectory(extern/union_find)
add_subdirectory(extern/raylib)

set(BUILD_WITH_DOCS OFF)
set(BUILD_EXAMPLES OFF)

add_executable(kruskal src/main.c)

include(CheckIPOSupported)

if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
  target_link_options(kruskal PRIVATE -Wl,--gc-sections)
endif()

check_ipo_supported(
  RESULT result
  OUTPUT output
  LANGUAGES C)
if(result)
  message(STATUS "IPO is supported")
  set_property(TARGET kruskal PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
else()
  message(WARNING "IPO is not supported: ${output}")
endif()

target_link_libraries(kruskal union_find_static raylib_static)

install(
  TARGETS kruskal
  CONFIGURATIONS Release
  RUNTIME DESTINATION .)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
set(CPACK_NSIS_UNINSTALL_NAME "uninstall")
set(CPACK_NSIS_MENU_LINKS
    "https://github.com/NilEis/kruskal" "GitHub Repository" "kruskal.exe"
    "Kruskal's algorithm for MSTs" "uninstall.exe" "Uninstall")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_NSIS_URL_INFO_ABOUT "https://github.com/NilEis/kruskal")
include(CPack)
