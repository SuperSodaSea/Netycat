cmake_minimum_required(VERSION 3.1)

project(Netycat)

set(CMAKE_CXX_STANDARD 14)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O2")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-s")
    if(WIN32)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
        link_libraries(ws2_32)
    endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /D_SCL_SECURE_NO_WARNINGS /EHsc /W3 /O2 /MT")
    link_libraries(ws2_32)
endif()

include_directories("include")

set(SRC
    src/Cats/Netycat/Filesystem/Directory.cpp
    src/Cats/Netycat/Filesystem/File.cpp
    src/Cats/Netycat/Filesystem/MappedFile.cpp
    src/Cats/Netycat/Network/IP/IPResolver.cpp)
if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    set(SRC
        ${SRC}
        src/Cats/Netycat/Network/Win32/WSA.cpp)
endif()

add_library(Netycat STATIC ${SRC})
link_libraries(Netycat)

set(EXAMPLE
    Filesystem_Directory
    Filesystem_File
    Filesystem_MappedFile
    Network_IPResolver)

foreach(example ${EXAMPLE})
    add_executable(${example} example/${example}/${example}.cpp)
    target_link_libraries(${example})
endforeach()
