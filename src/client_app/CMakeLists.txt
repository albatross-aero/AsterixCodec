cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

if (MSVC)
    add_compile_options(/W3 /WX)
else ()
    add_compile_options(-W -Wall #[[-Werror -Wno-unused-parameter]])
endif ()

project(
        AsterixCodecClient
        VERSION 0.9
        LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 11)

add_executable(AsterixCodecClient
        src/main.cpp)

# BOOST 'program_options' library
find_package(
        Boost REQUIRED
        COMPONENTS
        program_options
        serialization
)

if (Boost_FOUND)
    target_include_directories(AsterixCodecClient PUBLIC ${Boost_INCLUDE_DIRS})
endif ()

###
target_link_libraries(AsterixCodecClient
        PUBLIC
        AsterixCodecLibrary
        ${Boost_LIBRARIES}
        pthread
        )

target_include_directories(AsterixCodecClient
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/facade>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/lib>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        )