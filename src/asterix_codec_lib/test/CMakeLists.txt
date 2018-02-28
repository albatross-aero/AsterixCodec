cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

if (MSVC)
    add_compile_options(/W3 /WX)
else ()
    # "-Wno-unused-parameter" to remove later on
    add_compile_options(-W -Wall #[[-Werror -Wno-unused-parameter]])
endif ()

project(
        AsterixCodecLibraryUnitTest
        VERSION 0.9
        LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 11)

add_subdirectory(lib/googletest)

add_executable(runAsterixCodecTests
        src/CodecTests.hpp
        src/AllTests.cpp
        #        src/ParsingTests.cpp
        #        src/DecodingTests.cpp
        #        src/EncodingTests.cpp
        )

target_include_directories(runAsterixCodecTests
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        #            $<INSTALL_INTERFACE:include>
        )

target_link_libraries(runAsterixCodecTests
        AsterixCodecLibrary
        gtest
        gtest_main
        )