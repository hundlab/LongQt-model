# modellib_FOUND
# modellib_VERSION
# modellib_INCLUDE_DIR
# modellib_LIBRARY

#recursively get subdirectories
MACRO(SUBDIRLIST result curdir)
  FILE(GLOB children RELATIVE ${curdir} ${curdir}/*)
  SET(dirlist "")
  FOREACH(child ${children})
    IF(IS_DIRECTORY ${curdir}/${child})
      LIST(APPEND dirlist ${curdir}/${child})
    ENDIF()
  ENDFOREACH()
  SET(${result} ${dirlist})
ENDMACRO()

get_filename_component(PACKAGE_PREFIX_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../" ABSOLUTE)

#set(modellib_VERSION 1.1)
set(CMAKE_PREFIX_PATH "C:/Qt/Static/5.9.3/lib/cmake/Qt5Core")
find_package(Qt5Core)
SUBDIRLIST(modellib_INCLUDE_DIR "${PACKAGE_PREFIX_DIR}/include")
list(APPEND modellib_INCLUDE_DIR "${PACKAGE_PREFIX_DIR}/include")
list(APPEND modellib_INCLUDE_DIR ${Qt5Core_INCLUDE_DIRS})
set(modellib_LIBRARY "${PACKAGE_PREFIX_DIR}/")

set(modellib_FOUND TRUE)

#notes to self when building with qt
#set(CMAKE_AUTOMOC ON)

#this can potentially replaced by find_library but then again maybe not
#target_link_libraries(PyLongQt Qt5::Core libqtmain.a C:/Qt/Static/5.9.3/plugins/printsupport C:/Qt/Static/5.9.3/plugins/printsupport/libwindowsprintersupport.a C:/Qt/Static/5.9.3/plugins/platforms C:/Qt/Static/5.9.3/plugins/platforms/libqwindows.a libQt5EventDispatcherSupport.a libQt5FontDatabaseSupport.a libqtfreetype.a libQt5ThemeSupport.a libQt5AccessibilitySupport.a C:/Qt/Static/5.9.3/plugins/imageformats C:/Qt/Static/5.9.3/plugins/imageformats/libqgif.a C:/Qt/Static/5.9.3/plugins/imageformats/libqicns.a C:/Qt/Static/5.9.3/plugins/imageformats/libqico.a C:/Qt/Static/5.9.3/plugins/imageformats/libqjpeg.a C:/Qt/Static/5.9.3/plugins/imageformats/libqtga.a C:/Qt/Static/5.9.3/plugins/imageformats/libqtiff.a C:/Qt/Static/5.9.3/plugins/imageformats/libqwbmp.a C:/Qt/Static/5.9.3/plugins/imageformats/libqwebp.a libQt5PrintSupport.a  libQt5Widgets.a libQt5Gui.a libqtlibpng.a libqtharfbuzz.a libQt5Concurrent.a libQt5Core.a libqtpcre2.a mingw32 shlwapi winspool uxtheme dwmapi comdlg32 oleaut32 imm32 glu32 opengl32 gdi32 mpr version ws2_32 kernel32 user32 shell32 uuid ole32 advapi32 winmm)
