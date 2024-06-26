find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(RapidJSON REQUIRED)

add_subdirectory(tools)

file(GLOB schemalist
    ${CMAKE_CURRENT_SOURCE_DIR}/schema/*.json
)

add_custom_target(SchemaGeneration
  ALL
  COMMAND SchemaLoader ${CMAKE_BINARY_DIR} ${schemalist}
  DEPENDS ${schemalist} tools/schemaloader/schemaloader.cpp SchemaLoader
)

project(GameEngine LANGUAGES CXX)

file(GLOB src 
    keystate.cpp
    mousestate.cpp
    context.cpp
    view.cpp
    logging.cpp
    geerror.cpp
    object/object.cpp
    object/physicsobject.cpp
    object/image.cpp
    texture/texture.cpp
    texture/spritemap/spritemap.cpp
    texture/spritemap/spritemapdata.cpp
)

file(GLOB GAMEENGINE_HEADERS
    object
    texture
    texture/spritemap
    schema
    data
)

add_library(GameEngine STATIC ${src})
target_link_libraries(GameEngine PUBLIC ${SDL2_LIBRARIES} SDL2_image lua Packager)
target_include_directories(GameEngine PUBLIC . ${GAMEENGINE_HEADERS} ${SDL2_INCLUDE_DIRS} ../external ${CMAKE_BINARY_DIR})
add_dependencies(GameEngine SchemaGeneration)