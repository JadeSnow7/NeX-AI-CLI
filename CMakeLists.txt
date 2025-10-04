cmake_minimum_required(VERSION 3.24)
project(aicli LANGUAGES CXX)

option(AICLI_BUILD_TESTS "Build tests" OFF)
option(AICLI_WITH_LLAMA "Enable integration with llama.cpp" OFF)
option(AICLI_WITH_SQLITE "Enable SQLite integration" OFF)
set(LLAMA_AVAILABLE OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(aicli
    src/main.cpp
    src/cli/repl.cpp
    src/utils/logging.cpp
    src/utils/config.cpp
    src/core/inference/local_llama/llama_engine.cpp
    src/core/conversation/session.cpp
    src/core/sysbox/sysbox.cpp
    src/core/sysbox/sysbox_sqlite.cpp
    src/core/storage/sqlite_store.cpp
    src/core/tools/tools.cpp
    src/core/tools/schema.cpp
    src/core/tools/config.cpp
    src/core/fnshell/value.cpp
    src/core/fnshell/parser.cpp
    src/core/fnshell/evaluator.cpp
    src/core/fnshell/stdlib.cpp
    src/core/inference/remote/http_client.cpp
    src/core/inference/remote/openai_client.cpp
    src/core/inference/remote/gemini_client.cpp
    src/core/router/router.cpp
)

target_include_directories(aicli PRIVATE src)

if(AICLI_WITH_LLAMA)
  if(EXISTS ${CMAKE_SOURCE_DIR}/third_party/llama.cpp/CMakeLists.txt)
    message(STATUS "llama.cpp found - integrating")
    add_subdirectory(third_party/llama.cpp)
    target_link_libraries(aicli PRIVATE llama)
    target_include_directories(aicli PRIVATE third_party/llama.cpp)
    set(LLAMA_AVAILABLE ON)
  else()
    message(WARNING "AICLI_WITH_LLAMA=ON but third_party/llama.cpp not found. Falling back to stub.")
    set(LLAMA_AVAILABLE OFF)
  endif()
endif()

target_compile_definitions(aicli PRIVATE $<$<BOOL:${LLAMA_AVAILABLE}>:AICLI_WITH_LLAMA=1> $<$<NOT:$<BOOL:${LLAMA_AVAILABLE}>>:AICLI_WITH_LLAMA=0>)

if(AICLI_WITH_SQLITE)
  find_library(SQLITE3_LIB sqlite3)
  if(SQLITE3_LIB)
    message(STATUS "sqlite3 found - integrating")
    target_link_libraries(aicli PRIVATE ${SQLITE3_LIB})
    target_compile_definitions(aicli PRIVATE AICLI_WITH_SQLITE=1)
  else()
    message(WARNING "AICLI_WITH_SQLITE=ON but sqlite3 not found. Falling back to disabled.")
    target_compile_definitions(aicli PRIVATE AICLI_WITH_SQLITE=0)
  endif()
else()
  target_compile_definitions(aicli PRIVATE AICLI_WITH_SQLITE=0)
endif()

if(MSVC)
  target_compile_options(aicli PRIVATE /W4)
else()
  target_compile_options(aicli PRIVATE -Wall -Wextra -Wpedantic)
endif()

if(AICLI_BUILD_TESTS)
  add_executable(test_cli_repl tests/unit/test_cli_repl.cpp)
  if(MSVC)
    target_compile_options(test_cli_repl PRIVATE /W4)
  else()
    target_compile_options(test_cli_repl PRIVATE -Wall -Wextra -Wpedantic)
  endif()
endif()


