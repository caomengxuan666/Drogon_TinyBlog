include_directories(../service)

add_executable(QRCodeExample QRCodeExample.cc)

target_link_libraries(QRCodeExample PUBLIC QRCodeGenerator)

add_executable(UUIDExample UuidExample.cc)

add_executable(SMTPExample SMTPExample.cc)

target_link_libraries(SMTPExample PUBLIC email_wrapper)