include_directories(../service)

add_executable(QRCodeExample QRCodeExample.cc)

target_link_libraries(QRCodeExample PUBLIC QRCodeGenerator)