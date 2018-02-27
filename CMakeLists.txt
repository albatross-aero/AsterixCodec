cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

if (MSVC)
    add_compile_options(/W3 /WX)
else ()
    # "-Wno-unused-parameter" to remove later on
    add_compile_options(-W -Wall -Wno-unused-parameter) #-Werror)
endif ()

project(
        AsterixCodec
        VERSION 0.9
        LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 11)

# Targets that we develop here
add_subdirectory(src/asterix_codec_lib)
add_subdirectory(src/client_app)

add_dependencies(AsterixCodecClient AsterixCodecLibrary)