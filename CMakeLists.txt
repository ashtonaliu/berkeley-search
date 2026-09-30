cmake_minimum_required(VERSION 3.28)

project(BerkeleySearch LANGUAGES CXX)

if(NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    message(FATAL_ERROR
        "This module experiment currently uses Clang's explicit module build flags."
    )
endif()

find_library(CURL_LIBRARY NAMES curl REQUIRED)
find_path(CURL_INCLUDE_DIRECTORY curl/curl.h REQUIRED)

enable_testing()

set(MODULE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/modules")
set(OBJECT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/objects")

set(COMMON_COMPILE_FLAGS
    -std=c++20
    -fcxx-modules
    -Wall
    -Wextra
    -Wpedantic
)

set(TOKENIZER_PCM
    "${MODULE_DIRECTORY}/berkeley_search.tokenizer.pcm"
)
set(TOKENIZER_INTERFACE_OBJECT
    "${OBJECT_DIRECTORY}/tokenizer-interface.o"
)
set(TOKENIZER_IMPLEMENTATION_OBJECT
    "${OBJECT_DIRECTORY}/tokenizer.o"
)
set(TOKENIZER_TEST_OBJECT
    "${OBJECT_DIRECTORY}/tokenizer-test.o"
)
set(TOKENIZER_TEST_EXECUTABLE
    "${CMAKE_CURRENT_BINARY_DIR}/tokenizer-test"
)

add_custom_command(
    OUTPUT "${TOKENIZER_PCM}"
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${MODULE_DIRECTORY}"
    COMMAND "${CMAKE_CXX_COMPILER}"
        ${COMMON_COMPILE_FLAGS}
        "${CMAKE_CURRENT_SOURCE_DIR}/src/tokenizer.cppm"
        --precompile
        -o "${TOKENIZER_PCM}"
    DEPENDS src/tokenizer.cppm
    VERBATIM
)

add_custom_command(
    OUTPUT "${TOKENIZER_INTERFACE_OBJECT}"
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${OBJECT_DIRECTORY}"
    COMMAND "${CMAKE_CXX_COMPILER}"
        ${COMMON_COMPILE_FLAGS}
        "${TOKENIZER_PCM}"
        -c
        -o "${TOKENIZER_INTERFACE_OBJECT}"
    DEPENDS "${TOKENIZER_PCM}"
    VERBATIM
)

add_custom_command(
    OUTPUT "${TOKENIZER_IMPLEMENTATION_OBJECT}"
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${OBJECT_DIRECTORY}"
    COMMAND "${CMAKE_CXX_COMPILER}"
        ${COMMON_COMPILE_FLAGS}
        "-fmodule-file=berkeley_search.tokenizer=${TOKENIZER_PCM}"
        -c "${CMAKE_CURRENT_SOURCE_DIR}/src/tokenizer.cpp"
        -o "${TOKENIZER_IMPLEMENTATION_OBJECT}"
    DEPENDS src/tokenizer.cpp "${TOKENIZER_PCM}"
    VERBATIM
)

add_custom_command(
    OUTPUT "${TOKENIZER_TEST_OBJECT}"
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${OBJECT_DIRECTORY}"
    COMMAND "${CMAKE_CXX_COMPILER}"
        ${COMMON_COMPILE_FLAGS}
        "-fmodule-file=berkeley_search.tokenizer=${TOKENIZER_PCM}"
        -c "${CMAKE_CURRENT_SOURCE_DIR}/tests/tokenizer_test.cpp"
        -o "${TOKENIZER_TEST_OBJECT}"
    DEPENDS tests/tokenizer_test.cpp "${TOKENIZER_PCM}"
    VERBATIM
)

add_custom_command(
    OUTPUT "${TOKENIZER_TEST_EXECUTABLE}"
    COMMAND "${CMAKE_CXX_COMPILER}"
        "${TOKENIZER_INTERFACE_OBJECT}"
        "${TOKENIZER_IMPLEMENTATION_OBJECT}"
        "${TOKENIZER_TEST_OBJECT}"
        -o "${TOKENIZER_TEST_EXECUTABLE}"
    DEPENDS
        "${TOKENIZER_INTERFACE_OBJECT}"
        "${TOKENIZER_IMPLEMENTATION_OBJECT}"
        "${TOKENIZER_TEST_OBJECT}"
    VERBATIM
)

add_custom_target(tokenizer-tests ALL
    DEPENDS "${TOKENIZER_TEST_EXECUTABLE}"
)

add_test(NAME tokenizer COMMAND "${TOKENIZER_TEST_EXECUTABLE}")

set(HTML_TEXT_EXTRACTOR_PCM
    "${MODULE_DIRECTORY}/berkeley_search.html_text_extractor.pcm"
)
set(HTML_TEXT_EXTRACTOR_INTERFACE_OBJECT
    "${OBJECT_DIRECTORY}/html-text-extractor-interface.o"
)
set(HTML_TEXT_EXTRACTOR_IMPLEMENTATION_OBJECT
    "${OBJECT_DIRECTORY}/html-text-extractor.o"
)
set(HTML_TEXT_EXTRACTOR_TEST_OBJECT
    "${OBJECT_DIRECTORY}/html-text-extractor-test.o"
)
set(HTML_TEXT_EXTRACTOR_TEST_EXECUTABLE
    "${CMAKE_CURRENT_BINARY_DIR}/html-text-extractor-test"
)

add_custom_command(
    OUTPUT "${HTML_TEXT_EXTRACTOR_PCM}"
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${MODULE_DIRECTORY}"
    COMMAND "${CMAKE_CXX_COMPILER}"
        ${COMMON_COMPILE_FLAGS}
        "${CMAKE_CURRENT_SOURCE_DIR}/src/html_text_extractor.cppm"
        --precompile
        -o "${HTML_TEXT_EXTRACTOR_PCM}"
    DEPENDS src/html_text_extractor.cppm
    VERBATIM
)

add_custom_command(
    OUTPUT "${HTML_TEXT_EXTRACTOR_INTERFACE_OBJECT}"
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${OBJECT_DIRECTORY}"
    COMMAND "${CMAKE_CXX_COMPILER}"
        ${COMMON_COMPILE_FLAGS}
        "${HTML_TEXT_EXTRACTOR_PCM}"
        -c
        -o "${HTML_TEXT_EXTRACTOR_INTERFACE_OBJECT}"
    DEPENDS "${HTML_TEXT_EXTRACTOR_PCM}"
    VERBATIM
)

add_custom_command(
    OUTPUT "${HTML_TEXT_EXTRACTOR_IMPLEMENTATION_OBJECT}"
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${OBJECT_DIRECTORY}"
    COMMAND "${CMAKE_CXX_COMPILER}"
        ${COMMON_COMPILE_FLAGS}
        "-fmodule-file=berkeley_search.html_text_extractor=${HTML_TEXT_EXTRACTOR_PCM}"
        -c "${CMAKE_CURRENT_SOURCE_DIR}/src/html_text_extractor.cpp"
        -o "${HTML_TEXT_EXTRACTOR_IMPLEMENTATION_OBJECT}"
    DEPENDS src/html_text_extractor.cpp "${HTML_TEXT_EXTRACTOR_PCM}"
    VERBATIM
)

add_custom_command(
    OUTPUT "${HTML_TEXT_EXTRACTOR_TEST_OBJECT}"
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${OBJECT_DIRECTORY}"
    COMMAND "${CMAKE_CXX_COMPILER}"
        ${COMMON_COMPILE_FLAGS}
        "-fmodule-file=berkeley_search.html_text_extractor=${HTML_TEXT_EXTRACTOR_PCM}"
        -c "${CMAKE_CURRENT_SOURCE_DIR}/tests/html_text_extractor_test.cpp"
        -o "${HTML_TEXT_EXTRACTOR_TEST_OBJECT}"
    DEPENDS tests/html_text_extractor_test.cpp "${HTML_TEXT_EXTRACTOR_PCM}"
    VERBATIM
)

add_custom_command(
    OUTPUT "${HTML_TEXT_EXTRACTOR_TEST_EXECUTABLE}"
    COMMAND "${CMAKE_CXX_COMPILER}"
        "${HTML_TEXT_EXTRACTOR_INTERFACE_OBJECT}"
        "${HTML_TEXT_EXTRACTOR_IMPLEMENTATION_OBJECT}"
        "${HTML_TEXT_EXTRACTOR_TEST_OBJECT}"
        -o "${HTML_TEXT_EXTRACTOR_TEST_EXECUTABLE}"
    DEPENDS
        "${HTML_TEXT_EXTRACTOR_INTERFACE_OBJECT}"
        "${HTML_TEXT_EXTRACTOR_IMPLEMENTATION_OBJECT}"
        "${HTML_TEXT_EXTRACTOR_TEST_OBJECT}"
    VERBATIM
)

add_custom_target(html-text-extractor-tests ALL
    DEPENDS "${HTML_TEXT_EXTRACTOR_TEST_EXECUTABLE}"
)

add_test(
    NAME html-text-extractor
    COMMAND "${HTML_TEXT_EXTRACTOR_TEST_EXECUTABLE}"
)

set(DOCUMENT_PCM
    "${MODULE_DIRECTORY}/berkeley_search.document.pcm"
)
set(DOCUMENT_INTERFACE_OBJECT
    "${OBJECT_DIRECTORY}/document-interface.o"
)
set(DOCUMENT_TEST_OBJECT
    "${OBJECT_DIRECTORY}/document-test.o"
)
set(DOCUMENT_TEST_EXECUTABLE
    "${CMAKE_CURRENT_BINARY_DIR}/document-test"
)

add_custom_command(
    OUTPUT "${DOCUMENT_PCM}"
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${MODULE_DIRECTORY}"
    COMMAND "${CMAKE_CXX_COMPILER}"
        ${COMMON_COMPILE_FLAGS}
        "${CMAKE_CURRENT_SOURCE_DIR}/src/document.cppm"
        --precompile
        -o "${DOCUMENT_PCM}"
    DEPENDS src/document.cppm
    VERBATIM
)

add_custom_command(
    OUTPUT "${DOCUMENT_INTERFACE_OBJECT}"
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${OBJECT_DIRECTORY}"
    COMMAND "${CMAKE_CXX_COMPILER}"
        ${COMMON_COMPILE_FLAGS}
        "${DOCUMENT_PCM}"
        -c
        -o "${DOCUMENT_INTERFACE_OBJECT}"
    DEPENDS "${DOCUMENT_PCM}"
    VERBATIM
)

add_custom_command(
    OUTPUT "${DOCUMENT_TEST_OBJECT}"
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${OBJECT_DIRECTORY}"
    COMMAND "${CMAKE_CXX_COMPILER}"
        ${COMMON_COMPILE_FLAGS}
        "-fmodule-file=berkeley_search.document=${DOCUMENT_PCM}"
        -c "${CMAKE_CURRENT_SOURCE_DIR}/tests/document_test.cpp"
        -o "${DOCUMENT_TEST_OBJECT}"
    DEPENDS tests/document_test.cpp "${DOCUMENT_PCM}"
    VERBATIM
)

add_custom_command(
    OUTPUT "${DOCUMENT_TEST_EXECUTABLE}"
    COMMAND "${CMAKE_CXX_COMPILER}"
        "${DOCUMENT_INTERFACE_OBJECT}"
        "${DOCUMENT_TEST_OBJECT}"
        -o "${DOCUMENT_TEST_EXECUTABLE}"
    DEPENDS
        "${DOCUMENT_INTERFACE_OBJECT}"
        "${DOCUMENT_TEST_OBJECT}"
    VERBATIM
)

add_custom_target(document-tests ALL
    DEPENDS "${DOCUMENT_TEST_EXECUTABLE}"
)

add_test(NAME document COMMAND "${DOCUMENT_TEST_EXECUTABLE}")

set(CRAWLER_PCM
    "${MODULE_DIRECTORY}/berkeley_search.crawler.pcm"
)
set(CRAWLER_INTERFACE_OBJECT
    "${OBJECT_DIRECTORY}/crawler-interface.o"
)
set(CRAWLER_IMPLEMENTATION_OBJECT
    "${OBJECT_DIRECTORY}/crawler.o"
)
set(MAIN_OBJECT "${OBJECT_DIRECTORY}/main.o")
set(SEARCH_EXECUTABLE "${CMAKE_CURRENT_BINARY_DIR}/berkeley-search")

add_custom_command(
    OUTPUT "${CRAWLER_PCM}"
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${MODULE_DIRECTORY}"
    COMMAND "${CMAKE_CXX_COMPILER}"
        ${COMMON_COMPILE_FLAGS}
        "${CMAKE_CURRENT_SOURCE_DIR}/src/crawler.cppm"
        --precompile
        -o "${CRAWLER_PCM}"
    DEPENDS src/crawler.cppm
    VERBATIM
)

add_custom_command(
    OUTPUT "${CRAWLER_INTERFACE_OBJECT}"
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${OBJECT_DIRECTORY}"
    COMMAND "${CMAKE_CXX_COMPILER}"
        ${COMMON_COMPILE_FLAGS}
        "${CRAWLER_PCM}"
        -c
        -o "${CRAWLER_INTERFACE_OBJECT}"
    DEPENDS "${CRAWLER_PCM}"
    VERBATIM
)

add_custom_command(
    OUTPUT "${CRAWLER_IMPLEMENTATION_OBJECT}"
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${OBJECT_DIRECTORY}"
    COMMAND "${CMAKE_CXX_COMPILER}"
        ${COMMON_COMPILE_FLAGS}
        "-I${CURL_INCLUDE_DIRECTORY}"
        "-fmodule-file=berkeley_search.crawler=${CRAWLER_PCM}"
        -c "${CMAKE_CURRENT_SOURCE_DIR}/src/crawler.cpp"
        -o "${CRAWLER_IMPLEMENTATION_OBJECT}"
    DEPENDS src/crawler.cpp "${CRAWLER_PCM}"
    VERBATIM
)

add_custom_command(
    OUTPUT "${MAIN_OBJECT}"
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${OBJECT_DIRECTORY}"
    COMMAND "${CMAKE_CXX_COMPILER}"
        ${COMMON_COMPILE_FLAGS}
        "-I${CURL_INCLUDE_DIRECTORY}"
        "-fmodule-file=berkeley_search.crawler=${CRAWLER_PCM}"
        -c "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp"
        -o "${MAIN_OBJECT}"
    DEPENDS src/main.cpp "${CRAWLER_PCM}"
    VERBATIM
)

add_custom_command(
    OUTPUT "${SEARCH_EXECUTABLE}"
    COMMAND "${CMAKE_CXX_COMPILER}"
        "${CRAWLER_INTERFACE_OBJECT}"
        "${CRAWLER_IMPLEMENTATION_OBJECT}"
        "${MAIN_OBJECT}"
        "${CURL_LIBRARY}"
        -o "${SEARCH_EXECUTABLE}"
    DEPENDS
        "${CRAWLER_INTERFACE_OBJECT}"
        "${CRAWLER_IMPLEMENTATION_OBJECT}"
        "${MAIN_OBJECT}"
    VERBATIM
)

add_custom_target(search-app ALL
    DEPENDS "${SEARCH_EXECUTABLE}"
)
