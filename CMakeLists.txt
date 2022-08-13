cmake_minimum_required(VERSION 3.23)
set(CMAKE_C_COMPILER ${CMAKE_C_COMPILER})
project(helix VERSION 1.0)
include(GNUInstallDirs)

add_subdirectory(lib/json-c)

add_library(helix SHARED
    src/channel.c
    src/client.c
    src/game.c
    src/util.c
    src/videoplayer.c
)

configure_file(helix.pc.in helix.pc @ONLY)

target_link_libraries(helix json-c curl)

set_target_properties(helix PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(helix PROPERTIES PUBLIC_HEADER helix.h)
set_target_properties(helix PROPERTIES PRIVATE_HEADER "include/channel.h;include/client.h;include/game.h;include/hashmap.h;include/util.h;include/videoplayer.h")

define_property(TARGET PROPERTY LIB_HEADER)

set_property(TARGET PROPERTY LIB_HEADER "${CMAKE_SOURCE_DIR}/lib/json-c/json.h")

set_target_properties(helix PROPERTIES LIBRARY json-c)

include_directories("include" "lib")
target_include_directories(helix PRIVATE
    include
    lib
)

install(TARGETS helix LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/helix PRIVATE_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/helix/include)

install(FILES ${CMAKE_SOURCE_DIR}/lib/json-c/json.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/helix/lib/json-c)

install(FILES ${CMAKE_BINARY_DIR}/helix.pc DESTINATION ${CMAKE_BINARY_DIR}/pkgconfig)
