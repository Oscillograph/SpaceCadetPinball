set(MACOSX_RPATH)
set(CMAKE_BUILD_WITH_INSTALL_RPATH true)
set(CMAKE_INSTALL_RPATH "@executable_path/../Frameworks")
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.11")
set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
list(APPEND SDL2_PATH "${CMAKE_CURRENT_LIST_DIR}/Libs")
list(APPEND SDL2_MIXER_PATH "${CMAKE_CURRENT_LIST_DIR}/Libs")

# SDL2main is not needed
set(SDL2_BUILDING_LIBRARY ON)

find_package(SDL2 REQUIRED)
FIND_PACKAGE(SDL2_mixer REQUIRED)

include_directories(${SDL2_INCLUDE_DIR} ${SDL2_MIXER_INCLUDE_DIR})
get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
  message(STATUS "Include dir='${dir}'")
endforeach()

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

separate_arguments(cxx_compiler_flags UNIX_COMMAND "${compiler_flags}")
target_compile_options(${PROJECT_NAME} PRIVATE ${cxx_compiler_flags})

target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARY} ${SDL2_MIXER_LIBRARY})
