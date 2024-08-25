cmake_minimum_required (VERSION 3.10)

project (Explore-Ray LANGUAGES C CXX)
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

set (DEBUG_OPTIONS /Zi /Od)
set (RELEASE_OPTIONS /O2)

set (headers
    ${CMAKE_CURRENT_LIST_DIR}/include/raylib/raygui.h
    ${CMAKE_CURRENT_LIST_DIR}/include/iconswatch.h

    ${CMAKE_CURRENT_LIST_DIR}/include/catalog.h
    ${CMAKE_CURRENT_LIST_DIR}/include/partfx.h
    ${CMAKE_CURRENT_LIST_DIR}/include/partviewer.h

    ${CMAKE_CURRENT_LIST_DIR}/include/boids.h
)

set (source
    ${CMAKE_CURRENT_LIST_DIR}/src/main.c
    ${CMAKE_CURRENT_LIST_DIR}/src/iconswatch.c

    ${CMAKE_CURRENT_LIST_DIR}/src/catalog.c
    ${CMAKE_CURRENT_LIST_DIR}/src/partfx.c
    ${CMAKE_CURRENT_LIST_DIR}/src/partviewer.c

    ${CMAKE_CURRENT_LIST_DIR}/src/boids.c
)

set (cppsource
    ${CMAKE_CURRENT_LIST_DIR}/tracy/TracyClient.cpp
)

set (libs
    raylib.lib
    WinMM.lib
    yaml
)

add_executable (Explore-Ray
    ${source}
    ${cppsource}
    ${headers}
)

target_compile_options(Explore-Ray PUBLIC "$<$<CONFIG:DEBUG>:${DEBUG_OPTIONS}>")
target_compile_options(Explore-Ray PUBLIC "$<$<CONFIG:RELEASE>:${RELEASE_OPTIONS}>")
target_compile_definitions(Explore-Ray PRIVATE TRACY_ENABLE)

target_include_directories(Explore-Ray
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/include
        ${CMAKE_CURRENT_LIST_DIR}/include/raylib
        ${CMAKE_CURRENT_LIST_DIR}/include/yaml
        ${CMAKE_CURRENT_LIST_DIR}/tracy
        ${CMAKE_CURRENT_LIST_DIR}/tracy/tracy
)
target_link_directories(Explore-Ray
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/libs
)
target_link_libraries(Explore-Ray ${libs})

set_target_properties(Explore-Ray
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY       "${CMAKE_CURRENT_LIST_DIR}/"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_LIST_DIR}/"
)
