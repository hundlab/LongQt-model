cmake_minimum_required(VERSION 3.0)

project(LongQt-model VERSION 0.2)

set(CMAKE_CXX_STANDARD 17)
set(LongQT_model_tests FALSE CACHE BOOL "Build the tests")
#set(CMAKE_BUILD_TYPE Release)

find_package(Qt5 REQUIRED COMPONENTS Core)

# Get source files and setup LongQt-model to be build as
# a static library
# Note: Qt5 will still be linked dynamically
include("${CMAKE_CURRENT_LIST_DIR}/src/LongQt-model.cmake")

#set(CMAKE_AUTOMOC ON)

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

add_library(LongQt-model STATIC ${modellib_sources})
target_include_directories(LongQt-model PUBLIC
    "$<BUILD_INTERFACE:${build_includes}>"
    "$<INSTALL_INTERFACE:${install_includes}>")
set_target_properties(LongQt-model PROPERTIES
    VERSION ${PROJECT_VERSION_MAJOR}
    SOVERSION ${PROJECT_VERSION_MINOR})

target_link_libraries(LongQt-model PUBLIC Qt5::Core)

if(LongQT_model_tests)
  message("Tests will be built")
  # Add tests
  include(FetchContent)
  FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
  )
  # For Windows: Prevent overriding the parent project's compiler/linker settings
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(googletest)

  enable_testing()
  include(GoogleTest)
  add_executable(LongQt-model-tests "${CMAKE_CURRENT_LIST_DIR}/tests/main.cpp")
  gtest_discover_tests(LongQt-model-tests)
  target_link_libraries(LongQt-model-tests LongQt-model gtest_main gmock_main)
else()
  message("Tests will not be built")
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
