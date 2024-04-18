cmake_minimum_required (VERSION 3.14)
project (wat4ff C)

set(BUILD_SHARED_LIBS 0)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_library(wat4ff STATIC src/wat4ff.c)

if("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
target_compile_options(wat4ff PRIVATE -fno-ident
                      -fno-unwind-tables -fno-asynchronous-unwind-tables 
					  -nostdlib -nostartfiles -nodefaultlibs)
endif()

install(PROGRAMS bin/wat4ff_ld DESTINATION .)
install(TARGETS wat4ff
        LIBRARY
		ARCHIVE)
install(DIRECTORY include/ DESTINATION include)
