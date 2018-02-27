cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

if (MSVC)
    add_compile_options(/W3 /WX)
else ()
    # "-Wno-unused-parameter" to remove later on
    add_compile_options(-W -Wall #[[-Werror -Wno-unused-parameter]])
endif ()

project(
        AsterixCodecLibrary
        VERSION 0.9
        LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 11)

add_library(AsterixCodecLibrary STATIC
        include/config/CommonVariables.h
        src/config/CommonVariables.cpp
        include/config/Constants.h
        include/config/FacadeCommonVariables.h
        src/config/FacadeCommonVariables.cpp
        include/config/enum/FilterTypeEnum.h
        include/config/enum/FunctionNameEnum.h
        include/config/enum/ItemTypeEnum.h
        include/config/enum/RepresentationModeEnum.h
        src/config/enum/FunctionNameEnum.cpp
        src/config/enum/FilterTypeEnum.cpp
        src/config/enum/ItemTypeEnum.cpp
        src/config/enum/RepresentationModeEnum.cpp
        src/data_model/asterix_description/external/UapAndMandatoryItems.cpp
        src/data_model/decoder_objects/IntegerValue.cpp
        src/data_model/decoder_objects/FloatValue.cpp
        src/data_model/decoder_objects/DoubleValue.cpp
        src/data_model/decoder_objects/StringValue.cpp
        src/data_model/decoder_objects/UnsignedInteger8bitValue.cpp
        src/data_model/decoder_objects/UnsignedInteger16bitValue.cpp
        src/data_model/decoder_objects/UnsignedInteger32bitValue.cpp
        src/data_model/decoder_objects/UnsignedInteger64bitValue.cpp
        src/data_model/decoder_objects/SignedInteger8bitValue.cpp
        src/data_model/decoder_objects/SignedInteger16bitValue.cpp
        src/data_model/decoder_objects/SignedInteger32bitValue.cpp
        src/data_model/decoder_objects/SignedInteger64bitValue.cpp
        src/data_model/ItemsToFilter.cpp
        include/data_model/CategoryItemsFilter.h
        src/data_model/CategoryItemsFilter.cpp
        include/data_model/CategoryConfiguration.h
        src/data_model/CategoryConfiguration.cpp
        src/data_model/AsterixConfiguration.cpp
        include/data_model/AsterixConfiguration.h
        ###
        include/data_model/CategoryFilter.h
        src/data_model/CategoryFilter.cpp
        src/data_model/CategoriesToFilter.cpp
        include/data_model/CategoriesToFilter.h
        src/data_model/CategoriesFilters.cpp
        include/data_model/CategoriesFilters.h
        ###
        include/data_model/ItemsToFilter.h
        include/data_model/asterix_description/Item.h
        src/data_model/asterix_description/Item.cpp
        include/data_model/asterix_description/CategoryDescription.h
        src/data_model/asterix_description/CategoryDescription.cpp
        include/data_model/asterix_description/external/ExternalItem.h
        src/data_model/asterix_description/external/ExternalItem.cpp
        include/data_model/asterix_description/external/ExternalCategoryDescription.h
        src/data_model/asterix_description/external/ExternalCategoryDescription.cpp
        include/data_model/asterix_description/external/UapAndMandatoryItems.h
        include/data_model/blocking_queue/BlockingQueueManager.h
        src/data_model/blocking_queue/BlockingQueueManager.cpp
        include/data_model/blocking_queue/DecoderBlockingQueue.h
        src/data_model/blocking_queue/DecoderBlockingQueue.cpp
        include/data_model/blocking_queue/ParserBlockingQueue.h
        src/data_model/blocking_queue/ParserBlockingQueue.cpp
        include/data_model/decoder_objects/DoubleValue.h
        include/data_model/decoder_objects/ElementValue.hpp
        include/data_model/decoder_objects/FloatValue.h
        include/data_model/decoder_objects/IntegerValue.h
        include/data_model/decoder_objects/ParsedElement.h
        src/data_model/decoder_objects/ParsedElement.cpp
        include/data_model/decoder_objects/SignedInteger8bitValue.h
        include/data_model/decoder_objects/SignedInteger16bitValue.h
        include/data_model/decoder_objects/SignedInteger32bitValue.h
        include/data_model/decoder_objects/SignedInteger64bitValue.h
        include/data_model/decoder_objects/StringValue.h
        include/data_model/decoder_objects/UnsignedInteger8bitValue.h
        include/data_model/decoder_objects/UnsignedInteger16bitValue.h
        include/data_model/decoder_objects/UnsignedInteger32bitValue.h
        include/data_model/decoder_objects/UnsignedInteger64bitValue.h
        include/data_model/decoder_object_pool/DecodedValueObjectPoolManager.h
        src/data_model/decoder_object_pool/DecodedValueObjectPoolManager.cpp
        include/data_model/decoder_object_pool/ObjectPool.hpp
        include/data_model/decoder_object_pool/ParsedRecord.h
        src/data_model/decoder_object_pool/ParsedRecord.cpp
        include/utils/ConversionFromStringToEnum.hpp
        include/utils/asterix_json_description/JsonCategoriesDescriptionLoader.h
        src/utils/asterix_json_description/JsonCategoriesDescriptionLoader.cpp
        include/utils/asterix_json_description/JsonCategoriesDescriptionValidator.h
        src/utils/asterix_json_description/JsonCategoriesDescriptionValidator.cpp
        include/utils/BitUtils.h
        src/utils/BitUtils.cpp
        include/utils/decoderForParsedRecord.h
        src/utils/decoderForParsedRecord.cpp
        include/utils/DecodingUtils.h
        src/utils/DecodingUtils.cpp
        include/utils/ElementEncodingUtils.h
        src/utils/ElementEncodingUtils.cpp
        include/utils/encoderForDecodedRecord.hpp
        include/utils/ItemEncodingUtils.h
        src/utils/ItemEncodingUtils.cpp
        include/utils/ParsingUtils.h
        src/utils/ParsingUtils.cpp
        include/utils/PrintingUtils.h
        src/utils/PrintingUtils.cpp
        include/asterix_codec/CodecInterface.h
        include/asterix_codec/DecoderInterface.h
        include/asterix_codec/EncoderInterface.h
        include/asterix_codec/RecordCollection.h
        include/asterix_codec/Record.h
        include/asterix_codec/facade/CodecInterface.cpp
        include/asterix_codec/facade/DecoderInterface.cpp
        include/asterix_codec/facade/EncoderInterface.cpp
        include/asterix_codec/facade/Record.cpp
        include/asterix_codec/facade/RecordCollection.cpp
        include/asterix_codec/facade/reports/FailureReport.cpp
        )

# BOOST 'serialization' library
find_package(
        Boost REQUIRED
        COMPONENTS
        serialization
)

if (Boost_FOUND)
    target_include_directories(AsterixCodecLibrary PUBLIC ${Boost_INCLUDE_DIRS})
    target_link_libraries(AsterixCodecLibrary PUBLIC ${Boost_LIBRARIES})
endif ()

## Nlohmann JSON - header_only
add_library(nlohmann_json INTERFACE)
target_include_directories(nlohmann_json
        INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/lib/nlohmann_json>
        $<INSTALL_INTERFACE:lib/nlohmann_json>
        )

## Thread_Pool - header_only
add_library(thread_pool INTERFACE)
target_include_directories(thread_pool
        INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/lib/thread_pool>
        $<INSTALL_INTERFACE:lib/thread_pool>
        )

## PTHREADS library
find_package(Threads REQUIRED)
if (Threads_FOUND)
    #    message(STATUS ${CMAKE_THREAD_LIBS_INIT})
    target_link_libraries(AsterixCodecLibrary PUBLIC ${CMAKE_THREAD_LIBS_INIT})
endif ()

target_link_libraries(AsterixCodecLibrary
        PUBLIC
        Threads::Threads
        INTERFACE
        nlohmann_json
        thread_pool
        )

#####

target_include_directories(AsterixCodecLibrary
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/lib>
        PRIVATE
        include/config
        include/data_model
        include/utils
        )

#####

# da leggere
#https://stackoverflow.com/questions/25676277/cmake-target-include-directories-prints-an-error-when-i-try-to-add-the-source
#####

install(TARGETS AsterixCodecLibrary nlohmann_json thread_pool
        EXPORT AsterixCodecLibraryTargets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include
        )

install(EXPORT AsterixCodecLibraryTargets
        FILE AsterixCodecLibraryTargets.cmake
        NAMESPACE AsterixCodec::
        DESTINATION lib/cmake/AsterixCodecLibrary
        )

install(FILES "AsterixCodecLibraryTargets.cmake"
        DESTINATION
        lib/cmake/AsterixCodecLibrary
        )

#####

# UNIT TEST - w/GTest
add_subdirectory(test)