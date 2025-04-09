# 设置 CMake 最低版本要求
cmake_minimum_required(VERSION 3.15)

# 定义项目
project(service_components)

# 查找 pybind11 和 Python 开发包
find_package(pybind11 REQUIRED)
find_package(Python3 REQUIRED COMPONENTS Interpreter Development)
find_package(PNG CONFIG REQUIRED)

# 添加 Python 头文件路径
include_directories(${Python3_INCLUDE_DIRS})

# 创建接口库
add_library(email_wrapper SHARED email_wrapper.cpp)

# 添加头文件路径
target_include_directories(
  email_wrapper INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
                          ${Python3_INCLUDE_DIRS} # Python 头文件路径
)

target_link_libraries(
  email_wrapper PUBLIC pybind11::embed # pybind11 嵌入式解释器
  ${Python3_LIBRARIES} # Python 库
)

add_library(QRCodeGenerator INTERFACE)
# QRCodeGenerator 库
target_link_directories(QRCodeGenerator INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
                        ${Python3_INCLUDE_DIRS})

target_link_libraries(QRCodeGenerator INTERFACE pybind11::embed
                                                ${Python3_LIBRARIES})
