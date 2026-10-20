cmake_minimum_required(VERSION 3.21)
project(${SKBUILD_PROJECT_NAME} LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

find_package(Python 3.11 REQUIRED COMPONENTS Interpreter Development.Module)

file(STRINGS CMakeLists.sources LINPIPE_SOURCES)
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS CMakeLists.sources)

Python_add_library(_linpipe MODULE WITH_SOABI src/linpipe_python.cpp ${LINPIPE_SOURCES})
target_include_directories(_linpipe PRIVATE src)
target_compile_options(_linpipe PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/utf-8>)
set_target_properties(_linpipe PROPERTIES
  C_VISIBILITY_PRESET hidden
  CXX_VISIBILITY_PRESET hidden
  VISIBILITY_INLINES_HIDDEN ON
)
if(WIN32)
  target_link_libraries(_linpipe PRIVATE advapi32 bcrypt user32 ws2_32)
endif()

install(TARGETS _linpipe LIBRARY DESTINATION linpipe)
