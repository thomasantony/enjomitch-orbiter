# Create a library called "Hello" which includes the source file "hello.cxx".
# The extension is already found. Any number of sources could be listed here.
cmake_minimum_required(VERSION 3.6.2)
include(../cmake/util.cmake)
project( wx )

#message(FATAL_ERROR "${CMAKE_CURRENT_SOURCE_DIR}")
find_package(wxWidgets COMPONENTS core base REQUIRED)

file( GLOB SRCS src/*.cpp src/*.h src/*.hpp src/*/*.cpp src/*/*.h src/*/*.hpp)

include(${wxWidgets_USE_FILE})
add_library(${PROJECT_NAME} SHARED ${SRCS})
set_target_properties(${PROJECT_NAME} PROPERTIES LINK_DEPENDS_NO_SHARED true)
target_link_libraries(${PROJECT_NAME} EnjoLib ${wxWidgets_LIBRARIES})

# Make sure the compiler can find include files for our Hello library
# when other libraries or executables link to Hello
target_include_directories (${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
install (TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_BINARY_DIR}/bin/lib)
SetupCotire(${PROJECT_NAME})

