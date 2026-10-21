cmake_minimum_required(VERSION 3.20)
cmake_policy(SET CMP0135 NEW)

if(CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_CONFIGURATION_TYPES "Release" CACHE STRING "Supported configurations" FORCE)
elseif(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
elseif(NOT CMAKE_BUILD_TYPE STREQUAL "Release")
  message(FATAL_ERROR "Only Release builds are supported.")
endif()

file(READ "${CMAKE_CURRENT_SOURCE_DIR}/plugin-config.json" PLUGIN_CONFIG_JSON)
string(JSON PLUGIN_ID GET "${PLUGIN_CONFIG_JSON}" pluginId)
string(JSON CONTROL_MAPPER_ACTION_ID GET "${PLUGIN_CONFIG_JSON}" controlMapperActionId)
string(JSON INPUT_TRIGGER_ACTION_ID GET "${PLUGIN_CONFIG_JSON}" inputTriggerActionId)
string(JSON PLUGIN_NAME GET "${PLUGIN_CONFIG_JSON}" pluginName)
string(JSON EXECUTABLE_NAME GET "${PLUGIN_CONFIG_JSON}" executableName)
string(JSON PLUGIN_VERSION GET "${PLUGIN_CONFIG_JSON}" version)
string(JSON PLUGIN_AUTHOR GET "${PLUGIN_CONFIG_JSON}" author)
string(JSON PLUGIN_DESCRIPTION GET "${PLUGIN_CONFIG_JSON}" description)

project(StreamDockSimHub VERSION ${PLUGIN_VERSION} LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
  add_compile_options(/O2 /DNDEBUG /Zc:__cplusplus)
  add_compile_definitions(UNICODE _UNICODE NOMINMAX WIN32_LEAN_AND_MEAN)
endif()

add_subdirectory(sdk/StreamDockCPPSDK/Vendor)
add_subdirectory(sdk/StreamDockCPPSDK/StreamDockSDK)

add_executable(${EXECUTABLE_NAME}
  src/main.cpp
  src/SimHubPlugin.cpp
  src/SimHubHttp.cpp
  src/actions/ControlMapperAction.cpp
  src/actions/InputTriggerAction.cpp
)
target_include_directories(${EXECUTABLE_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_compile_definitions(${EXECUTABLE_NAME} PRIVATE
  CONTROL_MAPPER_ACTION_ID="${CONTROL_MAPPER_ACTION_ID}"
  INPUT_TRIGGER_ACTION_ID="${INPUT_TRIGGER_ACTION_ID}"
)
target_link_libraries(${EXECUTABLE_NAME} PRIVATE StreamDockSDK winhttp)

set(PLUGIN_FOLDER "${PLUGIN_ID}.sdPlugin")
set(DIST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/dist/${PLUGIN_FOLDER}")
configure_file(plugin/manifest.json.in "${CMAKE_CURRENT_BINARY_DIR}/manifest.json" @ONLY)

add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E rm -rf "${DIST_DIR}"
  COMMAND ${CMAKE_COMMAND} -E make_directory "${DIST_DIR}"
  COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:${EXECUTABLE_NAME}>" "${DIST_DIR}/$<TARGET_FILE_NAME:${EXECUTABLE_NAME}>"
  COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/manifest.json" "${DIST_DIR}/manifest.json"
  COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/plugin/assets" "${DIST_DIR}"
  COMMENT "Assembling Release plugin: ${DIST_DIR}"
)
