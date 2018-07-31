file(GLOB sources_localdir *.cpp *.h *.hpp)

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++14 ")
endif(MSVC)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

get_target_property(CURL_INCLUDE_DIRS libcurl INTERFACE_INCLUDE_DIRECTORIES)
get_target_property(CURL_LIBRARY libcurl INTERFACE_LINK_LIBRARIES)

include_directories(
  ${boost}
  ${CMAKE_BINARY_DIR}
  ${CURL_INCLUDE_DIRS}
  ${CMAKE_SOURCE_DIR}/src/shared
  ${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable(ribbitdump
  ${sources_localdir}
)

set_target_properties(ribbitdump PROPERTIES OUTPUT_NAME ribbitdump)

target_link_libraries(ribbitdump 
  boost
  mysql
  shared
)

install(TARGETS ribbitdump DESTINATION "bin")
