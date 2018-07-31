set(CURL_LIBRARY "-lcurl") 

if(WIN32)
  if(DEFINED ENV{CURL_ROOT})
    set(CURL_INCLUDE_DIR $ENV{CURL_ROOT}/include)
    set(CURL_LIBRARY $ENV{CURL_ROOT}/lib/libcurl.lib)
    message(STATUS "Found CURL root directory.")
  else()
    message(FATAL_ERROR "CURL root directory is not set! Set environment variable CURL_ROOT!")
  endif()
endif()

find_package(CURL REQUIRED)

add_library(libcurl INTERFACE)

target_link_libraries(libcurl
  INTERFACE
    ${CURL_LIBRARY})

target_include_directories(libcurl
  INTERFACE
    ${CURL_INCLUDE_DIRS})

set_target_properties(libcurl PROPERTIES 
  INTERFACE_INCLUDE_DIRECTORIES ${CURL_INCLUDE_DIRS}
  INTERFACE_LINK_LIBRARIES ${CURL_LIBRARY}
)

message(STATUS "  include: ${CURL_INCLUDE_DIRS}")
message(STATUS "  lib:     ${CURL_LIBRARY}")
