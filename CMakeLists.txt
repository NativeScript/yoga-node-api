cmake_minimum_required(VERSION 3.15)

project(yoga_node_api)

set(NAME yoga_node_api)
set(VERSION 0.1.0)

set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(CMAKE_CXX_STANDARD 20)

include_directories(
  ${CMAKE_SOURCE_DIR}/yoga
  ${CMAKE_SOURCE_DIR}/src
)

set(LIB_SOURCE_FILES
  src/yoga_node_api.cc
)

add_library(
  ${NAME}
  SHARED
  ${LIB_SOURCE_FILES}
)

target_link_directories(
  ${NAME}
  PRIVATE
  ${CMAKE_SOURCE_DIR}/yoga/build/yoga
)

target_link_libraries(
  ${NAME}
  PRIVATE
  libyogacore.a
)

target_link_options(
  ${NAME}
  PRIVATE
  "-undefined"
  "dynamic_lookup"
)
