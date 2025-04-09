if(POLICY CMP0148)
  cmake_policy(SET CMP0148 NEW)
endif()
cmake_minimum_required(VERSION 3.12)
project(web CXX)

include(CheckIncludeFileCXX)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)
check_include_file_cxx(any HAS_ANY)
check_include_file_cxx(string_view HAS_STRING_VIEW)
check_include_file_cxx(coroutine HAS_COROUTINE)
if(NOT "${CMAKE_CXX_STANDARD}" STREQUAL "")
  # Do nothing
elseif(
  HAS_ANY
  AND HAS_STRING_VIEW
  AND HAS_COROUTINE)
  set(CMAKE_CXX_STANDARD 20)
elseif(HAS_ANY AND HAS_STRING_VIEW)
  set(CMAKE_CXX_STANDARD 17)
else()
  set(CMAKE_CXX_STANDARD 14)
endif()

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(service)
# 将view/static 和view/templates复制到构建目录
file(COPY views/templates DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/views/templates)

# 拷贝static文件夹
file(COPY ../static DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/static)
file(COPY ../static/index.html DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/static)

# 拷贝python 拷贝python文件夹下的所有.py文件
file(GLOB PYTHON_FILES "service/python/*.py")
file(COPY ${PYTHON_FILES} DESTINATION ${CMAKE_BINARY_DIR})

file(GLOB PYTHON_FILES "service/*.yaml")
file(COPY ${PYTHON_FILES} DESTINATION ${CMAKE_BINARY_DIR})

add_executable(${PROJECT_NAME} main.cc)

# ##############################################################################
# If you include the drogon source code locally in your project, use this method
# to add drogon add_subdirectory(drogon) target_link_libraries(${PROJECT_NAME}
# PRIVATE drogon)
#
# and comment out the following lines
find_package(Drogon CONFIG REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE Drogon::Drogon email_wrapper
                                              QRCodeGenerator)

# ##############################################################################

if(CMAKE_CXX_STANDARD LESS 17)
  message(FATAL_ERROR "c++17 or higher is required")
elseif(CMAKE_CXX_STANDARD LESS 20)
  message(STATUS "use c++17")
else()
  message(STATUS "use c++20")
endif()

aux_source_directory(controllers CTL_SRC)
aux_source_directory(filters FILTER_SRC)
aux_source_directory(plugins PLUGIN_SRC)
aux_source_directory(models MODEL_SRC)
aux_source_directory(service SERVICE_SRC)
aux_source_directory(Utility UTIL_SRC)

drogon_create_views(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/views
                    ${CMAKE_CURRENT_BINARY_DIR})
# use the following line to create views with namespaces.
# drogon_create_views(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/views
# ${CMAKE_CURRENT_BINARY_DIR} TRUE) use the following line to create views with
# namespace CHANGE_ME prefixed and path namespaces.
# drogon_create_views(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/views
# ${CMAKE_CURRENT_BINARY_DIR} TRUE CHANGE_ME)

target_include_directories(
  ${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
                          ${CMAKE_CURRENT_SOURCE_DIR}/models)
target_sources(
  ${PROJECT_NAME}
  PRIVATE ${SRC_DIR}
          ${CTL_SRC}
          ${FILTER_SRC}
          ${PLUGIN_SRC}
          ${MODEL_SRC}
          ${SERVICE_SRC}
          ${UTIL_SRC})
# ##############################################################################
# uncomment the following line for dynamically loading views set_property(TARGET
# ${PROJECT_NAME} PROPERTY ENABLE_EXPORTS ON)

# ##############################################################################

add_subdirectory(test)

add_subdirectory(example)