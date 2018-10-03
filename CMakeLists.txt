cmake_minimum_required(VERSION 2.8.11)

project(LongQt-model)

# Find includes in corresponding build directories
#Release Debug
set(CMAKE_BUILD_TYPE Release)

find_package(Qt5Core)
find_package(Qt5Concurrent)
include("${CMAKE_CURRENT_LIST_DIR}/LongQt-model.cmake")
set(CMAKE_AUTOMOC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(${modellib_includes})

if(${BUILD_QT_STATIC})
	link_directories(${QT_LIB_DIR})
	SET(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
	SET(BUILD_SHARED_LIBRARIES OFF)
	SET(GCC_COVERAGE_LINK_FLAGS "-static")
endif()

add_library(LongQt-model STATIC ${modellib_sources})

if(${BUILD_QT_STATIC})
	target_link_libraries(LongQt-model Qt5::Core Qt5::Concurrent libQt5EventDispatcherSupport.a libQt5FontDatabaseSupport.a libqtfreetype.a libQt5ThemeSupport.a libQt5AccessibilitySupport.a libQt5PrintSupport.a libqtlibpng.a libqtharfbuzz.a libQt5Concurrent.a libQt5Core.a libqtpcre2.a mingw32 shlwapi winspool uxtheme dwmapi comdlg32 oleaut32 imm32 glu32 opengl32 gdi32 mpr version ws2_32 kernel32 user32 shell32 uuid ole32 advapi32 winmm) 
	#target_link_libraries(LongQt-model Qt5::Core C:/Qt/Static/5.9.3/plugins/printsupport/libwindowsprintersupport.a C:/Qt/Static/5.9.3/plugins/platforms/libqwindows.a libQt5EventDispatcherSupport.a libQt5FontDatabaseSupport.a libqtfreetype.a libQt5ThemeSupport.a libQt5AccessibilitySupport.a C:/Qt/Static/5.9.3/plugins/imageformats/libqgif.a C:/Qt/Static/5.9.3/plugins/imageformats/libqicns.a C:/Qt/Static/5.9.3/plugins/imageformats/libqico.a C:/Qt/Static/5.9.3/plugins/imageformats/libqjpeg.a C:/Qt/Static/5.9.3/plugins/imageformats/libqtga.a C:/Qt/Static/5.9.3/plugins/imageformats/libqtiff.a C:/Qt/Static/5.9.3/plugins/imageformats/libqwbmp.a C:/Qt/Static/5.9.3/plugins/imageformats/libqwebp.a libQt5PrintSupport.a  libQt5Widgets.a libQt5Gui.a libqtlibpng.a libqtharfbuzz.a libQt5Concurrent.a libQt5Core.a libqtpcre2.a mingw32 shlwapi winspool uxtheme dwmapi comdlg32 oleaut32 imm32 glu32 opengl32 gdi32 mpr version ws2_32 kernel32 user32 shell32 uuid ole32 advapi32 winmm) 
	target_compile_options(LongQt-model PRIVATE -static-libgcc -static-libstdc++)
else()
	target_link_libraries(LongQt-model Qt5::Core Qt5::Concurrent)
endif()

install(TARGETS LongQt-model
    DESTINATION ${INSTALL_LOC}
    LIBRARY DESTINATION ${INSTALL_LOC}
    ARCHIVE DESTINATION ${INSTALL_LOC}
    )
install(FILES LongQt-model.cmake LongQt-modelConfig.cmake DESTINATION ${INSTALL_LOC})
foreach(inc_dir ${modellib_includes})
    install(DIRECTORY ${inc_dir}
        DESTINATION ${INSTALL_LOC}
        FILES_MATCHING PATTERN "*.h"
        PATTERN ".git" EXCLUDE
        PATTERN "tests" EXCLUDE
        PATTERN "CMakeFiles" EXCLUDE
        PATTERN "resources" EXCLUDE
        PATTERN "LongQt-model_autogen" EXCLUDE
        )
endforeach()

