cmake_minimum_required(VERSION 3.28)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(GCC_GLOBAL_COMPILER_SETTINGS -Wall -Wextra -Wfatal-errors -O2)
set(GCC_GLOBAL_LINKER_SETTINGS "-Wl,-rpath,./bin")

project(SpaceCadetPinball)

set(PROJECT_BASE_SOURCE_DIR ${CMAKE_SOURCE_DIR})
set(PROJECT_EXTERNAL_DIR ${PROJECT_BASE_SOURCE_DIR}/external)
set(PROJECT_INSTALL_DIR ${PROJECT_BASE_SOURCE_DIR}/out)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_INSTALL_DIR})

# get SOURCE_FILES
separate_arguments(separate_sources UNIX_COMMAND "${PROJECT_BASE_SOURCE_DIR}/src")
foreach(dir IN LISTS separate_sources)
	message("adding source item: ${dir}")
	if (IS_DIRECTORY ${dir})
		file(GLOB_RECURSE append_target_sources "${dir}/*.cpp")
	else()
		file(GLOB_RECURSE append_target_sources "${dir}")
	endif()
	list(APPEND SOURCE_FILES ${append_target_sources})
endforeach()

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${PROJECT_BASE_SOURCE_DIR}/src
    ${PROJECT_EXTERNAL_DIR}
    )

# Skip pch on foreign code
set_source_files_properties(
        src/imgui.cpp
        src/imgui_sdl.cpp
        src/imgui_draw.cpp
        src/imgui_widgets.cpp
        src/imgui_tables.cpp
        src/imgui_demo.cpp
        src/imgui_impl_sdl.cpp
        src/imgui_impl_sdlrenderer.cpp
        PROPERTIES SKIP_PRECOMPILE_HEADERS 1
)

if(${CMAKE_VERSION} VERSION_GREATER "3.28" OR ${CMAKE_VERSION} VERSION_EQUAL "3.28")
    target_precompile_headers(
        ${PROJECT_NAME}
        PUBLIC
        src/pch.h
    )
endif()

# savannah_add_target(
#     ${PROJECT_NAME}
#     EXE
#     "${PROJECT_BASE_SOURCE_DIR}/src"
#     "${PROJECT_BASE_SOURCE_DIR}/src"
#     "${PROJECT_BASE_SOURCE_DIR}/out"
#     ".."
#     "${GCC_GLOBAL_COMPILER_SETTINGS}"
# )

list(APPEND CMAKE_MODULE_PATH "${PROJECT_BASE_SOURCE_DIR}/CMakeModules")

if(WIN32)
    include(${PROJECT_BASE_SOURCE_DIR}/cmake/build-on-windows.cmake)
endif()

if(APPLE)
    include(${PROJECT_BASE_SOURCE_DIR}/cmake/build-on-apple.cmake)
endif()

if(UNIX AND NOT APPLE)
    include(${PROJECT_BASE_SOURCE_DIR}/cmake/build-on-linux.cmake)
endif()


install(
    TARGETS ${PROJECT_NAME}
	RUNTIME DESTINATION ${PROJECT_INSTALL_DIR}
	LIBRARY DESTINATION ${PROJECT_INSTALL_DIR}
	ARCHIVE DESTINATION ${PROJECT_INSTALL_DIR}
)
