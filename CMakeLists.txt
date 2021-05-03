# GDK / Utility for CMake
cmake_minimum_required(VERSION 2.8)

project(utility)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(lib_name utility)
set(lib_target utility)
set(lib_target_shared utilityShared)
set(srcFolders INI Log Collect MT Network Persistence Performance )

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

if( CMAKE_BUILD_TYPE MATCHES Debug )

if( CMAKE_HOST_WIN32 )
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else( CMAKE_HOST_WIN32 )
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")
endif( CMAKE_HOST_WIN32 )
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY  ${PROJECT_BINARY_DIR}/Debug)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY  ${PROJECT_BINARY_DIR}/Debug)

endif( CMAKE_BUILD_TYPE MATCHES Debug )

if( CMAKE_BUILD_TYPE MATCHES Release )

if( CMAKE_HOST_WIN32 )
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else( CMAKE_HOST_WIN32 )
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -g")
endif( CMAKE_HOST_WIN32 )
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/Release)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/Release)

endif( CMAKE_BUILD_TYPE MATCHES Release )

message( STATUS ${PROJECT_BINARY_DIR} )
message( STATUS ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY} )
message( STATUS ProjectSourceDir=${PROJECT_SOURCE_DIR} )
MESSAGE( STATUS "CXX_FLAGS ${CMAKE_CXX_FLAGS}" )

file( GLOB srcMain "*.h" "*.cpp")
list( APPEND sList ${srcMain} )
set( allSources ${srcMain} )
source_group( "" FILES ${srcMain} )

foreach( subDir ${srcFolders} )
message( STATUS ${subDir} )
file( GLOB src${subDir} "${subDir}/*.h" "${subDir}/*.cpp")
list( APPEND sList ${src${subDir}} )
source_group( ${subDir} FILES ${src${subDir}} )
endforeach(subDir)

add_library( objlib OBJECT ${sList} )

# shared libraries need PIC
set_property(TARGET objlib PROPERTY POSITION_INDEPENDENT_CODE 1)

add_library( ${lib_target} STATIC $<TARGET_OBJECTS:objlib> )
add_library( ${lib_target_shared} SHARED $<TARGET_OBJECTS:objlib> )


set_target_properties( ${lib_target} ${lib_target_shared}
    PROPERTIES OUTPUT_NAME ${lib_name} VESION 1.0.0 SOVERSION 1 )

# find opencc
#FIND_LIBRARY( libOpenccInfo opencc /usr/local/lib)
#IF( libOpenccInfo )
#        Message( STATUS "Opencc is found" )
#ELSE( libOpenccInfo )
#        Message( STATUS "Opencc is not found ")
#ENDIF( libOpenccInfo )

# find iconv
FIND_LIBRARY( libICONVInfo iconv /usr/local/lib)
IF( libICONVInfo )
        Message( STATUS "iconv is found" )
ELSE( libICONVInfo )
        Message( STATUS "iconv is not found ")
ENDIF( libICONVInfo )

if( CMAKE_HOST_WIN32 )
add_definitions( -D_DEBUG )
message( STATUS "Platform WIN32" )
else(CMAKE_HOST_WIN32)
add_definitions( -D_OS_LINUX_ -D_DEBUG )
#IF( libOpenccInfo )
#add_definitions( -D_OPENCC_)
#ENDIF( libOpenccInfo )
message( STATUS "Platform LINUX" )
endif(CMAKE_HOST_WIN32)

set_property(TARGET ${lib_target} PROPERTY FOLDER "Utility")

#install (TARGETS Utility RUNTIME DESTINATION ${PROJECT_BINARY_DIR}/bin)
install( DIRECTORY ${PROJECT_SOURCE_DIR} DESTINATION
${CMAKE_INSTALL_PREFIX}/include FILES_MATCHING PATTERN "*.h"
PATTERN "_build" EXCLUDE
PATTERN ".svn" EXCLUDE
PATTERN "VS2013" EXCLUDE )

install( TARGETS ${lib_target} ${lib_target_shared}
         LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/share
         ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib )
