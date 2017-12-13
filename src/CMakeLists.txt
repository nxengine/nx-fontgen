set(SOURCES
	main.cpp
	FontInfo.cpp
	maxRectsBinPack/MaxRectsBinPack.cpp
	sdlSavePng/savepng.cpp
	ProgramOptions.cpp
	App.cpp)

add_executable(fontbm ${SOURCES})
target_link_libraries(fontbm ${SDL2_LIBRARY} ${SDL2TTF_LIBRARY} ${PNG_LIBRARY})
