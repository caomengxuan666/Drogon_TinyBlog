/**
 * @FilePath     : /tiny_blog/web/example/UuidExample.cc
 * @Description  :  
 * @Author       : caomengxuan666 2507560089@qq.com
 * @Version      : 0.0.1
 * @LastEditors  : caomengxuan666 2507560089@qq.com
 * @LastEditTime : 2025-04-06 11:26:49
 * @Copyright    : PESONAL DEVELOPER CMX., Copyright (c) 2025.
**/
#include "../Utility/uuid.hpp"
#include <iostream>

int main() {
    std::string uuid1 = UUID::generate(32);
    std::cout << "Generated UUID (32 bits): " << uuid1 << std::endl;

    std::string uuid2 = UUID::generate_rfc4122();
    std::cout << "Generated UUID (RFC 4122): " << uuid2 << std::endl;

    return 0;
}
