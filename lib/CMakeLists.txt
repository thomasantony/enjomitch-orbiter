# Create a library called "Hello" which includes the source file "hello.cxx".
# The extension is already found. Any number of sources could be listed here.
cmake_minimum_required(VERSION 3.6.2)
project( EnjoLib )

include(../cmake/util.cmake)

option(WITH_FEATURE_ARRAY_FIRE 	"Use ArrayFire" OFF)
option(WITH_FEATURE_NEW_MAT 	"Use NewMat"    ON)
option(WITH_FEATURE_EIGEN 	    "Use Eigen"     ON)

file( GLOB SRCS */*.cpp */*.h */*.hpp Math/*/*.cpp */*/*.h */*/*.hpp)
if (WITH_FEATURE_ARRAY_FIRE)
	find_package(ArrayFire REQUIRED)
	file( GLOB SRC_AF Statistical/3rdParty/CorrelationAF.* )
	set( SRCS ${SRCS} ${SRC_AF} )
	set (EXTRA_LIBS ${EXTRA_LIBS} af)
endif()
if (WITH_FEATURE_NEW_MAT)
	include_directories("~/devel/lib/tree/include")
	link_directories("~/devel/lib/tree/lib")
	file( GLOB SRC_NM Statistical/3rdParty/EigenNewmat10.* )
	set( SRCS ${SRCS} ${SRC_NM} )
	set (EXTRA_LIBS ${EXTRA_LIBS} newmat)
endif()
if (WITH_FEATURE_EIGEN)
	find_package(Eigen3)
	include_directories(${EIGEN3_INCLUDE_DIRS})
	
	file( GLOB SRC_NM  Statistical/3rdParty/EigenEigen3.* )
	file( GLOB SRC_MKL Statistical/3rdParty/EigenMKL.* )
    file( GLOB SRC_NP  Statistical/3rdParty/EigenNumpy.* )
	set( SRCS ${SRCS} ${SRC_NM} ${SRC_MKL} ${SRC_NP})
endif()

add_library (${PROJECT_NAME} SHARED ${SRCS})
target_link_libraries(${PROJECT_NAME} crypto pthread ${EXTRA_LIBS})


# Make sure the compiler can find include files for our Hello library
# when other libraries or executables link to Hello
target_include_directories (${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
install (TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_BINARY_DIR}/bin/lib)

