cmake_minimum_required(VERSION 3.1)

project(Netycat)

set(CMAKE_CXX_STANDARD 11)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -DNDEBUG -Ofast")
endif()

include_directories("include")

set(SRC
    src/Cats/Netycat/FileSystem/File.cpp
    src/Cats/Netycat/FileSystem/MappedFile.cpp)

add_library(Netycat STATIC ${SRC})

set(EXAMPLE_FILESYSTEM
    File
    MappedFile)

foreach(example ${EXAMPLE_FILESYSTEM})
    add_executable(FileSystem_${example} example/FileSystem/${example}/${example}.cpp)
    target_link_libraries(FileSystem_${example} Netycat)
endforeach()
