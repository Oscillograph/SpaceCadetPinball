# SDL2main is not needed
set(SDL2_BUILDING_LIBRARY ON)

# link external libraries - SDL3
add_library(sdl3 STATIC IMPORTED)
set_target_properties(sdl3 PROPERTIES IMPORTED_LOCATION ${MSE_BASE_SOURCE_DIR}/bin/libSDL3.so)

add_library(sdl3_mixer STATIC IMPORTED)
set_target_properties(sdl3_mixer PROPERTIES IMPORTED_LOCATION ${MSE_BASE_SOURCE_DIR}/bin/libSDL3_mixer.so)

include_directories(${PROJECT_EXTERNAL_DIR}/SDL3)



get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
	message(STATUS "Include dir='${dir}'")
endforeach()

if(UNIX AND NOT APPLE)
	include(GNUInstallDirs)
	install(TARGETS "${PROJECT_NAME}" RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")
	install(FILES "${CMAKE_SOURCE_DIR}/Platform/Linux/${PROJECT_NAME}.desktop" DESTINATION "share/applications")
	install(FILES "${CMAKE_SOURCE_DIR}/Platform/Linux/${PROJECT_NAME}.metainfo.xml" DESTINATION "share/metainfo")
	foreach(S 16 32 48 128 192)
		install(FILES "${CMAKE_SOURCE_DIR}/${PROJECT_NAME}/Icon_${S}x${S}.png" DESTINATION
			"share/icons/hicolor/${S}x${S}/apps" RENAME "${PROJECT_NAME}.png")
	endforeach(S)
endif()

separate_arguments(cxx_compiler_flags UNIX_COMMAND "${compiler_flags}")
target_compile_options(${PROJECT_NAME} PRIVATE ${cxx_compiler_flags})

target_link_libraries(
	${PROJECT_NAME}
	sdl3
	sdl3_mixer
)
