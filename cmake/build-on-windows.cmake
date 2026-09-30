# On Windows, set paths to SDL-devel packages here
list(APPEND SDL2_PATH "${CMAKE_CURRENT_LIST_DIR}/Libs/SDL2")
list(APPEND SDL2_MIXER_PATH "${CMAKE_CURRENT_LIST_DIR}/Libs/SDL2_mixer")

# Link mingw libs static
if(MINGW)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static")
endif()

# On Windows, include resource file with the icon
set_source_files_properties(src/SpaceCadetPinball.rc LANGUAGE RC)
list(APPEND SOURCE_FILES src/SpaceCadetPinball.rc)



# SDL2main is not needed
set(SDL2_BUILDING_LIBRARY ON)

find_package(SDL2 REQUIRED)
FIND_PACKAGE(SDL2_mixer REQUIRED)

include_directories(${SDL2_INCLUDE_DIR} ${SDL2_MIXER_INCLUDE_DIR})
get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
	message(STATUS "Include dir='${dir}'")
endforeach()


# On Windows, copy DLL to output
if(WIN32)
    list(GET SDL2_LIBRARY -1 SDL2_DLL_PATH)
    list(GET SDL2_MIXER_LIBRARY -1 SDL2_MIXER_DLL_PATH)
    get_filename_component(SDL2_DLL_PATH ${SDL2_DLL_PATH} DIRECTORY)
    get_filename_component(SDL2_MIXER_DLL_PATH ${SDL2_MIXER_DLL_PATH} DIRECTORY)
    if(MINGW)
        string(REGEX REPLACE "lib$" "bin" SDL2_DLL_PATH ${SDL2_DLL_PATH})
        string(REGEX REPLACE "lib$" "bin" SDL2_MIXER_DLL_PATH ${SDL2_MIXER_DLL_PATH})
    endif()
    message(STATUS "copy paths='${SDL2_DLL_PATH}' '${SDL2_MIXER_DLL_PATH}'")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${SDL2_DLL_PATH}/SDL2.dll" $<TARGET_FILE_DIR:SpaceCadetPinball>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${SDL2_MIXER_DLL_PATH}/SDL2_mixer.dll" $<TARGET_FILE_DIR:SpaceCadetPinball>
        )
endif()

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

separate_arguments(cxx_compiler_flags UNIX_COMMAND "${compiler_flags}")
target_compile_options(${PROJECT_NAME} PRIVATE ${cxx_compiler_flags})

target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARY} ${SDL2_MIXER_LIBRARY})
