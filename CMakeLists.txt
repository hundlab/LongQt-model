cmake_minimum_required(VERSION 3.0)

project(LongQt-model VERSION 0.2)

# Find includes in corresponding build directories
#Release Debug
set(CMAKE_BUILD_TYPE Release)

find_package(Qt5 COMPONENTS Core REQUIRED)
include("${CMAKE_CURRENT_LIST_DIR}/src/${CMAKE_PROJECT_NAME}.cmake")

set(CMAKE_AUTOMOC ON)

set(install_include_dir "include/${CMAKE_PROJECT_NAME}")
set(install_lib_dir "lib")
if(WIN32 AND NOT CYGWIN)
    set(install_cmake_dir cmake)
else()
    set(install_cmake_dir share/cmake/${CMAKE_PROJECT_NAME})
endif()

function(PREPEND var prefix)
   set(listVar "")
   foreach(f ${ARGN})
      list(APPEND listVar "${prefix}${f}")
   endforeach(f)
   set(${var} "${listVar}" PARENT_SCOPE)
endfunction(PREPEND)

PREPEND(build_includes "${CMAKE_CURRENT_LIST_DIR}/src" ${modellib_includes})
PREPEND(install_includes ${install_include_dir} ${modellib_includes})
#link_directories(${QT_LIB_DIR})

if(${BUILD_QT_STATIC})
#        link_directories("/usr/local/Qt-5.9.3/lib/")
	SET(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
	SET(BUILD_SHARED_LIBRARIES OFF)
#	SET(GCC_COVERAGE_LINK_FLAGS "-static")
endif()

add_library(LongQt-model STATIC ${modellib_sources})
target_include_directories(LongQt-model PUBLIC
    "$<BUILD_INTERFACE:${build_includes}>"
    "$<INSTALL_INTERFACE:${install_includes}>")
set_target_properties(LongQt-model PROPERTIES
    VERSION ${PROJECT_VERSION_MAJOR}
    SOVERSION ${PROJECT_VERSION_MINOR})


if(${BUILD_QT_STATIC})
#qt has a bug when its built statically where it doesn't link deps so
    if(WIN32 AND NOT CYGWIN)
        target_link_libraries(LongQt-model Qt5::Core libQt5EventDispatcherSupport.a libQt5FontDatabaseSupport.a libqtfreetype.a libQt5ThemeSupport.a libQt5AccessibilitySupport.a libQt5PrintSupport.a libqtlibpng.a libqtharfbuzz.a libQt5Concurrent.a libQt5Core.a  mingw32 shlwapi winspool uxtheme dwmapi comdlg32 oleaut32 imm32 glu32 opengl32 gdi32 mpr version ws2_32 kernel32 user32 shell32 uuid ole32 advapi32 winmm)
        target_compile_options(LongQt-model PRIVATE -static-libgcc -static-libstdc++)
    elseif(APPLE)
        set(QT_lib_dir ${Qt5_DIR}/../../)
        target_link_libraries(LongQt-model PUBLIC Qt5::Core ${QT_lib_dir}/libqtpcre2.a)
        target_compile_options(LongQt-model PRIVATE -framework Cocoa)
    else()
        target_link_libraries(LongQt-model PUBLIC Qt5::Core)
    endif()
else()
        target_link_libraries(LongQt-model PUBLIC Qt5::Core)
endif()

##create package
#include(GenerateExportHeader)
#generate_export_header(LongQt-model)
#set_property(TARGET LongQt-model PROPERTY VERSION 2)
install(TARGETS LongQt-model EXPORT LongQt-model
    LIBRARY DESTINATION ${install_lib_dir}
    ARCHIVE DESTINATION ${install_lib_dir}
    RUNTIME DESTINATION ${install_lib_dir}
    INCLUDES DESTINATION ${install_include_dir}
    )
install(FILES  LongQt-modelConfig.cmake DESTINATION ${install_cmake_dir})
install(DIRECTORY ${modellib_src}
    DESTINATION ${install_include_dir}
    FILES_MATCHING PATTERN "*.h*"
)
install(EXPORT LongQt-model
    DESTINATION ${install_cmake_dir})
