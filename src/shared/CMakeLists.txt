file(GLOB shared_localdir *.cpp *.h)

get_target_property(CURL_INCLUDE_DIRS libcurl INTERFACE_INCLUDE_DIRECTORIES)
get_target_property(CURL_LIBRARY libcurl INTERFACE_LINK_LIBRARIES)

set(SHARED_STAT_SRCS
  ${SHARED_STAT_SRCS}
  ${shared_localdir}
)

include_directories(
  ${CURL_INCLUDE_DIRS}
)

GroupSources(${CMAKE_CURRENT_SOURCE_DIR})

add_library(shared STATIC
  ${SHARED_STAT_SRCS}
)

target_link_libraries(shared
  ${CURL_LIBRARY}
)